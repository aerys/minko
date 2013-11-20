/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "ASSIMPParser.hpp"

#include "IOHandler.hpp"

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags

#include "minko/scene/Node.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/file/AssetLibrary.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;
using namespace minko::file;
using namespace minko::scene;

/*static*/	Vector3::Ptr		ASSIMPParser::_TMP_POSITION			= Vector3::create();
/*static*/	Quaternion::Ptr		ASSIMPParser::_TMP_ROTATION			= Quaternion::create();
/*static*/	Matrix4x4::Ptr		ASSIMPParser::_TMP_ROTATION_MATRIX	= Matrix4x4::create();
/*static*/	Vector3::Ptr		ASSIMPParser::_TMP_SCALING			= Vector3::create();
/*static*/	Matrix4x4::Ptr		ASSIMPParser::_TMP_MATRIX			= Matrix4x4::create();

const ASSIMPParser::TextureTypeToName ASSIMPParser::_textureTypeToName = ASSIMPParser::initializeTextureTypeToName();
ASSIMPParser::TextureTypeToName
ASSIMPParser::initializeTextureTypeToName()
{
	TextureTypeToName typeToString;

	typeToString[aiTextureType_DIFFUSE] = "diffuseMap";
	typeToString[aiTextureType_NORMALS] = "normalMap";

	return typeToString;
}

ASSIMPParser::ASSIMPParser() :
	_numDependencies(0),
	_numLoadedDependencies(0),
	_filename(),
	_assetLibrary(nullptr),
	_options(nullptr),
	_symbol(nullptr),
	_nameToNode(),
	_nameToMesh(),
	_nameToAnimation(),
	_loaderCompleteSlots(),
	_loaderErrorSlots()
{
}

std::set<std::string>
ASSIMPParser::getSupportedFileExensions()
{
	Assimp::Importer importer;
	std::string list;
	std::set<std::string> result;

	importer.GetExtensionList(list);

	auto pos = list.find_first_of(";");
	while (pos != std::string::npos)
	{
		result.insert(list.substr(2, pos - 2));
		list = list.substr(pos + 1);
		pos = list.find_first_of(";");
	}
	if (!list.empty())
		result.insert(list.substr(2));

	return result;
}

void
ASSIMPParser::parse(const std::string&					filename,
					const std::string&					resolvedFilename,
					std::shared_ptr<Options>			options,
					const std::vector<unsigned char>&	data,
					std::shared_ptr<AssetLibrary>	    assetLibrary)
{
	resetParser();

	int pos = resolvedFilename.find_last_of(file::separator);

	if (pos > 0)
	{
		options = file::Options::create(options);
		options->includePaths().insert(resolvedFilename.substr(0, pos));
	}

    _filename = filename;
	_assetLibrary = assetLibrary;
	_options = options;
    
    //Init the assimp scene
    Assimp::Importer importer;

	importer.SetIOHandler(new IOHandler(options));

	const aiScene* scene = importer.ReadFileFromMemory(
		&data[0],
		data.size(),
		aiProcess_CalcTangentSpace
		| aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices
		| aiProcess_FlipUVs
		| aiProcess_SortByPType
	);
	
    if (!scene)
    {
        std::cout << importer.GetErrorString() << std::endl;
        throw;
    }
    
    parseDependencies(resolvedFilename, scene);

	_symbol = scene::Node::create(_filename);

	const unsigned int numFPS = 30;

	createSceneTree(_symbol, scene, scene->mRootNode);
	getSkinningFromAssimp(scene, numFPS);

	if (_numDependencies == _numLoadedDependencies)
		finalize();
}

