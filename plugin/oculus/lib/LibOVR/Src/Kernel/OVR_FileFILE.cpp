/**************************************************************************

Filename    :   OVR_FileFILE.cpp
Content     :   File wrapper class implementation (Win32)

Created     :   April 5, 1999
Authors     :   Michael Antonov

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

**************************************************************************/

#define  GFILE_CXX

#include "OVR_Types.h"
#include "OVR_Log.h"

// Standard C library (Captain Obvious guarantees!)
#include <stdio.h>
#ifndef OVR_OS_WINCE
#include <sys/stat.h>
#endif

#include "OVR_SysFile.h"

#ifndef OVR_OS_WINCE
#include <errno.h>
#endif

namespace OVR {

// ***** File interface

// ***** FILEFile - C streams file

static int SFerror ()
{
    if (errno == ENOENT)
        return FileConstants::Error_FileNotFound;
    else if (errno == EACCES || errno == EPERM)
        return FileConstants::Error_Access;
    else if (errno == ENOSPC)
        return FileConstants::Error_DiskFull;
    else
        return FileConstants::Error_IOError;
};

#if defined(OVR_OS_WIN32)
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
// A simple helper class to disable/enable system error mode, if necessary
// Disabling happens conditionally only if a drive name is involved
class SysErrorModeDisabler
{
    BOOL    Disabled;
    UINT    OldMode;
public:
    SysErrorModeDisabler(const char* pfileName)
    {
        if (pfileName && (pfileName[0]!=0) && pfileName[1]==':')
        {
            Disabled = 1;
            OldMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
        }
        else
            Disabled = 0;
    }

    ~SysErrorModeDisabler()
    {
        if (Disabled) ::SetErrorMode(OldMode);
    }
};
#else
class SysErrorModeDisabler
{
public:
    SysErrorModeDisabler(const char* pfileName) { OVR_UNUSED(pfileName); }
};
#endif // OVR_OS_WIN32


// This macro enables verification of I/O results after seeks against a pre-loaded
// full file buffer copy. This is generally not necessary, but can been used to debug
// memory corruptions; we've seen this fail due to EAX2/DirectSound corrupting memory
// under FMOD with XP64 (32-bit) and Realtek HA Audio driver.
//#define GFILE_VERIFY_SEEK_ERRORS


// This is the simplest possible file implementation, it wraps around the descriptor
// This file is delegated to by SysFile.

class FILEFile : public File
{
protected:

    // Allocated filename
    String      FileName;

    // File handle & open mode
    bool        Opened;
    FILE*       fs;
    int         OpenFlags;
    // Error code for last request
    int         ErrorCode;

    int         LastOp;

#ifdef OVR_FILE_VERIFY_SEEK_ERRORS
    uint8_t*      pFileTestBuffer;
    unsigned    FileTestLength;
    unsigned    TestPos; // File pointer position during tests.
#endif

public:

    FILEFile()
    {
        Opened = 0; FileName = "";

#ifdef OVR_FILE_VERIFY_SEEK_ERRORS
        pFileTestBuffer =0;
        FileTestLength  =0;
        TestPos         =0;
#endif
    }
    // Initialize file by opening it
    FILEFile(const String& fileName, int flags, int Mode);
    // The 'pfileName' should be encoded as UTF-8 to support international file names.
    FILEFile(const char* pfileName, int flags, int Mode);

    ~FILEFile()
    {
        if (Opened)
            Close();
    }

    virtual const char* GetFilePath();

    // ** File Information
    virtual bool        IsValid();
    virtual bool        IsWritable();

    // Return position / file size
    virtual int         Tell();
    virtual int64_t     LTell();
    virtual int         GetLength();
    virtual int64_t     LGetLength();

//  virtual bool        Stat(FileStats *pfs);
    virtual int         GetErrorCode();

