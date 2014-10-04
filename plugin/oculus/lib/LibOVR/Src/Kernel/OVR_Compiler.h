/************************************************************************************

PublicHeader:   OVR.h
Filename    :   OVR_Compiler.h
Content     :   Compiler-specific feature identification and utilities
Created     :   June 19, 2014
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


#ifndef OVR_Compiler_h
#define OVR_Compiler_h

#pragma once


// References
//    https://gcc.gnu.org/projects/cxx0x.html
//    https://gcc.gnu.org/projects/cxx1y.html
//    http://clang.llvm.org/cxx_status.html
//    http://msdn.microsoft.com/en-us/library/hh567368.aspx
//    https://docs.google.com/spreadsheet/pub?key=0AoBblDsbooe4dHZuVTRoSTFBejk5eFBfVk1GWlE5UlE&output=html
//    http://nadeausoftware.com/articles/2012/10/c_c_tip_how_detect_compiler_name_and_version_using_compiler_predefined_macros


//-----------------------------------------------------------------------------------
// ***** Compiler
//
//  The following compilers are defined: (OVR_CC_x)
//
//     MSVC     - Microsoft Visual C/C++
//     INTEL    - Intel C++ for Linux / Windows
//     GNU      - GNU C++
//     ARM      - ARM C/C++

#if defined(__INTEL_COMPILER)
// Intel 4.0                    = 400
// Intel 5.0                    = 500
// Intel 6.0                    = 600
// Intel 8.0                    = 800
// Intel 9.0                    = 900
#  define OVR_CC_INTEL       __INTEL_COMPILER

#elif defined(_MSC_VER)
// MSVC 5.0                     = 1100
// MSVC 6.0                     = 1200
// MSVC 7.0 (VC2002)            = 1300
// MSVC 7.1 (VC2003)            = 1310
// MSVC 8.0 (VC2005)            = 1400
// MSVC 9.0 (VC2008)            = 1500
// MSVC 10.0 (VC2010)           = 1600
// MSVC 11.0 (VC2012)           = 1700
// MSVC 12.0 (VC2013)           = 1800
#  define OVR_CC_MSVC        _MSC_VER

#if _MSC_VER == 0x1600
#  if _MSC_FULL_VER < 160040219
#     error "Oculus does not support VS2010 without SP1 installed."
#  endif
#endif

#elif defined(__GNUC__)
#  define OVR_CC_GNU

#elif defined(__clang__)
#  define OVR_CC_CLANG

#elif defined(__CC_ARM)
#  define OVR_CC_ARM

#else
#  error "Oculus does not support this Compiler"
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CC_VERSION
//
//    M = major version
//    m = minor version
//    p = patch release
//    b = build number
//
//    Compiler      Format   Example
//    ----------------------------
//    OVR_CC_GNU    Mmm      408 means GCC 4.8
//    OVR_CC_CLANG  Mmm      305 means clang 3.5 
//    OVR_CC_MSVC   MMMM     1700 means VS2012
//    OVR_CC_ARM    Mmpbbb   401677 means 4.0, patch 1, build 677
//    OVR_CC_INTEL  MMmm     1210 means 12.10
//    OVR_CC_EDG    Mmm      407 means EDG 4.7
//
#if defined(OVR_CC_GNU)
    #define OVR_CC_VERSION ((__GNUC__ * 100) + __GNUC_MINOR__)
#elif defined(OVR_CC_CLANG)
    #define OVR_CC_VERSION ((__clang_major__ * 100) + __clang_minor__)
#elif defined(OVR_CC_MSVC)
    #define OVR_CC_VERSION _MSC_VER // Question: Should we recognize _MSC_FULL_VER?
#elif defined(OVR_CC_ARM)
    #define OVR_CC_VERSION __ARMCC_VERSION
#elif defined(OVR_CC_INTEL)
    #if defined(__INTEL_COMPILER)
        #define OVR_CC_VERSION __INTEL_COMPILER
    #elif defined(__ICL)
        #define OVR_CC_VERSION __ICL
    #elif defined(__ICC)
        #define OVR_CC_VERSION __ICC
    #elif defined(__ECC)
        #define OVR_CC_VERSION __ECC
    #endif
#elif defined(OVR_CC_EDG)
    #define OVR_CC_VERSION __EDG_VERSION__  // This is a generic fallback for EDG-based compilers which aren't specified above (e.g. as OVR_CC_ARM)
#endif



// -----------------------------------------------------------------------------------
// ***** OVR_DISABLE_OPTIMIZATION / OVR_RESTORE_OPTIMIZATION
//
// Allows for the dynamic disabling and restoring of compiler optimizations in code.
// This is useful for helping deal with potential compiler code generation problems.
// With VC++ the usage must be outside of function bodies. This can be used only to
// temporarily disable optimization for a block of code and not to temporarily enable
// optimization for a block of code.
//
// Clang doesn't support this as of June 2014, though function __attribute__((optimize(0))
// is supposedly supported by clang in addition to GCC. To consider: Make a wrapper for
// this attribute-based functionality.
//
// Example usage:
//     OVR_DISABLE_OPTIMIZATION()
//     void Test() { ... }
//     OVR_RESTORE_OPTIMIZATION()
//
#if !defined(OVR_DISABLE_OPTIMIZATION)
    #if defined(OVR_CC_GNU) && (OVR_CC_VERSION > 404) && (defined(OVR_CPU_X86) || defined(OVR_CPU_X86_64))
        #define OVR_DISABLE_OPTIMIZATION() \
            _Pragma("GCC push_options")    \
            _Pragma("GCC optimize 0")
    #elif defined(OVR_CC_MSVC)
        #define OVR_DISABLE_OPTIMIZATION() __pragma(optimize("", off))
    #else
        #define OVR_DISABLE_OPTIMIZATION()
    #endif
#endif

#if !defined(OVR_RESTORE_OPTIMIZATION)
    #if defined(OVR_CC_GNU) && (OVR_CC_VERSION > 404) && (defined(OVR_CPU_X86) || defined(OVR_CPU_X86_64))
        #define OVR_RESTORE_OPTIMIZATION() _Pragma("GCC pop_options")
    #elif defined(OVR_CC_MSVC)
        #define OVR_RESTORE_OPTIMIZATION() __pragma(optimize("", on))
    #else
        #define OVR_RESTORE_OPTIMIZATION()
    #endif
#endif


// -----------------------------------------------------------------------------------
// *****  OVR_DISABLE_GNU_WARNING / OVR_RESTORE_GNU_WARNING
//
// Portable wrapper for disabling GCC compiler warnings, one at a time. See example
// usage for usage by example.
//
// Example usage:
//     OVR_DISABLE_GNU_WARNING(-Wmissing-braces)  // Only one warning per usage.
//     OVR_DISABLE_GNU_WARNING(-Wunused-variable)
//     <code>
//     OVR_RESTORE_GNU_WARNINGS()
//     OVR_RESTORE_GNU_WARNINGS()                 // Must match each disable with a restore.
//
#if !defined(OVR_DISABLE_GNU_WARNING)
    #if defined(OVR_CC_GNU)
        #define ODGW1(x) #x
        #define ODGW2(x) ODGW1(GCC diagnostic ignored x)
        #define ODGW3(x) ODGW2(#x)
    #endif

    #if defined(OVR_CC_GNU) && (OVR_CC_VERSION >= 406)
        #define OVR_DISABLE_GNU_WARNING(w)  \
            _Pragma("GCC diagnostic push")  \
            _Pragma(ODGW3(w))
    #elif defined(OVR_CC_GNU) && (OVR_CC_VERSION >= 404)  // GCC 4.4 doesn't support diagnostic push, but supports disabling warnings.
        #define OVR_DISABLE_GNU_WARNING(w)  \
            _Pragma(ODGW3(w))
    #else
        #define OVR_DISABLE_GNU_WARNING(w)
    #endif
#endif

#if !defined(OVR_RESTORE_GNU_WARNING)
    #if defined(OVR_CC_GNU) && (OVR_CC_VERSION >= 4006)
        #define OVR_RESTORE_GNU_WARNINGS()  \
            _Pragma("GCC diagnostic pop")
    #else
        #define OVR_RESTORE_GNU_WARNING()
    #endif
#endif



// -----------------------------------------------------------------------------------
// *****  OVR_DISABLE_CLANG_WARNING / OVR_RESTORE_CLANG_WARNING
//
// Portable wrapper for disabling GCC compiler warnings, one at a time. See example
// usage for usage by example.
//
// Example usage:
//     OVR_DISABLE_CLANG_WARNING(-Wmissing-braces)  // Only one warning per usage.
//     OVR_DISABLE_CLANG_WARNING(-Wunused-variable)
//     <code>
//     OVR_RESTORE_CLANG_WARNINGS()
//     OVR_RESTORE_CLANG_WARNINGS()                 // Must match each disable with a restore.
//
//
#if !defined(OVR_DISABLE_CLANG_WARNING)
    #if defined(OVR_CC_CLANG)
        #define ODCW1(x) #x
        #define ODCW2(x) ODCW1(clang diagnostic ignored x)
        #define ODCW3(x) ODCW2(#x)

        #define OVR_DISABLE_CLANG_WARNING(w)   \
            _Pragma("clang diagnostic push")  \
            _Pragma(ODCW3(w))
    #else
        #define OVR_DISABLE_CLANG_WARNING(w)
    #endif
#endif

#if !defined(OVR_RESTORE_CLANG_WARNING)
    #if defined(OVR_CC_CLANG)
        #define OVR_RESTORE_CLANG_WARNING()    \
            _Pragma("clang diagnostic pop")
    #else
        #define OVR_RESTORE_CLANG_WARNING()
    #endif
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_DISABLE_MSVC_WARNING / OVR_RESTORE_MSVC_WARNING
//
// Portable wrapper for disabling VC++ compiler warnings. See example usage for usage
// by example.
//
// Example usage:
//     OVR_DISABLE_MSVC_WARNING(4556 4782 4422)
//     <code>
//     OVR_RESTORE_MSVC_WARNING()
//
#if !defined(OVR_DISABLE_MSVC_WARNING)
    #if defined(OVR_CC_MSVC)
        #define OVR_DISABLE_MSVC_WARNING(w) \
            __pragma(warning(push))         \
            __pragma(warning(disable:w))
    #else
        #define OVR_DISABLE_MSVC_WARNING(w)
    #endif
#endif

#if !defined(OVR_RESTORE_MSVC_WARNING)
    #if defined(OVR_CC_MSVC)
        #define OVR_RESTORE_MSVC_WARNING() \
            __pragma(warning(pop))
    #else
        #define OVR_RESTORE_MSVC_WARNING()
    #endif
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_DISABLE_ALL_MSVC_WARNINGS / OVR_RESTORE_ALL_MSVC_WARNINGS
//
// Portable wrapper for disabling all VC++ compiler warnings.
// OVR_RESTORE_ALL_MSVC_WARNINGS restores warnings that were disabled by 
// OVR_DISABLE_ALL_MSVC_WARNINGS. Any previously enabled warnings will still be 
// enabled after OVR_RESTORE_ALL_MSVC_WARNINGS.
//
// Example usage:
//     OVR_DISABLE_ALL_MSVC_WARNINGS()
//     <code>
//     OVR_RESTORE_ALL_MSVC_WARNINGS()

#if !defined(OVR_DISABLE_ALL_MSVC_WARNINGS)
    #if defined(OVR_CC_MSVC)
        #define OVR_DISABLE_ALL_MSVC_WARNINGS() \
            __pragma(warning(push, 0))
    #else
        #define OVR_DISABLE_ALL_MSVC_WARNINGS()
    #endif
#endif

#if !defined(OVR_RESTORE_ALL_MSVC_WARNINGS)
    #if defined(OVR_CC_MSVC)
        #define OVR_RESTORE_ALL_MSVC_WARNINGS() \
            __pragma(warning(pop))
    #else
        #define OVR_RESTORE_ALL_MSVC_WARNINGS()
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CC_HAS_FEATURE
//
// This is a portable way to use compile-time feature identification available 
// with some compilers in a clean way. Direct usage of __has_feature in preprocessing
// statements of non-supporting compilers results in a preprocessing error.
//
// Example usage:
//     #if OVR_CC_HAS_FEATURE(is_pod)
//         if(__is_pod(T)) // If the type is plain data then we can safely memcpy it.
//             memcpy(&destObject, &srcObject, sizeof(object));
//     #endif
//
#if !defined(OVR_CC_HAS_FEATURE)
    #if defined(__clang__) // http://clang.llvm.org/docs/LanguageExtensions.html#id2
        #define OVR_CC_HAS_FEATURE(x) __has_feature(x)
    #else
        #define OVR_CC_HAS_FEATURE(x) 0
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CC_HAS_BUILTIN
//
//
// This is a portable way to use compile-time builtin identification available 
// with some compilers in a clean way. Direct usage of __has_builtin in preprocessing
// statements of non-supporting compilers results in a preprocessing error.
//
// Example usage:
//     #if OVR_CC_HAS_BUILTIN(__builtin_trap)
//         #define DEBUG_BREAK __builtin_trap
//     #endif
//  
#if !defined(OVR_CC_HAS_BUILTIN)
    #if defined(__clang__) 
        #define OVR_CC_HAS_BUILTIN(x) __has_builtin(x) // http://clang.llvm.org/docs/LanguageExtensions.html#id2
    #else
        #define OVR_CC_HAS_BUILTIN(x) 0
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP11_ENABLED / OVR_CPP_CPP14_ENABLED
//
// Defined as 1 if the compiler has its available C++11 support enabled, else undefined.
// This does not mean that all of C++11 or any particular feature of C++11 is supported
// by the compiler. It means that whatever C++11 support the compiler has is enabled.
// This also includes existing and older compilers that still identify C++11 as C++0x.
//
#if !defined(OVR_CPP11_ENABLED) && defined(__cplusplus)
    #if defined(__GNUC__) && defined(__GXX_EXPERIMENTAL_CXX0X__)
        #define OVR_CPP11_ENABLED 1
    #elif defined(_MSC_VER) && (_MSC_VER >= 1500)   // VS2010+, the first version with any significant C++11 support. 
        #define OVR_CPP11_ENABLED 1
    #elif (__cplusplus >= 201103L)                  // 201103 is the first C++11 version.
        #define OVR_CPP11_ENABLED 1
    #else
        // Leave undefined
    #endif
#endif

#if !defined(OVR_CPP_CPP14_ENABLED) && defined(__cplusplus)
    #if defined(_MSC_VER) && (_MSC_VER >= 1800)     // VS2013+, the first version with any significant C++14 support. 
        #define OVR_CPP_CPP14_ENABLED 1
    #elif (__cplusplus > 201103L)
        #define OVR_CPP_CPP14_ENABLED 1
    #else
        // Leave undefined
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_EXCEPTIONS / OVR_CPP_NO_UNWIND
//
// OVR_CPP_NO_EXCEPTIONS is defined as 1 if the compiler doesn't support C++ 
// exceptions or is configured to disable support for them. Else not defined.
// If OVR_CPP_NO_EXCEPTIONS is defined then attempts to use try/catch
// related C++ statements result in a compilation error with many
// compilers.
//
// OVR_CPP_NO_UNWIND is defined as 1 if the compiler supports exceptions but 
// doesn't support stack unwinding in the presence of an exception. Else not defined.
// For the Microsoft compiler, disabling exceptions means disabling stack unwinding
// and not disabling exceptions themselves.
//
// Example usage:
//     void Test() {
//         #if !defined(OVR_CPP_NO_EXCEPTIONS)
//             try {
//         #endif
//             void* ptr = new Object;
//         #if !defined(OVR_CPP_NO_EXCEPTIONS)
//             catch(...) { ... }
//         #endif

#if !defined(OVR_CPP_NO_EXCEPTIONS)
    #if defined(OVR_CPP_GNUC) && defined(_NO_EX)
        #define OVR_CPP_NO_EXCEPTIONS 1
    #elif (defined(OVR_CC_GNU) || defined(OVR_CC_CLANG) || defined(OVR_CC_INTEL) || defined(OVR_CC_ARM)) && !defined(__EXCEPTIONS)
        #define OVR_CPP_NO_EXCEPTIONS 1
    #elif defined(OVR_CC_MSVC) && !defined(_CPPUNWIND)
        #define OVR_CPP_NO_UNWIND 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_RTTI
//
// Defined as 1 if C++ run-time type information support is unavailable or disabled
// by the compiler. Else undefined. Allows you to write portable code in the face
// of the possibility that RTTI is disabled.
//
// Example usage:
//     #if !OVR_CPP_NO_RTTI
//         #include <typeinfo>
//         int x = std::dynamic_cast<int>(3.4f);
//     #endif

#if defined(__clang__) && !OVR_CC_HAS_FEATURE(cxx_rtti)
    #define OVR_CPP_NO_RTTI 1
#elif defined(__GNUC__) && !defined(__GXX_RTTI)
    #define OVR_CPP_NO_RTTI 1
#elif defined(_MSC_VER) && !defined(_CPPRTTI)
    #define OVR_CPP_NO_RTTI 1
#elif defined(__CC_ARM) && defined(__TARGET_CPU_MPCORE) && !defined(__RTTI)
    #define OVR_CPP_NO_RTTI 1
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_STATIC_ASSERT
//
// Defined as 1 if C++ run-time type information support is available and enabled
// by the compiler. Else undefined.
//
// Example usage:
//     #if OVR_CPP_NO_STATIC_ASSERT
//         #define MY_ASSERT(x) { int zero = 0; switch(zero) {case 0: case (x):;} }
//     #else
//         #define MY_ASSERT(x) static_assert((x), #x)
//     #endif

#if !defined(OVR_CPP_NO_STATIC_ASSERT)
    #if !(defined(__GNUC__) && (defined(__GXX_EXPERIMENTAL_CXX0X__) || (defined(__cplusplus) && (__cplusplus >= 201103L)))) && \
        !(defined(__clang__) && defined(__cplusplus) && OVR_CC_HAS_FEATURE(cxx_static_assert)) &&                              \
        !(defined(_MSC_VER) && (_MSC_VER >= 1600) && defined(__cplusplus)) &&                 /* VS2010+  */                   \
        !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401) && defined(OVR_CPP11_ENABLED)) /* EDG 4.1+ */
	    #define OVR_CPP_NO_STATIC_ASSERT 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_NULLPTR
