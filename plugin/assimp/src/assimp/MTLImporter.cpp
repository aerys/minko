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

#ifndef ASSIMP_BUILD_NO_MTL_IMPORTER

#include "DefaultIOSystem.h"
#include "ObjFileImporter.h"
#include "ObjFileParser.h"
#include "ObjFileData.h"
#include <boost/scoped_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/ai_assert.h>
#include <assimp/DefaultLogger.hpp>
#include "assimp/MTLImporter.h"
#include "ObjFileMtlImporter.h"

static const aiImporterDesc desc = {
    "Wavefront Object Material Template Library Importer",
    "",
    "",
    "surfaces not supported",
    aiImporterFlags_SupportTextFlavour,
    0,
    0,
    0,
    0,
    "mtl"
};

namespace Assimp    {

using namespace std;

// ------------------------------------------------------------------------------------------------
//  Default constructor
MTLImporter::MTLImporter() :
    m_Buffer(), 
    m_pRootObject( NULL ),
    m_strAbsPath( "" )
{
    DefaultIOSystem io;
    m_strAbsPath = io.getOsSeparator();
}

// ------------------------------------------------------------------------------------------------
//  Destructor.
MTLImporter::~MTLImporter()
{
    delete m_pRootObject;
    m_pRootObject = NULL;
}

// ------------------------------------------------------------------------------------------------
//  Returns true, if file is an obj file.
bool MTLImporter::CanRead( const std::string& pFile, IOSystem*  pIOHandler , bool checkSig ) const
{
    if(!checkSig) //Check File Extension
    {
        return SimpleExtensionCheck(pFile, "mtl");
    }
    else //Check file Header
    {
        static const char *pTokens[] = { "newmtl", "Kd", "map_Kd", "Ks" };
        return BaseImporter::SearchFileHeaderForToken(pIOHandler, pFile, pTokens, 4 );
    }
}

// ------------------------------------------------------------------------------------------------
const aiImporterDesc* MTLImporter::GetInfo () const
{
    return &desc;
}

// ------------------------------------------------------------------------------------------------
//  Mtl-file import implementation
void MTLImporter::InternReadFile( const std::string& filename, aiScene* pScene, IOSystem* pIOHandler)
{
    DefaultIOSystem io;
    
    // Read file into memory
    const std::string mode = "rb";
    IOStream *pFile = pIOHandler->Open(filename);

    if(!pFile)
    {
        throw DeadlyImportError( "Failed to open file " + filename + "." );
    }

    // Allocate buffer and read file into it
    TextFileToBuffer(pFile, m_Buffer);

    pIOHandler->Close(pFile);

    // Get the model name
    std::string  strModelName;
    std::string::size_type pos = filename.find_last_of( "\\/" );
    if ( pos != std::string::npos ) 
    {
        strModelName = filename.substr(pos+1, filename.size() - pos - 1);
    }
    else
    {
        strModelName = filename;
    }
    
    // parse the file into a temporary representation
	ObjFile::Model* model = new ObjFile::Model();
	model->m_ModelName = strModelName;
	
    const std::string DEFAULT_MATERIAL = AI_DEFAULT_MATERIAL_NAME; 

    // create default material and store it
	model->m_pDefaultMaterial = new ObjFile::Material();
	model->m_pDefaultMaterial->MaterialName.Set( DEFAULT_MATERIAL );
    model->m_MaterialLib.push_back( DEFAULT_MATERIAL );
	model->m_MaterialMap[ DEFAULT_MATERIAL ] = model->m_pDefaultMaterial;
	ObjFileMtlImporter mtlImporter( m_Buffer, "", model );
    
    // And create the proper return structures out of it
    CreateDataFromImport(model, pScene);

    // Clean up allocated storage for the next import 
    m_Buffer.clear();
}

// ------------------------------------------------------------------------------------------------
//  Create the data from parsed Mtl-file
void MTLImporter::CreateDataFromImport(const ObjFile::Model* pModel, aiScene* pScene) {
    if( 0L == pModel ) {
        return;
    }
        
    // Create the root node of the scene
    pScene->mRootNode = new aiNode;
    if ( !pModel->m_ModelName.empty() )
    {
        // Set the name of the scene
        pScene->mRootNode->mName.Set(pModel->m_ModelName);
    }
    else
    {
        // This is a fatal error, so break down the application
        ai_assert(false);
    } 

    // Create all materials
    createMaterials( pModel, pScene );
}

// ------------------------------------------------------------------------------------------------
//   Add clamp mode property to material if necessary 
void MTLImporter::addTextureMappingModeProperty(aiMaterial* mat, aiTextureType type, int clampMode)
{
    ai_assert( NULL != mat);
    mat->AddProperty<int>(&clampMode, 1, AI_MATKEY_MAPPINGMODE_U(type, 0));
    mat->AddProperty<int>(&clampMode, 1, AI_MATKEY_MAPPINGMODE_V(type, 0));
}

// ------------------------------------------------------------------------------------------------
//  Creates the material 
void MTLImporter::createMaterials(const ObjFile::Model* pModel, aiScene* pScene )
{
    ai_assert( NULL != pScene );
    if ( NULL == pScene )
        return;

    const unsigned int numMaterials = (unsigned int) pModel->m_MaterialLib.size();
    pScene->mNumMaterials = 0;
    if ( pModel->m_MaterialLib.empty() ) {
        DefaultLogger::get()->debug("OBJ: no materials specified");
        return;
    }
    
    pScene->mMaterials = new aiMaterial*[ numMaterials ];
    for ( unsigned int matIndex = 0; matIndex < numMaterials; matIndex++ )
    {       
        // Store material name
        std::map<std::string, ObjFile::Material*>::const_iterator it;
        it = pModel->m_MaterialMap.find( pModel->m_MaterialLib[ matIndex ] );
        
        // No material found, use the default material
        if ( pModel->m_MaterialMap.end() == it )
            continue;

        aiMaterial* mat = new aiMaterial;
        ObjFile::Material *pCurrentMaterial = (*it).second;
        mat->AddProperty( &pCurrentMaterial->MaterialName, AI_MATKEY_NAME );

        // convert illumination model
        int sm = 0;
        switch (pCurrentMaterial->illumination_model) 
        {
        case 0:
            sm = aiShadingMode_NoShading;
            break;
        case 1:
            sm = aiShadingMode_Gouraud;
            break;
        case 2:
            sm = aiShadingMode_Phong;
            break;
        default:
            sm = aiShadingMode_Gouraud;
            DefaultLogger::get()->error("OBJ: unexpected illumination model (0-2 recognized)");
        }
    
        mat->AddProperty<int>( &sm, 1, AI_MATKEY_SHADING_MODEL);

        // multiplying the specular exponent with 2 seems to yield better results
        pCurrentMaterial->shineness *= 4.f;

        // Adding material colors
        mat->AddProperty( &pCurrentMaterial->ambient, 1, AI_MATKEY_COLOR_AMBIENT );
        mat->AddProperty( &pCurrentMaterial->diffuse, 1, AI_MATKEY_COLOR_DIFFUSE );
        mat->AddProperty( &pCurrentMaterial->specular, 1, AI_MATKEY_COLOR_SPECULAR );
        mat->AddProperty( &pCurrentMaterial->emissive, 1, AI_MATKEY_COLOR_EMISSIVE );
        mat->AddProperty( &pCurrentMaterial->shineness, 1, AI_MATKEY_SHININESS );
        mat->AddProperty( &pCurrentMaterial->alpha, 1, AI_MATKEY_OPACITY );

        // Adding refraction index
        mat->AddProperty( &pCurrentMaterial->ior, 1, AI_MATKEY_REFRACTI );

        // Adding textures
        if ( 0 != pCurrentMaterial->texture.length ) 
        {
            mat->AddProperty( &pCurrentMaterial->texture, AI_MATKEY_TEXTURE_DIFFUSE(0));
            if (pCurrentMaterial->clamp[ObjFile::Material::TextureDiffuseType])
            {
                addTextureMappingModeProperty(mat, aiTextureType_DIFFUSE);
            }
        }

        if ( 0 != pCurrentMaterial->textureAmbient.length )
        {
            mat->AddProperty( &pCurrentMaterial->textureAmbient, AI_MATKEY_TEXTURE_AMBIENT(0));
            if (pCurrentMaterial->clamp[ObjFile::Material::TextureAmbientType])
            {
                addTextureMappingModeProperty(mat, aiTextureType_AMBIENT);
            }
        }

        if ( 0 != pCurrentMaterial->textureEmissive.length )
            mat->AddProperty( &pCurrentMaterial->textureEmissive, AI_MATKEY_TEXTURE_EMISSIVE(0));

        if ( 0 != pCurrentMaterial->textureSpecular.length )
        {
            mat->AddProperty( &pCurrentMaterial->textureSpecular, AI_MATKEY_TEXTURE_SPECULAR(0));
            if (pCurrentMaterial->clamp[ObjFile::Material::TextureSpecularType])
            {
                addTextureMappingModeProperty(mat, aiTextureType_SPECULAR);
            }
        }

        if ( 0 != pCurrentMaterial->textureBump.length )
        {
            mat->AddProperty( &pCurrentMaterial->textureBump, AI_MATKEY_TEXTURE_HEIGHT(0));
            if (pCurrentMaterial->clamp[ObjFile::Material::TextureBumpType])
            {
                addTextureMappingModeProperty(mat, aiTextureType_HEIGHT);
            }
        }

        if ( 0 != pCurrentMaterial->textureNormal.length )
        {
            mat->AddProperty( &pCurrentMaterial->textureNormal, AI_MATKEY_TEXTURE_NORMALS(0));
            if (pCurrentMaterial->clamp[ObjFile::Material::TextureNormalType])
            {
                addTextureMappingModeProperty(mat, aiTextureType_NORMALS);
            }
        }

        if ( 0 != pCurrentMaterial->textureDisp.length )
        {
            mat->AddProperty( &pCurrentMaterial->textureDisp, AI_MATKEY_TEXTURE_DISPLACEMENT(0) );
            if (pCurrentMaterial->clamp[ObjFile::Material::TextureDispType])
            {
                addTextureMappingModeProperty(mat, aiTextureType_DISPLACEMENT);
            }
        }

        if ( 0 != pCurrentMaterial->textureOpacity.length )
        {
            mat->AddProperty( &pCurrentMaterial->textureOpacity, AI_MATKEY_TEXTURE_OPACITY(0));
            if (pCurrentMaterial->clamp[ObjFile::Material::TextureOpacityType])
            {
                addTextureMappingModeProperty(mat, aiTextureType_OPACITY);
            }
        }

        if ( 0 != pCurrentMaterial->textureSpecularity.length )
        {
            mat->AddProperty( &pCurrentMaterial->textureSpecularity, AI_MATKEY_TEXTURE_SHININESS(0));
            if (pCurrentMaterial->clamp[ObjFile::Material::TextureSpecularityType])
            {
                addTextureMappingModeProperty(mat, aiTextureType_SHININESS);
            }
        }
        
        // Store material property info in material array in scene
        pScene->mMaterials[ pScene->mNumMaterials ] = mat;
        pScene->mNumMaterials++;
    }
    
    // Test number of created materials.
    ai_assert( pScene->mNumMaterials == numMaterials );
}
}   // Namespace Assimp

#endif // !! ASSIMP_BUILD_NO_MTL_IMPORTER
