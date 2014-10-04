/************************************************************************************

PublicHeader:   Kernel
Filename    :   OVR_File.h
Content     :   Header for all internal file management - functions and structures
                to be inherited by OS specific subclasses.
Created     :   September 19, 2012
Notes       : 

Notes       :   errno may not be preserved across use of BaseFile member functions
            :   Directories cannot be deleted while files opened from them are in use
                (For the GetFullName function)

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

#ifndef OVR_File_h
#define OVR_File_h

#include "OVR_RefCount.h"
#include "OVR_Std.h"
#include "OVR_Alg.h"

#include <stdio.h>
#include "OVR_String.h"

namespace OVR {

// ***** Declared classes
class   FileConstants;
class   File;
class   DelegatedFile;
class   BufferedFile;


// ***** Flags for File & Directory accesses

class FileConstants
{
public:

    // *** File open flags
    enum OpenFlags
    {
        Open_Read       = 1,
        Open_Write      = 2,
        Open_ReadWrite  = 3,

        // Opens file and truncates it to zero length
        // - file must have write permission
        // - when used with Create, it opens an existing 
        //   file and empties it or creates a new file
        Open_Truncate   = 4,

        // Creates and opens new file 
        // - does not erase contents if file already
        //   exists unless combined with Truncate
        Open_Create     = 8,

         // Returns an error value if the file already exists
        Open_CreateOnly = 24,

        // Open file with buffering
        Open_Buffered    = 32
    };

    // *** File Mode flags
    enum Modes
    {
        Mode_Read       = 0444,
        Mode_Write      = 0222,
        Mode_Execute    = 0111,

        Mode_ReadWrite  = 0666
    };

    // *** Seek operations
    enum SeekOps
    {
        Seek_Set        = 0,
        Seek_Cur        = 1,
        Seek_End        = 2
    };

    // *** Errors
    enum Errors
    {
        Error_FileNotFound  = 0x1001,
        Error_Access        = 0x1002,
        Error_IOError       = 0x1003,
        Error_DiskFull      = 0x1004
    };
};


//-----------------------------------------------------------------------------------
// ***** File Class

// The pure virtual base random-access file
// This is a base class to all files

class File : public RefCountBase<File>, public FileConstants
{   
public:
    File() { }
    // ** Location Information

    // Returns a file name path relative to the 'reference' directory
    // This is often a path that was used to create a file
    // (this is not a global path, global path can be obtained with help of directory)
    virtual const char* GetFilePath() = 0;
                                                                                        

    // ** File Information

    // Return 1 if file's usable (open)
    virtual bool        IsValid() = 0;
    // Return 1 if file's writable, otherwise 0                                         
    virtual bool        IsWritable() = 0;
                                                                                        
    // Return position
    virtual int         Tell() = 0;
    virtual int64_t     LTell() = 0;
    
    // File size                                                                        
    virtual int         GetLength() = 0;
    virtual int64_t     LGetLength() = 0;
                                                                                        
    // Returns file stats                                                               
    // 0 for failure                                                                    
    //virtual bool      Stat(FileStats *pfs) = 0;
                                                                                        
    // Return errno-based error code                                                    
    // Useful if any other function failed                                              
    virtual int         GetErrorCode() = 0;
                                                                                        
                                                                                        
    // ** Stream implementation & I/O

    // Blocking write, will write in the given number of bytes to the stream
    // Returns : -1 for error
    //           Otherwise number of bytes read 
    virtual int         Write(const uint8_t *pbufer, int numBytes) = 0;
    // Blocking read, will read in the given number of bytes or less from the stream
    // Returns : -1 for error
    //           Otherwise number of bytes read,
    //           if 0 or < numBytes, no more bytes available; end of file or the other side of stream is closed
    virtual int         Read(uint8_t *pbufer, int numBytes) = 0;

    // Skips (ignores) a given # of bytes
    // Same return values as Read
    virtual int         SkipBytes(int numBytes) = 0;
        
    // Returns the number of bytes available to read from a stream without blocking
    // For a file, this should generally be number of bytes to the end
    virtual int         BytesAvailable() = 0;

    // Causes any implementation's buffered data to be delivered to destination
    // Return 0 for error
    virtual bool        Flush() = 0;
                                                                                            

    // Need to provide a more optimized implementation that doe snot necessarily involve a lot of seeking
    inline bool         IsEOF() { return !BytesAvailable(); }
    

    // Seeking                                                                              
    // Returns new position, -1 for error                                                   
    virtual int         Seek(int offset, int origin=Seek_Set) = 0;
    virtual int64_t     LSeek(int64_t offset, int origin=Seek_Set) = 0;
    // Seek simplification
    int                 SeekToBegin()           {return Seek(0); }
    int                 SeekToEnd()             {return Seek(0,Seek_End); }
    int                 Skip(int numBytes)     {return Seek(numBytes,Seek_Cur); }
                        

    // Appends other file data from a stream
    // Return -1 for error, else # of bytes written
    virtual int         CopyFromStream(File *pstream, int byteSize) = 0;

    // Closes the file
    // After close, file cannot be accessed 
    virtual bool        Close() = 0;


    // ***** Inlines for convenient primitive type serialization

    // Read/Write helpers
private:
    uint64_t  PRead64()           { uint64_t v = 0; Read((uint8_t*)&v, 8); return v; }
    uint32_t  PRead32()           { uint32_t v = 0; Read((uint8_t*)&v, 4); return v; }
    uint16_t  PRead16()           { uint16_t v = 0; Read((uint8_t*)&v, 2); return v; }
    uint8_t PRead8()            { uint8_t  v = 0; Read((uint8_t*)&v, 1); return v; }
    void    PWrite64(uint64_t v)  { Write((uint8_t*)&v, 8); }
    void    PWrite32(uint32_t v)  { Write((uint8_t*)&v, 4); }
    void    PWrite16(uint16_t v)  { Write((uint8_t*)&v, 2); }
    void    PWrite8(uint8_t v)  { Write((uint8_t*)&v, 1); }

public:

    // Writing primitive types - Little Endian
    inline void    WriteUByte(uint8_t v)       { PWrite8((uint8_t)Alg::ByteUtil::SystemToLE(v));     }
    inline void    WriteSByte(int8_t v)        { PWrite8((uint8_t)Alg::ByteUtil::SystemToLE(v));     }
    inline void    WriteUInt8(uint8_t v)       { PWrite8((uint8_t)Alg::ByteUtil::SystemToLE(v));     }
    inline void    WriteSInt8(int8_t v)        { PWrite8((uint8_t)Alg::ByteUtil::SystemToLE(v));     }
    inline void    WriteUInt16(uint16_t v)       { PWrite16((uint16_t)Alg::ByteUtil::SystemToLE(v));   }
    inline void    WriteSInt16(int16_t v)       { PWrite16((uint16_t)Alg::ByteUtil::SystemToLE(v));   }
    inline void    WriteUInt32(uint32_t v)       { PWrite32((uint32_t)Alg::ByteUtil::SystemToLE(v));   }
    inline void    WriteSInt32(int32_t v)       { PWrite32((uint32_t)Alg::ByteUtil::SystemToLE(v));   }
    inline void    WriteUInt64(uint64_t v)       { PWrite64((uint64_t)Alg::ByteUtil::SystemToLE(v));   }
    inline void    WriteSInt64(int64_t v)       { PWrite64((uint64_t)Alg::ByteUtil::SystemToLE(v));   }
    inline void    WriteFloat(float v)         { v = Alg::ByteUtil::SystemToLE(v); Write((uint8_t*)&v, 4); } 
    inline void    WriteDouble(double v)       { v = Alg::ByteUtil::SystemToLE(v); Write((uint8_t*)&v, 8); }
    // Writing primitive types - Big Endian
    inline void    WriteUByteBE(uint8_t v)     { PWrite8((uint8_t)Alg::ByteUtil::SystemToBE(v));     }
    inline void    WriteSByteBE(int8_t v)      { PWrite8((uint8_t)Alg::ByteUtil::SystemToBE(v));     }
    inline void    WriteUInt8BE(uint16_t v)      { PWrite8((uint8_t)Alg::ByteUtil::SystemToBE(v));     }
    inline void    WriteSInt8BE(int16_t v)      { PWrite8((uint8_t)Alg::ByteUtil::SystemToBE(v));     }
    inline void    WriteUInt16BE(uint16_t v)     { PWrite16((uint16_t)Alg::ByteUtil::SystemToBE(v));   }
    inline void    WriteSInt16BE(uint16_t v)     { PWrite16((uint16_t)Alg::ByteUtil::SystemToBE(v));   }
    inline void    WriteUInt32BE(uint32_t v)     { PWrite32((uint32_t)Alg::ByteUtil::SystemToBE(v));   }
    inline void    WriteSInt32BE(uint32_t v)     { PWrite32((uint32_t)Alg::ByteUtil::SystemToBE(v));   }
    inline void    WriteUInt64BE(uint64_t v)     { PWrite64((uint64_t)Alg::ByteUtil::SystemToBE(v));   }
    inline void    WriteSInt64BE(uint64_t v)     { PWrite64((uint64_t)Alg::ByteUtil::SystemToBE(v));   }
    inline void    WriteFloatBE(float v)       { v = Alg::ByteUtil::SystemToBE(v); Write((uint8_t*)&v, 4); }
    inline void    WriteDoubleBE(double v)     { v = Alg::ByteUtil::SystemToBE(v); Write((uint8_t*)&v, 8); }
        
    // Reading primitive types - Little Endian
    inline uint8_t ReadUByte()                 { return (uint8_t)Alg::ByteUtil::LEToSystem(PRead8());    }
    inline int8_t  ReadSByte()                 { return (int8_t)Alg::ByteUtil::LEToSystem(PRead8());    }
    inline uint8_t ReadUInt8()                 { return (uint8_t)Alg::ByteUtil::LEToSystem(PRead8());    }
    inline int8_t  ReadSInt8()                 { return (int8_t)Alg::ByteUtil::LEToSystem(PRead8());    }
    inline uint16_t  ReadUInt16()                { return (uint16_t)Alg::ByteUtil::LEToSystem(PRead16());  }
    inline int16_t ReadSInt16()                { return (int16_t)Alg::ByteUtil::LEToSystem(PRead16());  }
    inline uint32_t  ReadUInt32()                { return (uint32_t)Alg::ByteUtil::LEToSystem(PRead32());  }
    inline int32_t ReadSInt32()                { return (int32_t)Alg::ByteUtil::LEToSystem(PRead32());  }
    inline uint64_t  ReadUInt64()                { return (uint64_t)Alg::ByteUtil::LEToSystem(PRead64());  }
    inline int64_t ReadSInt64()                { return (int64_t)Alg::ByteUtil::LEToSystem(PRead64());  }
    inline float   ReadFloat()                 { float v = 0.0f; Read((uint8_t*)&v, 4); return Alg::ByteUtil::LEToSystem(v); }
    inline double  ReadDouble()                { double v = 0.0; Read((uint8_t*)&v, 8); return Alg::ByteUtil::LEToSystem(v); }
    // Reading primitive types - Big Endian
    inline uint8_t ReadUByteBE()               { return (uint8_t)Alg::ByteUtil::BEToSystem(PRead8());    }
    inline int8_t  ReadSByteBE()               { return (int8_t)Alg::ByteUtil::BEToSystem(PRead8());    }
    inline uint8_t ReadUInt8BE()               { return (uint8_t)Alg::ByteUtil::BEToSystem(PRead8());    }
    inline int8_t  ReadSInt8BE()               { return (int8_t)Alg::ByteUtil::BEToSystem(PRead8());    }
    inline uint16_t  ReadUInt16BE()              { return (uint16_t)Alg::ByteUtil::BEToSystem(PRead16());  }
    inline int16_t ReadSInt16BE()              { return (int16_t)Alg::ByteUtil::BEToSystem(PRead16());  }
    inline uint32_t  ReadUInt32BE()              { return (uint32_t)Alg::ByteUtil::BEToSystem(PRead32());  }
    inline int32_t ReadSInt32BE()              { return (int32_t)Alg::ByteUtil::BEToSystem(PRead32());  }
    inline uint64_t  ReadUInt64BE()              { return (uint64_t)Alg::ByteUtil::BEToSystem(PRead64());  }
    inline int64_t ReadSInt64BE()              { return (int64_t)Alg::ByteUtil::BEToSystem(PRead64());  }
    inline float   ReadFloatBE()               { float v = 0.0f; Read((uint8_t*)&v, 4); return Alg::ByteUtil::BEToSystem(v); }
    inline double  ReadDoubleBE()              { double v = 0.0; Read((uint8_t*)&v, 8); return Alg::ByteUtil::BEToSystem(v); }
};


// *** Delegated File

class DelegatedFile : public File
{
protected:
    // Delegating file pointer
    Ptr<File>     pFile;

    // Hidden default constructor
    DelegatedFile() : pFile(0)                             { }
    DelegatedFile(const DelegatedFile &source) : File()    { OVR_UNUSED(source); }
public:
    // Constructors
    DelegatedFile(File *pfile) : pFile(pfile)     { }

    // ** Location Information  
    virtual const char* GetFilePath()                               { return pFile->GetFilePath(); }    

    // ** File Information                                                      
    virtual bool        IsValid()                                   { return pFile && pFile->IsValid(); }   
    virtual bool        IsWritable()                                { return pFile->IsWritable(); }
//  virtual bool        IsRecoverable()                             { return pFile->IsRecoverable(); }          
                                                                    
    virtual int         Tell()                                      { return pFile->Tell(); }
    virtual int64_t     LTell()                                     { return pFile->LTell(); }
    
    virtual int         GetLength()                                 { return pFile->GetLength(); }
    virtual int64_t     LGetLength()                                { return pFile->LGetLength(); }
    
    //virtual bool      Stat(FileStats *pfs)                        { return pFile->Stat(pfs); }
    
    virtual int         GetErrorCode()                              { return pFile->GetErrorCode(); }
    
    // ** Stream implementation & I/O
    virtual int         Write(const uint8_t *pbuffer, int numBytes)   { return pFile->Write(pbuffer,numBytes); }  
    virtual int         Read(uint8_t *pbuffer, int numBytes)          { return pFile->Read(pbuffer,numBytes); }   
    
    virtual int         SkipBytes(int numBytes)                     { return pFile->SkipBytes(numBytes); }      
    
    virtual int         BytesAvailable()                            { return pFile->BytesAvailable(); } 
    
    virtual bool        Flush()                                     { return pFile->Flush(); }
                                                                    
    // Seeking                                                      
    virtual int         Seek(int offset, int origin=Seek_Set)       { return pFile->Seek(offset,origin); }
    virtual int64_t     LSeek(int64_t offset, int origin=Seek_Set)   { return pFile->LSeek(offset,origin); }

    virtual int         CopyFromStream(File *pstream, int byteSize) { return pFile->CopyFromStream(pstream,byteSize); }
                        
    // Closing the file 
    virtual bool        Close()                                     { return pFile->Close(); }    
};


//-----------------------------------------------------------------------------------
// ***** Buffered File

// This file class adds buffering to an existing file
// Buffered file never fails by itself; if there's not
// enough memory for buffer, no buffer's used

class BufferedFile : public DelegatedFile
{   
protected:  
    enum BufferModeType
    {
        NoBuffer,
        ReadBuffer,
        WriteBuffer
    };

    // Buffer & the mode it's in
    uint8_t*          pBuffer;
    BufferModeType  BufferMode;
    // Position in buffer
    unsigned        Pos;
    // Data in buffer if reading
    unsigned        DataSize;
    // Underlying file position 
    uint64_t        FilePos;

    // Initializes buffering to a certain mode
    bool    SetBufferMode(BufferModeType mode);
    // Flushes buffer
    // WriteBuffer - write data to disk, ReadBuffer - reset buffer & fix file position  
    void    FlushBuffer();
    // Loads data into ReadBuffer
    // WARNING: Right now LoadBuffer() assumes the buffer's empty
    void    LoadBuffer();

    // Hidden constructor
    BufferedFile();
    inline BufferedFile(const BufferedFile &source) : DelegatedFile() { OVR_UNUSED(source); }
public:

    // Constructor
    // - takes another file as source
    BufferedFile(File *pfile);
    ~BufferedFile();
    
    
    // ** Overridden functions

    // We override all the functions that can possibly
    // require buffer mode switch, flush, or extra calculations
    virtual int         Tell();
    virtual int64_t     LTell();

    virtual int         GetLength();
    virtual int64_t     LGetLength();

//  virtual bool        Stat(GFileStats *pfs);  

    virtual int         Write(const uint8_t *pbufer, int numBytes);
    virtual int         Read(uint8_t *pbufer, int numBytes);

    virtual int         SkipBytes(int numBytes);

    virtual int         BytesAvailable();

    virtual bool        Flush();

    virtual int         Seek(int offset, int origin=Seek_Set);
    virtual int64_t     LSeek(int64_t offset, int origin=Seek_Set);

    virtual int         CopyFromStream(File *pstream, int byteSize);
    
    virtual bool        Close();    
};                          


//-----------------------------------------------------------------------------------
// ***** Memory File

class MemoryFile : public File
{
public:

    const char* GetFilePath()       { return FilePath.ToCStr(); }

    bool        IsValid()           { return Valid; }
    bool        IsWritable()        { return false; }

    bool        Flush()             { return true; }
    int         GetErrorCode()      { return 0; }

    int         Tell()              { return FileIndex; }
    int64_t     LTell()             { return (int64_t) FileIndex; }

    int         GetLength()         { return FileSize; }
    int64_t     LGetLength()        { return (int64_t) FileSize; }

    bool        Close()
    {
        Valid = false;
        return false;
    }

    int         CopyFromStream(File *pstream, int byteSize)
    {   OVR_UNUSED2(pstream, byteSize);
        return 0;
    }

    int         Write(const uint8_t *pbuffer, int numBytes)
    {   OVR_UNUSED2(pbuffer, numBytes);
        return 0;
    }

    int         Read(uint8_t *pbufer, int numBytes)
    {
        if (FileIndex + numBytes > FileSize)
        {
            numBytes = FileSize - FileIndex;
        }

        if (numBytes > 0)
        {
            ::memcpy (pbufer, &FileData [FileIndex], numBytes);

            FileIndex += numBytes;
        }

        return numBytes;
    }

    int         SkipBytes(int numBytes)
    {
        if (FileIndex + numBytes > FileSize)
        {
            numBytes = FileSize - FileIndex;
        }

        FileIndex += numBytes;

        return numBytes;
    }

    int         BytesAvailable()
    {
        return (FileSize - FileIndex);
    }

    int         Seek(int offset, int origin = Seek_Set)
    {
        switch (origin)
        {
        case Seek_Set : FileIndex  = offset;               break;
        case Seek_Cur : FileIndex += offset;               break;
        case Seek_End : FileIndex  = FileSize - offset;  break;
        }

        return FileIndex;
    }

    int64_t     LSeek(int64_t offset, int origin = Seek_Set)
    {
        return (int64_t) Seek((int) offset, origin);
    }

public:

    MemoryFile (const String& fileName, const uint8_t *pBuffer, int buffSize)
        : FilePath(fileName)
    {
        FileData  = pBuffer;
        FileSize  = buffSize;
        FileIndex = 0;
        Valid     = (!fileName.IsEmpty() && pBuffer && buffSize > 0) ? true : false;
    }

    // pfileName should be encoded as UTF-8 to support international file names.
    MemoryFile (const char* pfileName, const uint8_t *pBuffer, int buffSize)
        : FilePath(pfileName)
    {
        FileData  = pBuffer;
        FileSize  = buffSize;
        FileIndex = 0;
        Valid     = (pfileName && pBuffer && buffSize > 0) ? true : false;
    }
private:

    String       FilePath;
    const uint8_t *FileData;
    int          FileSize;
    int          FileIndex;
    bool         Valid;
};


// ***** Global path helpers

// Find trailing short filename in a path.
const char* OVR_CDECL GetShortFilename(const char* purl);

} // OVR

#endif