//
// Defined as 1 if the compiler doesn't support C++11 nullptr built in type. 
// Otherwise undefined. Does not identify if the standard library defines 
// std::nullptr_t, as some standard libraries are further behind in standardization 
// than the compilers using them (e.g. Apple clang with the supplied libstdc++). 
//
// OVR_Nullptr.h provides a portable nullptr and std::nullptr_t for when the 
// compiler or standard library do not.

#if !defined(OVR_CPP_NO_NULLPTR)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_nullptr))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 406))          /* GCC 4.6+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1600))                /* VS2010+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403)))  /* EDG 4.3+  */    
        #define OVR_CPP_NO_NULLPTR 1
    #endif
#endif  


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_RVALUE_REFERENCES
//
// Defined as 1 if the compiler doesn't support C++11 rvalue references and move semantics.
// Otherwise undefined.

#if !defined(OVR_CPP_NO_RVALUE_REFERENCES)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_rvalue_references)) /* clang    */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 405))                   /* GCC 4.5+ */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1600))                         /* VS2010+  */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403)))           /* EDG 4.3+ */    
        #define OVR_CPP_NO_RVALUE_REFERENCES 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_AUTO
// 
// Defined as 1 if the compiler doesn't support C++11 auto keyword. Otherwise undefined.

#if !defined(OVR_CPP_NO_AUTO)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_auto_type))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 404))            /* GCC 4.4+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1600))                  /* VS2010+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 309)))    /* EDG 3.9+  */  
        #define OVR_CPP_NO_AUTO 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_RANGE_BASED_FOR_LOOP
