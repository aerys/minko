/*
Copyright (c) 2014 Aerys

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

#include "minko/file/AbstractASSIMPParser.hpp"

#include "IOHandler.hpp"

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags
#include "assimp/material.h"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Skinning.hpp"
#include "minko/component/MasterAnimation.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/AbstractAnimation.hpp"
#include "minko/component/Animation.hpp"
#include "minko/animation/Matrix4x4Timeline.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/geometry/Skin.hpp"
#include "minko/geometry/Bone.hpp"
#include "minko/material/Material.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/Effect.hpp"
#include "minko/material/Material.hpp"
#include "minko/material/BasicMaterial.hpp"
#include "minko/material/PhongMaterial.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/Priority.hpp"
#include "minko/render/Texture.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::file;
using namespace minko::scene;
using namespace minko::geometry;

#ifdef DEBUG_ASSIMP_DOT
	static
	void
	dotPrint(const std::string&, const aiScene*);

	static
	void
	dotPrint(const std::string&, Node::Ptr);
#endif // DEBUG_ASSIMP_DOT

#ifdef DEBUG_ASSIMP
	static
	std::ostream&
	printNode(std::ostream& out, Node::Ptr node, uint depth)
#endif // DEBUG_ASSIMP

/*static*/ const AbstractASSIMPParser::TextureTypeToName AbstractASSIMPParser::_textureTypeToName = AbstractASSIMPParser::initializeTextureTypeToName();
/*static*/ const std::string AbstractASSIMPParser::PNAME_TRANSFORM = "transform.matrix";

/*static*/
AbstractASSIMPParser::TextureTypeToName
AbstractASSIMPParser::initializeTextureTypeToName()
{
	TextureTypeToName typeToString;

	typeToString[aiTextureType_DIFFUSE]		= "diffuseMap";
	typeToString[aiTextureType_SPECULAR]	= "specularMap";
	typeToString[aiTextureType_OPACITY]		= "alphaMap";
	typeToString[aiTextureType_NORMALS]		= "normalMap";
	typeToString[aiTextureType_REFLECTION]	= "environmentMap2d"; // not sure about this one

	return typeToString;
}

AbstractASSIMPParser::AbstractASSIMPParser() :
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
	_alreadyAnimatedNodes(),
	_loaderCompleteSlots(),
	_loaderErrorSlots(),
    _importer(nullptr)
{
}

AbstractASSIMPParser::~AbstractASSIMPParser()
{
}

void
AbstractASSIMPParser::parse(const std::string&					filename,
							const std::string&					resolvedFilename,
							std::shared_ptr<Options>			options,
							const std::vector<unsigned char>&	data,
							std::shared_ptr<AssetLibrary>	    assetLibrary)
{
#ifdef DEBUG
	std::cout << "AbstractASSIMPParser::parse()" << std::endl;
#endif // DEBUG

	resetParser();
	initImporter();

	int pos = resolvedFilename.find_last_of("\\/");

	options = file::Options::create(options);

	if (pos > 0)
	{
        options->includePaths().push_front(resolvedFilename.substr(0, pos));
	}

    _filename		= filename;
	_assetLibrary	= assetLibrary;
	_options		= options;

	//fixme : find a way to handle loading dependencies asynchronously
	auto ioHandlerOptions = Options::create(options);
	ioHandlerOptions->loadAsynchronously(false);

    auto ioHandler = new IOHandler(ioHandlerOptions, _assetLibrary);

    ioHandler->errorFunction([this](IOHandler& self, const Error& error) -> void
    {
        this->error()->execute(shared_from_this(), error);
    });

    _importer->SetIOHandler(ioHandler);

#ifdef DEBUG
	std::cout << "AbstractASSIMPParser: preparing to parse" << std::endl;
#endif // DEBUG

	const aiScene* scene = _importer->ReadFileFromMemory(
		&data[0],
		data.size(),
		//| aiProcess_GenSmoothNormals // assertion is raised by assimp
		aiProcess_JoinIdenticalVertices
		| aiProcess_GenSmoothNormals
		| aiProcess_SplitLargeMeshes
		| aiProcess_LimitBoneWeights
		| aiProcess_GenUVCoords
		| aiProcess_OptimizeMeshes
		//| aiProcess_OptimizeGraph // makes the mesh simply vanish
		| aiProcess_FlipUVs
		| aiProcess_SortByPType
		| aiProcess_Triangulate,
		resolvedFilename.c_str()
	);

	if (!scene)
        _error->execute(shared_from_this(), Error(_importer->GetErrorString()));

#ifdef DEBUG
	std::cout << "AbstractASSIMPParser: scene parsed" << std::endl;
#endif // DEBUG

	parseDependencies(resolvedFilename, scene);

#ifdef DEBUG
	std::cout << "AbstractASSIMPParser: " << _numDependencies << " dependencies to load..." << std::endl;
#endif // DEBUG

	if (_numDependencies == 0)
		allDependenciesLoaded(scene);
}

void
AbstractASSIMPParser::allDependenciesLoaded(const aiScene* scene)
{
#ifdef DEBUG
	std::cout << "AbstractASSIMPParser: " << _numDependencies << " dependencies loaded!" << std::endl;
	if (_numDependencies != _numLoadedDependencies)
		throw std::logic_error("_numDependencies != _numLoadedDependencies");
#endif // DEBUG

	_symbol = scene::Node::create(_filename);
	createSceneTree(_symbol, scene, scene->mRootNode, _options->assetLibrary());

#ifdef DEBUG_ASSIMP
	printNode(std::cout << "\n", _symbol, 0) << std::endl;
#endif // DEBUG_ASSIMP

	createLights(scene);
	createCameras(scene);
	createSkins(scene); // must come before createAnimations
	//createAnimations(scene);

#ifdef DEBUG_ASSIMP
	printNode(std::cout << "\n", _symbol, 0) << std::endl;
#endif // DEBUG_ASSIMP

#ifdef DEBUG_ASSIMP_DOT
	dotPrint("aiscene.dot", scene);
	dotPrint("minkoscene.dot", _symbol);
#endif // DEBUG_ASSIMP_DOT

	// file::Options::nodeFunction
	apply(_symbol, _options->nodeFunction());

	// file::Options::startAnimation
	auto animations = NodeSet::create(_symbol)
		->descendants(true)
		->where([](Node::Ptr n)
		{
			return n->hasComponent<component::AbstractAnimation>();
		});
	for (auto& n : animations->nodes())
		if (_options->startAnimation())
			n->component<component::AbstractAnimation>()->play();
		else
			n->component<component::AbstractAnimation>()->stop();

	if (_numDependencies == _numLoadedDependencies)
		finalize();

	disposeNodeMaps();
}

