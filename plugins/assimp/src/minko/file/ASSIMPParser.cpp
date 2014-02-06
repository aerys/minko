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

#include "minko/file/ASSIMPParser.hpp"

#include "IOHandler.hpp"

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags
#include "assimp/material.h"

#include "minko/scene/Node.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Skinning.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/geometry/Skin.hpp"
#include "minko/geometry/Bone.hpp"
#include "minko/material/Material.hpp"
#include "minko/file/AssetLibrary.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;
using namespace minko::file;
using namespace minko::scene;
using namespace minko::geometry;

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
	_aiNodeToNode(),
	_aiMeshToNode(),
	_nameToNode(),
	_nameToAnimMatrices(),
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

	int pos = resolvedFilename.find_last_of("\\/");

	if (pos > 0)
	{
		options = file::Options::create(options);
		options->includePaths().push_back(resolvedFilename.substr(0, pos));
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
		aiProcessPreset_TargetRealtime_Fast
		| aiProcess_JoinIdenticalVertices
		| aiProcess_GenSmoothNormals
		| aiProcess_SplitLargeMeshes
		| aiProcess_LimitBoneWeights
		| aiProcess_GenUVCoords
		| aiProcess_OptimizeMeshes
		| aiProcess_OptimizeGraph
		| aiProcess_FlipUVs
		| aiProcess_SortByPType
		| aiProcess_Triangulate
		| aiProcess_JoinIdenticalVertices,
		resolvedFilename.c_str()
	);
	
    if (!scene)
    {
		throw std::runtime_error(importer.GetErrorString());
    }
    
    parseDependencies(resolvedFilename, scene);

	_symbol = scene::Node::create(_filename);
	createSceneTree(_symbol, scene, scene->mRootNode, assetLibrary);
	_symbol = _options->nodeFunction()(_symbol);

	getSkinningFromAssimp(scene);

	if (_numDependencies == _numLoadedDependencies)
		finalize();

	disposeNodeMaps();
}

void
ASSIMPParser::createSceneTree(scene::Node::Ptr 				minkoNode,
							  const aiScene* 				scene,
							  aiNode* 						ainode,
							  std::shared_ptr<AssetLibrary> assets)
{
	for (uint i = 0; i < ainode->mNumChildren; i++)
    {
		assert(ainode->mChildren[i]);

        auto childName	= std::string(ainode->mChildren[i]->mName.data);
        auto child		= scene::Node::create(childName);
        
		_aiNodeToNode[ainode->mChildren[i]] = child;
		if (!child->name().empty())
			_nameToNode[child->name()] = child;

        child->addComponent(getTransformFromAssimp(ainode->mChildren[i]));
        
		//if (!child->name().empty())
		//	_nameToNode[child->name()] = child;


#ifdef DEBUG_SKINNING
		std::cout << "nodemap\t<- '" << child->name() << "'" << std::endl;
#endif // DEBUG_SKINNING

        //Recursive call
		createSceneTree(child, scene, ainode->mChildren[i], assets);

        minkoNode->addChild(_options->nodeFunction()(child));
    }
    
    for (uint j = 0; j < ainode->mNumMeshes; j++)
    {
        aiMesh* mesh = scene->mMeshes[ainode->mMeshes[j]];
		assert(mesh);

		auto minkoMesh = scene::Node::create(mesh->mName.C_Str());

		_aiMeshToNode[mesh] = minkoMesh;

		minkoMesh->addComponent(Transform::create());
		createMeshSurface(minkoMesh, scene, mesh);

		minkoNode->addChild(_options->nodeFunction()(minkoMesh));

#ifdef DEBUG_SKINNING
		std::cout << "meshmap\t<- '" << minkoMesh->name() << "'" << std::endl;
#endif // DEBUG_SKINNING
    }
}