//
// Defined as 1 if the compiler doesn't support C++11 range-based for loops.
// Otherwise undefined.

#if !defined(OVR_CPP_NO_RANGE_BASED_FOR_LOOP)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_range_for)) /* clang    */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 406))           /* GCC 4.6+ */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1700))                 /* VS2012+  */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)))   /* EDG 4.5+ */    
        #define OVR_CPP_NO_RANGE_BASED_FOR_LOOP 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_CONSTEXPR / OVR_CPP_NO_RELAXED_CONSTEXPR
//
// OVR_CPP_NO_CONSTEXPR is defined as 1 if the compiler doesn't support C++11 constexpr.
// OVR_CPP_NO_RELAXED_CONSTEXPR is defined as 1 if the compiler doesn't support C++14 constexpr.
// Otherwise undefined.
// See the OVR_CONSTEXPR / OVR_CONSTEXPR_OR_CONST macros for portable wrappers of this functionality.

#if !defined(OVR_CPP_NO_CONSTEXPR)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_constexpr))  /* clang    */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 406))            /* GCC 4.6+ */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 406)))    /* EDG 4.6+ */    
        // Not supported by VC++ through at least VS2013.
        #define OVR_CPP_NO_CONSTEXPR 1
    #endif
#endif

#if !defined(OVR_CPP_NO_RELAXED_CONSTEXPR)
    #if !defined(OVR_CPP14_ENABLED) || \
        !(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_relaxed_constexpr)) /* clang */
        // Supported only by clang as of this writing.
        #define OVR_CPP_NO_RELAXED_CONSTEXPR 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_LAMBDA_EXPRESSIONS