void
ASSIMPParser::createSceneTree(scene::Node::Ptr minkoNode, const aiScene* scene, aiNode* ainode)
{
    for (uint i = 0; i < ainode->mNumChildren; i++)
    {
        auto childName = std::string(ainode->mChildren[i]->mName.data);
        auto child = scene::Node::create(childName);
        
        child->addComponent(getTransformFromAssimp(ainode->mChildren[i]));
        
        minkoNode->addChild(child);
		_nameToNode[child->name()]	= child;
		std::cout << "'" << child->name() << "' in nodemap" << std::endl;

        //Recursive call
        createSceneTree(child, scene, ainode->mChildren[i]);
    }
    
    for (uint j = 0; j < ainode->mNumMeshes; j++)
    {
        aiMesh *mesh = scene->mMeshes[ainode->mMeshes[j]];
		auto minkoMesh = scene::Node::create(mesh->mName.C_Str());

		minkoMesh->addComponent(Transform::create());
		createMeshGeometry(minkoMesh, mesh);
		createMeshSurface(minkoMesh, scene, mesh);

		minkoNode->addChild(minkoMesh);
		_nameToMesh[minkoMesh->name()]	= minkoMesh;
		std::cout << "'" << minkoMesh->name() << "' in meshmap" << std::endl;
    }
}

void
ASSIMPParser::getSkinningFromAssimp(const aiScene* aiscene, unsigned int numFPS)
{
	buildAnimationMap(aiscene);

	for (uint meshId = 0; meshId < aiscene->mNumMeshes; ++meshId)
	{
		const auto	aimesh		= aiscene->mMeshes[meshId];

		Skin skin;
		getSkinningFromAssimp(aimesh, skin);
	}
}

void
ASSIMPParser::createMeshGeometry(scene::Node::Ptr minkoNode, aiMesh* mesh)
{
    float *vertexArray;
    int numVertex;
    int vertexSize = 0;
    std::vector<unsigned short> indiceArray;
    std::vector<float> vertexVector;
    
    //VertexPositions
    if (mesh->HasPositions())
        vertexSize += 3;
    //VertexNormals
    if (mesh->HasNormals())
        vertexSize += 3;
    //VertexUVs
    if (mesh->GetNumUVChannels() > 0)
        vertexSize += 2;
    
    numVertex = mesh->mNumFaces*3;
    vertexArray = new float[numVertex*vertexSize];
    
    //foreach face, aka triangle
    for(unsigned int i=0;i<mesh->mNumFaces;i++)
    {
        const aiFace& face = mesh->mFaces[i];
        
        //foreach index
        for(int j=0;j<3;j++)//assume all faces are triangulated
        {
            if (mesh->HasPositions())
            {
                aiVector3D pos = mesh->mVertices[face.mIndices[j]];
                memcpy(vertexArray,&pos,sizeof(float)*3);
                vertexArray+=3;
            }
            if (mesh->HasNormals())
            {
                aiVector3D normal = mesh->mNormals[face.mIndices[j]];
                memcpy(vertexArray,&normal,sizeof(float)*3);
                vertexArray+=3;
            }
            if (mesh->GetNumUVChannels() > 0)
            {
                aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[j]];
                memcpy(vertexArray,&uv,sizeof(float)*2);
                vertexArray+=2;
            }
        }
    }
    
    vertexArray -= numVertex*vertexSize;
    
    for (unsigned short l = 0; l < numVertex; l++)
        indiceArray.push_back(l);
    
    for (int k = 0; k < numVertex*vertexSize; k++)
        vertexVector.push_back(vertexArray[k]);
    
    auto vBuffer = render::VertexBuffer::create(_assetLibrary->context(), vertexVector);
    auto iBuffer = render::IndexBuffer::create(_assetLibrary->context(), indiceArray);
    if (mesh->HasPositions())
        vBuffer->addAttribute("position", 3, 0);
    if (mesh->HasNormals())
        vBuffer->addAttribute("normal", 3, 3);
    if (mesh->GetNumUVChannels() > 0)
        vBuffer->addAttribute("uv", 2, 6);
    
    std::string meshGeometryName = std::string(mesh->mName.data);
    _assetLibrary->geometry(meshGeometryName, geometry::Geometry::create());
    _assetLibrary->geometry(meshGeometryName)->addVertexBuffer(vBuffer);
    _assetLibrary->geometry(meshGeometryName)->indices(iBuffer);
}