Geometry::Ptr
ASSIMPParser::createMeshGeometry(scene::Node::Ptr minkoNode, aiMesh* mesh)
{
	unsigned int vertexSize = 0; 

    if (mesh->HasPositions())
        vertexSize += 3;
    if (mesh->HasNormals())
        vertexSize += 3;
    if (mesh->GetNumUVChannels() > 0)
        vertexSize += 2;

	std::vector<float>	vertexData	(vertexSize * mesh->mNumVertices, 0.0f);
	unsigned int		vId			= 0;
	for (unsigned int vertexId = 0; vertexId < mesh->mNumVertices; ++vertexId)
	{
		if (mesh->HasPositions())
        {
			const aiVector3D&	vec	= mesh->mVertices[vertexId];

			vertexData[vId++]	= vec.x;
			vertexData[vId++]	= vec.y;
			vertexData[vId++]	= vec.z;
		}

		if (mesh->HasNormals())
		{
			const aiVector3D&	vec	= mesh->mNormals[vertexId];

			vertexData[vId++]	= vec.x;
			vertexData[vId++]	= vec.y;
			vertexData[vId++]	= vec.z;
		}

		if (mesh->GetNumUVChannels() > 0)
		{
			const aiVector3D&	vec = mesh->mTextureCoords[0][vertexId];

			vertexData[vId++]	= vec.x;
			vertexData[vId++]	= vec.y;
		}
	}

	// make sure the flag 'aiProcess_Triangulate' is specified before importing the scene
	std::vector<unsigned short>	indexData	(3 * mesh->mNumFaces, 0);

	for (unsigned int faceId = 0; faceId < mesh->mNumFaces; ++faceId)
	{
		const aiFace& face = mesh->mFaces[faceId];

		for (unsigned int j = 0; j < 3; ++j)
			indexData[j + 3*faceId] = face.mIndices[j];
	}

	// create the geometry's vertex and index buffers
	auto geometry		= Geometry::create();
	auto vertexBuffer	= render::VertexBuffer::create(_assetLibrary->context(), vertexData);

	unsigned int attrOffset = 0;
	if (mesh->HasPositions())
	{
		vertexBuffer->addAttribute("position", 3, attrOffset);
		attrOffset	+= 3;
	}
	if (mesh->HasNormals())
	{
		vertexBuffer->addAttribute("normal", 3, attrOffset);
		attrOffset	+= 3;
	}
	if (mesh->GetNumUVChannels() > 0)
	{
		vertexBuffer->addAttribute("uv", 2, attrOffset);
		attrOffset	+= 2;
	}

	geometry->addVertexBuffer(vertexBuffer);
	geometry->indices(render::IndexBuffer::create(_assetLibrary->context(), indexData));

	const std::string meshName = std::string(mesh->mName.C_Str());

	geometry = _options->geometryFunction()(meshName, geometry);

	// save the geometry in the assets library
	if (!meshName.empty())
		_assetLibrary->geometry(meshName, geometry);

	return geometry;
}