//
// Defined as 1 if the compiler doesn't support C++11 lambda expressions. Otherwise undefined.
// Some compilers have slightly crippled versions of this.

#if !defined(OVR_CPP_NO_LAMBDA_EXPRESSIONS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_lambdas))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 404))          /* GCC 4.4+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1600))                /* VS2010+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)))  /* EDG 4.1+  */ 
        // Conversion of lambdas to function pointers is not supported until EDG 4.5.
        #define OVR_CPP_NO_LAMBDA_EXPRESSIONS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_ALIGNOF
//
// Defined as 1 if the compiler supports C++11 alignof. Otherwise undefined.
// Some compilers support __alignof__ instead of alignof, so for portability you 
// should use OVR_ALIGNOF instead of directly using C++11 alignof.

#if !defined(OVR_CPP_NO_ALIGNOF)
    #if (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 209))  /* clang 2.9+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 300))  /* Apple clang 3.0+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 401))                    /* GCC 4.1+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1900))                          /* VS2014+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 400)))            /* EDG 4.0+         */
        #define OVR_CPP_NO_ALIGNOF 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_ALIGNAS
//
// Defined as 1 if the compiler supports C++11 alignas. Otherwise undefined.
// See the OVR_ALIGNAS for a portable wrapper for alignas functionality.

#if !defined(OVR_CPP_NO_ALIGNAS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 300))  /* clang 3.0+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 401))  /* Apple clang 4.1+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 408))                    /* GCC 4.8+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1900))                          /* VS2014+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 408)))            /* EDG 4.8+         */
        #define OVR_CPP_NO_ALIGNAS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_OVERRIDE
//
// Defined as 1 if the compiler doesn't support C++11 override. Otherwise undefined.
// See the OVR_OVERRIDE and OVR_FINALOVERRIDE macros for a portable wrapper.

#if !defined(OOVR_CPP_NO_OVERRIDE)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 209)) /* clang 2.9+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 400)) /* Apple clang 4.0+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 407))                   /* GCC 4.7+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1500))                         /* VS2008+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 408)))           /* EDG 4.8+         */     
        #define OVR_CPP_NO_OVERRIDE 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_FINAL
// 
// Defined as 1 if the compiler doesn't support C++11 final attribute. Otherwise undefined.
// See the OVR_FINAL and OVR_FINALOVERRIDE macros for a portable wrapper.

#if !defined(OOVR_CPP_NO_FINAL)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 209))  /* clang 2.9+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 400))  /* Apple clang 4.0+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 407))                    /* GCC 4.7+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1500))                          /* VS2008+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 408)))            /* EDG 4.8+         */    
        #define OVR_CPP_NO_FINAL 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_EXTERN_TEMPLATE
//
// Defined as 1 if the compiler doesn't support C++11 extern template.
// Otherwise undefined. See OVR_EXTERN_TEMPLATE for wrapper macro.

#if !defined(OVR_CPP_NO_EXTERN_TEMPLATE)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 209))  /* clang 2.9+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 401))  /* Apple clang 4.1+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 406))                    /* GCC 4.6+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1700))                          /* VS2012+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)))            /* EDG 4.1+         */ 
        #define OVR_CPP_NO_EXTERN_TEMPLATE 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_VARIADIC_TEMPLATES
//
// Defined as 1 if the compiler doesn't support C++11 variadic templates. Otherwise undefined.

#if !defined(OVR_CPP_NO_VARIADIC_TEMPLATES)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_variadic_templates)) /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 404))                    /* GCC 4.4+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1800))                          /* VS2013+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403)))            /* EDG 4.3+  */   
        #define OVR_CPP_NO_VARIADIC_TEMPLATES 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_NOEXCEPT
//
// Defined as 1 if the compiler supports C++11 noexcept. Otherwise undefined.
// http://en.cppreference.com/w/cpp/language/noexcept
// See OVR_NOEXCEPT / OVR_NOEXCEPT_IF / OVR_NOEXCEPT_EXPR for a portable wrapper
// for noexcept functionality.

#if !defined(OVR_CPP_NO_NOEXCEPT)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_noexcept))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 406))           /* GCC 4.6+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1900))                 /* VS2014+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)))   /* EDG 4.5+  */
        #define OVR_CPP_NO_NOEXCEPT 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_DECLTYPE
//
// Defined as 1 if the compiler doesn't support C++11 decltype. Otherwise undefined.
// Some compilers (e.g. VS2012) support most uses of decltype but don't support 
// decltype with incomplete types (which is an uncommon usage seen usually in 
// template metaprogramming).  We don't include this support as a requirement for
// our definition of decltype support here.

#if !defined(OVR_CPP_NO_DECLTYPE)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_decltype))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 403))           /* GCC 4.3+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1600))                 /* VS2010+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402)))   /* EDG 4.2+  */       
        // VC++ fails to support decltype for incomplete types until VS2013.
        // EDG fails to support decltype for incomplete types until v4.8.
        #define OVR_CPP_NO_DECLTYPE 1
    #endif
#endif  


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_DEFAULTED_FUNCTIONS
// 
// Defined as 1 if the compiler doesn't support C++11 defaulted functions. Otherwise undefined.
// Some compilers have slightly crippled versions of this.

