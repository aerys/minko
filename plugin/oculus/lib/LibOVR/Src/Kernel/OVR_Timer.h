/************************************************************************************

PublicHeader:   OVR
Filename    :   OVR_Timer.h
Content     :   Provides static functions for precise timing
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

#ifndef OVR_Timer_h
#define OVR_Timer_h

#include "OVR_Types.h"

namespace OVR {
    
//-----------------------------------------------------------------------------------
// ***** Timer

// Timer class defines a family of static functions used for application
// timing and profiling.

class Timer
{
public:
    enum {
        MsPerSecond     = 1000, // Milliseconds in one second.
        NanosPerSecond  = MsPerSecond * 1000 * 1000,
        MksPerSecond    = MsPerSecond * 1000
    };

    // ***** Timing APIs for Application    

    // These APIs should be used to guide animation and other program functions
    // that require precision.

    // Returns global high-resolution application timer in seconds.
    static double  OVR_STDCALL GetSeconds();    

    // Returns time in Nanoseconds, using highest possible system resolution.
    static uint64_t  OVR_STDCALL GetTicksNanos();

    // Kept for compatibility.
    // Returns ticks in milliseconds, as a 32-bit number. May wrap around every 49.2 days.
    // Use either time difference of two values of GetTicks to avoid wrap-around.
    static uint32_t  OVR_STDCALL GetTicksMs()
    { return  uint32_t(GetTicksNanos() / 1000000); }

    // for recorded data playback
    static void SetFakeSeconds(double fakeSeconds) 
    { 
        FakeSeconds = fakeSeconds; 
        useFakeSeconds = true; 
    }

private:
    friend class System;
    // System called during program startup/shutdown.
    static void initializeTimerSystem();
    static void shutdownTimerSystem();

    // for recorded data playback
    static double FakeSeconds;
    static bool   useFakeSeconds; 
};


} // OVR::Timer

#endif