void
AbstractASSIMPParser::initImporter()
{
    if (_importer != nullptr)
        return;

    _importer = new Assimp::Importer();

#if (defined ASSIMP_BUILD_NO_IMPORTER_INSTANCIATION)
    provideLoaders(*_importer);
#endif // ! ASSIMP_BUILD_NO_IMPORTER_INSTANCIATION
}

void
AbstractASSIMPParser::createSceneTree(scene::Node::Ptr 				minkoNode,
									  const aiScene* 				scene,
									  aiNode* 						ainode,
									  std::shared_ptr<AssetLibrary> assets)
{
	minkoNode->addComponent(getTransformFromAssimp(ainode));

	// create surfaces for each node mesh
	for (uint j = 0; j < ainode->mNumMeshes; j++)
	{
	    aiMesh* aimesh = scene->mMeshes[ainode->mMeshes[j]];
		if (aimesh == nullptr)
			continue;

		_aiMeshToNode[aimesh] = minkoNode;
		createMeshSurface(minkoNode, scene, aimesh);
	}

	// traverse the node's children
	for (uint i = 0; i < ainode->mNumChildren; i++)
    {
		aiNode* aichild = ainode->mChildren[i];
		if (aichild == nullptr)
			continue;

        auto childName	= std::string(aichild->mName.data);
        auto childNode	= scene::Node::create(childName);

		_aiNodeToNode[aichild] = childNode;
		if (!childName.empty())
			_nameToNode[childName] = childNode;

        //Recursive call
		createSceneTree(childNode, scene, aichild, assets);

        minkoNode->addChild(childNode);
    }
}

void
AbstractASSIMPParser::apply(Node::Ptr node, const std::function<Node::Ptr(Node::Ptr)>& func)
{
	func(node);

	if (node)
		for (auto& n : node->children())
			apply(n, func);
}

Transform::Ptr
AbstractASSIMPParser::getTransformFromAssimp(aiNode* ainode)
{
	return Transform::create(convert(ainode->mTransformation));
}

Geometry::Ptr
AbstractASSIMPParser::createMeshGeometry(scene::Node::Ptr minkoNode, aiMesh* mesh, const std::string& meshName)
{
    auto existingGeometry = _assetLibrary->geometry(meshName);

    if (existingGeometry != nullptr)
        return existingGeometry;

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

	geometry = _options->geometryFunction()(meshName, geometry);

    _assetLibrary->geometry(meshName, geometry);

	return geometry;
}

std::string
AbstractASSIMPParser::getMaterialName(const std::string& materialName)
{
    static int currentId = 0;

    return materialName.empty() ? std::string("default" + std::to_string(currentId++)) : materialName;
}

std::string
AbstractASSIMPParser::getMeshName(const std::string& meshName)
{
    static int currentId = 0;

    return meshName.empty() ? std::string("default" + std::to_string(currentId++)) : meshName;
}

void
AbstractASSIMPParser::createMeshSurface(scene::Node::Ptr 	minkoNode,
										const aiScene* 		scene,
										aiMesh* 			mesh)
{
	if (mesh == nullptr)
		return;

	const auto	meshName	= getMeshName(std::string(mesh->mName.data));
	const auto	aiMat		= scene->mMaterials[mesh->mMaterialIndex];
	auto		geometry	= createMeshGeometry(minkoNode, mesh, meshName);
	auto		material	= createMaterial(aiMat);
	auto		effect		= chooseEffectByShadingMode(aiMat);

	if (effect)
	{
		minkoNode->addComponent(
			Surface::create(
				meshName,
				geometry,
				material,
				effect,
				"default"
			)
		);
	}
#ifdef DEBUG
	else
		std::cerr << "Failed to find suitable effect for mesh '" << meshName << "' and no default effect provided." << std::endl;
#endif // DEBUG
}

void
AbstractASSIMPParser::createCameras(const aiScene* scene)
{
	for (uint i = 0; i < scene->mNumCameras; ++i)
	{
		const auto	aiCamera	= scene->mCameras[i];
		const auto	aiPosition	= aiCamera->mPosition;
		const auto	aiLookAt	= aiCamera->mLookAt;
		const auto	aiUp		= aiCamera->mUp;
		auto		position	= math::normalize(math::vec3(aiPosition.x, aiPosition.y, aiPosition.z));
		auto		target		= math::normalize(math::vec3(aiPosition.x + aiLookAt.x, aiPosition.y + aiLookAt.y, aiPosition.z + aiLookAt.z));
		auto		up			= math::normalize(math::vec3(aiUp.x, aiUp.y, aiUp.z));

		const auto	cameraName	= std::string(aiCamera->mName.data);
		auto		cameraNode = scene::Node::create(cameraName + "_camera_" + std::to_string(i))
			->addComponent(PerspectiveCamera::create(
				aiCamera->mAspect,
				aiCamera->mHorizontalFOV * aiCamera->mAspect, // need the vertical FOV
				aiCamera->mClipPlaneNear,
				aiCamera->mClipPlaneFar
			))
			->addComponent(Transform::create(
                math::inverse(math::lookAt(position, target, up))
			));

		scene::Node::Ptr parentNode = !cameraName.empty()
			? findNode(cameraName)
			: nullptr;

		if (parentNode)
			parentNode->addChild(cameraNode);
	}
}