#if !defined(OVR_CPP_NO_DEFAULTED_FUNCTIONS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_defaulted_functions))/* clang    */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 404))                    /* GCC 4.4+ */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1800))                          /* VS2013+  */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)))            /* EDG 4.1+ */
        // Up through at least VS2013 it's unsupported for defaulted move constructors and move assignment operators.
        // Until EDG 4.8 it's unsupported for defaulted move constructors and move assigment operators.
        #define OVR_CPP_NO_DEFAULTED_FUNCTIONS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_DELETED_FUNCTIONS
// 
// Defined as 1 if the compiler doesn't support C++11 deleted functions. Otherwise undefined.
// Some compilers have slightly crippled versions of this.

#if !defined(OVR_CPP_NO_DELETED_FUNCTIONS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_defaulted_functions)) /* clang    */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 404))                     /* GCC 4.4+ */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1800))                           /* VS2013+  */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)))             /* EDG 4.1+ */  
        // Up through at least VS2013 it's unsupported for defaulted move constructors and move assignment operators.
        // Until EDG 4.8 it's unsupported for defaulted move constructors and move assigment operators.
        #define OVR_CPP_NO_DELETED_FUNCTIONS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_STANDARD_LAYOUT_TYPES
// 
// Defined as 1 if the compiler doesn't support C++11 standard layout (relaxed POD). Otherwise undefined.
// http://en.cppreference.com/w/cpp/types/is_standard_layout

#if !defined(OVR_CPP_NO_STANDARD_LAYOUT_TYPES)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 300)) /* clang 3.0+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 401)) /* Apple clang 4.1+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 405))                   /* GCC 4.5+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1700))                         /* VS2013+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 406)))           /* EDG 4.6+         */  
        #define OVR_CPP_NO_STANDARD_LAYOUT_TYPES 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_FORWARD_DECLARED_ENUMS
//
// Defined as 1 if the compiler doesn't support C++11 forward declared enums. Otherwise undefined.

#if !defined(OVR_CPP_NO_FORWARD_DECLARED_ENUMS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 301))  /* clang 3.1+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 401))  /* Apple clang 4.1+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 406))                    /* GCC 4.6+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1700))                          /* VS2012+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)))            /* EDG 4.5+         */ 
        #define OVR_CPP_NO_FORWARD_DECLARED_ENUMS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_STRONGLY_TYPED_ENUMS
//
// Defined as 1 if the compiler doesn't support C++11 strongly typed enums. Otherwise undefined.

#if !defined(OVR_CPP_NO_STRONGLY_TYPED_ENUMS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_strong_enums))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 404))               /* GCC 4.4+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1700))                     /* VS2012+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 400)))       /* EDG 4.0+ */ 
        #define OVR_CPP_NO_STRONGLY_TYPED_ENUMS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_TRAILING_RETURN_TYPES
//
// Defined as 1 if the compiler doesn't support C++11 trailing return types. Otherwise undefined.
// http://en.wikipedia.org/wiki/C%2B%2B11#Alternative_function_syntax

#if !defined(OVR_CPP_NO_TRAILING_RETURN_TYPES)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_trailing_return)) /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 404))                 /* GCC 4.4+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1600))                       /* VS2010+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)))         /* EDG 4.1+ */    
        #define OVR_CPP_NO_TRAILING_RETURN_TYPES 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_TEMPLATE_ALIASES
//
// Defined as 1 if the compiler doesn't support C++11 template aliases. Otherwise undefined.

#if !defined(OVR_CPP_NO_TEMPLATE_ALIASES)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_alias_templates)) /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 407))                 /* GCC 4.7+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1800))                       /* VS2013+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402)))         /* EDG 4.2+  */ 
        #define OVR_CPP_NO_TEMPLATE_ALIASES 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_INITIALIZER_LISTS
//
// Defined as 1 if the compiler doesn't support C++11 initializer lists. Otherwise undefined.
// This refers to the compiler support for this and not the Standard Library support for std::initializer_list,
// as a new compiler with an old standard library (e.g. Apple clang with libstdc++) may not support std::initializer_list.

#if !defined(OVR_CPP_NO_INITIALIZER_LISTS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_generalized_initializers)) /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 404))                          /* GCC 4.4+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1800))                                /* VS2013+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)))                  /* EDG 4.5+  */
        #define OVR_CPP_NO_INITIALIZER_LISTS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_NORETURN
//
// Defined as 1 if the compiler doesn't support the C++11 noreturn attribute. Otherwise undefined.
// http://en.cppreference.com/w/cpp/language/attributes
//
#if !defined(OVR_CPP_NO_NORETURN)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 301))  /* clang 3.1+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 401))  /* Apple clang 4.1+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 408))                    /* GCC 4.8+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1500))                          /* VS2008+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402)))            /* EDG 4.2+         */
        // Supported with VC++ only via __declspec(noreturn) (see OVR_NORETURN).
        #define OVR_CPP_NO_NORETURN 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_NONSTATIC_MEMBER_INITIALIZERS
//
// Defined as 1 if the compiler doesn't support C++11 in-class non-static member initializers. Otherwise undefined.
// http://en.cppreference.com/w/cpp/language/data_members

#if !defined(OVR_CPP_NO_NONSTATIC_MEMBER_INITIALIZERS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 301))  /* clang 3.1+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 401))  /* Apple clang 4.1+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 407))                    /* GCC 4.7+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1800))                          /* VS2013+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 406)))            /* EDG 4.6+         */
        #define OVR_CPP_NO_NONSTATIC_MEMBER_INITIALIZERS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_DOUBLE_TEMPLATE_BRACKETS
//
// Defined as 1 if the compiler supports nested template declarations with >>, 
// as supported by C++11. Otherwise undefined.

#if !defined(OVR_CPP_NO_DOUBLE_TEMPLATE_ANGLE_BRACKETS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 209))  /* clang 2.9+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 400))  /* Apple clang 4.0+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 403))                    /* GCC 4.3+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1600))                          /* VS2010+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)))            /* EDG 4.1+         */
        #define OVR_CPP_NO_DOUBLE_TEMPLATE_BRACKETS 1
    #endif
#endif



//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_INHERITING_CONSTRUCTORS
//
// Defined as 1 if the compiler supports C++11 inheriting constructors. Otherwise undefined.
// Example usage:
//     struct A { explicit A(int x){} };
//     struct B : public A { using A::A; }; // As if B redeclared A::A(int).

#if !defined(OVR_CPP_NO_INHERITING_CONSTRUCTORS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_inheriting_constructors))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 408))                          /* GCC 4.8+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1900))                                /* VS2014+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 408)))                  /* EDG 4.8+  */
        #define OVR_CPP_NO_INHERITING_CONSTRUCTORS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_DELEGATING_CONSTRUCTORS
