/************************************************************************************

Filename    :   OVR_Allocator.cpp
Content     :   Installable memory allocator implementation
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

#include "OVR_Allocator.h"
#ifdef OVR_OS_MAC
 #include <stdlib.h>
#else
 #include <malloc.h>
#endif

namespace OVR {

//-----------------------------------------------------------------------------------
// ***** Allocator

Allocator* Allocator::pInstance = 0;

// Default AlignedAlloc implementation will delegate to Alloc/Free after doing rounding.
void* Allocator::AllocAligned(size_t size, size_t align)
{
    OVR_ASSERT((align & (align-1)) == 0);
    align = (align > sizeof(size_t)) ? align : sizeof(size_t);
    size_t p = (size_t)Alloc(size+align);
    size_t aligned = 0;
    if (p)
    {
        aligned = (size_t(p) + align-1) & ~(align-1);
        if (aligned == p) 
            aligned += align;
        *(((size_t*)aligned)-1) = aligned-p;
    }
    return (void*)aligned;
}

void Allocator::FreeAligned(void* p)
{
    size_t src = size_t(p) - *(((size_t*)p)-1);
    Free((void*)src);
}


//------------------------------------------------------------------------
// ***** Default Allocator

// This allocator is created and used if no other allocator is installed.
// Default allocator delegates to system malloc.

void* DefaultAllocator::Alloc(size_t size)
{
    return malloc(size);
}
void* DefaultAllocator::AllocDebug(size_t size, const char* file, unsigned line)
{
#if defined(OVR_CC_MSVC) && defined(_CRTDBG_MAP_ALLOC)
    return _malloc_dbg(size, _NORMAL_BLOCK, file, line);
#else
    OVR_UNUSED2(file, line);
    return malloc(size);
#endif
}

void* DefaultAllocator::Realloc(void* p, size_t newSize)
{
    return realloc(p, newSize);
}
void DefaultAllocator::Free(void *p)
{
    return free(p);
}


} // OVR