void
ASSIMPParser::createMeshSurface(scene::Node::Ptr minkoNode, const aiScene* scene, aiMesh* mesh)
{
    auto provider = material::Material::create(_options->material());
	aiString materialName;
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	material->Get(AI_MATKEY_NAME, materialName);
    
    //Diffuse color
    aiColor4D diffuse;
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
		provider->set("diffuseColor", Vector4::create(diffuse.r, diffuse.g, diffuse.b, diffuse.a));
	else
		provider->set("diffuseColor", Vector4::create(0.f, 0.f, 0.f, 1.f));
    
    //Specular color
    aiColor4D specular;
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, specular))
        provider->set("specularColor", Vector4::create(specular.r, specular.g, specular.b, specular.a));
    
    //Emissive color
	/*
    aiColor4D emissive;
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive))
        provider->set("emissiveColor", Vector4::create(emissive.r, emissive.g, emissive.b, emissive.a));
	//Ambient color
	aiColor4D ambient;
	if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, ambient))
		provider->set("ambientColor", Vector4::create(ambient.r, ambient.g, ambient.b, ambient.a));
	*/

	/*
    //Shininess
	float shininess;
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, shininess))
        provider->set("shininess", shininess);
    */

	provider = _options->materialFunction()(materialName.C_Str(), provider);

    int texIndex = 0;
    aiString path;
    aiReturn texFound = AI_SUCCESS;

	for (auto& typeAndString : _textureTypeToName)
	{
		if ((texFound = material->GetTexture(typeAndString.first, 0, &path)) == AI_SUCCESS)
		{
			auto texturePath = std::string(path.data);
			auto texture = _assetLibrary->texture(texturePath);

			if (!texturePath.empty() && texture)
				provider->set(typeAndString.second, texture);
		}
	}

    minkoNode->addComponent(Surface::create(
		_assetLibrary->geometry(std::string(mesh->mName.data)),
		provider,
		_options->effect()
    ));
}

void
ASSIMPParser::createLights(scene::Node::Ptr minkoRoot, const aiScene* scene)
{
    for (uint i = 0; i < scene->mNumLights; i++)
    {
        auto ailight = scene->mLights[i];
        auto light = findNode(std::string(ailight->mName.data), minkoRoot);
        
        switch (ailight->mType)
        {
            case aiLightSource_DIRECTIONAL:
            {
                light->addComponent(DirectionalLight::create());
                light->component<DirectionalLight>()->color()->setTo(
					ailight->mColorDiffuse.r,
                    ailight->mColorDiffuse.g,
                    ailight->mColorDiffuse.b
				);
                break;
            }
            case aiLightSource_POINT:
            {
                light->addComponent(PointLight::create());
                light->component<PointLight>()->color()->setTo(
					ailight->mColorDiffuse.r,
                    ailight->mColorDiffuse.g,
                    ailight->mColorDiffuse.b
				);
                break;
            }
            case aiLightSource_SPOT:
            {
                light->addComponent(SpotLight::create());
                light->component<PointLight>()->color()->setTo(
					ailight->mColorDiffuse.r,
                    ailight->mColorDiffuse.g,
                    ailight->mColorDiffuse.b
				);
                break;
            }
            default:
                light->addComponent(AmbientLight::create());
                light->component<AmbientLight>()->ambient(1.0f);
                light->component<PointLight>()->color()->setTo(
					ailight->mColorAmbient.r,
                    ailight->mColorAmbient.g,
                    ailight->mColorAmbient.b
				);
                break;
        }
        //minkoRoot->addChild(light);
    }
}

scene::Node::Ptr
ASSIMPParser::findNode(std::string name, scene::Node::Ptr root)
{
    scene::Node::Ptr result = nullptr;
    
    for (auto it = root->children().begin(); it != root->children().end(); it++)
    {
        if ((*it)->name() == name)
            return *it;
        
        result = findNode(name, *it);
        
        if (result != nullptr)
            return result;
    }
    
    return result;
}

Transform::Ptr
ASSIMPParser::getTransformFromAssimp(aiNode* ainode)
{
	return Transform::create(convert(ainode->mTransformation));
	/*
    aiMatrix4x4 aiTransform = ainode->mTransformation;
    Transform::Ptr result = Transform::create();

    result->transform()->initialize(
		aiTransform.a1, aiTransform.a2, aiTransform.a3, aiTransform.a4,
        aiTransform.b1, aiTransform.b2, aiTransform.b3, aiTransform.b4,
        aiTransform.c1, aiTransform.c2, aiTransform.c3, aiTransform.c4,
        aiTransform.d1, aiTransform.d2, aiTransform.d3, aiTransform.d4
	);
    
    return result;
	*/
}

