/************************************************************************************

Filename    :   OVR_System.cpp
Content     :   General kernel initialization/cleanup, including that
                of the memory allocator.
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

#include "OVR_System.h"
#include "OVR_Threads.h"
#include "OVR_Timer.h"
#include "../Displays/OVR_Display.h"
#ifdef OVR_OS_WIN32
#include "../Displays/OVR_Win32_ShimFunctions.h"
#endif

namespace OVR {

extern bool anyRiftsInExtendedMode();

// Stack of destroy listeners (push/pop semantics)
static SystemSingletonInternal *SystemShutdownListenerStack = 0;
static Lock stackLock;

void SystemSingletonInternal::PushDestroyCallbacks()
{
    Lock::Locker locker(&stackLock);

    // Push listener onto the stack
    NextSingleton = SystemShutdownListenerStack;
    SystemShutdownListenerStack = this;
}

void System::DirectDisplayInitialize()
{
#ifdef OVR_OS_WIN32
	// Set up display code for Windows
	Win32::DisplayShim::GetInstance();

	// This code will look for the first display. If it's a display
	// that's extending the destkop, the code will assume we're in
	// compatibility mode. Compatibility mode prevents shim loading
	// and renders only to extended Rifts.
	// If we find a display and it's application exclusive,
	// we load the shim so we can render to it.
	// If no display is available, we revert to whatever the
	// driver tells us we're in

	bool anyExtendedRifts = anyRiftsInExtendedMode() || Display::InCompatibilityMode( false );
	
	Win32::DisplayShim::GetInstance().Initialize(anyExtendedRifts);
#endif
}

// Initializes System core, installing allocator.
void System::Init(Log* log, Allocator *palloc)
{    
    if (!Allocator::GetInstance())
    {
        Log::SetGlobalLog(log);
        Timer::initializeTimerSystem();
        Allocator::setInstance(palloc);
		Display::Initialize();
		DirectDisplayInitialize();
    }
    else
    {
        OVR_DEBUG_LOG(("System::Init failed - duplicate call."));
    }
}

void System::Destroy()
{    
    if (Allocator::GetInstance())
    {
#ifdef OVR_OS_WIN32
		Win32::DisplayShim::GetInstance().Shutdown();
#endif

		// Invoke all of the post-finish callbacks (normal case)
        for (SystemSingletonInternal *listener = SystemShutdownListenerStack; listener; listener = listener->NextSingleton)
		{
			listener->OnThreadDestroy();
		}

#ifdef OVR_ENABLE_THREADS
		// Wait for all threads to finish; this must be done so that memory
		// allocator and all destructors finalize correctly.
		Thread::FinishAllThreads();
#endif

		// Invoke all of the post-finish callbacks (normal case)
        for (SystemSingletonInternal *next, *listener = SystemShutdownListenerStack; listener; listener = next)
		{
            next = listener->NextSingleton;

			listener->OnSystemDestroy();
		}

        SystemShutdownListenerStack = 0;

		// Shutdown heap and destroy SysAlloc singleton, if any.
        Allocator::GetInstance()->onSystemShutdown();
        Allocator::setInstance(0);

        Timer::shutdownTimerSystem();
        Log::SetGlobalLog(Log::GetDefaultLog());
    }
    else
    {
        OVR_DEBUG_LOG(("System::Destroy failed - System not initialized."));
    }
}

// Returns 'true' if system was properly initialized.
bool System::IsInitialized()
{
    return Allocator::GetInstance() != 0;
}


} // namespace OVR