void
AbstractASSIMPParser::createLights(const aiScene* scene)
{
    for (uint i = 0; i < scene->mNumLights; i++)
    {
        const auto	aiLight		= scene->mLights[i];
		const auto	lightName	= std::string(aiLight->mName.data);

		if (aiLight->mType == aiLightSource_UNDEFINED)
		{
#ifdef DEBUG
			std::cerr << "The type of the '" << lightName << "' has not been properly recognized." << std::endl;
#endif // DEBUG
			continue;
		}

		auto lightNode	= findNode(lightName);

		if (lightNode == nullptr)
			continue;

		//// specular colors are ignored (diffuse colors are sent to discrete lights, ambient colors create ambient lights)
		//const aiColor3D& aiAmbientColor = aiLight->mColorAmbient;
		//if (!aiAmbientColor.IsBlack())
		//{
		//	auto ambientLight = AmbientLight::create()
		//		->ambient(1.0f)
		//		->color(Vector3::create(aiAmbientColor.r, aiAmbientColor.g, aiAmbientColor.b));

		//	lightNode->addComponent(ambientLight);
		//}

		const aiColor3D&	aiDiffuseColor	= aiLight->mColorDiffuse;
		const aiVector3D&	aiDirection		= aiLight->mDirection;
		const aiVector3D&	aiPosition		= aiLight->mPosition;

		if (aiDirection.Length() > 0.0f)
		{
			auto	direction	= math::vec3(aiDirection.x, aiDirection.y, aiDirection.z);
			auto	position	= math::vec3(aiPosition.x, aiPosition.y, aiPosition.z);

			auto	transform   = lightNode->component<Transform>();
			if (transform)
			{
				direction	= math::mat3(transform->matrix()) * direction;
				position	= math::vec3(math::mat4(transform->matrix()) * math::vec4(position, 1.f));
			}
			else
				lightNode->addComponent(Transform::create());

			auto lookAt		= math::normalize(position + direction);
            auto matrix     = math::lookAt(position, lookAt, math::vec3(0.f, 1.f, 0.f));
		}

		const float			diffuse			= 1.0f;
		const float			specular		= 1.0f;
        switch (aiLight->mType)
        {
            case aiLightSource_DIRECTIONAL:
				lightNode->addComponent(
					DirectionalLight::create(
						diffuse,
						specular
					)->color(math::vec3(aiDiffuseColor.r, aiDiffuseColor.g, aiDiffuseColor.b))
				);
                break;

            case aiLightSource_POINT:
				lightNode->addComponent(
					PointLight::create(
						diffuse,
						specular,
						aiLight->mAttenuationConstant,
						aiLight->mAttenuationLinear,
						aiLight->mAttenuationQuadratic
					)->color(math::vec3(aiDiffuseColor.r, aiDiffuseColor.g, aiDiffuseColor.b))
				);
                break;

            case aiLightSource_SPOT:
				lightNode->addComponent(
					SpotLight::create(
						aiLight->mAngleInnerCone,
						aiLight->mAngleOuterCone,
						diffuse,
						specular,
						aiLight->mAttenuationConstant,
						aiLight->mAttenuationLinear,
						aiLight->mAttenuationQuadratic
					)->color(math::vec3(aiDiffuseColor.r, aiDiffuseColor.g, aiDiffuseColor.b))
				);
                break;

            default:
                break;
        }
    }
}

scene::Node::Ptr
AbstractASSIMPParser::findNode(const std::string& name) const
{
	const auto foundNodeIt = _nameToNode.find(name);
	return foundNodeIt != _nameToNode.end()
		? foundNodeIt->second
		: nullptr;
}

void
AbstractASSIMPParser::parseDependencies(const std::string& 	filename,
										const aiScene*		scene)
{
	std::set<std::string>	loadedFilenames;
	aiString				path;

	_numDependencies = 0;

	for (unsigned int materialId = 0; materialId < scene->mNumMaterials; ++materialId)
	{
		const aiMaterial* aiMat = scene->mMaterials[materialId];

		for (auto& textureTypeAndName : _textureTypeToName)
		{
			const auto			textureType = static_cast<aiTextureType>(textureTypeAndName.first);
			const unsigned int	numTextures = aiMat->GetTextureCount(textureType);

			for (unsigned int textureId = 0; textureId < numTextures; ++textureId)
			{
				aiReturn texFound = aiMat->GetTexture(textureType, textureId, &path);

				if (texFound == AI_SUCCESS)
				{
					std::string filename(path.data);

					if (!filename.empty() &&
						loadedFilenames.find(filename) == loadedFilenames.end())
					{
#ifdef DEBUG
						std::cout << "ASSIMParser: loading texture '" << filename << "'..." << std::endl;
#endif
						loadedFilenames.insert(filename);
					}
				}
			}
		}
	}

	_numDependencies = loadedFilenames.size();

	for (auto& name : loadedFilenames)
		loadTexture(name, name, _options, scene);
}

void
AbstractASSIMPParser::finalize()
{
	_loaderErrorSlots.clear();
	_loaderCompleteSlots.clear();

	_assetLibrary->symbol(_filename, _symbol);

	complete()->execute(shared_from_this());
}

void
AbstractASSIMPParser::loadTexture(const std::string&	textureFilename,
								  const std::string&	assetName,
								  Options::Ptr			options,
								  const aiScene*		scene)
{
	auto loader = Loader::create();

    loader->options(options);

	_loaderCompleteSlots[loader] = loader->complete()->connect(std::bind(
		&AbstractASSIMPParser::textureCompleteHandler,
		std::dynamic_pointer_cast<AbstractASSIMPParser>(shared_from_this()),
		std::placeholders::_1,
        scene
	));

    _loaderErrorSlots[loader] = loader->error()->connect([=](Loader::Ptr textureLoader, const Error& error)
	{
		++_numLoadedDependencies;
#ifdef DEBUG
        std::cerr << "AbstractASSIMPParser: unable to find texture with filename '" << textureFilename << "'" << std::endl;
#endif // DEBUG

        _error->execute(shared_from_this(), Error("MissingTextureDependency", "Missing texture dependency: '" + textureFilename + "'"));
        
        if (_numDependencies == _numLoadedDependencies)
            allDependenciesLoaded(scene);
	});

	loader->queue(textureFilename, options)->load();
}

