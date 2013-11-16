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
	_numLoadedDependencies(0)
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
	int pos = resolvedFilename.find_last_of("/");

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
	createSceneTree(_symbol, scene, scene->mRootNode);
	
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
    auto provider = material::Material::create();
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    
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
    aiMatrix4x4 aiTransform = ainode->mTransformation;
    Transform::Ptr result = Transform::create();

    result->transform()->initialize(
		aiTransform.a1, aiTransform.a2, aiTransform.a3, aiTransform.a4,
        aiTransform.b1, aiTransform.b2, aiTransform.b3, aiTransform.b4,
        aiTransform.c1, aiTransform.c2, aiTransform.c3, aiTransform.c4,
        aiTransform.d1, aiTransform.d2, aiTransform.d3, aiTransform.d4
	);
    
    return result;
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