void
ASSIMPParser::parseDependencies(const std::string& 	filename,
								const aiScene*		scene)
{
	std::list<std::string> loading;
	aiString path;

	for (unsigned int m = 0; m < scene->mNumMaterials; m++)
	{
		for (auto& textureTypeAndName : _textureTypeToName)
		{
			aiReturn texFound = scene->mMaterials[m]->GetTexture(textureTypeAndName.first, 0, &path);

			if (texFound == AI_SUCCESS)
			{
				std::string filename(path.data);

				std::cout << textureTypeAndName.second << ": " << filename << std::endl;

				if (!filename.empty() && std::find(loading.begin(), loading.end(), filename) == loading.end())
				{
					loading.push_back(filename);
					loadTexture(filename, _options);
				}
			}
		}
	}
}

void
ASSIMPParser::finalize()
{
	_loaderErrorSlots.clear();
	_loaderCompleteSlots.clear();

	_assetLibrary->node(_filename, _symbol);
	
	complete()->execute(shared_from_this());
}

void
ASSIMPParser::loadTexture(const std::string&	textureFilename,
						  Options::Ptr			options)
{
	auto loader = _options->loaderFunction()(textureFilename);

	_numDependencies++;

	_loaderCompleteSlots[loader] = loader->complete()->connect([&](file::AbstractLoader::Ptr loader)
	{
		auto pos = loader->resolvedFilename().find_last_of('.');
		auto extension = loader->resolvedFilename().substr(pos + 1);
		auto parser = _assetLibrary->parser(extension);

		auto complete = parser->complete()->connect([&](file::AbstractParser::Ptr parser)
		{
			_numLoadedDependencies++;

			if (_numDependencies == _numLoadedDependencies && _symbol)
				finalize();
		});

		parser->parse(
			loader->filename(),
			loader->resolvedFilename(),
			loader->options(),
			loader->data(),
			_assetLibrary
		);
	});

	_loaderErrorSlots[loader] = loader->error()->connect([&](file::AbstractLoader::Ptr loader)
	{
		_numLoadedDependencies++;
		std::cerr << "unable to find texture with filename '" << loader->filename() << "'" << std::endl;
	});

	loader->load(textureFilename, options);
}

void
ASSIMPParser::resetParser()
{
	_numDependencies		= 0;
	_numLoadedDependencies	= 0;
	_filename.clear();
	_symbol	= nullptr;
	_nameToNode.clear();
	_nameToMesh.clear();
	_nameToAnimation.clear();
	_nameToAnimMatrices.clear();
}

void
ASSIMPParser::buildAnimationMap(const aiScene* scene)
{
	_nameToAnimation.clear();

	if (scene == nullptr)
		return;

	for (unsigned int animId = 0; animId < scene->mNumAnimations; ++animId)
	{
		const auto anim = scene->mAnimations[animId];
		assert(anim != nullptr);

		sampleAnimation(anim, 30);

		for (unsigned int channelId = 0; channelId < anim->mNumChannels; ++channelId)
		{
			const auto nodeAnim = anim->mChannels[channelId];
			assert(nodeAnim != nullptr);

			_nameToAnimation[std::string(nodeAnim->mNodeName.C_Str())] = std::make_pair(anim, nodeAnim);
		}
	}
}

