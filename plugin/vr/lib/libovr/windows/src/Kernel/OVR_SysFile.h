/************************************************************************************

PublicHeader:   Kernel
Filename    :   OVR_SysFile.h
Content     :   Header for all internal file management - functions and structures
                to be inherited by OS specific subclasses.
Created     :   September 19, 2012
Notes       : 

Notes       :   errno may not be preserved across use of GBaseFile member functions
            :   Directories cannot be deleted while files opened from them are in use
                (For the GetFullName function)

Copyright   :   Copyright 2014 Oculus VR, LLC All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.2 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.2 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#ifndef OVR_SysFile_h
#define OVR_SysFile_h

#include "OVR_File.h"

namespace OVR {

// ***** Declared classes
class   SysFile;

//-----------------------------------------------------------------------------------
// *** File Statistics

// This class contents are similar to _stat, providing
// creation, modify and other information about the file.
struct FileStat
{
    // No change or create time because they are not available on most systems
    int64_t ModifyTime;
    int64_t AccessTime;
    int64_t FileSize;

    bool operator== (const FileStat& stat) const
    {
        return ( (ModifyTime == stat.ModifyTime) &&
                 (AccessTime == stat.AccessTime) &&
                 (FileSize == stat.FileSize) );
    }
};

//-----------------------------------------------------------------------------------
// *** System File

// System file is created to access objects on file system directly
// This file can refer directly to path.
// System file can be open & closed several times; however, such use is not recommended
// This class is realy a wrapper around an implementation of File interface for a 
// particular platform.

class SysFile : public DelegatedFile
{
protected:
  SysFile(const SysFile &source) : DelegatedFile () { OVR_UNUSED(source); }
public:

    // ** Constructor
    SysFile();
    // Opens a file
    SysFile(const String& path, int flags = Open_Read|Open_Buffered, int mode = Mode_ReadWrite); 

    // ** Open & management 
    bool  Open(const String& path, int flags = Open_Read|Open_Buffered, int mode = Mode_ReadWrite);
        
    OVR_FORCE_INLINE bool  Create(const String& path, int mode = Mode_ReadWrite)
    { return Open(path, Open_ReadWrite|Open_Create, mode); }

    // Helper function: obtain file statistics information. In OVR, this is used to detect file changes.
    // Return 0 if function failed, most likely because the file doesn't exist.
    static bool OVR_CDECL GetFileStat(FileStat* pfileStats, const String& path);
    
    // ** Overrides
    // Overridden to provide re-open support
    virtual int   GetErrorCode();

    virtual bool  IsValid();

    virtual bool  Close();    
};

} // Namespace OVR

#endif
