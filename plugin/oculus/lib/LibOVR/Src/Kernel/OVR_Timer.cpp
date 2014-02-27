/************************************************************************************

Filename    :   OVR_Timer.cpp
Content     :   Provides static functions for precise timing
Created     :   September 19, 2012
Notes       : 

Copyright   :   Copyright 2013 Oculus VR, Inc. All Rights reserved.

Licensed under the Oculus VR SDK License Version 2.0 (the "License"); 
you may not use the Oculus VR SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#include "OVR_Timer.h"
#include "OVR_Log.h"

#if defined (OVR_OS_WIN32)
#include <windows.h>
#elif defined(OVR_OS_ANDROID)
#include <time.h>
#include <android/log.h>

#else
#include <sys/time.h>
#endif

namespace OVR {


//------------------------------------------------------------------------
// *** Timer - Platform Independent functions

double ovr_GetTimeInSeconds()
{
    return Timer::GetSeconds();
}


UInt64 Timer::GetProfileTicks()
{
    return (GetRawTicks() * MksPerSecond) / GetRawFrequency();
}
double Timer::GetProfileSeconds()
{
    static UInt64 StartTime = GetProfileTicks();
    return TicksToSeconds(GetProfileTicks()-StartTime);
}

#ifndef OVR_OS_ANDROID

double Timer::GetSeconds()
{
    return (double)Timer::GetRawTicks() / (double) GetRawFrequency();
}
#endif


//------------------------------------------------------------------------
// *** Android Specific Timer


#if defined(OVR_OS_ANDROID)

// Returns global high-resolution application timer in seconds.
double Timer::GetSeconds()
{
    return double(Timer::GetRawTicks()) * 0.000000001;
}

UInt64 Timer::GetRawTicks()
{
    // Choreographer vsync timestamp is based on.
    struct timespec tp;
    const int       status = clock_gettime(CLOCK_MONOTONIC, &tp);

    if (status != 0)
    {
        OVR_DEBUG_LOG(("clock_gettime status=%i", status ));
    }
    const UInt64 result = (UInt64)tp.tv_sec * (UInt64)(1000 * 1000 * 1000) + UInt64(tp.tv_nsec);
    return result;
}

UInt64 Timer::GetRawFrequency()
{
    return MksPerSecond * 1000;
}

#endif


//------------------------------------------------------------------------
// *** Win32 Specific Timer

#if defined (OVR_OS_WIN32)

CRITICAL_SECTION WinAPI_GetTimeCS;
volatile UInt32  WinAPI_OldTime = 0;
volatile UInt32  WinAPI_WrapCounter = 0;


UInt32 Timer::GetTicksMs()
{
    return timeGetTime();
}

UInt64 Timer::GetTicks()
{
    DWORD  ticks = timeGetTime();
    UInt64 result;

    // On Win32 QueryPerformanceFrequency is unreliable due to SMP and
    // performance levels, so use this logic to detect wrapping and track
    // high bits.
    ::EnterCriticalSection(&WinAPI_GetTimeCS);

    if (WinAPI_OldTime > ticks)
        WinAPI_WrapCounter++;
    WinAPI_OldTime = ticks;

    result = (UInt64(WinAPI_WrapCounter) << 32) | ticks;
    ::LeaveCriticalSection(&WinAPI_GetTimeCS);

    return result * MksPerMs;
}

UInt64 Timer::GetRawTicks()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return li.QuadPart;
}

UInt64 Timer::GetRawFrequency()
{
    static UInt64 perfFreq = 0;
    if (perfFreq == 0)
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        perfFreq = freq.QuadPart;
    }
    return perfFreq;
}

void Timer::initializeTimerSystem()
{
    timeBeginPeriod(1);
    InitializeCriticalSection(&WinAPI_GetTimeCS);

}
void Timer::shutdownTimerSystem()
{
    DeleteCriticalSection(&WinAPI_GetTimeCS);
    timeEndPeriod(1);
}

#else   // !OVR_OS_WIN32


//------------------------------------------------------------------------
// *** Standard OS Timer     

UInt32 Timer::GetTicksMs()
{
    return (UInt32)(GetProfileTicks() / 1000);
}
// The profile ticks implementation is just fine for a normal timer.
UInt64 Timer::GetTicks()
{
    return GetProfileTicks();
}

void Timer::initializeTimerSystem()
{
}
void Timer::shutdownTimerSystem()
{
}


#if !defined(OVR_OS_ANDROID)

UInt64  Timer::GetRawTicks()
{
    // TODO: prefer rdtsc when available?
	UInt64 result;

    // Return microseconds.
    struct timeval tv;

    gettimeofday(&tv, 0);

    result = (UInt64)tv.tv_sec * 1000000;
    result += tv.tv_usec;

    return result;
}

UInt64 Timer::GetRawFrequency()
{
    return MksPerSecond;
}

#endif // !OVR_OS_ANDROID

#endif  // !OVR_OS_WIN32



} // OVR