unsigned int
ASSIMPParser::getNumFrames(const aiMesh* aimesh) const
{
	assert(aimesh);

	unsigned int	numFrames	= 0;

	const auto		meshName	= std::string(aimesh->mName.C_Str());
	assert(_nameToMesh.count(meshName) > 0);

	const auto		minkoMesh	= _nameToMesh.find(meshName)->second;
	const auto		meshNode	= minkoMesh->parent();
	assert(meshNode && _nameToNode.count(meshNode->name()));

	for (unsigned int boneId = 0; boneId < aimesh->mNumBones; ++boneId)
	{
		const auto	boneName	= std::string(aimesh->mBones[boneId]->mName.C_Str());
		assert(_nameToNode.count(boneName) > 0);
		auto		currentNode	= _nameToNode.find(boneName)->second;
		do
		{
			if (currentNode == nullptr)
				break;

			if (_nameToAnimMatrices.count(currentNode->name()) > 0)
			{
				const unsigned int numNodeFrames = _nameToAnimMatrices.find(currentNode->name())->second.size();
				assert(numNodeFrames > 0);

				if (numFrames == 0)
					numFrames = numNodeFrames;
				else if (numFrames != numNodeFrames)
					return 0;
			}
			currentNode = currentNode->parent();
		}
		while(currentNode != meshNode && currentNode != meshNode->parent());
	}

	return numFrames;
}

void
ASSIMPParser::getSkinningFromAssimp(const aiMesh*	aimesh, 
									Skin&			skin) const
{
	skin.clear();

	if (aimesh == nullptr || aimesh->mNumBones == 0)
		return;

	const unsigned int numFrames = getNumFrames(aimesh);
	if (numFrames == 0)
	{
		std::cerr << "Failed to flatten skinning information. Most likely involved nodes do not share a common animation." << std::endl;
		return;
	}
	
	skin.bones				 .resize(aimesh->mNumBones);
	skin.boneMatricesPerFrame.resize(numFrames, std::vector<Matrix4x4::Ptr>(aimesh->mNumBones));


	const auto	meshName	= std::string(aimesh->mName.C_Str());
	assert(_nameToMesh.count(meshName) > 0);

	const auto	minkoMesh	= _nameToMesh.find(meshName)->second;
	const auto	meshNode	= minkoMesh->parent();
	assert(meshNode && _nameToNode.count(meshNode->name()));


	for (unsigned int boneId = 0; boneId < aimesh->mNumBones; ++boneId)
	{
		const auto	aibone		= aimesh->mBones[boneId];
		const auto	boneName	= std::string(aibone->mName.C_Str());
		assert(_nameToNode.count(boneName) > 0);

		const auto	boneNode	= _nameToNode.find(boneName)->second;
		assert(boneNode);
		skin.bones[boneId]		= boneNode;

		for (unsigned int frameId = 0; frameId < numFrames; ++frameId)
		{
			auto boneLocalToObject	= Matrix4x4::create();
			auto currentNode		= boneNode;

			// manually compute the bone's local-to-object matrix at specified frame
			do
			{
				if (currentNode == nullptr)
					break;
				assert(_nameToNode.count(currentNode->name()) > 0);

				Matrix4x4::Ptr currentTransform = nullptr;

				if (_nameToAnimMatrices.count(currentNode->name()) > 0)
					currentTransform = _nameToAnimMatrices.find(currentNode->name())->second[frameId];
				else
					currentTransform = currentNode->component<Transform>()->transform();

				boneLocalToObject->append(currentTransform);
				currentNode = currentNode->parent();
			} while(currentNode != meshNode && currentNode != meshNode->parent());

			// compute the matrix from bind space to object space at specified frame
			convert(aibone->mOffsetMatrix, _TMP_MATRIX);
			boneLocalToObject->prepend(_TMP_MATRIX);

			skin.boneMatricesPerFrame[frameId][boneId] = boneLocalToObject;
 		}
	}
}

/*
const aiAnimation*
ASSIMPParser::getSkeletonAnimation(const aiMesh* aimesh)
{
	const aiAnimation* ret = nullptr;

	if (aimesh)
	{
		const auto meshNode = _nameToNode[std::string(aimesh->mName.C_Str())];

		for (unsigned int boneId = 0; boneId < aimesh->mNumBones; ++boneId)
		{
			auto current = _nameToNode[std::string(aimesh->mBones[boneId]->mName.C_Str())];
			do
			{
				if (current == nullptr)
					break;

				const auto foundAnimIt = _nameToAnimation.find(current->name());
				if (foundAnimIt != _nameToAnimation.end())
				{
					if (ret == nullptr)
						ret = foundAnimIt->second.first;
					else if (ret != foundAnimIt->second.first)
					{
						std::cerr << "All bones of a mesh must correspond to the channels of a same animation." << std::endl;
						throw;
					}
				}

				current = current->parent();
			} while(current != meshNode && current != meshNode->parent());
		}
	}
	return ret;
}
*/