//
// Defined as 1 if the compiler supports C++11 delegating constructors. Otherwise undefined.

#if !defined(OVR_CPP_NO_DELEGATING_CONSTRUCTORS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 300))  /* clang 3.0+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 401))  /* Apple clang 4.1+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 407))                    /* GCC 4.7+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1800))                          /* VS2013+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 407)))            /* EDG 4.7+         */
        #define OVR_CPP_NO_DELEGATING_CONSTRUCTORS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
//  
// Defined as 1 if the compiler supports C++11 function template default arguments. Otherwise undefined.

#if !defined(OVR_CPP_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 209))  /* clang 2.9+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 401))  /* Apple clang 4.0+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 403))                    /* GCC 4.3+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1800))                          /* VS2013+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403)))            /* EDG 4.3+         */
        #define OVR_CPP_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_UNRESTRICTED_UNIONS
//
// Defined as 1 if the compiler supports C++11 unrestricted unions. Otherwise undefined.

#if !defined(OVR_CPP_NO_UNRESTRICTED_UNIONS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 301))  /* clang 3.1+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 401))  /* Apple clang 4.1+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 406))                    /* GCC 4.6+         */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 406)))            /* EDG 4.6+         */
        // Not supported by VC++ as of VS2013.
        #define OVR_CPP_NO_UNRESTRICTED_UNIONS 1
    #endif
#endif



//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_EXTENDED_SIZEOF
//
// Defined as 1 if the compiler supports C++11 class sizeof extensions (e.g. sizeof SomeClass::someMember). 
// Otherwise undefined.

#if !defined(OVR_CPP_NO_EXTENDED_SIZEOF)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 301))  /* clang 3.1+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 401))  /* Apple clang 4.1+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 405))                    /* GCC 4.5+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1900))                          /* VS2014+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)))            /* EDG 4.5+         */
        #define OVR_CPP_NO_EXTENDED_SIZEOF 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_INLINE_NAMESPACES
//
// Defined as 1 if the compiler supports C++11 inlined namespaces. Otherwise undefined.
// http://en.cppreference.com/w/cpp/language/namespace#Inline_namespaces

#if !defined(OVR_CPP_NO_INLINE_NAMESPACES)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 209))  /* clang 2.9+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 400))  /* Apple clang 4.0+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 404))                    /* GCC 4.4+         */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)))            /* EDG 4.5+         */
        // Not supported by VC++ as of VS2013.
        #define OVR_CPP_NO_INLINE_NAMESPACES 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_EXPLICIT_CONVERSION_OPERATORS
//
// Defined as 1 if the compiler supports C++11 explicit conversion operators. Otherwise undefined.
// http://en.cppreference.com/w/cpp/language/explicit

#if !defined(OVR_CPP_NO_EXPLICIT_CONVERSION_OPERATORS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_explicit_conversions))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 405))                       /* GCC 4.5+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1800))                             /* VS2013+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 404)))               /* EDG 4.4+  */
        #define OVR_CPP_NO_EXPLICIT_CONVERSION_OPERATORS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
//
// Defined as 1 if the compiler supports C++11 local class template parameters. Otherwise undefined.
// Example:
//     void Test() {   
//         struct LocalClass{ };
//         SomeTemplateClass<LocalClass> t; // Allowed only in C++11
//     }

#if !defined(OVR_CPP_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS) 
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_local_type_template_args))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 405))                           /* GCC 4.5+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1600))                                 /* VS2010+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402)))                   /* EDG 4.2+  */
        #define OVR_CPP_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_NEW_CHARACTER_TYPES
//
// Defined as 1 if the compiler natively supports C++11 char16_t and char32_t. Otherwise undefined.
// VC++ through at least VS2013 defines char16_t as unsigned short in its standard library,
// but it is not a native type or unique type, nor can you for a string literal with it.

#if !defined(OVR_CPP_NO_NEW_CHARACTER_TYPES)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_unicode_literals))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 404))                   /* GCC 4.4+  */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 407)))           /* EDG 4.7+  */
        // Not supported by VC++ as of VS2013.
        #define OVR_CPP_NO_NEW_CHARACTER_TYPES 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_UNICODE_CHAR_NAME_LITERALS
//
// Defined as 1 if the compiler supports C++11 \u and \U character literals for 
// native char16_t and char32_t types.
//
#if !defined(OVR_CPP_NO_UNICODE_CHAR_NAME_LITERALS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 301))  /* clang 3.1+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 401))  /* Apple clang 4.1+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 405))                    /* GCC 4.5+         */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 408)))            /* EDG 4.8+         */
        // Not supported by VC++ as of VS2013. VC++'s existing \U and \u are non-conforming.
        #define OVR_CPP_NO_UNICODE_CHAR_NAME_LITERALS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_USER_DEFINED_LITERALS
//
// Defined as 1 if the compiler supports C++11 user-defined literals. Otherwise undefined.

#if !defined(OVR_CPP_NO_USER_DEFINED_LITERALS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 301))  /* clang 3.1+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 401))  /* Apple clang 4.1+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 407))                    /* GCC 4.7+         */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 408)))            /* EDG 4.8+         */
        // Not supported by VC++ as of VS2013.
        #define OVR_CPP_NO_USER_DEFINED_LITERALS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_UNICODE_STRING_LITERALS
//
// Defined as 1 if the compiler supports C++11 Unicode string literals. Otherwise undefined.
// http://en.wikipedia.org/wiki/C%2B%2B11#New_string_literals

#if !defined(OVR_CPP_NO_UNICODE_STRING_LITERALS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_unicode_literals))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 404))                   /* GCC 4.4+  */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 407)))           /* EDG 4.7+  */
        // Not supported by VC++ as of VS2013.
        #define OVR_CPP_NO_UNICODE_STRING_LITERALS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_RAW_STRING_LITERALS
//
// Defined as 1 if the compiler supports C++11 raw literals. Otherwise undefined.
// http://en.wikipedia.org/wiki/C%2B%2B11#New_string_literals

#if !defined(OVR_CPP_NO_RAW_STRING_LITERALS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_raw_string_literals))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 405))                      /* GCC 4.5+  */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 407)))              /* EDG 4.7+  */
        // Not supported by VC++ as of VS2013.
        #define OVR_CPP_NO_RAW_STRING_LITERALS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_UNIFIED_INITIALIZATION_SYNTAX
//
// Defined as 1 if the compiler supports C++11 unified initialization.
// http://en.wikipedia.org/wiki/C%2B%2B11#Uniform_initialization

#if !defined(OVR_CPP_NO_UNIFIED_INITIALIZATION_SYNTAX)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_generalized_initializers))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 404))                           /* GCC 4.4+  */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1800))                                 /* VS2013+   */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 406)))                   /* EDG 4.6+  */
        #define OVR_CPP_NO_UNIFIED_INITIALIZATION_SYNTAX 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_EXTENDED_FRIEND_DECLARATIONS
