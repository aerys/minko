/************************************************************************************

PublicHeader:   OVR_Kernel.h
Filename    :   OVR_String.h
Content     :   String UTF8 string implementation with copy-on-write semantics
                (thread-safe for assignment but not modification).
Created     :   September 19, 2012
Notes       : 

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

#ifndef OVR_String_h
#define OVR_String_h

#include "OVR_Types.h"
#include "OVR_Allocator.h"
#include "OVR_UTF8Util.h"
#include "OVR_Atomic.h"
#include "OVR_Std.h"
#include "OVR_Alg.h"

namespace OVR {

// ***** Classes

class String;
class StringBuffer;


//-----------------------------------------------------------------------------------
// ***** String Class 

// String is UTF8 based string class with copy-on-write implementation
// for assignment.

class String
{
protected:

    enum FlagConstants
    {
        //Flag_GetLength      = 0x7FFFFFFF,
        // This flag is set if GetLength() == GetSize() for a string.
        // Avoid extra scanning is Substring and indexing logic.
        Flag_LengthIsSizeShift   = (sizeof(size_t)*8 - 1)
    };


    // Internal structure to hold string data
    struct DataDesc
    {
        // Number of bytes. Will be the same as the number of chars if the characters
        // are ascii, may not be equal to number of chars in case string data is UTF8.
        size_t  Size;       
        volatile int32_t RefCount;
        char    Data[1];

        void    AddRef()
        {
            AtomicOps<int32_t>::ExchangeAdd_NoSync(&RefCount, 1);
        }
        // Decrement ref count. This needs to be thread-safe, since
        // a different thread could have also decremented the ref count.
        // For example, if u start off with a ref count = 2. Now if u
        // decrement the ref count and check against 0 in different
        // statements, a different thread can also decrement the ref count
        // in between our decrement and checking against 0 and will find
        // the ref count = 0 and delete the object. This will lead to a crash
        // when context switches to our thread and we'll be trying to delete
        // an already deleted object. Hence decrementing the ref count and
        // checking against 0 needs to made an atomic operation.
        void    Release()
        {
            if ((AtomicOps<int32_t>::ExchangeAdd_NoSync(&RefCount, -1) - 1) == 0)
                OVR_FREE(this);
        }

        static size_t GetLengthFlagBit()     { return size_t(1) << Flag_LengthIsSizeShift; }
        size_t      GetSize() const         { return Size & ~GetLengthFlagBit() ; }
        size_t      GetLengthFlag()  const  { return Size & GetLengthFlagBit(); }
        bool        LengthIsSize() const    { return GetLengthFlag() != 0; }
    };

    // Heap type of the string is encoded in the lower bits.
    enum HeapType
    {
        HT_Global   = 0,    // Heap is global.
        HT_Local    = 1,    // SF::String_loc: Heap is determined based on string's address.
        HT_Dynamic  = 2,    // SF::String_temp: Heap is stored as a part of the class.
        HT_Mask     = 3
    };

    union {
        DataDesc* pData;
        size_t    HeapTypeBits;
    };
    typedef union {
        DataDesc* pData;
        size_t    HeapTypeBits;
    } DataDescUnion;

    inline HeapType    GetHeapType() const { return (HeapType) (HeapTypeBits & HT_Mask); }

    inline DataDesc*   GetData() const
    {
        DataDescUnion u;
        u.pData    = pData;
        u.HeapTypeBits = (u.HeapTypeBits & ~(size_t)HT_Mask);
        return u.pData;
    }
    
    inline void        SetData(DataDesc* pdesc)
    {
        HeapType ht = GetHeapType();
        pData = pdesc;
        OVR_ASSERT((HeapTypeBits & HT_Mask) == 0);
        HeapTypeBits |= ht;        
    }

    
    DataDesc*   AllocData(size_t size, size_t lengthIsSize);
    DataDesc*   AllocDataCopy1(size_t size, size_t lengthIsSize,
                               const char* pdata, size_t copySize);
    DataDesc*   AllocDataCopy2(size_t size, size_t lengthIsSize,
                               const char* pdata1, size_t copySize1,
                               const char* pdata2, size_t copySize2);

    // Special constructor to avoid data initalization when used in derived class.
    struct NoConstructor { };
    String(const NoConstructor&) { }

public:

    // For initializing string with dynamic buffer
    struct InitStruct
    {
        virtual ~InitStruct() { }
        virtual void InitString(char* pbuffer, size_t size) const = 0;
    };


    // Constructors / Destructors.
    String();
    String(const char* data);
    String(const char* data1, const char* pdata2, const char* pdata3 = 0);
    String(const char* data, size_t buflen);
    String(const String& src);
    String(const StringBuffer& src);
    String(const InitStruct& src, size_t size);
    explicit String(const wchar_t* data);      

    // Destructor (Captain Obvious guarantees!)
    ~String()
    {
        GetData()->Release();
    }

    // Declaration of NullString
    static DataDesc NullData;


    // *** General Functions

    void        Clear();

    // For casting to a pointer to char.
    operator const char*() const        { return GetData()->Data; }
    // Pointer to raw buffer.
    const char* ToCStr() const          { return GetData()->Data; }

    // Returns number of bytes
    size_t      GetSize() const         { return GetData()->GetSize() ; }
    // Tells whether or not the string is empty
    bool        IsEmpty() const         { return GetSize() == 0; }

    // Returns  number of characters
    size_t      GetLength() const;
    int         GetLengthI() const      { return (int)GetLength(); }

    // Returns  character at the specified index
    uint32_t    GetCharAt(size_t index) const;
    uint32_t    GetFirstCharAt(size_t index, const char** offset) const;
    uint32_t    GetNextChar(const char** offset) const;

    // Appends a character
    void        AppendChar(uint32_t ch);

    // Append a string
    void        AppendString(const wchar_t* pstr, intptr_t len = -1);
    void        AppendString(const char* putf8str, intptr_t utf8StrSz = -1);

    // Assigned a string with dynamic data (copied through initializer).
    void        AssignString(const InitStruct& src, size_t size);
    // Assigns string with known size.
    void        AssignString(const char* putf8str, size_t size);

    //  Resize the string to the new size
//  void        Resize(size_t _size);

    // Removes the character at posAt
    void        Remove(size_t posAt, intptr_t len = 1);

    // Returns a String that's a substring of this.
    //  -start is the index of the first UTF8 character you want to include.
    //  -end is the index one past the last UTF8 character you want to include.
    String   Substring(size_t start, size_t end) const;

    // Case-conversion
    String   ToUpper() const;
    String   ToLower() const;

    // Inserts substr at posAt
    String&    Insert (const char* substr, size_t posAt, intptr_t len = -1);

    // Inserts character at posAt
    size_t      InsertCharAt(uint32_t c, size_t posAt);

    // Inserts substr at posAt, which is an index of a character (not byte).
    // Of size is specified, it is in bytes.
//  String&    Insert(const uint32_t* substr, size_t posAt, intptr_t size = -1);

    // Get Byte index of the character at position = index
    size_t      GetByteIndex(size_t index) const { return (size_t)UTF8Util::GetByteIndex(index, GetData()->Data); }

    // Utility: case-insensitive string compare.  stricmp() & strnicmp() are not
    // ANSI or POSIX, do not seem to appear in Linux.
    static int OVR_STDCALL   CompareNoCase(const char* a, const char* b);
    static int OVR_STDCALL   CompareNoCase(const char* a, const char* b, intptr_t len);

    // Hash function, case-insensitive
    static size_t OVR_STDCALL BernsteinHashFunctionCIS(const void* pdataIn, size_t size, size_t seed = 5381);

    // Hash function, case-sensitive
    static size_t OVR_STDCALL BernsteinHashFunction(const void* pdataIn, size_t size, size_t seed = 5381);


    // ***** File path parsing helper functions.
    // Implemented in OVR_String_FilePath.cpp.

    // Absolute paths can star with:
    //  - protocols:        'file://', 'http://'
    //  - windows drive:    'c:\'
    //  - UNC share name:   '\\share'
    //  - unix root         '/'
    static bool HasAbsolutePath(const char* path);
    static bool HasExtension(const char* path);
    static bool HasProtocol(const char* path);

    bool    HasAbsolutePath() const { return HasAbsolutePath(ToCStr()); }
    bool    HasExtension() const    { return HasExtension(ToCStr()); }
    bool    HasProtocol() const     { return HasProtocol(ToCStr()); }

    String  GetProtocol() const;    // Returns protocol, if any, with trailing '://'.
    String  GetPath() const;        // Returns path with trailing '/'.
    String  GetFilename() const;    // Returns filename, including extension.
    String  GetExtension() const;   // Returns extension with a dot.

    void    StripProtocol();        // Strips front protocol, if any, from the string.
    void    StripExtension();       // Strips off trailing extension.
    

    // Operators
    // Assignment
    void        operator =  (const char* str);
    void        operator =  (const wchar_t* str);
    void        operator =  (const String& src);
    void        operator =  (const StringBuffer& src);

    // Addition
    void        operator += (const String& src);
    void        operator += (const char* psrc)       { AppendString(psrc); }
    void        operator += (const wchar_t* psrc)    { AppendString(psrc); }
    void        operator += (char  ch)               { AppendChar(ch); }
    String      operator +  (const char* str) const;
    String      operator +  (const String& src)  const;

    // Comparison
    bool        operator == (const String& str) const
    {
        return (OVR_strcmp(GetData()->Data, str.GetData()->Data)== 0);
    }

    bool        operator != (const String& str) const
    {
        return !operator == (str);
    }

    bool        operator == (const char* str) const
    {
        return OVR_strcmp(GetData()->Data, str) == 0;
    }

    bool        operator != (const char* str) const
    {
        return !operator == (str);
    }

    bool        operator <  (const char* pstr) const
    {
        return OVR_strcmp(GetData()->Data, pstr) < 0;
    }

    bool        operator <  (const String& str) const
    {
        return *this < str.GetData()->Data;
    }

    bool        operator >  (const char* pstr) const
    {
        return OVR_strcmp(GetData()->Data, pstr) > 0;
    }

    bool        operator >  (const String& str) const
    {
        return *this > str.GetData()->Data;
    }

    int CompareNoCase(const char* pstr) const
    {
        return CompareNoCase(GetData()->Data, pstr);
    }
    int CompareNoCase(const String& str) const
    {
        return CompareNoCase(GetData()->Data, str.ToCStr());
    }

    // Accesses raw bytes
    const char&     operator [] (int index) const
    {
        OVR_ASSERT(index >= 0 && (size_t)index < GetSize());
        return GetData()->Data[index];
    }
    const char&     operator [] (size_t index) const
    {
        OVR_ASSERT(index < GetSize());
        return GetData()->Data[index];
    }


    // Case insensitive keys are used to look up insensitive string in hash tables
    // for SWF files with version before SWF 7.
    struct NoCaseKey
    {   
        const String* pStr;
        NoCaseKey(const String &str) : pStr(&str){};
    };

    bool    operator == (const NoCaseKey& strKey) const
    {
        return (CompareNoCase(ToCStr(), strKey.pStr->ToCStr()) == 0);
    }
    bool    operator != (const NoCaseKey& strKey) const
    {
        return !(CompareNoCase(ToCStr(), strKey.pStr->ToCStr()) == 0);
    }

    // Hash functor used for strings.
    struct HashFunctor
    {    
        size_t operator()(const String& data) const
        {
            size_t size = data.GetSize();
            return String::BernsteinHashFunction((const char*)data, size);
        }        
    };
    // Case-insensitive hash functor used for strings. Supports additional
    // lookup based on NoCaseKey.
    struct NoCaseHashFunctor
    {    
        size_t operator()(const String& data) const
        {
            size_t size = data.GetSize();
            return String::BernsteinHashFunctionCIS((const char*)data, size);
        }
        size_t operator()(const NoCaseKey& data) const
        {       
            size_t size = data.pStr->GetSize();
            return String::BernsteinHashFunctionCIS((const char*)data.pStr->ToCStr(), size);
        }
    };

};


//-----------------------------------------------------------------------------------
// ***** String Buffer used for Building Strings

class StringBuffer
{
    char*           pData;
    size_t          Size;
    size_t          BufferSize;
    size_t          GrowSize;    
    mutable bool    LengthIsSize;    

public:

    // Constructors / Destructor.    
    StringBuffer();
    explicit StringBuffer(size_t growSize);
    StringBuffer(const char* data);
    StringBuffer(const char* data, size_t buflen);
    StringBuffer(const String& src);
    StringBuffer(const StringBuffer& src);
    explicit StringBuffer(const wchar_t* data);
    ~StringBuffer();
    

    // Modify grow size used for growing/shrinking the buffer.
    size_t      GetGrowSize() const         { return GrowSize; }
    void        SetGrowSize(size_t growSize);
    

    // *** General Functions
    // Does not release memory, just sets Size to 0
    void        Clear();

    // For casting to a pointer to char.
    operator const char*() const        { return (pData) ? pData : ""; }
    // Pointer to raw buffer.
    const char* ToCStr() const          { return (pData) ? pData : ""; }

    // Returns number of bytes.
    size_t      GetSize() const         { return Size ; }
    // Tells whether or not the string is empty.
    bool        IsEmpty() const         { return GetSize() == 0; }

    // Returns  number of characters
    size_t      GetLength() const;

    // Returns  character at the specified index
    uint32_t    GetCharAt(size_t index) const;
    uint32_t    GetFirstCharAt(size_t index, const char** offset) const;
    uint32_t    GetNextChar(const char** offset) const;


    //  Resize the string to the new size
    void        Resize(size_t _size);
    void        Reserve(size_t _size);

    // Appends a character
    void        AppendChar(uint32_t ch);

    // Append a string
    void        AppendString(const wchar_t* pstr, intptr_t len = -1);
    void        AppendString(const char* putf8str, intptr_t utf8StrSz = -1);
    void        AppendFormat(const char* format, ...);

    // Assigned a string with dynamic data (copied through initializer).
    //void        AssignString(const InitStruct& src, size_t size);

    // Inserts substr at posAt
    void        Insert (const char* substr, size_t posAt, intptr_t len = -1);
    // Inserts character at posAt
    size_t      InsertCharAt(uint32_t c, size_t posAt);

    // Assignment
    void        operator =  (const char* str);
    void        operator =  (const wchar_t* str);
    void        operator =  (const String& src);
    void        operator =  (const StringBuffer& src);

    // Addition
    void        operator += (const String& src)      { AppendString(src.ToCStr(),src.GetSize()); }
    void        operator += (const char* psrc)       { AppendString(psrc); }
    void        operator += (const wchar_t* psrc)    { AppendString(psrc); }
    void        operator += (char  ch)               { AppendChar(ch); }
    //String   operator +  (const char* str) const ;
    //String   operator +  (const String& src)  const ;

    // Accesses raw bytes
    char&       operator [] (int index)
    {
        OVR_ASSERT(((size_t)index) < GetSize());
        return pData[index];
    }
    char&       operator [] (size_t index)
    {
        OVR_ASSERT(index < GetSize());
        return pData[index];
    }

    const char&     operator [] (int index) const 
    {
        OVR_ASSERT(((size_t)index) < GetSize());
        return pData[index];
    }
    const char&     operator [] (size_t index) const
    {
        OVR_ASSERT(index < GetSize());
        return pData[index];
    }
};


//
// Wrapper for string data. The data must have a guaranteed 
// lifespan throughout the usage of the wrapper. Not intended for 
// cached usage. Not thread safe.
//
class StringDataPtr
{
public:
    StringDataPtr() : pStr(NULL), Size(0) {}
    StringDataPtr(const StringDataPtr& p)
        : pStr(p.pStr), Size(p.Size) {}
    StringDataPtr(const char* pstr, size_t sz)
        : pStr(pstr), Size(sz) {}
    StringDataPtr(const char* pstr)
        : pStr(pstr), Size((pstr != NULL) ? OVR_strlen(pstr) : 0) {}
    explicit StringDataPtr(const String& str)
        : pStr(str.ToCStr()), Size(str.GetSize()) {}
    template <typename T, int N> 
    StringDataPtr(const T (&v)[N])
        : pStr(v), Size(N) {}

public:
    const char* ToCStr() const { return pStr; }
    size_t      GetSize() const { return Size; }
    bool        IsEmpty() const { return GetSize() == 0; }

    // value is a prefix of this string
    // Character's values are not compared.
    bool        IsPrefix(const StringDataPtr& value) const
    {
        return ToCStr() == value.ToCStr() && GetSize() >= value.GetSize();
    }
    // value is a suffix of this string
    // Character's values are not compared.
    bool        IsSuffix(const StringDataPtr& value) const
    {
        return ToCStr() <= value.ToCStr() && (End()) == (value.End());
    }

    // Find first character.
    // init_ind - initial index.
    intptr_t    FindChar(char c, size_t init_ind = 0) const 
    {
        for (size_t i = init_ind; i < GetSize(); ++i)
            if (pStr[i] == c)
                return static_cast<intptr_t>(i);

        return -1; 
    }

    // Find last character.
    // init_ind - initial index.
    intptr_t    FindLastChar(char c, size_t init_ind = ~0) const 
    {
        if (init_ind == (size_t)~0 || init_ind > GetSize())
            init_ind = GetSize();
        else
            ++init_ind;

        for (size_t i = init_ind; i > 0; --i)
            if (pStr[i - 1] == c)
                return static_cast<intptr_t>(i - 1);

        return -1; 
    }

    // Create new object and trim size bytes from the left.
    StringDataPtr  GetTrimLeft(size_t size) const
    {
        // Limit trim size to the size of the string.
        size = Alg::PMin(GetSize(), size);

        return StringDataPtr(ToCStr() + size, GetSize() - size);
    }
    // Create new object and trim size bytes from the right.
    StringDataPtr  GetTrimRight(size_t size) const
    {
        // Limit trim to the size of the string.
        size = Alg::PMin(GetSize(), size);

        return StringDataPtr(ToCStr(), GetSize() - size);
    }

    // Create new object, which contains next token.
    // Useful for parsing.
    StringDataPtr GetNextToken(char separator = ':') const
    {
        size_t cur_pos = 0;
        const char* cur_str = ToCStr();

        for (; cur_pos < GetSize() && cur_str[cur_pos]; ++cur_pos)
        {
            if (cur_str[cur_pos] == separator)
            {
                break;
            }
        }

        return StringDataPtr(ToCStr(), cur_pos);
    }

    // Trim size bytes from the left.
    StringDataPtr& TrimLeft(size_t size)
    {
        // Limit trim size to the size of the string.
        size = Alg::PMin(GetSize(), size);
        pStr += size;
        Size -= size;

        return *this;
    }
    // Trim size bytes from the right.
    StringDataPtr& TrimRight(size_t size)
    {
        // Limit trim to the size of the string.
        size = Alg::PMin(GetSize(), size);
        Size -= size;

        return *this;
    }

    const char* Begin() const { return ToCStr(); }
    const char* End() const { return ToCStr() + GetSize(); }

    // Hash functor used string data pointers
    struct HashFunctor
    {    
        size_t operator()(const StringDataPtr& data) const
        {
            return String::BernsteinHashFunction(data.ToCStr(), data.GetSize());
        }        
    };

    bool operator== (const StringDataPtr& data) const 
    {
        return (OVR_strncmp(pStr, data.pStr, data.Size) == 0);
    }

protected:
    const char* pStr;
    size_t      Size;
};

} // OVR

#endif
