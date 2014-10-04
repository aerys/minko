/************************************************************************************

PublicHeader:   OVR_Kernel.h
Filename    :   OVR_ContainerAllocator.h
Content     :   Template allocators and constructors for containers.
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

#ifndef OVR_ContainerAllocator_h
#define OVR_ContainerAllocator_h

#include "OVR_Allocator.h"
#include <string.h>


namespace OVR {


//-----------------------------------------------------------------------------------
// ***** Container Allocator

// ContainerAllocator serves as a template argument for allocations done by
// containers, such as Array and Hash; replacing it could allow allocator
// substitution in containers.

class ContainerAllocatorBase
{
public:
    static void* Alloc(size_t size)                { return OVR_ALLOC(size); }
    static void* Realloc(void* p, size_t newSize)  { return OVR_REALLOC(p, newSize); }
    static void  Free(void *p)                    { OVR_FREE(p); }
};



//-----------------------------------------------------------------------------------
// ***** Constructors, Destructors, Copiers

// Plain Old Data - movable, no special constructors/destructor.
template<class T> 
class ConstructorPOD
{
public:
    static void Construct(void *) {}
    static void Construct(void *p, const T& source) 
    { 
        *(T*)p = source;
    }

    // Same as above, but allows for a different type of constructor.
    template <class S> 
    static void ConstructAlt(void *p, const S& source)
    {
        *(T*)p = source;
    }

    static void ConstructArray(void*, size_t) {}

    static void ConstructArray(void* p, size_t count, const T& source)
    {
        uint8_t *pdata = (uint8_t*)p;
        for (size_t i=0; i< count; ++i, pdata += sizeof(T))
            *(T*)pdata = source;
    }

    static void ConstructArray(void* p, size_t count, const T* psource)
    {
        memcpy(p, psource, sizeof(T) * count);
    }

    static void Destruct(T*) {}
    static void DestructArray(T*, size_t) {}

    static void CopyArrayForward(T* dst, const T* src, size_t count)
    {
        memmove(dst, src, count * sizeof(T));
    }

    static void CopyArrayBackward(T* dst, const T* src, size_t count)
    {
        memmove(dst, src, count * sizeof(T));
    }

    static bool IsMovable() { return true; }
};


//-----------------------------------------------------------------------------------
// ***** ConstructorMov
//
// Correct C++ construction and destruction for movable objects
template<class T> 
class ConstructorMov
{
public:
    static void Construct(void* p) 
    { 
        OVR::Construct<T>(p);
    }

    static void Construct(void* p, const T& source) 
    { 
        OVR::Construct<T>(p, source);
    }

    // Same as above, but allows for a different type of constructor.
    template <class S> 
    static void ConstructAlt(void* p, const S& source)
    {
        OVR::ConstructAlt<T,S>(p, source);
    }

    static void ConstructArray(void* p, size_t count)
    {
        uint8_t* pdata = (uint8_t*)p;
        for (size_t i=0; i< count; ++i, pdata += sizeof(T))
            Construct(pdata);
    }

    static void ConstructArray(void* p, size_t count, const T& source)
    {
        uint8_t* pdata = (uint8_t*)p;
        for (size_t i=0; i< count; ++i, pdata += sizeof(T))
            Construct(pdata, source);
    }

    static void ConstructArray(void* p, size_t count, const T* psource)
    {
        uint8_t* pdata = (uint8_t*)p;
        for (size_t i=0; i< count; ++i, pdata += sizeof(T))
            Construct(pdata, *psource++);
    }

    static void Destruct(T* p)
    {
        p->~T();
        OVR_UNUSED(p); // Suppress silly MSVC warning
    }

    static void DestructArray(T* p, size_t count)
    {   
        p += count - 1;
        for (size_t i=0; i<count; ++i, --p)
            p->~T();
    }

    static void CopyArrayForward(T* dst, const T* src, size_t count)
    {
        memmove(dst, src, count * sizeof(T));
    }

    static void CopyArrayBackward(T* dst, const T* src, size_t count)
    {
        memmove(dst, src, count * sizeof(T));
    }

    static bool IsMovable() { return true; }
};


//-----------------------------------------------------------------------------------
// ***** ConstructorCPP
//
// Correct C++ construction and destruction for movable objects
template<class T> 
class ConstructorCPP
{
public:
    static void Construct(void* p) 
    { 
        OVR::Construct<T>(p);        
    }

    static void Construct(void* p, const T& source) 
    { 
        OVR::Construct<T>(p, source);        
    }

    // Same as above, but allows for a different type of constructor.
    template <class S> 
    static void ConstructAlt(void* p, const S& source)
    {
        OVR::ConstructAlt<T,S>(p, source);        
    }

    static void ConstructArray(void* p, size_t count)
    {
        uint8_t* pdata = (uint8_t*)p;
        for (size_t i=0; i< count; ++i, pdata += sizeof(T))
            Construct(pdata);
    }

    static void ConstructArray(void* p, size_t count, const T& source)
    {
        uint8_t* pdata = (uint8_t*)p;
        for (size_t i=0; i< count; ++i, pdata += sizeof(T))
            Construct(pdata, source);
    }

    static void ConstructArray(void* p, size_t count, const T* psource)
    {
        uint8_t* pdata = (uint8_t*)p;
        for (size_t i=0; i< count; ++i, pdata += sizeof(T))
            Construct(pdata, *psource++);
    }

    static void Destruct(T* p)
    {
        p->~T();
        OVR_UNUSED(p); // Suppress silly MSVC warning
    }

    static void DestructArray(T* p, size_t count)
    {   
        p += count - 1;
        for (size_t i=0; i<count; ++i, --p)
            p->~T();
    }

    static void CopyArrayForward(T* dst, const T* src, size_t count)
    {
        for(size_t i = 0; i < count; ++i)
            dst[i] = src[i];
    }

    static void CopyArrayBackward(T* dst, const T* src, size_t count)
    {
        for(size_t i = count; i; --i)
            dst[i-1] = src[i-1];
    }

    static bool IsMovable() { return false; }
};


//-----------------------------------------------------------------------------------
// ***** Container Allocator with movement policy
//
// Simple wraps as specialized allocators
template<class T> struct ContainerAllocator_POD : ContainerAllocatorBase, ConstructorPOD<T> {};
template<class T> struct ContainerAllocator     : ContainerAllocatorBase, ConstructorMov<T> {};
template<class T> struct ContainerAllocator_CPP : ContainerAllocatorBase, ConstructorCPP<T> {};


} // OVR


#endif