//
// Defined as 1 if the compiler supports C++11 extended friends.

#if !defined(OVR_CPP_NO_EXTENDED_FRIEND_DECLARATIONS)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && !defined(__APPLE__) && (__clang__ >= 209))  /* clang 2.9+       */ && \
         !(defined(__clang__) &&  defined(__APPLE__) && (__clang__ >= 400))  /* Apple clang 4.0+ */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 407))                    /* GCC 4.7+         */ && \
         !(defined(_MSC_VER) && (_MSC_VER >= 1600))                          /* VS2010+          */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)))            /* EDG 4.1+         */
        #define OVR_CPP_NO_EXTENDED_FRIEND_DECLARATIONS 1
    #endif
#endif


//-----------------------------------------------------------------------------------
// ***** OVR_CPP_NO_THREAD_LOCAL
//
// Defined as 1 if the compiler supports C++11 thread_local. Else undefined. Does not
// indicate if the compiler supports C thread-local compiler extensions such as __thread
// and declspec(thread). Use OVR_THREAD_LOCAL if you want to declare a thread-local 
// variable that supports C++11 thread_local when available but the C extension when 
// it's available. The primary difference between C++11 thread_local and C extensions is
// that C++11 thread_local supports non-PODs and calls their constructors and destructors.
//
// Note that thread_local requires both compiler and linker support, and so it's possible
// that the compiler may support thread_local but the linker does not.

#if !defined(OVR_CPP_NO_THREAD_LOCAL)
    #if !defined(OVR_CPP11_ENABLED) || \
        (!(defined(__clang__) && OVR_CC_HAS_FEATURE(cxx_thread_local))  /* clang     */ && \
         !(defined(__GNUC__) && (OVR_CPP_VERSION >= 408))               /* GCC 4.8+  */ && \
         !(defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 408)))       /* EDG 4.8+  */
        #define OVR_CPP_NO_THREAD_LOCAL 1
    #endif
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_ALIGNAS / OVR_ALIGNOF
//
//    OVR_ALIGNAS(n)        // Specifies a size_t power of two alignment for a type or instance.
//    OVR_ALIGNOF(type)     // Returns the size_t alignment of a type or instance.
//
// Example usage:
//    OVR_ALIGNAS(8) char c = 'c';                      // Specifies that the instance c be aligned to an 8 byte boundary.
//    typedef OVR_ALIGNAS(8) char C;                    // Specifies that the type C be aligned to an 8 byte boundary.
//    struct OVR_ALIGNAS(64) S{ char array[16]; };      // Specfies that the struct S have a natural alignment of 64.
//    OVR_ALIGNAS(32) S s;                              // Specifies that the instance s of struct S be aligned to an 32 byte boundary.
//    OVR_ALIGNAS(32) struct T{ char array[16]; } t;    // Specfies that the instance t of struct T have a natural alignment of 32.
//    struct OVR_ALIGNAS(T) U{};                        // Specifes that U be aligned the same as T. Supported only by C++11 compilers (see OVR_CPP_NO_ALIGNAS).
//
//    size_t a = OVR_ALIGNOF(double);                   // Returns the natural alignment of the double type.
//    size_t a = OVR_ALIGNOF(S);                        // Returns the natural alignment of the struct S type.
//
// Note: If C++11 alignas is supported, then alignas/OVR_ALIGNAS may take a const expression in addition to a constant.
// Note: The C11 Standard species the _Alignas keyword and alignas as a macro for it in <stdalign.h>

#if !defined(OVR_ALIGNAS)
    #if defined(OVR_CC_GNU) && !defined(OVR_CPP_NO_ALIGNAS)     // If C++11 alignas is supported...
        #define OVR_ALIGNAS(n) alignas(n)
    #elif defined(__clang__) && !defined(OVR_CPP_NO_ALIGNAS)
        #define OVR_ALIGNAS(n) alignas(n)
    #elif defined(OVR_CC_GNU) || defined(__clang__)
        #define OVR_ALIGNAS(n) __attribute__((aligned(n)))
    #elif defined(OVR_CC_MSVC) || defined(OVR_CC_INTEL)
        #define OVR_ALIGNAS(n) __declspec(align(n))             // For Microsoft the alignment must be a literal integer.
    #elif defined(OVR_CC_ARM)
        #define OVR_ALIGNAS(n) __align(n)
    #else
        #error Need to define OVR_ALIGNAS
    #endif
#endif

#if !defined(OVR_ALIGNOF)
    #if defined(OVR_CC_GNU) && !defined(OVR_CPP_NO_ALIGNOF)     // If C++11 alignof is supported...
        #define OVR_ALIGNOF(type) alignof(t)
    #elif defined(__clang__) && !defined(OVR_CPP_NO_ALIGNOF)
        #define OVR_ALIGNOF(type) alignof(t)
    #elif defined(OVR_CC_GNU) || defined(__clang__)
        #define OVR_ALIGNOF(type) ((size_t)__alignof__(type))
    #elif defined(OVR_CC_MSVC) || defined(OVR_CC_INTEL)
        #define OVR_ALIGNOF(type) ((size_t)__alignof(type))
    #elif defined(OVR_CC_ARM)
        #define OVR_ALIGNOF(type) ((size_t)__ALIGNOF__(type))
    #else
        #error Need to define OVR_ALIGNOF
    #endif
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_ASSUME / OVR_ANALYSIS_ASSUME
//
// This is a portable wrapper for VC++'s __assume and __analysis_assume.
// __analysis_assume is typically used to quell VC++ static analysis warnings.
//
// Example usage:
//    void Test(char c){
//       switch(c){
//          case 'a':
//          case 'b':
//          case 'c':
//          case 'd':
//             break;
//          default:
//             OVR_ASSUME(0); // Unreachable code.
//       }
//    }
//
//    size_t Test(char* str){
//       OVR_ANALYSIS_ASSUME(str != nullptr);
//       return strlen(str);
//    }

#if !defined(OVR_ASSUME)
    #if defined(OVR_CC_MSVC)
        #define OVR_ASSUME(x)          __assume(x)
        #define OVR_ANALYSIS_ASSUME(x) __analysis_assume(!!(x))
    #else
        #define OVR_ASSUME(x)
        #define OVR_ANALYSIS_ASSUME(x)
    #endif
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_RESTRICT
//
// Wraps the C99 restrict keyword in a portable way.
// C++11 and C++14 don't have restrict but this functionality is supported by 
// all C++ compilers.
//
// Example usage:
//    void* memcpy(void* OVR_RESTRICT s1, const void* OVR_RESTRICT s2, size_t n);