void
AbstractASSIMPParser::textureCompleteHandler(file::Loader::Ptr loader, const aiScene* scene)
{
#ifdef DEBUG
	std::cerr << "AbstractASSIMPParser: " << _numLoadedDependencies << "/" << _numDependencies << " texture(s) loaded" << std::endl;
#endif // DEBUG

	++_numLoadedDependencies;

	if (_numDependencies == _numLoadedDependencies)
		allDependenciesLoaded(scene);
}

void
AbstractASSIMPParser::resetParser()
{
	_numDependencies		= 0;
	_numLoadedDependencies	= 0;
	_filename.clear();
	_symbol	= nullptr;

	disposeNodeMaps();
}
void
AbstractASSIMPParser::disposeNodeMaps()
{
	_aiNodeToNode.clear();
	_aiMeshToNode.clear();
	_nameToNode.clear();
	_nameToAnimMatrices.clear();
	_alreadyAnimatedNodes.clear();
}

unsigned int
AbstractASSIMPParser::getSkinNumFrames(const aiMesh* aimesh) const
{
	assert(aimesh && _aiMeshToNode.count(aimesh) > 0);
	const auto	minkoMesh	= _aiMeshToNode.find(aimesh)->second;
	const auto	meshNode	= minkoMesh->parent();
	assert(meshNode);

	unsigned int numFrames = 0;

	for (unsigned int boneId = 0; boneId < aimesh->mNumBones; ++boneId)
	{
		auto currentNode = findNode(aimesh->mBones[boneId]->mName.data);
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

	return numFrames;
}

void
AbstractASSIMPParser::createSkins(const aiScene* aiscene)
{
	if (_options->skinningFramerate() == 0)
		return;

	// resample all animations with the specified temporal precision
	// and store them in the _nameToAnimMatrices map.
	sampleAnimations(aiscene);

	// add a Skinning component to all animated mesh
	for (unsigned int meshId = 0; meshId < aiscene->mNumMeshes; ++meshId)
		createSkin(aiscene->mMeshes[meshId]);
}

void
AbstractASSIMPParser::createSkin(const aiMesh* aimesh)
{
	if (aimesh == nullptr || aimesh->mNumBones == 0)
		return;

	const auto	meshName	= std::string(aimesh->mName.data);
	if (_aiMeshToNode.count(aimesh) == 0)
		return;


	auto supposedSkeletonRoot = getSkeletonRoot(aimesh);

	auto		meshNode	= _aiMeshToNode.find(aimesh)->second;
	const uint	numBones	= aimesh->mNumBones;
	const uint	numFrames	= getSkinNumFrames(aimesh);
	if (numFrames == 0)
	{
#ifdef DEBUG
		std::cerr << "Failed to flatten skinning information. Most likely involved nodes do not share a common animation." << std::endl;
#endif // DEBUG

		return;
	}
	const uint	duration		= uint(floorf(1e+3f * numFrames / (float)_options->skinningFramerate())); // in milliseconds
	auto	skin				= Skin::create(numBones, duration, numFrames);
	auto	skeletonRoot		= getSkeletonRoot(aimesh); //findNode("ALL");
	auto	boneTransforms		= std::vector<std::vector<float>>(numBones, std::vector<float>(numFrames * 16, 0.0f));
	auto	modelToRootMatrices	= std::vector<math::mat4>(numFrames);

	std::vector<scene::Node::Ptr> boneNodes;

	for (auto& m : modelToRootMatrices)
		m = math::mat4();

	for (uint boneId = 0; boneId < numBones; ++boneId)
	{
		
		const auto bone = createBone(aimesh->mBones[boneId]);
		const auto boneName = std::string(aimesh->mBones[boneId]->mName.data);
		auto node = _nameToNode.find(boneName)->second;
		boneNodes.push_back(node);
		if (!bone)
			return;

		const auto boneOffsetMatrix	= bone->offsetMatrix();

		precomputeModelToRootMatrices(node, skeletonRoot, modelToRootMatrices);
		skin->bone(boneId, bone);

		for (uint frameId = 0; frameId < numFrames; ++frameId)
		{
            modelToRootMatrices[frameId] = boneOffsetMatrix * modelToRootMatrices[frameId];
			skin->matrix(frameId, boneId, modelToRootMatrices[frameId]);
		}
	}

	// also find all bone children that must also be animated and synchronized with the
	// skinning component.
	std::set<Node::Ptr>			slaves;
	std::vector<Animation::Ptr>	slaveAnimations;

	for (uint boneId = 0; boneId < numBones; ++boneId)
	{
		auto childrenWithSurface = NodeSet::create(boneNodes[boneId])
		->descendants(true)
		->where([](Node::Ptr n)
		{
			return n->hasComponent<Surface>();
		});

		slaves.insert(childrenWithSurface->nodes().begin(), childrenWithSurface->nodes().end());
	}

	auto timetable = std::vector<uint>(numFrames, 0);
	for (uint i = 0; i < numFrames; ++i)
		timetable[i] = uint(floorf(i * duration / float(numFrames - 1)));

	slaveAnimations.reserve(slaves.size());
	for (auto& n : slaves)
	{
		auto matrices = std::vector<math::mat4>(numFrames);
		for (auto& m : matrices)
			m = math::mat4();

		precomputeModelToRootMatrices(n, skeletonRoot, matrices);

		auto timeline	= animation::Matrix4x4Timeline::create(PNAME_TRANSFORM, duration, timetable, matrices);
		auto animation	= Animation::create(std::vector<animation::AbstractTimeline::Ptr>(1, timeline));

		n->addComponent(animation);
		slaveAnimations.push_back(animation);
		_alreadyAnimatedNodes.insert(n);
	}

	for (auto& n : slaves) // FIXME
	{
		if (n->parent())
			n->parent()->removeChild(n);
		skeletonRoot->addChild(n);
	}

	// add skinning component to mesh
	meshNode->addComponent(Skinning::create(
		skin->reorganizeByVertices()->transposeMatrices(),
		_options->skinningMethod(),
		_assetLibrary->context(),
		skeletonRoot
	));

	meshNode->addComponent(MasterAnimation::create());

	auto irrelevantTransforms = NodeSet::create(skeletonRoot)
		->descendants(false)
		->where([](Node::Ptr n){
			return n->hasComponent<Transform>() && !n->hasComponent<Animation>() && !n->hasComponent<Surface>();
		});

	for (auto& n : irrelevantTransforms->nodes())
		n->removeComponent(n->component<Transform>());
}

Node::Ptr
AbstractASSIMPParser::getSkeletonRoot(const aiMesh* aimesh) const
{
	Node::Ptr	skeletonRoot	= nullptr;
	auto		boneAncestor	= getBoneCommonAncestor(aimesh);
	auto		currentNode		= boneAncestor;

	while (true)
	{
		if (currentNode == nullptr)
			break;

		if (_nameToAnimMatrices.count(currentNode->name()) > 0)
			skeletonRoot = currentNode;

		currentNode = currentNode->parent();
	}

	return skeletonRoot
		? (skeletonRoot->parent() ? skeletonRoot->parent() : _symbol)
		: boneAncestor;
}

Node::Ptr
AbstractASSIMPParser::getBoneCommonAncestor(const aiMesh* aimesh) const
{
	if (aimesh && aimesh->mNumBones > 0)
	{
		std::vector< std::vector<Node::Ptr> > bonePath;
		bonePath.reserve(aimesh->mNumBones);

		// compute the common ancestor of all bones influencing the specified mesh
		uint minDepth = UINT_MAX;
		for (uint boneId = 0; boneId < aimesh->mNumBones; ++boneId)
		{
			auto boneNode = findNode(aimesh->mBones[boneId]->mName.data);
			if (boneNode == nullptr)
				continue;

			bonePath.push_back(std::vector<Node::Ptr>());

			auto currentNode = boneNode;
			do
			{
				if (currentNode == nullptr)
					break;

				bonePath.back().push_back(currentNode);
				currentNode = currentNode->parent();
			}
			while(true);

			std::reverse(bonePath.back().begin(), bonePath.back().end());

			if (bonePath.back().size() < minDepth)
				minDepth = bonePath.back().size();
		}

		if (bonePath.empty())
			return _symbol;

		for (uint d = 0; d < minDepth; ++d)
		{
			auto node		= bonePath.front()[d];
			bool isCommon	= true;
			for (uint boneId = 1; boneId < aimesh->mNumBones && isCommon; ++boneId)
				if (bonePath[boneId][d] != node)
				{
					isCommon = false;
					break;
				}

			if (!isCommon)
			{
				if (d > 0)
					return bonePath.front()[d-1];
				else
					return _symbol;
			}
		}
	}
	return _symbol;
}

void
AbstractASSIMPParser::precomputeModelToRootMatrices(Node::Ptr			node,
											Node::Ptr					root,
											std::vector<math::mat4>&	modelToRootMatrices) const
{
	assert(node && !modelToRootMatrices.empty());

	typedef std::tuple<Node::Ptr, const std::vector<math::mat4>*, const math::mat4*> NodeTransformInfo;

	// precompute the sequence of local-to-parent transformations from node to root
	std::list<NodeTransformInfo> transformsUpToRoot;
	auto currentNode = node;
	do
	{
		if (currentNode == nullptr)
			break;

		const std::string& currentName = currentNode->name();

		transformsUpToRoot.push_back(NodeTransformInfo());

		std::get<0>(transformsUpToRoot.back()) = currentNode;
		std::get<1>(transformsUpToRoot.back()) = nullptr;
		std::get<2>(transformsUpToRoot.back()) = nullptr;

		const auto foundAnimMatricesIt = _nameToAnimMatrices.find(currentName);
		if (foundAnimMatricesIt != _nameToAnimMatrices.end())
			std::get<1>(transformsUpToRoot.back()) = &(foundAnimMatricesIt->second);
		else if (currentNode->hasComponent<Transform>())
			std::get<2>(transformsUpToRoot.back()) = &currentNode->component<Transform>()->matrix();

		currentNode = currentNode->parent();
	}
	while(currentNode != root); // the transform of the root is not accounted for!

	// collapse transform from node to root for each frame of the animation
	const uint numFrames = modelToRootMatrices.size();

	for (uint frameId = 0; frameId < numFrames; ++frameId)
	{
		auto& modelToRoot = modelToRootMatrices[frameId]; // warning: not a copy

        modelToRoot = math::mat4();
		
        for (auto& trfInfo : transformsUpToRoot)
		{
			const auto animMatrices	= std::get<1>(trfInfo);
			const auto matrix		= std::get<2>(trfInfo);

			if (animMatrices)
                modelToRoot = modelToRoot * (*animMatrices)[std::min (int(frameId), int(animMatrices->size() - 1))];
			else if (matrix)
				modelToRoot = modelToRoot * *matrix;
		}
	}
}

Bone::Ptr
AbstractASSIMPParser::createBone(const aiBone* aibone) const
{
	const auto boneName = std::string(aibone->mName.data);
	if (aibone == nullptr || _nameToNode.count(boneName) == 0)
		return nullptr;

	auto offsetMatrix	= convert(aibone->mOffsetMatrix);

	std::vector<unsigned short> boneVertexIds		(aibone->mNumWeights, 0);
	std::vector<float>			boneVertexWeights	(aibone->mNumWeights, 0.0f);

	for (unsigned int i = 0; i < aibone->mNumWeights; ++i)
	{
		boneVertexIds[i]		= (unsigned short)aibone->mWeights[i].mVertexId;
		boneVertexWeights[i]	= aibone->mWeights[i].mWeight;
	}

	return Bone::create(_nameToNode.at(boneName), offsetMatrix, boneVertexIds, boneVertexWeights);
}

void
AbstractASSIMPParser::sampleAnimations(const aiScene* scene)
{
	_nameToAnimMatrices.clear();

	if (scene == nullptr)
		return;

	for (unsigned int animId = 0; animId < scene->mNumAnimations; ++animId)
		sampleAnimation(scene->mAnimations[animId]);
}

void
AbstractASSIMPParser::sampleAnimation(const aiAnimation* animation)
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
		const auto	nodeAnimation	= animation->mChannels[channelId];
		const auto	nodeName		= std::string(nodeAnimation->mNodeName.data);
		// According to the ASSIMP documentation, animated nodes should come with existing, unique names.

		if (!nodeName.empty())
		{
			_nameToAnimMatrices[nodeName] = std::vector<math::mat4>();
			sample(nodeAnimation, sampleTimes, _nameToAnimMatrices[nodeName]);
		}
	}
}

