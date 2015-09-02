/************************************************************************************

Filename    :   OVR_Timer.cpp
Content     :   Provides static functions for precise timing
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

#include "OVR_Timer.h"
#include "OVR_Log.h"

#if defined(OVR_OS_MS) && !defined(OVR_OS_MS_MOBILE)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <MMSystem.h>
#elif defined(OVR_OS_ANDROID)
#include <time.h>
#include <android/log.h>
#elif defined(OVR_OS_MAC)
#include <mach/mach_time.h>
#else
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#endif


#if defined(OVR_BUILD_DEBUG) && defined(OVR_OS_WIN32)
    #ifndef NTSTATUS
        #define NTSTATUS DWORD
    #endif

    typedef NTSTATUS (NTAPI* NtQueryTimerResolutionType)(PULONG MaximumTime, PULONG MinimumTime, PULONG CurrentTime);
    NtQueryTimerResolutionType pNtQueryTimerResolution;
#endif



#if defined(OVR_OS_MS) && !defined(OVR_OS_WIN32) // Non-desktop Microsoft platforms...

// Add this alias here because we're not going to include OVR_CAPI.cpp
extern "C" {
    double ovr_GetTimeInSeconds()
    {
        return Timer::GetSeconds();
    }
}

#endif




namespace OVR {

// For recorded data playback
bool   Timer::useFakeSeconds = false;
double Timer::FakeSeconds    = 0;




//------------------------------------------------------------------------
// *** Android Specific Timer

#if defined(OVR_OS_ANDROID) // To consider: This implementation can also work on most Linux distributions

//------------------------------------------------------------------------
// *** Timer - Platform Independent functions

// Returns global high-resolution application timer in seconds.
double Timer::GetSeconds()
{
	if(useFakeSeconds)
		return FakeSeconds;

    // Choreographer vsync timestamp is based on.
    struct timespec tp;
    const int       status = clock_gettime(CLOCK_MONOTONIC, &tp);

#ifdef OVR_BUILD_DEBUG
    if (status != 0)
    {
        OVR_DEBUG_LOG(("clock_gettime status=%i", status ));
    }
#else
    OVR_UNUSED(status);
#endif

    return (double)tp.tv_sec;
}



uint64_t Timer::GetTicksNanos()
{
    if (useFakeSeconds)
        return (uint64_t) (FakeSeconds * NanosPerSecond);

    // Choreographer vsync timestamp is based on.
    struct timespec tp;
    const int       status = clock_gettime(CLOCK_MONOTONIC, &tp);

#ifdef OVR_BUILD_DEBUG
    if (status != 0)
    {
        OVR_DEBUG_LOG(("clock_gettime status=%i", status ));
    }
#else
    OVR_UNUSED(status);
#endif

    const uint64_t result = (uint64_t)tp.tv_sec * (uint64_t)(1000 * 1000 * 1000) + uint64_t(tp.tv_nsec);
    return result;
}


void Timer::initializeTimerSystem()
{
    // Empty for this platform.
}

void Timer::shutdownTimerSystem()
{
    // Empty for this platform.
}





//------------------------------------------------------------------------
// *** Win32 Specific Timer

#elif defined (OVR_OS_MS)


// This helper class implements high-resolution wrapper that combines timeGetTime() output
// with QueryPerformanceCounter.  timeGetTime() is lower precision but drives the high bits,
// as it's tied to the system clock.
struct PerformanceTimer
{
    PerformanceTimer()
        : UsingVistaOrLater(false),
          TimeCS(),
          OldMMTimeMs(0), 
          MMTimeWrapCounter(0), 
          PerfFrequency(0),
          PerfFrequencyInverse(0),
          PerfFrequencyInverseNanos(0),
          PerfMinusTicksDeltaNanos(0),
          LastResultNanos(0)
    { }
    
    enum {
        MMTimerResolutionNanos = 1000000
    };
   
    void    Initialize();
    void    Shutdown();

    uint64_t  GetTimeSeconds();
    double    GetTimeSecondsDouble();
    uint64_t  GetTimeNanos();

    UINT64 getFrequency()
    {
        if (PerfFrequency == 0)
        {
            LARGE_INTEGER freq;
            QueryPerformanceFrequency(&freq);
            PerfFrequency = freq.QuadPart;
            PerfFrequencyInverse = 1.0 / (double)PerfFrequency;
            PerfFrequencyInverseNanos = 1000000000.0 / (double)PerfFrequency;
        }        
        return PerfFrequency;
    }
    
    double GetFrequencyInverse()
    {
        OVR_ASSERT(PerfFrequencyInverse != 0.0); // Assert that the frequency has been initialized.
        return PerfFrequencyInverse;
    }

	bool            UsingVistaOrLater;

    CRITICAL_SECTION TimeCS;
    // timeGetTime() support with wrap.
    uint32_t        OldMMTimeMs;
    uint32_t        MMTimeWrapCounter;
    // Cached performance frequency result.
    uint64_t        PerfFrequency;              // cycles per second, typically a large value like 3000000, but usually not the same as the CPU clock rate.
    double          PerfFrequencyInverse;       // seconds per cycle (will be a small fractional value).
    double          PerfFrequencyInverseNanos;  // nanoseconds per cycle.
    
    // Computed as (perfCounterNanos - ticksCounterNanos) initially,
    // and used to adjust timing.
    uint64_t        PerfMinusTicksDeltaNanos;
    // Last returned value in nanoseconds, to ensure we don't back-step in time.
    uint64_t        LastResultNanos;
};

static PerformanceTimer Win32_PerfTimer;


void PerformanceTimer::Initialize()
{
    #if defined(OVR_OS_WIN32) // Desktop Windows only
        // The following has the effect of setting the NT timer resolution (NtSetTimerResolution) to 1 millisecond.
        MMRESULT mmr = timeBeginPeriod(1);
        OVR_ASSERT(TIMERR_NOERROR == mmr);
        OVR_UNUSED(mmr);
    #endif

    InitializeCriticalSection(&TimeCS);
    MMTimeWrapCounter = 0;
    getFrequency();

    #if defined(OVR_OS_WIN32) // Desktop Windows only
	    // Set Vista flag.  On Vista, we can just use QPC() without all the extra work
        OSVERSIONINFOEX ver;
	    ZeroMemory(&ver, sizeof(OSVERSIONINFOEX));
	    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	    ver.dwMajorVersion = 6; // Vista+

        DWORDLONG condMask = 0;
        VER_SET_CONDITION(condMask, VER_MAJORVERSION, VER_GREATER_EQUAL);

	    // VerifyVersionInfo returns true if the OS meets the conditions set above
	    UsingVistaOrLater = VerifyVersionInfo(&ver, VER_MAJORVERSION, condMask) != 0;
    #else
        UsingVistaOrLater = true;
    #endif

	OVR_DEBUG_LOG(("PerformanceTimer UsingVistaOrLater = %d", (int)UsingVistaOrLater));

    #if defined(OVR_BUILD_DEBUG) && defined(OVR_OS_WIN32)
        HMODULE hNtDll = LoadLibrary(L"NtDll.dll");
        if (hNtDll)
        {
            pNtQueryTimerResolution = (NtQueryTimerResolutionType)GetProcAddress(hNtDll, "NtQueryTimerResolution");
          //pNtSetTimerResolution = (NtSetTimerResolutionType)GetProcAddress(hNtDll, "NtSetTimerResolution");

            if(pNtQueryTimerResolution)
            {
                ULONG MinimumResolution; // in 100-ns units
                ULONG MaximumResolution;
                ULONG ActualResolution;
                pNtQueryTimerResolution(&MinimumResolution, &MaximumResolution, &ActualResolution);
	            OVR_DEBUG_LOG(("NtQueryTimerResolution = Min %ld us, Max %ld us, Current %ld us", MinimumResolution / 10, MaximumResolution / 10, ActualResolution / 10));
            }

            FreeLibrary(hNtDll);
        }
    #endif
}

void PerformanceTimer::Shutdown()
{
    DeleteCriticalSection(&TimeCS);

    #if defined(OVR_OS_WIN32) // Desktop Windows only
        MMRESULT mmr = timeEndPeriod(1);
        OVR_ASSERT(TIMERR_NOERROR == mmr);
        OVR_UNUSED(mmr);
    #endif
}


uint64_t PerformanceTimer::GetTimeSeconds()
{
	if (UsingVistaOrLater)
	{
        LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
        OVR_ASSERT(PerfFrequencyInverse != 0); // Initialize should have been called earlier.
        return (uint64_t)(li.QuadPart * PerfFrequencyInverse);
    }

    return (uint64_t)(GetTimeNanos() * .0000000001);
}


double PerformanceTimer::GetTimeSecondsDouble()
{
	if (UsingVistaOrLater)
	{
        LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
        OVR_ASSERT(PerfFrequencyInverse != 0);
        return (li.QuadPart * PerfFrequencyInverse);
    }

    return (GetTimeNanos() * .0000000001);
}


uint64_t PerformanceTimer::GetTimeNanos()
{
    uint64_t      resultNanos;
    LARGE_INTEGER li;

    OVR_ASSERT(PerfFrequencyInverseNanos != 0); // Initialize should have been called earlier.

    if (UsingVistaOrLater) // Includes non-desktop platforms
	{
		// Then we can use QPC() directly without all that extra work
		QueryPerformanceCounter(&li);
        resultNanos = (uint64_t)(li.QuadPart * PerfFrequencyInverseNanos);
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
        uint64_t  perfCounterNanos   = (uint64_t)(li.QuadPart * PerfFrequencyInverseNanos);
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


//------------------------------------------------------------------------
// *** Timer - Platform Independent functions

// Returns global high-resolution application timer in seconds.
double Timer::GetSeconds()
{
	if(useFakeSeconds)
		return FakeSeconds;

    return Win32_PerfTimer.GetTimeSecondsDouble();
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



#elif defined(OVR_OS_MAC)


double Timer::TimeConvertFactorNanos   = 0.0;
double Timer::TimeConvertFactorSeconds = 0.0;


//------------------------------------------------------------------------
// *** Standard OS Timer     

// Returns global high-resolution application timer in seconds.
double Timer::GetSeconds()
{
	if(useFakeSeconds)
		return FakeSeconds;
    
    OVR_ASSERT(TimeConvertFactorNanos != 0.0);
    return (double)mach_absolute_time() * TimeConvertFactorNanos;
}


uint64_t Timer::GetTicksNanos()
{
    if (useFakeSeconds)
        return (uint64_t) (FakeSeconds * NanosPerSecond);
    
    OVR_ASSERT(TimeConvertFactorSeconds != 0.0);
    return (uint64_t)(mach_absolute_time() * TimeConvertFactorSeconds);
}

void Timer::initializeTimerSystem()
{
    mach_timebase_info_data_t timeBase;
    mach_timebase_info(&timeBase);
    TimeConvertFactorSeconds = ((double)timeBase.numer / (double)timeBase.denom);
    TimeConvertFactorNanos   = TimeConvertFactorSeconds / 1000000000.0;
}

void Timer::shutdownTimerSystem()
{
    // Empty for this platform.
}


#else // Posix platforms (e.g. Linux, BSD Unix)


bool Timer::MonotonicClockAvailable = false;


// Returns global high-resolution application timer in seconds.
double Timer::GetSeconds()
{
	if(useFakeSeconds)
		return FakeSeconds;

    // http://linux/die/netman3/clock_gettime
    #if defined(CLOCK_MONOTONIC) // If we can use clock_gettime, which has nanosecond precision...
        if(MonotonicClockAvailable)
        {
            timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts); // Better to use CLOCK_MONOTONIC than CLOCK_REALTIME.
            return static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) / 1E9;
        }
    #endif

    // We cannot use rdtsc because its frequency changes at runtime.
    struct timeval tv;
    gettimeofday(&tv, 0);

    return static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) / 1E6;
}


uint64_t Timer::GetTicksNanos()
{
    if (useFakeSeconds)
        return (uint64_t) (FakeSeconds * NanosPerSecond);

    #if defined(CLOCK_MONOTONIC) // If we can use clock_gettime, which has nanosecond precision...
        if(MonotonicClockAvailable)
        {
            timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return ((uint64_t)ts.tv_sec * 1000000000ULL) + (uint64_t)ts.tv_nsec;
        }
    #endif


    // We cannot use rdtsc because its frequency changes at runtime.
	uint64_t result;

    // Return microseconds.
    struct timeval tv;

    gettimeofday(&tv, 0);

    result = (uint64_t)tv.tv_sec * 1000000;
    result += tv.tv_usec;

    return result * 1000;
}


void Timer::initializeTimerSystem()
{
    #if defined(CLOCK_MONOTONIC)
        timespec ts; // We could also check for the availability of CLOCK_MONOTONIC with sysconf(_SC_MONOTONIC_CLOCK)
        int result = clock_gettime(CLOCK_MONOTONIC, &ts);
        MonotonicClockAvailable = (result == 0);
    #endif
}

void Timer::shutdownTimerSystem()
{
    // Empty for this platform.
}



#endif  // OS-specific



} // OVR

