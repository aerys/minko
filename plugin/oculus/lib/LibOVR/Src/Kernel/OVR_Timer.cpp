/************************************************************************************

Filename    :   OVR_Timer.cpp
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

#include "OVR_Timer.h"
#include "OVR_Log.h"

#if defined (OVR_OS_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <MMSystem.h>
#elif defined(OVR_OS_ANDROID)
#include <time.h>
#include <android/log.h>

#else
#include <sys/time.h>
#endif

namespace OVR {

// For recorded data playback
bool   Timer::useFakeSeconds  = false; 
double Timer::FakeSeconds     = 0;


//------------------------------------------------------------------------
// *** Timer - Platform Independent functions

// Returns global high-resolution application timer in seconds.
double Timer::GetSeconds()
{
	if(useFakeSeconds)
		return FakeSeconds;

    return double(Timer::GetTicksNanos()) * 0.000000001;
}


#if !defined(OVR_OS_WIN32)

// Unused on OSs other then Win32.
void Timer::initializeTimerSystem()
{
}
void Timer::shutdownTimerSystem()
{
}

#endif



//------------------------------------------------------------------------
// *** Android Specific Timer

#if defined(OVR_OS_ANDROID)

uint64_t Timer::GetTicksNanos()
{
    if (useFakeSeconds)
        return (uint64_t) (FakeSeconds * NanosPerSecond);

    // Choreographer vsync timestamp is based on.
    struct timespec tp;
    const int       status = clock_gettime(CLOCK_MONOTONIC, &tp);

    if (status != 0)
    {
        OVR_DEBUG_LOG(("clock_gettime status=%i", status ));
    }
    const uint64_t result = (uint64_t)tp.tv_sec * (uint64_t)(1000 * 1000 * 1000) + uint64_t(tp.tv_nsec);
    return result;
}


//------------------------------------------------------------------------
// *** Win32 Specific Timer

#elif defined (OVR_OS_WIN32)


// This helper class implements high-resolution wrapper that combines timeGetTime() output
// with QueryPerformanceCounter.  timeGetTime() is lower precision but drives the high bits,
// as it's tied to the system clock.
struct PerformanceTimer
{
    PerformanceTimer()
        : OldMMTimeMs(0), MMTimeWrapCounter(0), PrefFrequency(0),
          LastResultNanos(0), PerfMinusTicksDeltaNanos(0)
    { }
    
    enum {
        MMTimerResolutionNanos = 1000000
    };
   
    void    Initialize();
    void    Shutdown();

    uint64_t  GetTimeNanos();


    UINT64 getFrequency()
    {
        if (PrefFrequency == 0)
        {
            LARGE_INTEGER freq;
            QueryPerformanceFrequency(&freq);
            PrefFrequency = freq.QuadPart;
        }        
        return PrefFrequency;
    }
    
	bool            UsingVista;

    CRITICAL_SECTION TimeCS;
    // timeGetTime() support with wrap.
    uint32_t        OldMMTimeMs;
    uint32_t        MMTimeWrapCounter;
    // Cached performance frequency result.
    uint64_t        PrefFrequency;
    
    // Computed as (perfCounterNanos - ticksCounterNanos) initially,
    // and used to adjust timing.
    uint64_t        PerfMinusTicksDeltaNanos;
    // Last returned value in nanoseconds, to ensure we don't back-step in time.
    uint64_t        LastResultNanos;
};

PerformanceTimer Win32_PerfTimer;


void PerformanceTimer::Initialize()
{
    MMRESULT mmr = timeBeginPeriod(1);
    OVR_ASSERT(TIMERR_NOERROR == mmr);
    OVR_UNUSED(mmr);

    InitializeCriticalSection(&TimeCS);
    MMTimeWrapCounter = 0;
    getFrequency();

	// Set Vista flag.  On Vista, we can just use QPC() without all the extra work
    OSVERSIONINFOEX ver;
	ZeroMemory(&ver, sizeof(OSVERSIONINFOEX));
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	ver.dwMajorVersion = 6; // Vista+

    DWORDLONG condMask = 0;
    VER_SET_CONDITION(condMask, VER_MAJORVERSION, VER_GREATER_EQUAL);

	// VerifyVersionInfo returns true if the OS meets the conditions set above
	UsingVista = VerifyVersionInfo(&ver, VER_MAJORVERSION, condMask) != 0;

	OVR_DEBUG_LOG(("Performance timer Vista flag = %d", (int)UsingVista));
}

void PerformanceTimer::Shutdown()
{
    DeleteCriticalSection(&TimeCS);
    MMRESULT mmr = timeEndPeriod(1);
    OVR_ASSERT(TIMERR_NOERROR == mmr);
    OVR_UNUSED(mmr);
}

uint64_t PerformanceTimer::GetTimeNanos()
{
    uint64_t        resultNanos;
    LARGE_INTEGER   li;

	// If on Vista,
	if (UsingVista)
	{
		// Then we can use QPC() directly without all that extra work
		QueryPerformanceCounter(&li);

		uint64_t  frequency = getFrequency();
		uint64_t  perfCounterSeconds = (uint64_t)li.QuadPart / frequency;
		uint64_t  perfRemainderNanos = (((uint64_t)li.QuadPart - perfCounterSeconds * frequency) *
									  Timer::NanosPerSecond) / frequency;
		resultNanos = perfCounterSeconds * Timer::NanosPerSecond + perfRemainderNanos;
	}
	else
	{

    // On Win32 QueryPerformanceFrequency is unreliable due to SMP and
    // performance levels, so use this logic to detect wrapping and track
    // high bits.
    ::EnterCriticalSection(&TimeCS);

    // Get raw value and perf counter "At the same time".
    QueryPerformanceCounter(&li);

    DWORD mmTimeMs = timeGetTime();
    if (OldMMTimeMs > mmTimeMs)
        MMTimeWrapCounter++;
    OldMMTimeMs = mmTimeMs;

    // Normalize to nanoseconds.
    uint64_t  frequency          = getFrequency();
    uint64_t  perfCounterSeconds = uint64_t(li.QuadPart) / frequency;
    uint64_t  perfRemainderNanos = ( (uint64_t(li.QuadPart) - perfCounterSeconds * frequency) *
                                   Timer::NanosPerSecond ) / frequency;
    uint64_t  perfCounterNanos   = perfCounterSeconds * Timer::NanosPerSecond + perfRemainderNanos;

    uint64_t  mmCounterNanos     = ((uint64_t(MMTimeWrapCounter) << 32) | mmTimeMs) * 1000000;
    if (PerfMinusTicksDeltaNanos == 0)
        PerfMinusTicksDeltaNanos = perfCounterNanos - mmCounterNanos;
 
    // Compute result before snapping. 
    //
    // On first call, this evaluates to:
    //          resultNanos = mmCounterNanos.    
    // Next call, assuming no wrap:
    //          resultNanos = prev_mmCounterNanos + (perfCounterNanos - prev_perfCounterNanos).        
    // After wrap, this would be:
    //          resultNanos = snapped(prev_mmCounterNanos +/- 1ms) + (perfCounterNanos - prev_perfCounterNanos).
    //
    resultNanos = perfCounterNanos - PerfMinusTicksDeltaNanos;    

    // Snap the range so that resultNanos never moves further apart then its target resolution.
    // It's better to allow more slack on the high side as timeGetTime() may be updated at sporadically 
    // larger then 1 ms intervals even when 1 ms resolution is requested.
    if (resultNanos > (mmCounterNanos + MMTimerResolutionNanos*2))
    {
        resultNanos = mmCounterNanos + MMTimerResolutionNanos*2;
        if (resultNanos < LastResultNanos)
            resultNanos = LastResultNanos;
        PerfMinusTicksDeltaNanos = perfCounterNanos - resultNanos;
    }
    else if (resultNanos < (mmCounterNanos - MMTimerResolutionNanos))
    {
        resultNanos = mmCounterNanos - MMTimerResolutionNanos;
        if (resultNanos < LastResultNanos)
            resultNanos = LastResultNanos;
        PerfMinusTicksDeltaNanos = perfCounterNanos - resultNanos;
    }

    LastResultNanos = resultNanos;
    ::LeaveCriticalSection(&TimeCS);
	}

	//Tom's addition, to keep precision
	//static uint64_t    initial_time = 0;
	//if (!initial_time) initial_time = resultNanos;
	//resultNanos -= initial_time;
	// FIXME: This cannot be used for cross-process timestamps

    return resultNanos;
}


// Delegate to PerformanceTimer.
uint64_t Timer::GetTicksNanos()
{
    if (useFakeSeconds)
        return (uint64_t) (FakeSeconds * NanosPerSecond);

    return Win32_PerfTimer.GetTimeNanos();
}
void Timer::initializeTimerSystem()
{
    Win32_PerfTimer.Initialize();

}
void Timer::shutdownTimerSystem()
{
    Win32_PerfTimer.Shutdown();
}

#else   // !OVR_OS_WIN32 && !OVR_OS_ANDROID


//------------------------------------------------------------------------
// *** Standard OS Timer     

uint64_t Timer::GetTicksNanos()
{
    if (useFakeSeconds)
        return (uint64_t) (FakeSeconds * NanosPerSecond);

    // TODO: prefer rdtsc when available?
	uint64_t result;

    // Return microseconds.
    struct timeval tv;

    gettimeofday(&tv, 0);

    result = (uint64_t)tv.tv_sec * 1000000;
    result += tv.tv_usec;

    return result * 1000;
}

#endif  // OS-specific



} // OVR