void
ASSIMPParser::sampleAnimation(const aiAnimation*	animation,
							  unsigned int			numFPS)
{
	std::unordered_map<std::string, std::vector<Matrix4x4::Ptr>> _nodeMatrices;
	if (animation == nullptr || animation->mTicksPerSecond < 1e-6)
		return;

	unsigned int numFrames	= (unsigned int)ceil((double)numFPS * animation->mDuration / animation->mTicksPerSecond);
	numFrames				= numFrames < 2 ? 2 : numFrames;

	const float			timeStep	= (float)animation->mDuration / (float)numFrames;
	std::vector<float>	sampleTimes	(numFrames, 0.0f);
	for (unsigned int frameId = 1; frameId < numFrames; ++frameId)
	{
		sampleTimes[frameId] = sampleTimes[frameId - 1] + timeStep; 
	}

	for (unsigned int channelId = 0; channelId < animation->mNumChannels; ++channelId)
	{
		const auto nodeAnimation	= animation->mChannels[channelId];
		const auto nodeName			= nodeAnimation->mNodeName.C_Str();
		std::cout << "'" << nodeAnimation->mNodeName.C_Str() << "' animated" << std::endl;

		_nameToAnimMatrices[nodeName] = std::vector<Matrix4x4::Ptr>();

		sample(nodeAnimation, sampleTimes, _nameToAnimMatrices[nodeName]);
	}

}

/*static*/
void
ASSIMPParser::sample(const aiNodeAnim*				nodeAnimation, 
					 const std::vector<float>&		times, 
					 std::vector<Matrix4x4::Ptr>&	matrices)
{
	assert(nodeAnimation);

	matrices.resize(times.size());

	// precompute time factors
	std::vector<float> positionKeyTimeFactors;
	std::vector<float> rotationKeyTimeFactors;
	std::vector<float> scalingKeyTimeFactors;

	computeTimeFactors(nodeAnimation->mNumPositionKeys,	nodeAnimation->mPositionKeys,	positionKeyTimeFactors);
	computeTimeFactors(nodeAnimation->mNumRotationKeys,	nodeAnimation->mRotationKeys,	rotationKeyTimeFactors);
	computeTimeFactors(nodeAnimation->mNumScalingKeys,	nodeAnimation->mScalingKeys,	scalingKeyTimeFactors);

	for (unsigned int frameId = 0; frameId < times.size(); ++frameId)
	{
		const float time = times[frameId];

		// sample position from keys
		sample(nodeAnimation->mPositionKeys, positionKeyTimeFactors, time, _TMP_POSITION);

		// sample rotation from keys
		sample(nodeAnimation->mRotationKeys, rotationKeyTimeFactors, time, _TMP_ROTATION);
		_TMP_ROTATION->toMatrix(_TMP_ROTATION_MATRIX);

		const std::vector<float>&	rotation	= _TMP_ROTATION_MATRIX->data();

		// sample scaling from keys
		sample(nodeAnimation->mScalingKeys, scalingKeyTimeFactors, time, _TMP_SCALING);

		// recompose the interpolated matrix at the specified frame
		matrices[frameId] = Matrix4x4::create()
			->initialize(
				_TMP_SCALING->x() * rotation[0], _TMP_SCALING->y() * rotation[1], _TMP_SCALING->z() * rotation[2],  _TMP_POSITION->x(),
				_TMP_SCALING->x() * rotation[4], _TMP_SCALING->y() * rotation[5], _TMP_SCALING->z() * rotation[6],  _TMP_POSITION->y(),
				_TMP_SCALING->x() * rotation[8], _TMP_SCALING->y() * rotation[9], _TMP_SCALING->z() * rotation[10], _TMP_POSITION->z(),
				0.0, 0.0, 0.0, 1.0f
			);
	}
}

