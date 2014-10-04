/************************************************************************************

Filename    :   OVR_String_PathUtil.cpp
Content     :   String filename/url helper function
Created     :   September 19, 2012
Notes       : 

Copyright   :   Copyright 2014 Oculus VR, Inc. All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.1 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.1 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#include "OVR_String.h"
#include "OVR_UTF8Util.h"

namespace OVR {

//--------------------------------------------------------------------
// ***** Path-Scanner helper function 

// Scans file path finding filename start and extension start, fills in their addess.
void ScanFilePath(const char* url, const char** pfilename, const char** pext)
{ 
    const char* urlStart = url;
    const char *filename = 0;
    const char *lastDot = 0;

    uint32_t charVal = UTF8Util::DecodeNextChar(&url);

    while (charVal != 0)
    {
        if ((charVal == '/') || (charVal == '\\'))
        {
            filename = url;
            lastDot  = 0;
        }
        else if (charVal == '.')
        {
            lastDot = url - 1;
        }
        
        charVal = UTF8Util::DecodeNextChar(&url);
    }

    if (pfilename)
    {
        // It was a naked filename
        if (urlStart && (*urlStart != '.') && *urlStart)
            *pfilename = urlStart;
        else
            *pfilename = filename;
    }

    if (pext)
    {
        *pext = lastDot;
    }
}

// Scans till the end of protocol. Returns first character past protocol,
// 0 if not found.
//  - protocol: 'file://', 'http://'
const char* ScanPathProtocol(const char* url)
{    
    uint32_t charVal = UTF8Util::DecodeNextChar(&url);
    uint32_t charVal2;
   
    while (charVal != 0)
    {
        // Treat a colon followed by a slash as absolute.
        if (charVal == ':')
        {
            charVal2 = UTF8Util::DecodeNextChar(&url);
            charVal  = UTF8Util::DecodeNextChar(&url);
            if ((charVal == '/') && (charVal2 == '\\'))
                return url;
        }
        charVal = UTF8Util::DecodeNextChar(&url);
    }
    return 0;
}


//--------------------------------------------------------------------
// ***** String Path API implementation

bool String::HasAbsolutePath(const char* url)
{
    // Absolute paths can star with:
    //  - protocols:        'file://', 'http://'
    //  - windows drive:    'c:\'
    //  - UNC share name:   '\\share'
    //  - unix root         '/'

    // On the other hand, relative paths are:
    //  - directory:        'directory/file'
    //  - this directory:   './file'
    //  - parent directory: '../file'
    // 
    // For now, we don't parse '.' or '..' out, but instead let it be concatenated
    // to string and let the OS figure it out. This, however, is not good for file
    // name matching in library/etc, so it should be improved.

    if (!url || !*url)
        return true; // Treat empty strings as absolute.    

    uint32_t charVal = UTF8Util::DecodeNextChar(&url);

    // Fist character of '/' or '\\' means absolute url.
    if ((charVal == '/') || (charVal == '\\'))
        return true;

    while (charVal != 0)
    {
        // Treat a colon followed by a slash as absolute.
        if (charVal == ':')
        {
            charVal = UTF8Util::DecodeNextChar(&url);
            // Protocol or windows drive. Absolute.
            if ((charVal == '/') || (charVal == '\\'))
                return true;
        }
        else if ((charVal == '/') || (charVal == '\\'))
        {
            // Not a first character (else 'if' above the loop would have caught it).
            // Must be a relative url.
            break;
        }

        charVal = UTF8Util::DecodeNextChar(&url);
    }

    // We get here for relative paths.
    return false;    
}


bool String::HasExtension(const char* path)
{
    const char* ext = 0;
    ScanFilePath(path, 0, &ext);
    return ext != 0;
}
bool String::HasProtocol(const char* path)
{
    return ScanPathProtocol(path) != 0;
}


String  String::GetPath() const
{
    const char* filename = 0;
    ScanFilePath(ToCStr(), &filename, 0);

    // Technically we can have extra logic somewhere for paths,
    // such as enforcing protocol and '/' only based on flags,
    // but we keep it simple for now.
    return String(ToCStr(), filename ? (filename-ToCStr()) : GetSize());
}

String  String::GetProtocol() const
{
    const char* protocolEnd = ScanPathProtocol(ToCStr());
    return String(ToCStr(), protocolEnd ? (protocolEnd-ToCStr()) : 0);
}

String  String::GetFilename() const
{
    const char* filename = 0;
    ScanFilePath(ToCStr(), &filename, 0);
    return String(filename);
}
String  String::GetExtension() const
{
    const char* ext = 0;
    ScanFilePath(ToCStr(), 0, &ext);
    return String(ext);
}

void    String::StripExtension()
{
    const char* ext = 0;
    ScanFilePath(ToCStr(), 0, &ext);    
    if (ext)
    {
        *this = String(ToCStr(), ext-ToCStr());
    }
}

void    String::StripProtocol()
{
    const char* protocol = ScanPathProtocol(ToCStr());
    if (protocol)
        AssignString(protocol, OVR_strlen(protocol));
}

} // OVR