/*static*/
void
AbstractASSIMPParser::sample(const aiNodeAnim*				nodeAnimation,
					         const std::vector<float>&		times,
					         std::vector<math::mat4>&	    matrices)
{
	assert(nodeAnimation);

	static auto position		= math::vec3();
	static auto scaling			= math::vec3();
	static auto rotation		= math::quat();
	static auto rotationMatrix	= math::mat4();

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
		position = sample(nodeAnimation->mPositionKeys, positionKeyTimeFactors, time);

		// sample rotation from keys
		rotation = sample(nodeAnimation->mRotationKeys, rotationKeyTimeFactors, time);
        rotation = math::normalize(rotation);

		if (math::length(rotation) == 0.f)
            rotationMatrix = math::mat4();
		else
            rotationMatrix = math::mat4_cast(rotation);

		// sample scaling from keys
		scaling = sample(nodeAnimation->mScalingKeys, scalingKeyTimeFactors, time);

		// recompose the interpolated matrix at the specified frame
        matrices[frameId] = math::mat4(
				scaling.x * rotationMatrix[0][0], scaling.y * rotationMatrix[0][1], scaling.z * rotationMatrix[0][2],  position.x,
				scaling.x * rotationMatrix[1][0], scaling.y * rotationMatrix[1][1], scaling.z * rotationMatrix[1][2],  position.y,
				scaling.x * rotationMatrix[2][0], scaling.y * rotationMatrix[2][1], scaling.z * rotationMatrix[2][2], position.z,
				0.0, 0.0, 0.0, 1.0f
			);

#ifdef DEBUG
		// std::cout << "\tframeID = " << frameId << "\ttime = " << time << "\nM = " << matrices[frameId]->toString() << std::endl;
#endif // DEBUG
	}
}

