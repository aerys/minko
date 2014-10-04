/************************************************************************************

PublicHeader:   OVR_Kernel.h
Filename    :   OVR_Types.h
Content     :   Standard library defines and simple types
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

#ifndef OVR_Types_H
#define OVR_Types_H

#include "OVR_Compiler.h"


// Unsupported compiler configurations
#if _MSC_VER == 0x1600
#  if _MSC_FULL_VER < 160040219
#     error "Oculus does not support VS2010 without SP1 installed: It will crash in Release mode"
#  endif
#endif


//-----------------------------------------------------------------------------------
// ****** Operating System
//
// Type definitions exist for the following operating systems: (OVR_OS_x)
//
//    WIN32    - Win32 (Windows 95/98/ME and Windows NT/2000/XP)
//    DARWIN   - Darwin OS (Mac OS X)
//    LINUX    - Linux
//    ANDROID  - Android
//    IPHONE   - iPhone

#if (defined(__APPLE__) && (defined(__GNUC__) ||\
     defined(__xlC__) || defined(__xlc__))) || defined(__MACOS__)
#  if (defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED))
#    define OVR_OS_IPHONE
#  else
#    define OVR_OS_DARWIN
#    define OVR_OS_MAC
#  endif
#elif (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define OVR_OS_WIN32
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  define OVR_OS_WIN32
#elif defined(__linux__) || defined(__linux)
#  define OVR_OS_LINUX
#else
#  define OVR_OS_OTHER
#endif

#if defined(ANDROID)
#  define OVR_OS_ANDROID
#endif


//-----------------------------------------------------------------------------------
// ***** CPU Architecture
//
// The following CPUs are defined: (OVR_CPU_x)
//
//    X86        - x86 (IA-32)
//    X86_64     - x86_64 (amd64)
//    PPC        - PowerPC
//    PPC64      - PowerPC64
//    MIPS       - MIPS
//    OTHER      - CPU for which no special support is present or needed


#if defined(__x86_64__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#  define OVR_CPU_X86_64
#  define OVR_64BIT_POINTERS
#elif defined(__i386__) || defined(OVR_OS_WIN32)
#  define OVR_CPU_X86
#elif defined(__powerpc64__)
#  define OVR_CPU_PPC64
#elif defined(__ppc__)
#  define OVR_CPU_PPC
#elif defined(__mips__) || defined(__MIPSEL__)
#  define OVR_CPU_MIPS
#elif defined(__arm__)
#  define OVR_CPU_ARM
#else
#  define OVR_CPU_OTHER
#endif

//-----------------------------------------------------------------------------------
// ***** Co-Processor Architecture
//
// The following co-processors are defined: (OVR_CPU_x)
//
//    SSE        - Available on all modern x86 processors.
//    Altivec    - Available on all modern ppc processors.
//    Neon       - Available on some armv7+ processors.

#if defined(__SSE__) || defined(OVR_OS_WIN32)
#  define  OVR_CPU_SSE
#endif // __SSE__

#if defined( __ALTIVEC__ )
#  define OVR_CPU_ALTIVEC
#endif // __ALTIVEC__

#if defined(__ARM_NEON__)
#  define OVR_CPU_ARM_NEON
#endif // __ARM_NEON__


//-----------------------------------------------------------------------------------
// ***** Compiler Warnings

// Disable MSVC warnings
#if defined(OVR_CC_MSVC)
#  pragma warning(disable : 4127)    // Inconsistent dll linkage
#  pragma warning(disable : 4530)    // Exception handling
#  if (OVR_CC_MSVC<1300)
#    pragma warning(disable : 4514)  // Unreferenced inline function has been removed
#    pragma warning(disable : 4710)  // Function not inlined
#    pragma warning(disable : 4714)  // _force_inline not inlined
#    pragma warning(disable : 4786)  // Debug variable name longer than 255 chars
#  endif // (OVR_CC_MSVC<1300)
#endif // (OVR_CC_MSVC)



// *** Linux Unicode - must come before Standard Includes

#ifdef OVR_OS_LINUX
// Use glibc unicode functions on linux.
#  ifndef  _GNU_SOURCE
#    define _GNU_SOURCE
#  endif
#endif

//-----------------------------------------------------------------------------------
// ***** Standard Includes
//
#include    <stddef.h>
#include    <limits.h>
#include    <float.h>


// MSVC Based Memory Leak checking - for now
#if defined(OVR_CC_MSVC) && defined(OVR_BUILD_DEBUG)
#  define _CRTDBG_MAP_ALLOC
#  include <stdlib.h>
#  include <crtdbg.h>
#endif


//-----------------------------------------------------------------------------------
// ***** int8_t, int16_t, etc.

#if defined(OVR_CC_MSVC) && (OVR_CC_VER <= 1500) // VS2008 and earlier
    typedef signed char        int8_t; 
    typedef unsigned char     uint8_t;
    typedef signed short      int16_t;
    typedef unsigned short   uint16_t;
    typedef signed int        int32_t;
    typedef unsigned int     uint32_t;
    typedef signed __int64    int64_t;
    typedef unsigned __int64 uint64_t;
#else
    #include <stdint.h>
#endif


//-----------------------------------------------------------------------------------
// ***** Type definitions for Common Systems

namespace OVR {

typedef char            Char;

// Pointer-sized integer
typedef size_t          UPInt;
typedef ptrdiff_t       SPInt;


#if defined(OVR_OS_WIN32)

typedef char            SByte;  // 8 bit Integer (Byte)
typedef unsigned char   UByte;
typedef short           SInt16; // 16 bit Integer (Word)
typedef unsigned short  UInt16;
typedef long            SInt32; // 32 bit Integer
typedef unsigned long   UInt32;
typedef __int64         SInt64; // 64 bit Integer (QWord)
typedef unsigned __int64 UInt64;

 
#elif defined(OVR_OS_MAC) || defined(OVR_OS_IPHONE) || defined(OVR_CC_GNU)

typedef int             SByte  __attribute__((__mode__ (__QI__)));
typedef unsigned int    UByte  __attribute__((__mode__ (__QI__)));
typedef int             SInt16 __attribute__((__mode__ (__HI__)));
typedef unsigned int    UInt16 __attribute__((__mode__ (__HI__)));
typedef int             SInt32 __attribute__((__mode__ (__SI__)));
typedef unsigned int    UInt32 __attribute__((__mode__ (__SI__)));
typedef int             SInt64 __attribute__((__mode__ (__DI__)));
typedef unsigned int    UInt64 __attribute__((__mode__ (__DI__)));

#else

#include <sys/types.h>
typedef int8_t          SByte;
typedef uint8_t         UByte;
typedef int16_t         SInt16;
typedef uint16_t        UInt16;
typedef int32_t         SInt32;
typedef uint32_t        UInt32;
typedef int64_t         SInt64;
typedef uint64_t        UInt64;

#endif
    
    
//osx PID is a signed int32 (already defined to pid_t in OSX framework)
//linux PID is a signed int32 (already defined)
//win32 PID is an unsigned int64
#ifdef OVR_OS_WIN32
//process ID representation
typedef unsigned long pid_t;
#endif

struct OVR_GUID
{
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t  Data4[8];
};



} // OVR



//-----------------------------------------------------------------------------------
// ****** Standard C/C++ Library
//
// Identifies which standard library is currently being used. 
//
//    LIBSTDCPP   - GNU libstdc++, used by GCC.
//    LIBCPP      - LLVM libc++, typically used by clang and GCC.
//    DINKUMWARE  - Used by Microsoft and various non-Microsoft compilers (e.g. Sony clang).

#if !defined(OVR_STDLIB_LIBSTDCPP)
    #if defined(__GLIBCXX__)
        #define OVR_STDLIB_LIBSTDCPP 1
    #endif
#endif

#if !defined(OVR_STDLIB_LIBCPP)
    #if defined(__clang__)
        #if defined(__cplusplus) && __has_include(<__config>)
            #define OVR_STDLIB_LIBCPP 1
        #endif
    #endif 
#endif

#if !defined(OVR_STDLIB_DINKUMWARE)
    #if defined(_YVALS) // Dinkumware globally #defines _YVALS from the #includes above.
        #define OVR_STDLIB_DINKUMWARE 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** Macro Definitions
//
// We define the following:
//
//  OVR_BYTE_ORDER      - Defined to either OVR_LITTLE_ENDIAN or OVR_BIG_ENDIAN
//  OVR_FORCE_INLINE    - Forces inline expansion of function
//  OVR_ASM             - Assembly language prefix
//  OVR_STR             - Prefixes string with L"" if building unicode
// 
//  OVR_STDCALL         - Use stdcall calling convention (Pascal arg order)
//  OVR_CDECL           - Use cdecl calling convention (C argument order)
//  OVR_FASTCALL        - Use fastcall calling convention (registers)
//

// Byte order constants, OVR_BYTE_ORDER is defined to be one of these.
#define OVR_LITTLE_ENDIAN       1
#define OVR_BIG_ENDIAN          2


#if defined(OVR_OS_WIN32)
    
    // ***** Win32

    // Byte order
    #define OVR_BYTE_ORDER    OVR_LITTLE_ENDIAN

    // Calling convention - goes after function return type but before function name
    #ifdef __cplusplus_cli
    #  define OVR_FASTCALL      __stdcall
    #else
    #  define OVR_FASTCALL      __fastcall
    #endif

    #define OVR_STDCALL         __stdcall
    #define OVR_CDECL           __cdecl


    // Assembly macros
    #if defined(OVR_CC_MSVC)
    #  define OVR_ASM           _asm
    #else
    #  define OVR_ASM           asm
    #endif // (OVR_CC_MSVC)

    #ifdef UNICODE
    #  define OVR_STR(str)      L##str
    #else
    #  define OVR_STR(str)      str
    #endif // UNICODE

#else

    // **** Standard systems

    #if (defined(BYTE_ORDER) && (BYTE_ORDER == BIG_ENDIAN))|| \
        (defined(_BYTE_ORDER) && (_BYTE_ORDER == _BIG_ENDIAN))
    #  define OVR_BYTE_ORDER    OVR_BIG_ENDIAN
    #elif (defined(__ARMEB__) || defined(OVR_CPU_PPC) || defined(OVR_CPU_PPC64))
    #  define OVR_BYTE_ORDER    OVR_BIG_ENDIAN
    #else
    #  define OVR_BYTE_ORDER    OVR_LITTLE_ENDIAN
    #endif
    
    // Assembly macros
    #define OVR_ASM                  __asm__
    #define OVR_ASM_PROC(procname)   OVR_ASM
    #define OVR_ASM_END              OVR_ASM
    
    // Calling convention - goes after function return type but before function name
    #define OVR_FASTCALL
    #define OVR_STDCALL
    #define OVR_CDECL

#endif // defined(OVR_OS_WIN32)


//-----------------------------------------------------------------------------------
// ***** OVR_PTR_SIZE
// 
// Specifies the byte size of pointers (same as sizeof void*).

#if !defined(OVR_PTR_SIZE)
    #if defined(__WORDSIZE)
        #define OVR_PTR_SIZE ((__WORDSIZE) / 8)
    #elif defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(_M_IA64) || defined(__ia64__) || defined(__arch64__) || defined(__64BIT__) || defined(__Ptr_Is_64)
        #define OVR_PTR_SIZE 8
    #elif defined(__CC_ARM) && (__sizeof_ptr == 8)
        #define OVR_PTR_SIZE 8
    #else
        #define OVR_PTR_SIZE 4
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_WORD_SIZE
// 
// Specifies the byte size of a machine word/register. Not necessarily the same as
// the size of pointers, but usually >= the size of pointers.

#if !defined(OVR_WORD_SIZE)
   #define OVR_WORD_SIZE OVR_PTR_SIZE // For our currently supported platforms these are equal.
#endif


// ------------------------------------------------------------------------
// ***** OVR_FORCE_INLINE
//
// Force inline substitute - goes before function declaration
// Example usage:
//     OVR_FORCE_INLINE void Test();

#if !defined(OVR_FORCE_INLINE)
    #if defined(OVR_CC_MSVC)
        #define OVR_FORCE_INLINE  __forceinline
    #elif defined(OVR_CC_GNU)
        #define OVR_FORCE_INLINE  __attribute__((always_inline)) inline
    #else
        #define OVR_FORCE_INLINE  inline
    #endif  // OVR_CC_MSVC
#endif


// ------------------------------------------------------------------------
// ***** OVR_NO_INLINE
//
// Cannot be used with inline or OVR_FORCE_INLINE.
// Example usage:
//     OVR_NO_INLINE void Test();

#if !defined(OVR_NO_INLINE)
    #if defined(OVR_CC_MSVC) && (_MSC_VER >= 1500) // VS2008+
        #define OVR_NO_INLINE __declspec(noinline)
    #elif !defined(OVR_CC_MSVC)
        #define OVR_NO_INLINE __attribute__((noinline))
    #endif
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_STRINGIZE
//
// Converts a preprocessor symbol to a string.
//
// Example usage:
//     printf("Line: %s", OVR_STRINGIZE(__LINE__));
//
#if !defined(OVR_STRINGIFY)
    #define OVR_STRINGIZEIMPL(x) #x
    #define OVR_STRINGIZE(x)     OVR_STRINGIZEIMPL(x)
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_JOIN
//
// Joins two preprocessing symbols together. Supports the case when either or the
// the symbols are macros themselves.
//
// Example usage:
//    char OVR_JOIN(unique_, __LINE__);  // Results in (e.g.) char unique_123;
//
#if !defined(OVR_JOIN)
    #define OVR_JOIN(a, b)  OVR_JOIN1(a, b)
    #define OVR_JOIN1(a, b) OVR_JOIN2(a, b)
    #define OVR_JOIN2(a, b) a##b
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_OFFSETOF
// 
// Portable implementation of offsetof for structs and classes. offsetof and GCC's 
// __builtin_offsetof work only with POD types (standard-layout types under C++11), 
// despite that it can safely work with a number of types that aren't POD. This 
// version works with more types without generating compiler warnings or errors.
// Returns the offset as a size_t, as per offsetof.
//
// Example usage:
//     struct Test{ int i; float f; };
//     size_t fPos = OVR_OFFSETOF(Test, f);

#if defined(OVR_CC_GNU)
    #define OVR_OFFSETOF(class_, member_) ((size_t)(((uintptr_t)&reinterpret_cast<const volatile char&>((((class_*)65536)->member_))) - 65536))
#else
    #define OVR_OFFSETOF(class_, member_) offsetof(class_, member_)
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_SIZEOF_MEMBER
//
// Implements a portable way to determine the size of struct or class data member. 
// C++11 allows this directly via sizeof (see OVR_CPP_NO_EXTENDED_SIZEOF), and this 
// macro exists to handle pre-C++11 compilers.
// Returns the offset as a size_t, as per sizeof.
//
// Example usage:
//     struct Test{ int i; float f; };
//     size_t fSize = OVR_SIZEOF_MEMBER(Test, f);
//
#if defined(OVR_CPP_NO_EXTENDED_SIZEOF)
    #define OVR_SIZEOF_MEMBER(class_, member_) (sizeof(((class_*)0)->member_))
#else
    #define OVR_SIZEOF_MEMBER(class_, member_) (sizeof(class_::member_))
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_DEBUG_BREAK, OVR_ASSERT
//
// If not in debug build, macros do nothing
#ifndef OVR_BUILD_DEBUG

#  define OVR_DEBUG_CODE(c)
#  define OVR_DEBUG_BREAK  ((void)0)
#  define OVR_ASSERT(p)    ((void)0)

#else 

// Microsoft Win32 specific debugging support
#if defined(OVR_OS_WIN32)
#  ifdef OVR_CPU_X86
#    if defined(__cplusplus_cli)
#      define OVR_DEBUG_BREAK   do { __debugbreak(); } while(0)
#    elif defined(OVR_CC_GNU)
#      define OVR_DEBUG_BREAK   do { OVR_ASM("int $3\n\t"); } while(0)
#    else
#      define OVR_DEBUG_BREAK   do { OVR_ASM int 3 } while (0)
#    endif
#  else
#    define OVR_DEBUG_BREAK     do { __debugbreak(); } while(0)
#  endif
// Unix specific debugging support
#elif defined(OVR_CPU_X86) || defined(OVR_CPU_X86_64)
#  define OVR_DEBUG_BREAK       do { OVR_ASM("int $3\n\t"); } while(0)
#else
#  define OVR_DEBUG_BREAK       do { *((int *) 0) = 1; } while(0)
#endif

#define OVR_DEBUG_CODE(c) c

// This will cause compiler breakpoint
#define OVR_ASSERT(p)           do { if (!(p))  { OVR_DEBUG_BREAK; } } while(0)

#endif // OVR_BUILD_DEBUG


// ------------------------------------------------------------------------
// ***** OVR_COMPILER_ASSERT
//
// Compile-time assert; produces compiler error if condition is false.
// The expression must be a compile-time constant expression.
// 
// Example usage:
//     OVR_COMPILER_ASSERT(sizeof(int32_t == 4));

#if OVR_CPP_NO_STATIC_ASSERT
    #define OVR_COMPILER_ASSERT(x)  { int zero = 0; switch(zero) {case 0: case x:;} }
#else
    #define OVR_COMPILER_ASSERT(x)  static_assert((x), #x)
#endif


// ------------------------------------------------------------------------
// ***** static_assert
//
// Portable support for C++11 static_assert.
// Acts as if the following were declared:
//     void static_assert(bool const_expression, const char* msg);
//
// Example usage:
//     static_assert(sizeof(int32_t) == 4, "int32_t expected to be 4 bytes.");

#if defined(OVR_CPP_NO_STATIC_ASSERT)
    #if defined(OVR_CC_GNU) || defined(OVR_CC_CLANG)
        #define OVR_SA_UNUSED __attribute__((unused))
    #else
        #define OVR_SA_UNUSED
    #endif
    #define OVR_SA_PASTE(a,b) a##b
    #define OVR_SA_HELP(a,b)  OVR_SA_PASTE(a,b)

    #if defined(__COUNTER__)
        #define static_assert(expression, msg) typedef char OVR_SA_HELP(compileTimeAssert, __COUNTER__) [((expression) != 0) ? 1 : -1] OVR_SA_UNUSED
    #else
        #define static_assert(expression, msg) typedef char OVR_SA_HELP(compileTimeAssert, __LINE__) [((expression) != 0) ? 1 : -1] OVR_SA_UNUSED
    #endif
#endif


// ***** OVR_PROCESSOR_PAUSE
//
// Yields the processor for other hyperthreads, usually for the purpose of implementing spins and spin locks. 
//
// Example usage:
//     while(!finished())
//         OVR_PROCESSOR_PAUSE();

#if defined(OVR_CPU_X86) || defined(OVR_CPU_X86_64)
    #if defined(OVR_CC_GNU) || defined(OVR_CC_CLANG)
        #define OVR_PROCESSOR_PAUSE() asm volatile("pause" ::: "memory") // Consumes 38-40 clocks on current Intel x86 and x64 hardware.
    #elif defined(OVR_CC_MSVC)
        #include <emmintrin.h>
        #pragma intrinsic(_mm_pause) // Maps to asm pause.
        #define OVR_PROCESSOR_PAUSE _mm_pause
    #else
        #define OVR_PROCESSOR_PAUSE()
    #endif
#else
    #define OVR_PROCESSOR_PAUSE()
#endif


// ------------------------------------------------------------------------
// ***** OVR_ARRAY_COUNT
//
// Returns the element count of a C array. 
//
// Example usage:
//     float itemArray[16];
//     for(size_t i = 0; i < OVR_ARRAY_COUNT(itemArray); i++) { ... }

#if defined(OVR_CPP_NO_CONSTEXPR)
    #ifndef OVR_ARRAY_COUNT
        #define OVR_ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))
    #endif
#else
    // Smarter C++11 version which knows the difference between arrays and pointers. 
    template <typename T, size_t N>
    char (&OVRArrayCountHelper(T (&x)[N]))[N];
    #define OVR_ARRAY_COUNT(x) (sizeof(OVRArrayCountHelper(x)))
#endif


// ------------------------------------------------------------------------
// ***** OVR_CURRENT_FUNCTION
//
// Portable wrapper for __PRETTY_FUNCTION__, C99 __func__, __FUNCTION__.
// This represents the most expressive version available.
// Acts as if the following were declared:
//     static const char OVR_CURRENT_FUNCTION[] = "function-name";
//
// Example usage:
//     void Test() { printf("%s", OVR_CURRENT_FUNCTION); }

#if defined(OVR_CC_GNU) || defined(OVR_CC_CLANG) || (defined(__ICC) && (__ICC >= 600)) // GCC, clang, Intel
    #define OVR_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__) // VC++
    #define OVR_CURRENT_FUNCTION __FUNCSIG__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901) // C99 compilers
    #define OVR_CURRENT_FUNCTION __func__
#else
    #define OVR_CURRENT_FUNCTION __FUNCTION__
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_DEPRECATED / OVR_DEPRECATED_MSG
// 
// Portably annotates a function or struct as deprecated.
// Note that clang supports __deprecated_enum_msg, which may be useful to support.
//
// Example usage:
//    OVR_DEPRECATED void Test();       // Use on the function declaration, as opposed to definition.
//
//    struct OVR_DEPRECATED Test{ ... };
//
//    OVR_DEPRECATED_MSG("Test is deprecated")
//    void Test();

#if !defined(OVR_DEPRECATED)
    #if defined(OVR_CC_MSVC) && (OVR_CC_VERSION > 1400) // VS2005+
        #define OVR_DEPRECATED          __declspec(deprecated)
        #define OVR_DEPRECATED_MSG(msg) __declspec(deprecated(msg))
    #elif defined(OVR_CC_CLANG) && OVR_CC_HAS_FEATURE(attribute_deprecated_with_message)
        #define OVR_DEPRECATED          __declspec(deprecated)
        #define OVR_DEPRECATED_MSG(msg) __attribute__((deprecated(msg)))
    #elif defined(OVR_CC_GNU) && (OVR_CC_VERSION >= 405)
        #define OVR_DEPRECATED          __declspec(deprecated)
        #define OVR_DEPRECATED_MSG(msg) __attribute__((deprecated(msg)))
    #elif !defined(OVR_CC_MSVC)
        #define OVR_DEPRECATED          __attribute__((deprecated))
        #define OVR_DEPRECATED_MSG(msg) __attribute__((deprecated))
    #else
        #define OVR_DEPRECATED
        #define OVR_DEPRECATED_MSG(msg)
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_UNUSED - Unused Argument handling
// Macro to quiet compiler warnings about unused parameters/variables.
//
// Example usage:
//     void Test() {
//         int x = SomeFunction();
//         OVR_UNUSED(x);
//     }
//

#if defined(OVR_CC_GNU)
#  define   OVR_UNUSED(a)   do {__typeof__ (&a) __attribute__ ((unused)) __tmp = &a; } while(0)
#else
#  define   OVR_UNUSED(a)   (a)
#endif

#define     OVR_UNUSED1(a1) OVR_UNUSED(a1)
#define     OVR_UNUSED2(a1,a2) OVR_UNUSED(a1); OVR_UNUSED(a2)
#define     OVR_UNUSED3(a1,a2,a3) OVR_UNUSED2(a1,a2); OVR_UNUSED(a3)
#define     OVR_UNUSED4(a1,a2,a3,a4) OVR_UNUSED3(a1,a2,a3); OVR_UNUSED(a4)
#define     OVR_UNUSED5(a1,a2,a3,a4,a5) OVR_UNUSED4(a1,a2,a3,a4); OVR_UNUSED(a5)
#define     OVR_UNUSED6(a1,a2,a3,a4,a5,a6) OVR_UNUSED4(a1,a2,a3,a4); OVR_UNUSED2(a5,a6)
#define     OVR_UNUSED7(a1,a2,a3,a4,a5,a6,a7) OVR_UNUSED4(a1,a2,a3,a4); OVR_UNUSED3(a5,a6,a7)
#define     OVR_UNUSED8(a1,a2,a3,a4,a5,a6,a7,a8) OVR_UNUSED4(a1,a2,a3,a4); OVR_UNUSED4(a5,a6,a7,a8)
#define     OVR_UNUSED9(a1,a2,a3,a4,a5,a6,a7,a8,a9) OVR_UNUSED4(a1,a2,a3,a4); OVR_UNUSED5(a5,a6,a7,a8,a9)


//-----------------------------------------------------------------------------------
// ***** Configuration Macros
//
// Expands to the current build type as a const char string literal.
// Acts as the following declaration: const char OVR_BUILD_STRING[];

#ifdef OVR_BUILD_DEBUG
#  define OVR_BUILD_STRING  "Debug"
#else
#  define OVR_BUILD_STRING  "Release"
#endif


//// Enables SF Debugging information
//# define OVR_BUILD_DEBUG

// OVR_DEBUG_STATEMENT injects a statement only in debug builds.
// OVR_DEBUG_SELECT injects first argument in debug builds, second argument otherwise.
#ifdef OVR_BUILD_DEBUG
#define OVR_DEBUG_STATEMENT(s)   s
#define OVR_DEBUG_SELECT(d, nd)  d
#else
#define OVR_DEBUG_STATEMENT(s)
#define OVR_DEBUG_SELECT(d, nd)  nd
#endif


#define OVR_ENABLE_THREADS
//
// Prevents OVR from defining new within
// type macros, so developers can override
// new using the #define new new(...) trick
// - used with OVR_DEFINE_NEW macro
//# define OVR_BUILD_DEFINE_NEW
//


//-----------------------------------------------------------------------------------
// ***** Find normal allocations
//
// Our allocations are all supposed to go through the OVR System Allocator, so that
// they can be run through a game's own preferred allocator.  Occasionally we will
// accidentally introduce new code that doesn't adhere to this contract.  And it
// then becomes difficult to track down these normal allocations.  This piece of
// code makes it easy to check for normal allocations by asserting whenever they
// happen in our code.

//#define OVR_FIND_NORMAL_ALLOCATIONS
#ifdef OVR_FIND_NORMAL_ALLOCATIONS

inline void* operator new (size_t size, const char* filename, int line)
{
    void* ptr = new char[size];
    OVR_ASSERT(false);
    return ptr;
}

#define new new(__FILE__, __LINE__)

#endif // OVR_FIND_NORMAL_ALLOCATIONS



#endif  // OVR_Types_h
