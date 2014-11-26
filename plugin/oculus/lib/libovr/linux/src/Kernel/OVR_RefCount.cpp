/************************************************************************************

Filename    :   OVR_RefCount.cpp
Content     :   Reference counting implementation
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

#include "OVR_RefCount.h"
#include "OVR_Atomic.h"
#include "OVR_Log.h"

namespace OVR {

#ifdef OVR_CC_ARM
void* ReturnArg0(void* p)
{
    return p;
}
#endif

// ***** Reference Count Base implementation

RefCountImplCore::~RefCountImplCore()
{
    // RefCount can be either 1 or 0 here.
    //  0 if Release() was properly called.
    //  1 if the object was declared on stack or as an aggregate.
    OVR_ASSERT(RefCount <= 1);
}

#ifdef OVR_BUILD_DEBUG
void RefCountImplCore::reportInvalidDelete(void *pmem)
{
    OVR_DEBUG_LOG(
        ("Invalid delete call on ref-counted object at %p. Please use Release()", pmem));
    OVR_ASSERT(0);
}
#endif

RefCountNTSImplCore::~RefCountNTSImplCore()
{
    // RefCount can be either 1 or 0 here.
    //  0 if Release() was properly called.
    //  1 if the object was declared on stack or as an aggregate.
    OVR_ASSERT(RefCount <= 1);
}

#ifdef OVR_BUILD_DEBUG
void RefCountNTSImplCore::reportInvalidDelete(void *pmem)
{
    OVR_DEBUG_LOG(
        ("Invalid delete call on ref-counted object at %p. Please use Release()", pmem));
    OVR_ASSERT(0);
}
#endif


// *** Thread-Safe RefCountImpl

void    RefCountImpl::AddRef()
{
    AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, 1);
}
void    RefCountImpl::Release()
{
    if ((AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, -1) - 1) == 0)
        delete this;
}

// *** Thread-Safe RefCountVImpl w/virtual AddRef/Release

void    RefCountVImpl::AddRef()
{
    AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, 1);
}
void    RefCountVImpl::Release()
{
    if ((AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, -1) - 1) == 0)
        delete this;
}

// *** NON-Thread-Safe RefCountImpl

void    RefCountNTSImpl::Release() const
{
    RefCount--;
    if (RefCount == 0)
        delete this;
}


} // OVR