void
ASSIMPParser::createMeshSurface(scene::Node::Ptr 	minkoNode,
								const aiScene* 		scene,
								aiMesh* 			mesh)
{
	auto geom = createMeshGeometry(minkoNode, mesh);
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
		if ((texFound = material->GetTexture(static_cast<aiTextureType>(typeAndString.first), 0, &path)) == AI_SUCCESS)
		{
			auto texturePath	= std::string(path.data);
			auto texture		= _assetLibrary->texture(texturePath);

			if (!texturePath.empty() && texture)
				provider->set(typeAndString.second, texture);
		}
	}

    minkoNode->addComponent(Surface::create(geom, provider, _options->effect()));
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
			aiReturn texFound = scene->mMaterials[m]->GetTexture(static_cast<aiTextureType>(textureTypeAndName.first), 0, &path);

			if (texFound == AI_SUCCESS)
			{
				std::string filename(path.data);

				if (!filename.empty() && std::find(loading.begin(), loading.end(), filename) == loading.end())
				{
#ifdef DEBUG
					std::cout << "ASSIMParser: loading texture '" << filename << "'..." << std::endl;
#endif

					loading.push_back(filename);
					_numDependencies++;
					loadTexture(filename, filename, _options);
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

	_assetLibrary->symbol(_filename, _symbol);
	
	complete()->execute(shared_from_this());
}

void
ASSIMPParser::loadTexture(const std::string&	textureFilename,
						  const std::string&	assetName,
						  Options::Ptr			options)
{
	auto loader = _options->loaderFunction()(textureFilename);

	_loaderCompleteSlots[loader] = loader->complete()->connect([&](file::AbstractLoader::Ptr loader)
	{
		auto pos = loader->resolvedFilename().find_last_of('.');
		auto extension = loader->resolvedFilename().substr(pos + 1);
		auto parser = _assetLibrary->parser(extension);

		if (!parser)
		{
			_numLoadedDependencies++;
			if (_numDependencies == _numLoadedDependencies && _symbol)
				finalize();

			return;
		}

		auto complete = parser->complete()->connect([&](file::AbstractParser::Ptr parser)
		{
#ifdef DEBUG
			std::cout << "ASSIMParser: texture '" << textureFilename << "' loaded" << std::endl;
#endif

			_numLoadedDependencies++;
			if (_numDependencies == _numLoadedDependencies && _symbol)
				finalize();
		});

		parser->parse(
			assetName,
			loader->resolvedFilename(),
			loader->options(),
			loader->data(),
			_assetLibrary
		);
	});

	_loaderErrorSlots[loader] = loader->error()->connect([&](file::AbstractLoader::Ptr loader)
	{
		auto pos = loader->filename().find_last_of("\\/");

		if (pos != std::string::npos)
		{
			loadTexture(loader->filename().substr(pos + 1), assetName, options);
		}
		else
		{
			_numLoadedDependencies++;
			std::cerr << "unable to find texture with filename '" << loader->filename() << "'" << std::endl;
		}
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

	disposeNodeMaps();
}
void
ASSIMPParser::disposeNodeMaps()
{
	_aiNodeToNode.clear();
	_aiMeshToNode.clear();
	_nameToNode.clear();
	_nameToAnimMatrices.clear();
}

unsigned int
ASSIMPParser::getNumFrames(const aiMesh* aimesh) const
{
	assert(aimesh && _aiMeshToNode.count(aimesh) > 0);
	const auto	minkoMesh	= _aiMeshToNode.find(aimesh)->second;
	const auto	meshNode	= minkoMesh->parent();
	assert(meshNode);

	unsigned int numFrames = 0;

	for (unsigned int boneId = 0; boneId < aimesh->mNumBones; ++boneId)
	{
		const auto	boneName	= std::string(aimesh->mBones[boneId]->mName.C_Str());
		const auto	foundBoneIt = _nameToNode.find(boneName);

		if (foundBoneIt != _nameToNode.end())
		{
			auto	currentNode = foundBoneIt->second;
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
					{
#ifdef DEBUG_SKINNING
						std::cerr << "Warning: Inconsistent number of frames between the different parts of a same mesh!" << std::endl;
#endif // DEBUG_SKINNING
						numFrames = std::max(numFrames, numNodeFrames); // FIXME
					}
				}
				currentNode = currentNode->parent();
			} 
			while (currentNode != meshNode);
		}
	}

	return numFrames;
}

void
ASSIMPParser::getSkinningFromAssimp(const aiScene* aiscene)
{
	if (_options->skinningFramerate() == 0)
		return;

	// resample all animations with the specified temporal precision 
	// and store them in the _nameToAnimMatrices map.
	sampleAnimations(aiscene);

	// add a Skinning component to all animated mesh
	for (unsigned int meshId = 0; meshId < aiscene->mNumMeshes; ++meshId)
	{
		const auto	aimesh		= aiscene->mMeshes[meshId];
		const auto	meshName	= std::string(aimesh->mName.C_Str());
		const auto	skin		= getSkinningFromAssimp(aimesh);
		
		if (skin)
		{
			auto		meshNode		= _aiMeshToNode.find(aimesh)->second;
			const float	durationSeconds	= skin->numFrames() / (float)_options->skinningFramerate();

			skin->duration((uint)floorf(1000.0f * durationSeconds));
			meshNode->addComponent(Skinning::create(skin, _options->skinningMethod(), _assetLibrary->context()));
		}
	}
}

Skin::Ptr
ASSIMPParser::getSkinningFromAssimp(const aiMesh* aimesh) const
{
	if (aimesh == nullptr || aimesh->mNumBones == 0)
		return nullptr;

	const unsigned int numFrames = getNumFrames(aimesh);
	if (numFrames == 0)
	{
		std::cerr << "Failed to flatten skinning information. Most likely involved nodes do not share a common animation." << std::endl;
		return nullptr;
	}
	
	auto skin = Skin::create(aimesh->mNumBones, numFrames);

	const auto	minkoMesh	= _aiMeshToNode.find(aimesh)->second;
	const auto	meshNode	= minkoMesh->parent();

#ifdef DEBUG_SKINNING
	std::cout << "mesh '" << minkoMesh->name() << "'\n\t- # bones = " << aimesh->mNumBones << "\n\t- # vertices = " << aimesh->mNumVertices << std::endl;
#endif // DEBUG_SKINNING

	for (unsigned int boneId = 0; boneId < aimesh->mNumBones; ++boneId)
	{
#ifdef DEBUG_SKINNING
		//std::cout << "\ncollapsed trf bone '" << aimesh->mBones[boneId]->mName.C_Str() << "'" << std::endl;
#endif // DEBUG_SKINNING

		const auto bone = getSkinningFromAssimp(aimesh->mBones[boneId]);
		if (!bone)
			return nullptr;
		
		const auto boneNode			= bone->node();
		const auto boneOffsetMatrix	= bone->offsetMatrix();

		skin->bone(boneId, bone);

		for (unsigned int frameId = 0; frameId < numFrames; ++frameId)
		{
			auto currentNode		= bone->node();
			auto boneLocalToObject	= Matrix4x4::create()->copyFrom(bone->offsetMatrix());

			// manually compute the bone's local-to-object matrix at specified frame
			do
			{
				if (currentNode == nullptr)
					break;

				Matrix4x4::Ptr currentTransform = nullptr;

				if (_nameToAnimMatrices.count(currentNode->name()) > 0)
				{
					// get the animated node's resampled transform matrix
					const std::vector<Matrix4x4::Ptr>& matrices = _nameToAnimMatrices.find(currentNode->name())->second;
					currentTransform = matrices[frameId % matrices.size()]; // FIXME
				}
				else if (currentNode->hasComponent<Transform>())
				{
					// get the constant node's transform
					currentTransform = currentNode->component<Transform>()->matrix();
				}

				if (currentTransform)
					boneLocalToObject->append(currentTransform);

				currentNode = currentNode->parent();
			} 
			while(currentNode != meshNode);

			skin->matrix(frameId, boneId, boneLocalToObject);

#ifdef DEBUG_SKINNING
			//std::cout << "time[" << frameId << "]\t=> " << skin->matrix(frameId, boneId)->toString() << std::endl;
#endif // DEBUG_SKINNING
 		}
	}

	return skin->reorganizeByVertices()->disposeBones();
}

Bone::Ptr 
ASSIMPParser::getSkinningFromAssimp(const aiBone* aibone) const
{
	const auto boneName = std::string(aibone->mName.C_Str());
	if (aibone == nullptr || _nameToNode.count(boneName) == 0)
		return nullptr;

	auto node			= _nameToNode.find(boneName)->second;
	auto offsetMatrix	= convert(aibone->mOffsetMatrix);

	std::vector<unsigned short> boneVertexIds		(aibone->mNumWeights, 0);
	std::vector<float>			boneVertexWeights	(aibone->mNumWeights, 0.0f);

	for (unsigned int i = 0; i < aibone->mNumWeights; ++i)
	{
		boneVertexIds[i]		= (unsigned short)aibone->mWeights[i].mVertexId;
		boneVertexWeights[i]	= aibone->mWeights[i].mWeight; 
	}

	return Bone::create(node, offsetMatrix, boneVertexIds, boneVertexWeights);
}

void
ASSIMPParser::sampleAnimations(const aiScene*	scene)
{
	_nameToAnimMatrices.clear();

	if (scene == nullptr)
		return;

	for (unsigned int animId = 0; animId < scene->mNumAnimations; ++animId)
		sampleAnimation(scene->mAnimations[animId]);
}

void
ASSIMPParser::sampleAnimation(const aiAnimation* animation)
{
	if (animation == nullptr || animation->mTicksPerSecond < 1e-6 || _options->skinningFramerate() == 0)
		return;

	unsigned int numFrames	= (unsigned int)floorf(float(_options->skinningFramerate() * animation->mDuration / animation->mTicksPerSecond));
	numFrames				= numFrames < 2 ? 2 : numFrames;

	const float			timeStep	= (float)animation->mDuration / (float)(numFrames - 1);
	std::vector<float>	sampleTimes	(numFrames, 0.0f);
	for (unsigned int frameId = 1; frameId < numFrames; ++frameId)
	{
		sampleTimes[frameId] = sampleTimes[frameId - 1] + timeStep; 
	}

	for (unsigned int channelId = 0; channelId < animation->mNumChannels; ++channelId)
	{
		const auto nodeAnimation	= animation->mChannels[channelId];
		const auto nodeName			= nodeAnimation->mNodeName.C_Str();
		// According to the ASSIMP documentation, animated nodes should come with existing, unique names.

		if (nodeName)
		{
			_nameToAnimMatrices[nodeName] = std::vector<Matrix4x4::Ptr>();
			sample(nodeAnimation, sampleTimes, _nameToAnimMatrices[nodeName]);
		}
	}
}

/*static*/
void
ASSIMPParser::sample(const aiNodeAnim*				nodeAnimation, 
					 const std::vector<float>&		times, 
					 std::vector<Matrix4x4::Ptr>&	matrices)
{
	assert(nodeAnimation);

#ifdef DEBUG
	//std::cout << "\nsample animation of mesh('" << nodeAnimation->mNodeName.C_Str() << "')" << std::endl;
#endif // DEBUG

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
		_TMP_ROTATION->normalize();

		if (_TMP_ROTATION->length() == 0.)
			_TMP_ROTATION_MATRIX->identity();
		else
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

#ifdef DEBUG
		// std::cout << "\tframeID = " << frameId << "\ttime = " << time << "\nM = " << matrices[frameId]->toString() << std::endl;
#endif // DEBUG
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

		aiQuaternion interp;
		aiQuaternion::Interpolate(interp, value0, value1, w1);

		output->setTo(interp.x, interp.y, interp.z, interp.w);

		/*
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
			*/
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