/*static*/
Vector3::Ptr
ASSIMPParser::sample(const aiVectorKey*			keys,
					 const std::vector<float>&	keyTimeFactors,
					 float						time,
					 Vector3::Ptr				output)
{
	if (output == nullptr)
		output = Vector3::create(0.0f, 0.0f, 0.0f);

	const unsigned int	numKeys	= keyTimeFactors.size();
	const unsigned int	id		= getIndexForTime(numKeys, keys, time);
	const aiVector3D&	value0	= keys[id].mValue;

	if (id == numKeys - 1)
		output->setTo(value0.x, value0.y, value0.z);
	else
	{
		const float			w1		= (time - (float)keys[id].mTime) * keyTimeFactors[id];
		const float			w0		= 1.0f - w1;
		const aiVector3D&	value1	= keys[id+1].mValue;

		output->setTo(
			w0 * value0.x + w1 * value1.x, 
			w0 * value0.y + w1 * value1.y, 
			w0 * value0.z + w1 * value1.z
		);
	}

	return output;
}

/*static*/
Quaternion::Ptr
ASSIMPParser::sample(const aiQuatKey*			keys, 
					 const std::vector<float>&	keyTimeFactors,
					 float						time, 
					 Quaternion::Ptr			output)
{
	if (output == nullptr)
		output = Quaternion::create();

	const unsigned int	numKeys	= keyTimeFactors.size();
	const unsigned int	id		= getIndexForTime(numKeys, keys, time);
	const aiQuaternion&	value0	= keys[id].mValue;

	if (id == numKeys - 1)
		output->setTo(value0.x, value0.y, value0.z, value0.w);
	else
	{
		const float			w1		= (time - (float)keys[id].mTime) * keyTimeFactors[id];
		const float			w0		= 1.0f - w1;
		const aiQuaternion&	value1	= keys[id+1].mValue;

		// normalized linear interpolation, should do spherical but too costly
		const float	qi			= w0 * value0.x + w1 * value1.x;
		const float	qj			= w0 * value0.y + w1 * value1.y;
		const float	qk			= w0 * value0.z + w1 * value1.z;
		const float	qr			= w0 * value0.w + w1 * value1.w;
		const float length		= sqrtf(qi * qi + qj * qj + qk * qk + qr * qr);

		if (length > 1e-3f)
		{
			const float invLength	= 1.0f / length;
			output->setTo(qi * invLength, qj * invLength, qk * invLength, qr * invLength);
		}
		else
			output->identity();
	}

	return output;
}

template<class AiKey>
/*static*/
void
ASSIMPParser::computeTimeFactors(unsigned int			numKeys,
								 const AiKey*			keys,
								 std::vector<float>&	keyTimeFactors)
{
	keyTimeFactors.resize(numKeys);

	if (numKeys == 0 || keys == nullptr)
		return;

	for (unsigned int keyId = 0; keyId < numKeys - 1; ++keyId)
		keyTimeFactors[keyId] = (float)(1.0 / (keys[keyId + 1].mTime - keys[keyId].mTime + 1e-6f));
	keyTimeFactors.back() = 1.0f;
}

template<class AiKey>
/*static*/
unsigned int
ASSIMPParser::getIndexForTime(unsigned int	numKeys,
							  const AiKey*	keys,
							  double		time)
{
	if (numKeys == 0 || keys == nullptr)
		return 0;

	unsigned int id			= 0;
	unsigned int lowerId	= 0;
	unsigned int upperId	= numKeys - 1;
	while(upperId - lowerId > 1)
	{
		id = (lowerId + upperId) >> 1;
		if (keys[id].mTime > time)
			upperId = id;
		else
			lowerId = id;
	}

	return lowerId;
}

/*static*/
Matrix4x4::Ptr
ASSIMPParser::convert(const aiMatrix4x4& matrix, Matrix4x4::Ptr output)
{
	if (output == nullptr)
		output = Matrix4x4::create();

    output->initialize(
		matrix.a1, matrix.a2, matrix.a3, matrix.a4,
        matrix.b1, matrix.b2, matrix.b3, matrix.b4,
        matrix.c1, matrix.c2, matrix.c3, matrix.c4,
        matrix.d1, matrix.d2, matrix.d3, matrix.d4
	);
    
	return output;
}