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


using namespace minko;
using namespace minko::component;
using namespace minko::math;
using namespace minko::file;

void
ASSIMPParser::parse(const std::string&				filename,
				 const std::string&                 resolvedFilename,
                 std::shared_ptr<Options>           options,
				 const std::vector<unsigned char>&	data,
				 std::shared_ptr<AssetLibrary>	    assetLibrary)
{
    _filename = filename;
	_assetLibrary = assetLibrary;
    
    //Init the assimp scene
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
		filename.c_str(),
		aiProcess_CalcTangentSpace
			| aiProcess_Triangulate
			| aiProcess_JoinIdenticalVertices
			| aiProcess_FlipUVs
			| aiProcess_SortByPType
	);//aiProcessPreset_TargetRealtime_Fast has the configs we'll need
    if (!scene)
    {
        std::cout << importer.GetErrorString() << std::endl;
        throw;
    }
    else
        _aiscene = scene;
    
    parseDependencies(resolvedFilename, options, _dependencies);
    
	if (_numDependencies == _numLoadedDependencies)
		finalize();
}

void
ASSIMPParser::createSceneTree(scene::Node::Ptr minkoNode, aiNode* ainode, component::SceneManager::Ptr sceneManager)
{
    for (int i = 0; i < ainode->mNumChildren; i++)
    {
        auto childName = std::string(ainode->mChildren[i]->mName.data);
        auto child = scene::Node::create(childName);
        
        child->addComponent(getTransformFromAssimp(ainode->mChildren[i]));
        
        minkoNode->addChild(child);
        
        //Recursive call
        createSceneTree(child, ainode->mChildren[i], sceneManager);
    }
    
    for (int j = 0; j < ainode->mNumMeshes; j++)
    {
        aiMesh *mesh = _aiscene->mMeshes[ainode->mMeshes[j]];
        createMeshGeometry(minkoNode, mesh, sceneManager);
        createMeshSurface(minkoNode, mesh, sceneManager);
    }
}

void
ASSIMPParser::createMeshGeometry(scene::Node::Ptr minkoNode ,aiMesh* mesh, SceneManager::Ptr sceneManager)
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
    
    vertexArray-=numVertex*vertexSize;
    
    for (unsigned short l = 0; l < numVertex; l++)
        indiceArray.push_back(l);
    
    for (int k = 0; k < numVertex*vertexSize; k++)
        vertexVector.push_back(vertexArray[k]);
    
    auto vBuffer = render::VertexBuffer::create(sceneManager->assets()->context(), vertexVector);
    auto iBuffer = render::IndexBuffer::create(sceneManager->assets()->context(), indiceArray);
    if (mesh->HasPositions())
        vBuffer->addAttribute("position", 3, 0);
    if (mesh->HasNormals())
        vBuffer->addAttribute("normal", 3, 3);
    if (mesh->GetNumUVChannels() > 0)
        vBuffer->addAttribute("uv", 2, 6);
    
    std::string meshGeometryName = std::string(mesh->mName.data);
    sceneManager->assets()->geometry(meshGeometryName, geometry::Geometry::create());
    sceneManager->assets()->geometry(meshGeometryName)->addVertexBuffer(vBuffer);
    sceneManager->assets()->geometry(meshGeometryName)->indices(iBuffer);
}

void
ASSIMPParser::createMeshSurface(scene::Node::Ptr minkoNode, aiMesh* mesh, SceneManager::Ptr sceneManager)
{
    auto provider = material::Material::create();
    aiMaterial* material = _aiscene->mMaterials[mesh->mMaterialIndex];
    
    //Diffuse color
    aiColor4D diffuse;
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse))
        provider->set("diffuseColor", Vector4::create(diffuse.r, diffuse.g, diffuse.b, diffuse.a));
    
    //Ambient color
    aiColor4D ambient;
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, ambient))
        provider->set("ambientColor", Vector4::create(ambient.r, ambient.g, ambient.b, ambient.a));
    
    //Specular color
    aiColor4D specular;
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, specular))
        provider->set("specularColor", Vector4::create(specular.r, specular.g, specular.b, specular.a));
    
    //Emissive color
    aiColor4D emissive;
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive))
        provider->set("emissiveColor", Vector4::create(emissive.r, emissive.g, emissive.b, emissive.a));
    
    //Shininess
    float shininess;
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, shininess))
        provider->set("shininess", shininess);
    
    int texIndex = 0;
    aiString path;
    aiReturn texFound = material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
    
    
    while (AI_SUCCESS == texFound)
    {
        auto texturePath = std::string(path.data);
        auto textPath  = std::string("texture/ducktexture.png");
        std::cout << "Path of texture in .DAE : " << texturePath << std::endl;
        //sceneManager->assets()->queue(textPath);
        provider->set("diffuseMap", sceneManager->assets()->texture(textPath));
        texIndex++;
        texFound = material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
    }
    
    minkoNode->addComponent(Surface::create(
                                            sceneManager->assets()->geometry(std::string(mesh->mName.data)),
                                            provider,
                                            sceneManager->assets()->effect("effect/Basic.effect")
                                            ));
}

void
ASSIMPParser::createLights(scene::Node::Ptr minkoRoot)
{
    for (int i = 0; i < _aiscene->mNumLights; i++)
    {
        auto ailight = _aiscene->mLights[i];
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
ASSIMPParser::queueAssimpTexture(SceneManager::Ptr sceneManager)
{
    for (int i = 0; i < _aiscene->mNumMeshes; i++)
    {
        aiMesh* mesh = _aiscene->mMeshes[i];
        
        aiMaterial* material = _aiscene->mMaterials[mesh->mMaterialIndex];
        
        int texIndex = 0;
        aiString path;
        aiReturn texFound = material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
        
        
        while (AI_SUCCESS == texFound)
        {
            auto textPath  = std::string(path.data);//std::string("texture/");
            std::cout << "texture path from assimp: " << textPath << std::endl;
            sceneManager->assets()->queue(textPath);
            texIndex++;
            texFound = material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
        }
    }
}