#if !defined(OVR_RESTRICT)
    #define OVR_RESTRICT __restrict // Currently supported by all compilers of significance to us.
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_NOEXCEPT / OVR_NOEXCEPT_IF(predicate) / OVR_NOEXCEPT_EXPR(expression)
//
// Implements a portable wrapper for C++11 noexcept.
// http://en.cppreference.com/w/cpp/language/noexcept
//
// Example usage:
//     void Test() OVR_NOEXCEPT {} // This function doesn't throw.
//
//     template <typename T>
//     void DoNothing() OVR_NOEXCEPT_IF(OVR_NOEXCEPT_EXPR(T())) // Throws an if and only if T::T(int) throws.
//         { T t(3); }
//
#if !defined(OVR_NOEXCEPT)
    #if defined(OVR_CPP_NOEXCEPT)
        #define OVR_NOEXCEPT
        #define OVR_NOEXCEPT_IF(predicate)
        #define OVR_NOEXCEPT_EXPR(expression) false
    #else
        #define OVR_NOEXCEPT noexcept
        #define OVR_NOEXCEPT_IF(predicate) noexcept((predicate))
        #define OVR_NOEXCEPT_EXPR(expression) noexcept((expression))
    #endif
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_FINAL
//
// Wraps the C++11 final keyword in a portable way.
// http://en.cppreference.com/w/cpp/language/final
//
// Example usage:
//     struct Test { virtual int GetValue() OVR_FINAL; };

#if !defined(OVR_FINAL)
    #if defined(OVR_CC_MSVC) && (OVR_CC_VERSION < 1700) // VC++ 2012 and earlier
        #define OVR_FINAL sealed
    #elif defined(OVR_CPP_INHERITANCE_FINAL)
        #define OVR_FINAL
    #else
        #define OVR_FINAL final
    #endif
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_OVERRIDE
//
// Wraps the C++11 override keyword in a portable way.
// http://en.cppreference.com/w/cpp/language/override
//
// Example usage:
//        struct Parent { virtual void Func(int); };
//        struct Child : public Parent { void Func(int) OVR_OVERRIDE; };

#if !defined(OVR_CPP11_ENABLED)
#define OVR_OVERRIDE
#elif !defined(OVR_OVERRIDE)
    #if defined(OVR_CPP_OVERRIDE)
        #define OVR_OVERRIDE
    #else
        #if (defined(_MSC_VER) && (_MSC_VER <= 1600))
            #pragma warning(disable : 4481)
        #endif
        #define OVR_OVERRIDE override
    #endif
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_FINAL_OVERRIDE
//
// Wraps the C++11 final+override keywords (a common combination) in a portable way.
//
// Example usage:
//     struct Parent { virtual void Func(); };
//     struct Child : public Parent { virtual void Func() OVR_FINAL_OVERRIDE; };

#if !defined(OVR_FINAL_OVERRIDE)
    #define OVR_FINAL_OVERRIDE OVR_FINAL OVR_OVERRIDE
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_EXTERN_TEMPLATE
//
// Portable wrapper for C++11 extern template. This tells the compiler to not instantiate
// the template in the current translation unit, which can significantly speed up 
// compilation and avoid problems due to two translation units compiling code with 
// different settings.
//
// Example usage:
//     OVR_EXTERN_TEMPLATE(class basic_string<char>); // Nothing to do for non-C++11 compilers.

#if !defined(OVR_EXTERN_TEMPLATE)
    #if defined(OVR_CPP_EXTERN_TEMPLATE)
        #define OVR_EXTERN_TEMPLATE(decl)
    #else
        #define OVR_EXTERN_TEMPLATE(decl) extern template decl
    #endif
#endif


// -----------------------------------------------------------------------------------
// ***** OVR_CONSTEXPR  / OVR_CONSTEXPR_OR_CONST
//
// Portable wrapper for C++11 constexpr. Doesn't include C++14 relaxed constexpr,
// for which a different wrapper name is reserved.
//
// Example usage:
//     OVR_CONSTEXPR int Test() { return 15; }          // This can be optimized better by a C++11 compiler that supports constexpr.
//     OVR_CONSTEXPR_OR_CONST float x = 3.14159f;       // This can be optimized better by a C++11 compiler, but if not then at least make it const.

#if !defined(OVR_CONSTEXPR)
    #if defined(OVR_CPP_NO_CONSTEXPR)
        #define OVR_CONSTEXPR
    #else
        #define OVR_CONSTEXPR constexpr
    #endif
#endif

#if !defined(OVR_CONSTEXPR_OR_CONST)
    #if defined(OVR_CPP_NO_CONSTEXPR)
        #define OVR_CONSTEXPR_OR_CONST const
    #else
        #define OVR_CONSTEXPR_OR_CONST constexpr
    #endif
#endif



// -----------------------------------------------------------------------------------
// ***** OVR_FUNCTION_DELETE / OVR_FUNCTION_DEFAULT
//
// Wraps the C++11 delete and default keywords in a way that allows for cleaner code
// while making for a better version of uncallable or default functions.
//
// Example usage:
//     struct Test{
//         Test() OVR_FUNCTION_DEFAULT;            // Non-C++11 compilers will require a separate definition for Test().
//     private:                                   // Users should put OVR_FUNCTION_DELETE usage in a private 
//         void Uncallable() OVR_FUNCTION_DELETE;  // area for compatibility with pre-C++11 compilers.
//     };

#if defined(OVR_CPP_NO_DELETED_FUNCTIONS)
    #define OVR_FUNCTION_DELETE
#else
    #define OVR_FUNCTION_DELETE = delete
#endif

#if defined(OVR_CPP_NO_DEFAULTED_FUNCTIONS)
    #define OVR_FUNCTION_DEFAULT
#else
    #define OVR_FUNCTION_DEFAULT = default
#endif



// -----------------------------------------------------------------------------------
// ***** OVR_NON_COPYABLE
//
// Allows you to specify a class as being neither copy-constructible nor assignable,
// which is a commonly needed pattern in C++ programming. Classes with this declaration
// are required to be default constructible (as are most classes). For pre-C++11
// compilers this macro declares a private section for the class, which will be
// inherited by whatever code is directly below the macro invocation by default.
//
// Example usage:
//    struct Test {
//       Test();
//       ...
//       OVR_NON_COPYABLE(Test)
//    };

#if !defined(OVR_NON_COPYABLE)
    #if defined(OVR_CPP_NO_DELETED_FUNCTIONS)
        #define OVR_NON_COPYABLE(Type)   \
            private:                     \
            Type(const Type&);           \
            void operator=(const Type&);
    #else
        #define OVR_NON_COPYABLE(Type)   \
            Type(const Type&) = delete;  \
            void operator=(const Type&) = delete;
    #endif
#endif



#endif  // header include guard