/*static*/
math::vec3
AbstractASSIMPParser::sample(const aiVectorKey*	        keys,
					         const std::vector<float>&	keyTimeFactors,
					         float						time)
{
    auto output = math::vec3();
	const unsigned int	numKeys	= keyTimeFactors.size();
	const unsigned int	id		= getIndexForTime(numKeys, keys, time);
	const aiVector3D&	value0	= keys[id].mValue;

	if (id == numKeys - 1)
		output = math::vec3(value0.x, value0.y, value0.z);
	else
	{
		const float			w1		= (time - (float)keys[id].mTime) * keyTimeFactors[id];
		const float			w0		= 1.0f - w1;
		const aiVector3D&	value1	= keys[id+1].mValue;

		output = math::vec3(
			w0 * value0.x + w1 * value1.x,
			w0 * value0.y + w1 * value1.y,
			w0 * value0.z + w1 * value1.z
		);
	}

	return output;
}

/*static*/
math::quat
AbstractASSIMPParser::sample(const aiQuatKey*			keys,
					         const std::vector<float>&	keyTimeFactors,
					         float						time)
{
    auto output = math::quat();
	const unsigned int	numKeys	= keyTimeFactors.size();
	const unsigned int	id		= getIndexForTime(numKeys, keys, time);
	const aiQuaternion&	value0	= keys[id].mValue;

	if (id == numKeys - 1)
		output = math::quat(value0.x, value0.y, value0.z, value0.w);
	else
	{
		const float			w1		= (time - (float)keys[id].mTime) * keyTimeFactors[id];
		const float			w0		= 1.0f - w1;
		const aiQuaternion&	value1	= keys[id+1].mValue;

		aiQuaternion interp;
		aiQuaternion::Interpolate(interp, value0, value1, w1);

		output = convert(interp);
	}

	return output;
}

