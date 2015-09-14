/************************************************************************************

Filename    :   CAPI_LatencyStatistics.cpp
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

#include "CAPI_LatencyStatistics.h"

#include "../Kernel/OVR_Log.h"
#include "../Kernel/OVR_Threads.h"
#include "../Util/Util_SystemInfo.h"

namespace OVR { namespace CAPI {

//-----------------------------------------------------------------------------
// ***** LatencyStatisticsObserver

LatencyStatisticsCSV::LatencyStatisticsCSV()
{
}

LatencyStatisticsCSV::~LatencyStatisticsCSV()
{
    Stop();
}

bool LatencyStatisticsCSV::Start(String fileName, String userData1)
{
    if (_File.IsValid())
    {
        if (fileName == FileName)
        {
            UserData1 = userData1;
            return true;
        }
        else
        {
            Stop();
        }
    }

    OVR::String basePath = OVR::GetBaseOVRPath(true);
    OVR::String path = basePath;
    path.AppendString("\\");
    path.AppendString(fileName);
    OS = OVR::Util::OSAsString();
    OSVersion = OVR::Util::OSVersionAsString();
#if defined (OVR_OS_WIN64) || defined (OVR_OS_WIN32)
    ProcessInfo = OVR::Util::GetProcessInfo();
    DisplayDriverVersion = OVR::Util::GetDisplayDriverVersion();
    CameraDriverVersion = OVR::Util::GetCameraDriverVersion();
    OVR::Array< OVR::String > gpus;
    OVR::Util::GetGraphicsCardList(gpus);
    StringBuffer sb;
    size_t gpuIdx;
    for (gpuIdx = 0; gpuIdx < gpus.GetSize(); gpuIdx++)
    {
        sb.AppendString(gpus[gpuIdx]);
        if (gpuIdx + 1 < gpus.GetSize())
            sb.AppendString("; ");
    }
    GPUVersion = sb;
#endif
    Guid = OVR::Util::GetGuidString();

    if (!_File.Open(path, OVR::File::Open_Write, OVR::File::Mode_Write))
    {
        _File.Create(path, OVR::File::Mode_Write);
        WriteHeaderV1();
    }
    else
    {
        _File.Seek(0, FileConstants::Seek_End);
    }

    if (_File.IsValid())
    {
        UserData1 = userData1;
        FileName = fileName;
        _Observer.SetHandler(LatencyStatisticsSlot::FromMember<LatencyStatisticsCSV, &LatencyStatisticsCSV::OnResults>(this));

        return true;
    }

    return false;
}
bool LatencyStatisticsCSV::Stop()
{
    if (_File.IsValid())
    {
        _File.Flush();
        _File.Close();
        _Observer.ReleaseAll();

        Guid.Clear();
        FileName.Clear();
        return true;
    }
    return false;
}
void LatencyStatisticsCSV::WriteHeaderV1()
{
    if (_File.IsValid())
    {
        // Write header if creating the file
        const char *str = "GUID,OS,OSVersion,Process,DisplayDriver,CameraDriver,GPU,Time,Interval,FPS,EndFrameExecutionTime,LatencyRender,LatencyTimewarp,LatencyPostPresent,LatencyVisionProc,LatencyVisionFrame,UserData1\n";
        _File.Write((const uint8_t *) str, (int)OVR_strlen(str));
    }
}

void LatencyStatisticsCSV::WriteResultsV1(LatencyStatisticsResults *results)
{
    if (_File.IsValid())
    {
        char str[512];
        OVR_sprintf(str, sizeof(str),
            "%s,%s,%s,%s,%s,%s,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%s\n",
            Guid.ToCStr(),
            OS.ToCStr(),
            OSVersion.ToCStr(),
            ProcessInfo.ToCStr(),
            DisplayDriverVersion.ToCStr(),
            CameraDriverVersion.ToCStr(),
            GPUVersion.ToCStr(),
            ovr_GetTimeInSeconds(),
            results->IntervalSeconds,
            results->FPS,
            results->EndFrameExecutionTime,
            results->LatencyRender,
            results->LatencyTimewarp,
            results->LatencyPostPresent,
            results->LatencyVisionProc,
            results->LatencyVisionFrame,
            UserData1.ToCStr());
        str[sizeof(str)-1] = 0;
        _File.Write((const uint8_t *)str, (int)OVR_strlen(str));
    }
}
void LatencyStatisticsCSV::OnResults(LatencyStatisticsResults *results)
{
    WriteResultsV1(results);
}
//-------------------------------------------------------------------------------------
// ***** LatencyStatisticsCalculator
    
LagStatsCalculator::LagStatsCalculator()
{
    resetPerfStats();
}

void LagStatsCalculator::resetPerfStats(double resetTime)
{
    EndFrameStartTime = 0.;
    EndFrameEndTime = 0.;

    LastCameraFrameCounter = ~(uint32_t)0;

    EpochBegin = resetTime; // Set epoch start to now

    FrameCount = 0;
    //EndFrameSum = 0.;

    //VisionProcSum = 0.;
    //VisionLagSum = 0.;
    VisionFrames = 0;

    //for (int i = 0; i < 3; ++i)
    //{
    //    LatencyData[i] = 0.f;
    //    LatencySum[i] = 0.;
    //}

    latencyStatisticsData.EndFrameExecutionTime = 0;
    latencyStatisticsData.LatencyRender = 0;
    latencyStatisticsData.LatencyTimewarp = 0;
    latencyStatisticsData.LatencyPostPresent = 0;
    latencyStatisticsData.LatencyVisionProc = 0;
    latencyStatisticsData.LatencyVisionFrame = 0;
}

void LagStatsCalculator::GetLatestResults(LatencyStatisticsResults* results)
{
    *results = Results.GetState();
}
void LagStatsCalculator::AddResultsObserver(ObserverScope<LatencyStatisticsSlot> *calculateResultsObserver)
{
    Lock::Locker locker(&calculateResultsLock);
    calculateResultsObserver->GetPtr()->Observe(calculateResultsSubject);
}
void LagStatsCalculator::InstrumentEndFrameStart(double timestamp)
{
    EndFrameStartTime = timestamp;
}

void LagStatsCalculator::InstrumentLatencyTimings(FrameTimeManager& ftm)
{
    //latencies[0] = (float)RenderLatencySeconds;
    //latencies[1] = (float)TimewarpLatencySeconds;
    //latencies[2] = (float)FrameDeltas.GetMedianTimeDelta();

    // Note: This assumes that it is called right before InstrumentEndFrameEnd()
	float latencyRender, latencyTimewarp, latencyPostPresent;
    ftm.GetLatencyTimings(latencyRender, latencyTimewarp, latencyPostPresent);
	latencyStatisticsData.LatencyRender += latencyRender;
	latencyStatisticsData.LatencyTimewarp += latencyTimewarp;
	latencyStatisticsData.LatencyPostPresent += latencyPostPresent;
}

void LagStatsCalculator::InstrumentEndFrameEnd(double timestamp)
{
    EndFrameEndTime = timestamp;

    calculateResults();
}

void LagStatsCalculator::InstrumentEyePose(const ovrTrackingState& state)
{
    // If the camera frame counter has rolled,
    if (LastCameraFrameCounter != state.LastCameraFrameCounter)
    {
        // Accumulate eye pose data
        if (state.StatusFlags != 0)
        {
            latencyStatisticsData.LatencyVisionProc += state.LastVisionProcessingTime;
            latencyStatisticsData.LatencyVisionFrame += state.LastVisionFrameLatency;
        }
        ++VisionFrames;

        LastCameraFrameCounter = state.LastCameraFrameCounter;
    }
}

// Note: Currently assumes this is being called from OnEndFrameEnd() above.
void LagStatsCalculator::calculateResults()
{
    // Calculate time in the current epoch so far
    double intervalDuration = EndFrameEndTime - EpochBegin;

    // If stats should be reset due to inactivity,
    if (intervalDuration >= OVR_LAG_STATS_RESET_LIMIT)
    {
        resetPerfStats(EndFrameEndTime);
        return;
    }

    // Calculate EndFrame() duration
    double endFrameDuration = EndFrameEndTime - EndFrameStartTime;

    // Incorporate EndFrame() duration into the running sum
    latencyStatisticsData.EndFrameExecutionTime += endFrameDuration;

    //for (int i = 0; i < 3; ++i)
    //{
    //    LatencySum[i] += LatencyData[i];
    //}

    // Increment frame counter
    ++FrameCount;

    // If enough time has passed,
    if (intervalDuration >= OVR_LAG_STATS_EPOCH)
    {
        LatencyStatisticsResults results;

        float invFrameCount = 1.0f / (float) FrameCount;

        // EndFrame() instrumentation
        results.IntervalSeconds = intervalDuration;
        results.FPS = FrameCount / intervalDuration;
        results.EndFrameExecutionTime = latencyStatisticsData.EndFrameExecutionTime * invFrameCount;

        // Latency data
        results.LatencyRender = latencyStatisticsData.LatencyRender * invFrameCount;
        results.LatencyTimewarp = latencyStatisticsData.LatencyTimewarp * invFrameCount;
        results.LatencyPostPresent = latencyStatisticsData.LatencyPostPresent * invFrameCount;

        double invVisionFrameCount = 1. / VisionFrames;

        // Eye pose instrumentation
        results.LatencyVisionProc = latencyStatisticsData.LatencyVisionProc * invVisionFrameCount;
        results.LatencyVisionFrame = latencyStatisticsData.LatencyVisionFrame * invVisionFrameCount;

        Results.SetState(results);

        {
            Lock::Locker locker(&calculateResultsLock);
            calculateResultsSubject.GetPtr()->Call(&results);
        }

        // Reset for next frame
        resetPerfStats();
    }
}


}} // namespace OVR::CAPI
