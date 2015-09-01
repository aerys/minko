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

#pragma once

#include "BaseImporter.h"
#include "../include/assimp/material.h"
#include <vector>

struct aiMesh;
struct aiNode;

namespace Assimp
{

namespace ObjFile
{
struct Object;
struct Model;
}

// ------------------------------------------------------------------------------------------------
/// \class  MTLImporter
/// \brief  Imports a waveform obj material template library file
// ------------------------------------------------------------------------------------------------
class MTLImporter : public BaseImporter
{   
public:
    /// \brief  Default constructor
    MTLImporter();

    /// \brief  Destructor
    ~MTLImporter();

public:
    /// \brief  Returns whether the class can handle the format of the given file. 
    /// \remark See BaseImporter::CanRead() for details.
    bool CanRead( const std::string& pFile, IOSystem* pIOHandler, bool checkSig) const;

private:

    //! \brief  Appends the supported extension.
    const aiImporterDesc* GetInfo () const;

    //! \brief  File import implementation.
    void InternReadFile(const std::string& pFile, aiScene* pScene, IOSystem* pIOHandler);
    
    //! \brief  Create the data from imported content.
    void CreateDataFromImport(const ObjFile::Model* pModel, aiScene* pScene);
    
    //! \brief  Material creation.
    void createMaterials(const ObjFile::Model* pModel, aiScene* pScene);
    void addTextureMappingModeProperty(aiMaterial* mat, aiTextureType type, int clampMode = 1);

private:
    //! Data buffer
    std::vector<char> m_Buffer;
    //! Pointer to root object instance
    ObjFile::Object *m_pRootObject;
    //! Absolute pathname of model in file system
    std::string m_strAbsPath;
};

// ------------------------------------------------------------------------------------------------

} // Namespace Assimp