template<class AiKey>
/*static*/
void
AbstractASSIMPParser::computeTimeFactors(unsigned int			numKeys,
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
AbstractASSIMPParser::getIndexForTime(unsigned int	numKeys,
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
math::quat
AbstractASSIMPParser::convert(const aiQuaternion& quaternion)
{
	return math::quat(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

/*static*/
math::mat4
AbstractASSIMPParser::convert(const aiMatrix4x4& matrix)
{
    // Assimp aiMatrix4x4 are row-major meanwhile
    // glm mat4 are column-major (so are OpenGL matrices)

    return math::mat4(
        matrix.a1, matrix.b1, matrix.c1, matrix.d1,
        matrix.a2, matrix.b2, matrix.c2, matrix.d2,
        matrix.a3, matrix.b3, matrix.c3, matrix.d3,
        matrix.a4, matrix.b4, matrix.c4, matrix.d4
    );
}

/*static*/
math::mat4
AbstractASSIMPParser::convert(const aiVector3D&		scaling,
					          const aiQuaternion&	quaternion,
					          const aiVector3D&		translation)
{
    auto output = math::mat4();

	static auto rotation		= math::quat();
	static auto rotationMatrix	= math::mat4();

    rotationMatrix              = math::mat4_cast(convert(quaternion));
	const auto& rotationData	= rotationMatrix;

    return math::mat4(
		scaling.x * rotationData[0][0], scaling.y * rotationData[0][1], scaling.z * rotationData[0][2],  translation.x,
		scaling.x * rotationData[1][0], scaling.y * rotationData[1][1], scaling.z * rotationData[1][2],  translation.y,
		scaling.x * rotationData[2][0], scaling.y * rotationData[2][1], scaling.z * rotationData[2][2], translation.z,
		0.f, 0.f, 0.f, 1.f
	);
}

material::Material::Ptr
AbstractASSIMPParser::createMaterial(const aiMaterial* aiMat)
{
    auto material = chooseMaterialByShadingMode(aiMat);

	if (aiMat == nullptr)
		return material;

    auto materialName = std::string();

    aiString rawMaterialName;
    if (aiMat->Get(AI_MATKEY_NAME, rawMaterialName) == AI_SUCCESS)
    {
        materialName = rawMaterialName.data;
    }

    materialName = getMaterialName(materialName);

    auto existingMaterial = _assetLibrary->material(materialName);

    if (existingMaterial != nullptr)
        return existingMaterial;

	material->data()->set("blendMode",			getBlendingMode(aiMat));
	material->data()->set("triangleCulling",	getTriangleCulling(aiMat));
	material->data()->set("wireframe",			getWireframe(aiMat)); // bool

	float opacity		= setScalarProperty(material, "opacity",			aiMat, AI_MATKEY_OPACITY,				1.0f);
	float shininess		= setScalarProperty(material, "shininess",			aiMat, AI_MATKEY_SHININESS,				0.0f);
	float reflectivity	= setScalarProperty(material, "reflectivity",		aiMat, AI_MATKEY_REFLECTIVITY,			1.0f);
	float shininessStr	= setScalarProperty(material, "shininessStrength",	aiMat, AI_MATKEY_SHININESS_STRENGTH,	1.0f);
	float refractiveIdx	= setScalarProperty(material, "refractiveIndex",	aiMat, AI_MATKEY_REFRACTI,				1.0f);
	float bumpScaling	= setScalarProperty(material, "bumpScaling",		aiMat, AI_MATKEY_BUMPSCALING,			1.0f);

	auto diffuseColor		= setColorProperty(material, "diffuseColor",		aiMat, AI_MATKEY_COLOR_DIFFUSE);
	auto specularColor		= setColorProperty(material, "specularColor",		aiMat, AI_MATKEY_COLOR_SPECULAR);
	auto ambientColor		= setColorProperty(material, "ambientColor",		aiMat, AI_MATKEY_COLOR_AMBIENT);
	auto emissiveColor		= setColorProperty(material, "emissiveColor",		aiMat, AI_MATKEY_COLOR_EMISSIVE);
	auto reflectiveColor	= setColorProperty(material, "reflectiveColor",		aiMat, AI_MATKEY_COLOR_REFLECTIVE);
	auto transparentColor	= setColorProperty(material, "transparentColor",	aiMat, AI_MATKEY_COLOR_TRANSPARENT);

	if (shininess < 1.0f)
		// Gouraud-like shading (-> no specular)
		specularColor.w = 0.f;

	if (opacity < 1.0f)
	{
		diffuseColor.w = opacity;
		specularColor.w = opacity;
		ambientColor.w = opacity;
		emissiveColor.w = opacity;
		reflectiveColor.w = opacity;
		transparentColor.w = opacity;

		material->data()->set("priority",	render::Priority::TRANSPARENT);
		material->data()->set("zSort",		true);
	}
	else
	{
		material->data()->set("priority",	render::Priority::OPAQUE);
		material->data()->set("zSort",		false);
	}

	for (auto& textureTypeAndName : _textureTypeToName)
	{
		const auto			textureType	= static_cast<aiTextureType>(textureTypeAndName.first);
		const std::string&	textureName	= textureTypeAndName.second;

		const unsigned int	numTextures	= aiMat->GetTextureCount(textureType);
		if (numTextures == 0)
			continue;

		aiString path;
		if (aiMat->GetTexture(textureType, 0, &path) == AI_SUCCESS)
		{
			render::AbstractTexture::Ptr texture = _assetLibrary->texture(std::string(path.data));

			if (texture)
				material->data()->set(textureName, texture->sampler());
		}
	}

    auto processedMaterial = _options->materialFunction()(materialName, material);

    _assetLibrary->material(materialName, processedMaterial);

    return processedMaterial;
}

material::Material::Ptr
AbstractASSIMPParser::chooseMaterialByShadingMode(const aiMaterial* aiMat) const
{
	if (aiMat == nullptr || _options->material())
		return material::Material::create(_options->material());

	int shadingMode;
	unsigned int max;
	if (aiMat->Get(AI_MATKEY_SHADING_MODEL, &shadingMode, &max) == AI_SUCCESS)
	{
		switch(static_cast<aiShadingMode>(shadingMode))
		{
			case aiShadingMode_Flat:
				return std::static_pointer_cast<material::Material>(material::BasicMaterial::create());

			case aiShadingMode_Phong:
			case aiShadingMode_Blinn:
			case aiShadingMode_CookTorrance:
			case aiShadingMode_Fresnel:
			case aiShadingMode_Toon:
			case aiShadingMode_Gouraud:
			case aiShadingMode_OrenNayar:
			case aiShadingMode_Minnaert:
				return std::static_pointer_cast<material::Material>(material::PhongMaterial::create());

			case aiShadingMode_NoShading:
			default:
				return material::Material::create(_options->material());
		}
	}
	else
		return material::Material::create(_options->material());
}

#ifdef NDEBUG
# pragma optimize("", off)
#endif // NDEBUG

render::Effect::Ptr
AbstractASSIMPParser::chooseEffectByShadingMode(const aiMaterial* aiMat) const
{
	render::Effect::Ptr effect = _options->effect();

	if (effect == nullptr && aiMat)
	{
		int shadingMode;
        unsigned int max = 1;
		if (aiMat->Get(AI_MATKEY_SHADING_MODEL, &shadingMode, &max) == AI_SUCCESS)
		{
			switch(static_cast<aiShadingMode>(shadingMode))
			{
				case aiShadingMode_Flat:
				case aiShadingMode_Gouraud:
				case aiShadingMode_Toon:
				case aiShadingMode_OrenNayar:
				case aiShadingMode_Minnaert:
					if (_assetLibrary->effect("basic"))
						effect = _assetLibrary->effect("basic");
#ifdef DEBUG
					else
						std::cerr << "Basic effect not available in the asset library." << std::endl;
#endif // DEBUG
					break;

				case aiShadingMode_Phong:
				case aiShadingMode_Blinn:
				case aiShadingMode_CookTorrance:
				case aiShadingMode_Fresnel:
					if (_assetLibrary->effect("phong"))
						effect = _assetLibrary->effect("phong");
#ifdef DEBUG
					else
						std::cerr << "Phong effect not available in the asset library." << std::endl;
#endif // DEBUG
					break;

				case aiShadingMode_NoShading:
				default:
					break;
			}
		}
	}

	// apply effect function
	return _options->effectFunction()(effect);
}

#ifdef NDEBUG
# pragma optimize("", on)
#endif // NDEBUG

render::Blending::Mode
AbstractASSIMPParser::getBlendingMode(const aiMaterial* aiMat) const
{
	int blendMode;
	unsigned int max;
	if (aiMat && aiMat->Get(AI_MATKEY_BLEND_FUNC, &blendMode, &max) == AI_SUCCESS)
	{
		switch (static_cast<aiBlendMode>(blendMode))
		{
			case aiBlendMode_Default: // src * alpha + dst * (1 - alpha)
				return render::Blending::Mode::ALPHA;
			case aiBlendMode_Additive:
				return render::Blending::Mode::ADDITIVE;
			default:
				return render::Blending::Mode::DEFAULT;
		}
	}
	else
		return render::Blending::Mode::ALPHA;
}

render::TriangleCulling
AbstractASSIMPParser::getTriangleCulling(const aiMaterial* aiMat) const
{
	int twoSided;
	unsigned int max;
	if (aiMat && aiMat->Get(AI_MATKEY_TWOSIDED, &twoSided, &max) == AI_SUCCESS)
	{
		return twoSided == 0
			? render::TriangleCulling::NONE
			: render::TriangleCulling::BACK;
	}
	else
		return render::TriangleCulling::BACK;
}

bool
AbstractASSIMPParser::getWireframe(const aiMaterial* aiMat) const
{
	int wireframe;
	unsigned int max;

	return (aiMat && aiMat->Get(AI_MATKEY_TWOSIDED, &wireframe, &max) == AI_SUCCESS)
		? wireframe != 0
		: false;
}

math::vec4
AbstractASSIMPParser::setColorProperty(material::Material::Ptr	material,
							           const std::string&		propertyName,
							           const aiMaterial*		aiMat,
							           const char*				aiMatKeyName,
							           unsigned int				aiType,
							           unsigned int				aiIndex,
							           const math::vec4&		defaultValue)
{
	assert(material && aiMat);

	aiColor4D color;
	color.r = defaultValue.x;
	color.g = defaultValue.y;
	color.b = defaultValue.z;
	color.a = defaultValue.w;

	auto ret = aiMat->Get(aiMatKeyName, aiType, aiIndex, color);
	material->data()->set(propertyName, math::vec4(color.r, color.g, color.b, color.a));

	return material->data()->get<math::vec4>(propertyName);
}

float
AbstractASSIMPParser::setScalarProperty(material::Material::Ptr	material,
							    const std::string&		propertyName,
							    const aiMaterial*		aiMat,
							    const char*				aiMatKeyName,
							    unsigned int			aiType,
							    unsigned int			aiIndex,
								float					defaultValue)
{
	assert(material && aiMat);

	float scalar = defaultValue;

	auto ret = aiMat->Get(aiMatKeyName, aiType, aiIndex, scalar);
	material->data()->set(propertyName, scalar);

	return material->data()->get<float>(propertyName);
	//if (aiMat->Get(aiMatKeyName, aiType, aiIndex, scalar) == AI_SUCCESS)
	//	material->set(propertyName, scalar);
}

void
AbstractASSIMPParser::createAnimations(const aiScene* scene, bool interpolate)
{
	std::unordered_map<Node::Ptr, std::vector<animation::AbstractTimeline::Ptr>> nodeToTimelines;

	for (uint i = 0; i < scene->mNumAnimations; ++i)
	{
		const auto animation = scene->mAnimations[i];
		if (animation->mTicksPerSecond < 1e-6)
			continue;

		const uint duration = (uint)floor(1e+3 * animation->mDuration / animation->mTicksPerSecond); // in milliseconds

		for (uint j = 0; j < animation->mNumChannels; ++j)
		{
			const auto	channel	= animation->mChannels[j];
			auto		node	= findNode(channel->mNodeName.data);
			if (node == nullptr || _alreadyAnimatedNodes.count(node) > 0)
				continue;

			const uint	numKeys	= channel->mNumPositionKeys;
			// currently assume all keys are synchronized
			assert(channel->mNumRotationKeys == numKeys &&
				   channel->mNumScalingKeys == numKeys);

			std::vector<uint>			timetable	(numKeys, 0);
			std::vector<math::mat4>	    matrices	(numKeys, math::mat4());

			for (uint k = 0; k < numKeys; ++k)
			{
				const double keyTime = channel->mPositionKeys[k].mTime;
				 // currently assume all keys are synchronized
				assert(abs(keyTime - channel->mRotationKeys[k].mTime) < 1e-6 &&
					   abs(keyTime - channel->mScalingKeys[k].mTime) < 1e-6);

				const int time	= std::max(0, std::min(int(duration), (int)floor(1e+3 * keyTime)));

				timetable[k]	= time;
				matrices[k]		= convert(
					channel->mScalingKeys[k].mValue,
					channel->mRotationKeys[k].mValue,
					channel->mPositionKeys[k].mValue
				);

			}

			nodeToTimelines[node].push_back(animation::Matrix4x4Timeline::create(
				PNAME_TRANSFORM,
				duration,
				timetable,
				matrices,
				interpolate
			));
		}

		// unroll the node to matrix timeline
		for (auto& nodeAndTimelines : nodeToTimelines)
			nodeAndTimelines.first->addComponent(Animation::create(nodeAndTimelines.second));
	}
}

#include "ASSIMPParserDebug.hpp"
