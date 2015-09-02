/************************************************************************************

Filename    :   CAPI_LatencyStatistics.h
Content     :   Record latency statistics during rendering
Created     :   Sept 23, 2014
Authors     :   Chris Taylor, Kevin Jenkins

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

#ifndef OVR_CAPI_LatencyStatistics_h
#define OVR_CAPI_LatencyStatistics_h

#include "../OVR_CAPI.h"
#include "../Kernel/OVR_Timer.h"
#include "../Kernel/OVR_Lockless.h"
#include "../Kernel/OVR_SysFile.h"
#include "CAPI_FrameTimeManager.h"

namespace OVR { namespace CAPI {


// Define epoch period for lag statistics
#define OVR_LAG_STATS_EPOCH 1.0 /* seconds */
// Define seconds without frames before resetting stats
#define OVR_LAG_STATS_RESET_LIMIT 2.0 /* seconds */


//-------------------------------------------------------------------------------------
// ***** LatencyStatisticsResults

// Results from statistics collection
struct LatencyStatisticsResults
{
    // Number of seconds of data represented by these statistics.
    double IntervalSeconds;

    // Frames per second during the epoch.
    double FPS;

    // Measures average time for EndFrame() call over each of the frames.
    double EndFrameExecutionTime;

    // Measures the time between first scanline and first pose request before app starts rendering eyes.
    float LatencyRender;

    // Measures the time between first scanline and distortion/timewarp rendering.
    float LatencyTimewarp;

    // Time between Present() call and photon emission from first scanline of screen
    float LatencyPostPresent;

    // Measures the time from receiving the camera frame until vision CPU processing completes.
    double LatencyVisionProc;

    // Measures the time from exposure until the pose is available for the frame, including processing time.
    double LatencyVisionFrame;
};

//-----------------------------------------------------------------------------
typedef Delegate1<void, LatencyStatisticsResults*> LatencyStatisticsSlot;

// ***** LatencyStatisticsObserver
class LatencyStatisticsCSV
{
public:
    LatencyStatisticsCSV();
    ~LatencyStatisticsCSV();
    bool Start(String fileName, String userData1);
    bool Stop();
    void OnResults(LatencyStatisticsResults *results);

    // Internal
    void WriteHeaderV1();
    void WriteResultsV1(LatencyStatisticsResults *results);
    ObserverScope<LatencyStatisticsSlot>* GetObserver() { return &_Observer; }

protected:
    ObserverScope<LatencyStatisticsSlot> _Observer;
    String Guid, UserData1;
    String FileName;
    OVR::SysFile _File;
    String OS, OSVersion, ProcessInfo, DisplayDriverVersion, CameraDriverVersion, GPUVersion;
};

//-----------------------------------------------------------------------------
// ***** LatencyStatisticsCalculator

// Calculator for latency statistics
class LagStatsCalculator
{
    // Statistics instrumentation inputs:

    // Timestamp when the EndFrame() call started executing
    double              EndFrameStartTime;
    // Timestamp when the EndFrame() call finished executing
    double              EndFrameEndTime;

    // Latency statistics for the epoch
    LatencyStatisticsResults latencyStatisticsData;

    // Last latency data
    // float               LatencyData[3]; // render, timewarp, median post-present

    // Last vision processing frame counter number
    uint32_t            LastCameraFrameCounter;

    // Running statistics:

    void resetPerfStats(double resetTime = 0.);

    // Start of the current statistics epoch
    double              EpochBegin;
    // Count of frames in this stats epoch, measured at EndFrame()
    int                 FrameCount;
    // Sum of end frame durations for this stats epoch
    //double              EndFrameSum;

    // Sum of latencies
    // double              LatencySum[3];

    // Sum of vision processing times
    //double              VisionProcSum;
    // Sum of vision latency times
    //double              VisionLagSum;
    // Count of vision frames
    int                 VisionFrames;

    // Statistics results:

    LocklessUpdater<LatencyStatisticsResults, LatencyStatisticsResults> Results;

    void calculateResults();

    OVR::ObserverScope<LatencyStatisticsSlot> calculateResultsSubject;
    OVR::Lock calculateResultsLock;

public:
    LagStatsCalculator();

    void GetLatestResults(LatencyStatisticsResults* results);
    void AddResultsObserver(ObserverScope<LatencyStatisticsSlot> *calculateResultsObserver);

public:
    // Internal instrumentation interface:

    // EndFrame() instrumentation
    void InstrumentEndFrameStart(double timestamp);
    void InstrumentEndFrameEnd(double timestamp);

    // DK2 latency tester instrumentation
    // Note: This assumes that it is called right before InstrumentEndFrameEnd()
    void InstrumentLatencyTimings(FrameTimeManager& ftm);

    // Eye pose instrumentation
    void InstrumentEyePose(const ovrTrackingState& state);
};


}} // namespace OVR::CAPI

#endif // OVR_CAPI_LatencyStatistics_h