    // ** Stream implementation & I/O
    virtual int         Write(const uint8_t *pbuffer, int numBytes);
    virtual int         Read(uint8_t *pbuffer, int numBytes);
    virtual int         SkipBytes(int numBytes);
    virtual int         BytesAvailable();
    virtual bool        Flush();
    virtual int         Seek(int offset, int origin);
    virtual int64_t     LSeek(int64_t offset, int origin);
    
    virtual int         CopyFromStream(File *pStream, int byteSize);
    virtual bool        Close();    
private:
    void                init();
};


// Initialize file by opening it
FILEFile::FILEFile(const String& fileName, int flags, int mode)
  : FileName(fileName), OpenFlags(flags)
{
    OVR_UNUSED(mode);
    init();
}

// The 'pfileName' should be encoded as UTF-8 to support international file names.
FILEFile::FILEFile(const char* pfileName, int flags, int mode)
  : FileName(pfileName), OpenFlags(flags)
{
    OVR_UNUSED(mode);
    init();
}

void FILEFile::init()
{
    // Open mode for file's open
    const char *omode = "rb";

    if (OpenFlags & Open_Truncate)
    {
        if (OpenFlags & Open_Read)
            omode = "w+b";
        else
            omode = "wb";
    }
    else if (OpenFlags & Open_Create)
    {
        if (OpenFlags & Open_Read)
            omode = "a+b";
        else
            omode = "ab";
    }
    else if (OpenFlags & Open_Write)
        omode = "r+b";

#if defined(OVR_OS_WIN32)
    SysErrorModeDisabler disabler(FileName.ToCStr());
#endif

#if defined(OVR_CC_MSVC) && (OVR_CC_MSVC >= 1400)
    wchar_t womode[16];
    wchar_t *pwFileName = (wchar_t*)OVR_ALLOC((UTF8Util::GetLength(FileName.ToCStr())+1) * sizeof(wchar_t));
    UTF8Util::DecodeString(pwFileName, FileName.ToCStr());
    OVR_ASSERT(strlen(omode) < sizeof(womode)/sizeof(womode[0]));
    UTF8Util::DecodeString(womode, omode);
    _wfopen_s(&fs, pwFileName, womode);
    OVR_FREE(pwFileName);
#else
    fs = fopen(FileName.ToCStr(), omode);
#endif
    if (fs)
        rewind (fs);
    Opened = (fs != NULL);
    // Set error code
    if (!Opened)
        ErrorCode = SFerror();
    else
    {
        // If we are testing file seek correctness, pre-load the entire file so
        // that we can do comparison tests later.
#ifdef OVR_FILE_VERIFY_SEEK_ERRORS        
        TestPos         = 0;
        fseek(fs, 0, SEEK_END);
        FileTestLength  = ftell(fs);
        fseek(fs, 0, SEEK_SET);
        pFileTestBuffer = (uint8_t*)OVR_ALLOC(FileTestLength);
        if (pFileTestBuffer)
        {
            OVR_ASSERT(FileTestLength == (unsigned)Read(pFileTestBuffer, FileTestLength));
            Seek(0, Seek_Set);
        }        
#endif

        ErrorCode = 0;
    }
    LastOp = 0;
}


const char* FILEFile::GetFilePath()
{
    return FileName.ToCStr();
}


// ** File Information
bool    FILEFile::IsValid()
{
    return Opened;
}
bool    FILEFile::IsWritable()
{
    return IsValid() && (OpenFlags&Open_Write);
}
/*
bool    FILEFile::IsRecoverable()
{
    return IsValid() && ((OpenFlags&OVR_FO_SAFETRUNC) == OVR_FO_SAFETRUNC);
}
*/

// Return position / file size
int     FILEFile::Tell()
{
    int pos = (int)ftell (fs);
    if (pos < 0)
        ErrorCode = SFerror();
    return pos;
}

int64_t FILEFile::LTell()
{
    int64_t pos = ftell(fs);
    if (pos < 0)
        ErrorCode = SFerror();
    return pos;
}

int     FILEFile::GetLength()
{
    int pos = Tell();
    if (pos >= 0)
    {
        Seek (0, Seek_End);
        int size = Tell();
        Seek (pos, Seek_Set);
        return size;
    }
    return -1;
}
int64_t FILEFile::LGetLength()
{
    int64_t pos = LTell();
    if (pos >= 0)
    {
        LSeek (0, Seek_End);
        int64_t size = LTell();
        LSeek (pos, Seek_Set);
        return size;
    }
    return -1;
}

int     FILEFile::GetErrorCode()
{
    return ErrorCode;
}

// ** Stream implementation & I/O
int     FILEFile::Write(const uint8_t *pbuffer, int numBytes)
{
    if (LastOp && LastOp != Open_Write)
        fflush(fs);
    LastOp = Open_Write;
    int written = (int) fwrite(pbuffer, 1, numBytes, fs);
    if (written < numBytes)
        ErrorCode = SFerror();

#ifdef OVR_FILE_VERIFY_SEEK_ERRORS
    if (written > 0)
        TestPos += written;
#endif

    return written;
}

int     FILEFile::Read(uint8_t *pbuffer, int numBytes)
{
    if (LastOp && LastOp != Open_Read)
        fflush(fs);
    LastOp = Open_Read;
    int read = (int) fread(pbuffer, 1, numBytes, fs);
    if (read < numBytes)
        ErrorCode = SFerror();

#ifdef OVR_FILE_VERIFY_SEEK_ERRORS
    if (read > 0)
    {
        // Read-in data must match our pre-loaded buffer data!
        uint8_t* pcompareBuffer = pFileTestBuffer + TestPos;
        for (int i=0; i< read; i++)
        {
            OVR_ASSERT(pcompareBuffer[i] == pbuffer[i]);
        }

        //OVR_ASSERT(!memcmp(pFileTestBuffer + TestPos, pbuffer, read));
        TestPos += read;
        OVR_ASSERT(ftell(fs) == (int)TestPos);
    }
#endif

    return read;
}

// Seeks ahead to skip bytes
int     FILEFile::SkipBytes(int numBytes)
{
    int64_t pos    = LTell();
    int64_t newPos = LSeek(numBytes, Seek_Cur);

    // Return -1 for major error
    if ((pos==-1) || (newPos==-1))
    {
        return -1;
    }
    //ErrorCode = ((NewPos-Pos)<numBytes) ? errno : 0;

    return int (newPos-(int)pos);
}

// Return # of bytes till EOF
int     FILEFile::BytesAvailable()
{
    int64_t pos    = LTell();
    int64_t endPos = LGetLength();

    // Return -1 for major error
    if ((pos==-1) || (endPos==-1))
    {
        ErrorCode = SFerror();
        return 0;
    }
    else
        ErrorCode = 0;

    return int (endPos-(int)pos);
}

// Flush file contents
bool    FILEFile::Flush()
{
    return !fflush(fs);
}

int     FILEFile::Seek(int offset, int origin)
{
    int newOrigin = 0;
    switch(origin)
    {
    case Seek_Set: newOrigin = SEEK_SET; break;
    case Seek_Cur: newOrigin = SEEK_CUR; break;
    case Seek_End: newOrigin = SEEK_END; break;
    }

    if (newOrigin == SEEK_SET && offset == Tell())
        return Tell();

    if (fseek (fs, offset, newOrigin))
    {
#ifdef OVR_FILE_VERIFY_SEEK_ERRORS
        OVR_ASSERT(0);
#endif
        return -1;
    }
    
#ifdef OVR_FILE_VERIFY_SEEK_ERRORS
    // Track file position after seeks for read verification later.
    switch(origin)
    {
    case Seek_Set:  TestPos = offset;       break;
    case Seek_Cur:  TestPos += offset;      break;    
    case Seek_End:  TestPos = FileTestLength + offset; break;
    }
    OVR_ASSERT((int)TestPos == Tell());
#endif

    return (int)Tell();
}

int64_t FILEFile::LSeek(int64_t offset, int origin)
{
    return Seek((int)offset,origin);
}

int FILEFile::CopyFromStream(File *pstream, int byteSize)
{
    uint8_t*  buff = new uint8_t[0x4000];
    int     count = 0;
    int     szRequest, szRead, szWritten;

    while (byteSize)
    {
        szRequest = (byteSize > int(sizeof(buff))) ? int(sizeof(buff)) : byteSize;

        szRead    = pstream->Read(buff, szRequest);
        szWritten = 0;
        if (szRead > 0)
            szWritten = Write(buff, szRead);

        count    += szWritten;
        byteSize -= szWritten;
        if (szWritten < szRequest)
            break;
    }

	delete[] buff;

    return count;
}


bool FILEFile::Close()
{
#ifdef OVR_FILE_VERIFY_SEEK_ERRORS
    if (pFileTestBuffer)
    {
        OVR_FREE(pFileTestBuffer);
        pFileTestBuffer = 0;
        FileTestLength  = 0;
    }
#endif

    bool closeRet = !fclose(fs);

    if (!closeRet)
    {
        ErrorCode = SFerror();
        return 0;
    }
    else
    {
        Opened    = 0;
        fs        = 0;
        ErrorCode = 0;
    }

    // Handle safe truncate
    /*
    if ((OpenFlags & OVR_FO_SAFETRUNC) == OVR_FO_SAFETRUNC)
    {
        // Delete original file (if it existed)
        DWORD oldAttributes = FileUtilWin32::GetFileAttributes(FileName);
        if (oldAttributes!=0xFFFFFFFF)
            if (!FileUtilWin32::DeleteFile(FileName))
            {
                // Try to remove the readonly attribute
                FileUtilWin32::SetFileAttributes(FileName, oldAttributes & (~FILE_ATTRIBUTE_READONLY) );
                // And delete the file again
                if (!FileUtilWin32::DeleteFile(FileName))
                    return 0;
            }

        // Rename temp file to real filename
        if (!FileUtilWin32::MoveFile(TempName, FileName))
        {
            //ErrorCode = errno;
            return 0;
        }
    }
    */
    return 1;
}

/*
bool    FILEFile::CloseCancel()
{
    bool closeRet = (bool)::CloseHandle(fd);

    if (!closeRet)
    {
        //ErrorCode = errno;
        return 0;
    }
    else
    {
        Opened    = 0;
        fd        = INVALID_HANDLE_VALUE;
        ErrorCode = 0;
    }

    // Handle safe truncate (delete tmp file, leave original unchanged)
    if ((OpenFlags&OVR_FO_SAFETRUNC) == OVR_FO_SAFETRUNC)
        if (!FileUtilWin32::DeleteFile(TempName))
        {
            //ErrorCode = errno;
            return 0;
        }
    return 1;
}
*/

Ptr<File> FileFILEOpen(const String& path, int flags, int mode)
{
    Ptr<File> result = *new FILEFile(path, flags, mode);
	return result;
}

// Helper function: obtain file information time.
bool    SysFile::GetFileStat(FileStat* pfileStat, const String& path)
{
#if defined(OVR_OS_WIN32)
    // 64-bit implementation on Windows.
    struct __stat64 fileStat;
    // Stat returns 0 for success.
    wchar_t *pwpath = (wchar_t*)OVR_ALLOC((UTF8Util::GetLength(path.ToCStr())+1)*sizeof(wchar_t));
    UTF8Util::DecodeString(pwpath, path.ToCStr());

    int ret = _wstat64(pwpath, &fileStat);
    OVR_FREE(pwpath);
    if (ret) return false;
#else
    struct stat fileStat;
    // Stat returns 0 for success.
    if (stat(path, &fileStat) != 0)
        return false;
#endif
    pfileStat->AccessTime = fileStat.st_atime;
    pfileStat->ModifyTime = fileStat.st_mtime;
    pfileStat->FileSize   = fileStat.st_size;
    return true;
}

} // Namespace OVR
