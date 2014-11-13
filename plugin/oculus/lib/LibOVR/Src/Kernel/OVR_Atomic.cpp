/************************************************************************************

Filename    :   OVR_Atomic.cpp
Content     :   Contains atomic operations and inline fastest locking
                functionality. Will contain #ifdefs for OS efficiency.
                Have non-thread-safe implementation if not available.
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

#include "OVR_Atomic.h"
#include "OVR_Allocator.h"

#ifdef OVR_ENABLE_THREADS

// Include Windows 8-Metro compatible Synchronization API
#if defined(OVR_OS_WIN32) && defined(NTDDI_WIN8) && (NTDDI_VERSION >= NTDDI_WIN8)
#include <synchapi.h>
#endif


namespace OVR {

// ***** Windows Lock implementation

#if defined(OVR_OS_WIN32)

// ***** Standard Win32 Lock implementation

// Constructors
Lock::Lock(unsigned spinCount)
{
#if defined(NTDDI_WIN8) && (NTDDI_VERSION >= NTDDI_WIN8)
   // On Windows 8 we use InitializeCriticalSectionEx due to Metro-Compatibility
   InitializeCriticalSectionEx(&cs, spinCount,
                               OVR_DEBUG_SELECT(NULL, CRITICAL_SECTION_NO_DEBUG_INFO));
#else
    // Spin count init critical section function prototype for Window NT
    typedef BOOL (WINAPI *Function_InitializeCriticalSectionAndSpinCount) 
                 (LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);


    // Try to load function dynamically so that we don't require NT
    // On Windows NT we will use InitializeCriticalSectionAndSpinCount
    static  bool initTried = 0;
    static  Function_InitializeCriticalSectionAndSpinCount pInitFn = 0;

    if (!initTried)
    {
        HMODULE hmodule = ::LoadLibrary(OVR_STR("kernel32.dll"));
        pInitFn     = (Function_InitializeCriticalSectionAndSpinCount)
                      ::GetProcAddress(hmodule, "InitializeCriticalSectionAndSpinCount");
        initTried   = true;
    }

    // Initialize the critical section
    if (pInitFn)
        pInitFn(&cs, spinCount);
    else
        ::InitializeCriticalSection(&cs);
#endif
   
}


Lock::~Lock()
{
    DeleteCriticalSection(&cs);
}


#endif


//-------------------------------------------------------------------------------------
// ***** SharedLock

// This is a general purpose globally shared Lock implementation that should probably be
// moved to Kernel.
// May in theory busy spin-wait if we hit contention on first lock creation,
// but this shouldn't matter in practice since Lock* should be cached.


enum { LockInitMarker = 0xFFFFFFFF };

Lock* SharedLock::GetLockAddRef()
{
    int oldUseCount;

    do {
        oldUseCount = UseCount;
        if (oldUseCount == (int)LockInitMarker)
            continue;

        if (oldUseCount == 0)
        {
            // Initialize marker
            if (AtomicOps<int>::CompareAndSet_Sync(&UseCount, 0, LockInitMarker))
            {
                Construct<Lock>(Buffer);
                do { }
                while (!AtomicOps<int>::CompareAndSet_Sync(&UseCount, LockInitMarker, 1));
                return toLock();
            }
            continue;
        }

    } while (!AtomicOps<int>::CompareAndSet_NoSync(&UseCount, oldUseCount, oldUseCount + 1));

    return toLock();
}

void SharedLock::ReleaseLock(Lock* plock)
{
    OVR_UNUSED(plock);
    OVR_ASSERT(plock == toLock());

    int oldUseCount;

    do {
        oldUseCount = UseCount;
        OVR_ASSERT(oldUseCount != (int)LockInitMarker);

        if (oldUseCount == 1)
        {
            // Initialize marker
            if (AtomicOps<int>::CompareAndSet_Sync(&UseCount, 1, LockInitMarker))
            {
                Destruct<Lock>(toLock());

                do { }
                while (!AtomicOps<int>::CompareAndSet_Sync(&UseCount, LockInitMarker, 0));

                return;
            }
            continue;
        }

    } while (!AtomicOps<int>::CompareAndSet_NoSync(&UseCount, oldUseCount, oldUseCount - 1));
}

} // OVR

#endif // OVR_ENABLE_THREADS
