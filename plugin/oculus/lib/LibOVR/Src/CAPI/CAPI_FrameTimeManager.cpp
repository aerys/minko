/************************************************************************************

Filename    :   CAPI_FrameTimeManager.cpp
Content     :   Manage frame timing and pose prediction for rendering
Created     :   November 30, 2013
Authors     :   Volga Aksoy, Michael Antonov

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

#include "CAPI_FrameTimeManager.h"

#include "../Kernel/OVR_Log.h"

namespace OVR { namespace CAPI {


//-------------------------------------------------------------------------------------
// ***** FrameLatencyTracker
    

FrameLatencyTracker::FrameLatencyTracker()
{
   Reset();
}

void FrameLatencyTracker::Reset()
{
    TrackerEnabled         = true;
    WaitMode               = SampleWait_Zeroes;
    FrameIndex             = 0;    
    MatchCount             = 0;
    RenderLatencySeconds   = 0.0;
    TimewarpLatencySeconds = 0.0;
    LatencyRecordTime      = 0.0;
    
    FrameDeltas.Clear();
}


unsigned char FrameLatencyTracker::GetNextDrawColor()
{   
    if (!TrackerEnabled || (WaitMode == SampleWait_Zeroes) ||
        (FrameIndex >= FramesTracked))
    {        
        return (unsigned char)Util::FrameTimeRecord::ReadbackIndexToColor(0);
    }

    OVR_ASSERT(FrameIndex < FramesTracked);    
    return (unsigned char)Util::FrameTimeRecord::ReadbackIndexToColor(FrameIndex+1);
}


void FrameLatencyTracker::SaveDrawColor(unsigned char drawColor, double endFrameTime,
                                        double renderIMUTime, double timewarpIMUTime )
{
    if (!TrackerEnabled || (WaitMode == SampleWait_Zeroes))
        return;

    if (FrameIndex < FramesTracked)
    {
        OVR_ASSERT(Util::FrameTimeRecord::ReadbackIndexToColor(FrameIndex+1) == drawColor);
        OVR_UNUSED(drawColor);

        // saves {color, endFrame time}
        FrameEndTimes[FrameIndex].ReadbackIndex         = FrameIndex + 1;
        FrameEndTimes[FrameIndex].TimeSeconds           = endFrameTime;
        FrameEndTimes[FrameIndex].RenderIMUTimeSeconds  = renderIMUTime;
        FrameEndTimes[FrameIndex].TimewarpIMUTimeSeconds= timewarpIMUTime;
        FrameEndTimes[FrameIndex].MatchedRecord         = false;
        FrameIndex++;
    }
    else
    {
        // If the request was outstanding for too long, switch to zero mode to restart.
        if (endFrameTime > (FrameEndTimes[FrameIndex-1].TimeSeconds + 0.15))
        {
            if (MatchCount == 0)
            {
                // If nothing was matched, we have no latency reading.
                RenderLatencySeconds   = 0.0;
                TimewarpLatencySeconds = 0.0;
            }

            WaitMode   =  SampleWait_Zeroes;
            MatchCount = 0;
            FrameIndex = 0;
        }
    }
}


void FrameLatencyTracker::MatchRecord(const Util::FrameTimeRecordSet &r)
{
    if (!TrackerEnabled)
        return;

    if (WaitMode == SampleWait_Zeroes)
    {
        // Do we have all zeros?
        if (r.IsAllZeroes())
        {
            OVR_ASSERT(FrameIndex == 0);
            WaitMode = SampleWait_Match;
            MatchCount = 0;
        }
        return;
    }

    // We are in Match Mode. Wait until all colors are matched or timeout,
    // at which point we go back to zeros.

    for (int i = 0; i < FrameIndex; i++)
    {
        int recordIndex = 0;
        int consecutiveMatch  = 0;

        OVR_ASSERT(FrameEndTimes[i].ReadbackIndex != 0);

        if (r.FindReadbackIndex(&recordIndex, FrameEndTimes[i].ReadbackIndex))
        {
            // Advance forward to see that we have several more matches.
            int  ri = recordIndex + 1;
            int  j  = i + 1;

            consecutiveMatch++;

            for (; (j < FrameIndex) && (ri < Util::FrameTimeRecordSet::RecordCount); j++, ri++)
            {
                if (r[ri].ReadbackIndex != FrameEndTimes[j].ReadbackIndex)
                    break;
                consecutiveMatch++;
            }

            // Match at least 2 items in the row, to avoid accidentally matching color.
            if (consecutiveMatch > 1)
            {
                // Record latency values for all but last samples. Keep last 2 samples
                // for the future to simplify matching.
                for (int q = 0; q < consecutiveMatch; q++)
                {
                    const Util::FrameTimeRecord &scanoutFrame = r[recordIndex+q];
                    FrameTimeRecordEx           &renderFrame  = FrameEndTimes[i+q];
                    
                    if (!renderFrame.MatchedRecord)
                    {
                        double deltaSeconds = scanoutFrame.TimeSeconds - renderFrame.TimeSeconds;
                        if (deltaSeconds > 0.0)
                        {
                            FrameDeltas.AddTimeDelta(deltaSeconds);

                            // FIRMWARE HACK: don't take new readings if they're 10ms higher than previous reading
                            // but only do that for 1 second, after that accept it regardless of the timing difference
                            double newRenderLatency = scanoutFrame.TimeSeconds - renderFrame.RenderIMUTimeSeconds;                            
                            if( newRenderLatency < RenderLatencySeconds + 0.01 ||
                                scanoutFrame.TimeSeconds > LatencyRecordTime + 1.0)
                            {
                                LatencyRecordTime      = scanoutFrame.TimeSeconds;
                                RenderLatencySeconds   = scanoutFrame.TimeSeconds - renderFrame.RenderIMUTimeSeconds;
                                TimewarpLatencySeconds = (renderFrame.TimewarpIMUTimeSeconds == 0.0)  ?  0.0 :
                                                         (scanoutFrame.TimeSeconds - renderFrame.TimewarpIMUTimeSeconds);
                            }
                        }

                        renderFrame.MatchedRecord = true;
                        MatchCount++;
                    }
                }

                // Exit for.
                break;
            }
        }
    } // for ( i => FrameIndex )


    // If we matched all frames, start over.
    if (MatchCount == FramesTracked)
    {
        WaitMode   =  SampleWait_Zeroes;
        MatchCount = 0;
        FrameIndex = 0;
    }
}

bool FrameLatencyTracker::IsLatencyTimingAvailable()
{
    return ovr_GetTimeInSeconds() < (LatencyRecordTime + 2.0);
}

void FrameLatencyTracker::GetLatencyTimings(float latencies[3])
{
    if (!IsLatencyTimingAvailable())
    {
        latencies[0] = 0.0f;
        latencies[1] = 0.0f;
        latencies[2] = 0.0f;
    }
    else
    {
        latencies[0] = (float)RenderLatencySeconds;
        latencies[1] = (float)TimewarpLatencySeconds;
        latencies[2] = (float)FrameDeltas.GetMedianTimeDelta();
    }    
}


//-------------------------------------------------------------------------------------

FrameTimeManager::FrameTimeManager(bool vsyncEnabled) :
    VsyncEnabled(vsyncEnabled),
    DynamicPrediction(true),
    SdkRender(false),
    DirectToRift(false),
#ifndef NO_SCREEN_TEAR_HEALING
    ScreenTearing(false),
    TearingFrameCount(0),
#endif // NO_SCREEN_TEAR_HEALING
    FrameTiming()
{    
    RenderIMUTimeSeconds = 0.0;
    TimewarpIMUTimeSeconds = 0.0;

    // If driver is in use,
    DirectToRift = !Display::InCompatibilityMode(false);
    if (DirectToRift)
    {
        // The latest driver provides a post-present vsync-to-scan-out delay
        // that is roughly zero.  The latency tester will provide real numbers
        // but when it is unavailable for some reason, we should default to
        // an expected value.
        VSyncToScanoutDelay = 0.0001f;
    }
    else
    {
        // HACK: SyncToScanoutDelay observed close to 1 frame in video cards.
        //       Overwritten by dynamic latency measurement on DK2.
        VSyncToScanoutDelay = 0.013f;
    }
    NoVSyncToScanoutDelay = 0.004f;
}

void FrameTimeManager::Init(HmdRenderInfo& renderInfo)
{
    // Set up prediction distances.
    // With-Vsync timings.
    RenderInfo = renderInfo;

    ScreenSwitchingDelay = RenderInfo.Shutter.PixelSettleTime * 0.5f + 
                           RenderInfo.Shutter.PixelPersistence * 0.5f;
}

void FrameTimeManager::ResetFrameTiming(unsigned frameIndex,
                                        bool dynamicPrediction,
                                        bool sdkRender)
{    
    DynamicPrediction   = dynamicPrediction;
    SdkRender           = sdkRender;

    FrameTimeDeltas.Clear();
    DistortionRenderTimes.Clear();
    ScreenLatencyTracker.Reset();
    //Revisit dynamic pre-Timewarp delay adjustment logic
    //TimewarpAdjuster.Reset();

    FrameTiming.FrameIndex               = frameIndex;
    FrameTiming.NextFrameTime            = 0.0;
    FrameTiming.ThisFrameTime            = 0.0;
    FrameTiming.Inputs.FrameDelta        = calcFrameDelta();
    // This one is particularly critical, and has been missed in the past because
    // this init function wasn't called for app-rendered.
    FrameTiming.Inputs.ScreenDelay       = calcScreenDelay();
    FrameTiming.Inputs.TimewarpWaitDelta = 0.0f;

    LocklessTiming.SetState(FrameTiming);
}


double  FrameTimeManager::calcFrameDelta() const
{
    // Timing difference between frame is tracked by FrameTimeDeltas, or
    // is a hard-coded value of 1/FrameRate.
    double  frameDelta;    

    if (!VsyncEnabled)
    {
        frameDelta = 0.0;
    }
    else if (FrameTimeDeltas.GetCount() > 3)
    {
        frameDelta = FrameTimeDeltas.GetMedianTimeDelta();
        if (frameDelta > (RenderInfo.Shutter.VsyncToNextVsync + 0.001))
            frameDelta = RenderInfo.Shutter.VsyncToNextVsync;
    }
    else
    {
        frameDelta = RenderInfo.Shutter.VsyncToNextVsync;
    }

    return frameDelta;
}


double  FrameTimeManager::calcScreenDelay() const
{
    double  screenDelay = ScreenSwitchingDelay;
    double  measuredVSyncToScanout;

#ifndef NO_SCREEN_TEAR_HEALING
    bool tearing = false;
#endif // NO_SCREEN_TEAR_HEALING

    // Use real-time DK2 latency tester HW for prediction if its is working.
    // Do sanity check under 60 ms
    if (!VsyncEnabled)
    {
        screenDelay += NoVSyncToScanoutDelay;
    }
    else if ( DynamicPrediction &&
              (ScreenLatencyTracker.FrameDeltas.GetCount() > 3) &&
              (measuredVSyncToScanout = ScreenLatencyTracker.FrameDeltas.GetMedianTimeDelta(),
               (measuredVSyncToScanout > -0.0001) && (measuredVSyncToScanout < 0.06)) ) 
    {
#ifndef NO_SCREEN_TEAR_HEALING
        if (DirectToRift && measuredVSyncToScanout > 0.010 && measuredVSyncToScanout < 0.030)
        {
            tearing = true;
        }
#endif // NO_SCREEN_TEAR_HEALING
        screenDelay += measuredVSyncToScanout;
    }
    else
    {
        screenDelay += VSyncToScanoutDelay;
    }

#ifndef NO_SCREEN_TEAR_HEALING
    if (tearing)
    {
        if (TearingFrameCount > 75)
        {
            if (!ScreenTearing)
            {
                ScreenTearing = true;
                HealingFrameCount = 0;
            }
        }
        else
        {
            TearingFrameCount++;
        }
    }
    else
    {
        TearingFrameCount = 0;
        ScreenTearing = false;
    }
#endif // NO_SCREEN_TEAR_HEALING

    return screenDelay;
}

#ifndef NO_SCREEN_TEAR_HEALING

bool FrameTimeManager::ScreenTearingReaction()
{
    if (ScreenTearing)
    {
        if (HealingFrameCount < 75)
        {
            ++HealingFrameCount;
            return true;
        }
    }

    return false;
}

#endif // NO_SCREEN_TEAR_HEALING

double FrameTimeManager::calcTimewarpWaitDelta() const
{
    // If timewarp timing hasn't been calculated, we should wait.
    if (!VsyncEnabled)
        return 0.0;

    if (SdkRender)
    {
        if (NeedDistortionTimeMeasurement())
            return 0.0;
        return -(DistortionRenderTimes.GetMedianTimeDelta() + 0.0035);

        //Revisit dynamic pre-Timewarp delay adjustment logic
        /*return -(DistortionRenderTimes.GetMedianTimeDelta() + 0.002 +
                 TimewarpAdjuster.GetDelayReduction());*/
    }
   
    // Just a hard-coded "high" value for game-drawn code.
    // TBD: Just return 0 and let users calculate this themselves?
    return -0.004;

    //Revisit dynamic pre-Timewarp delay adjustment logic
    //return -(0.003 + TimewarpAdjuster.GetDelayReduction());
}

//Revisit dynamic pre-Timewarp delay adjustment logic
/*
void FrameTimeManager::updateTimewarpTiming()
{
    // If timewarp timing changes based on this sample, update it.
    double newTimewarpWaitDelta = calcTimewarpWaitDelta();
    if (newTimewarpWaitDelta != FrameTiming.Inputs.TimewarpWaitDelta)
    {
        FrameTiming.Inputs.TimewarpWaitDelta = newTimewarpWaitDelta;
        LocklessTiming.SetState(FrameTiming);
    }
}
*/

void FrameTimeManager::Timing::InitTimingFromInputs(const FrameTimeManager::TimingInputs& inputs,
                                                    HmdShutterTypeEnum shutterType,
                                                    double thisFrameTime, unsigned int frameIndex)
{    
    // ThisFrameTime comes from the end of last frame, unless it it changed.  
    double  nextFrameBase;
    double  frameDelta = inputs.FrameDelta;

    FrameIndex        = frameIndex;

    ThisFrameTime     = thisFrameTime;
    NextFrameTime     = ThisFrameTime + frameDelta;
    nextFrameBase     = NextFrameTime + inputs.ScreenDelay;
    MidpointTime      = nextFrameBase + frameDelta * 0.5;
    TimewarpPointTime = (inputs.TimewarpWaitDelta == 0.0) ?
                        0.0 : (NextFrameTime + inputs.TimewarpWaitDelta);

    // Calculate absolute points in time when eye rendering or corresponding time-warp
    // screen edges will become visible.
    // This only matters with VSync.
    switch(shutterType)
    {            
    case HmdShutter_RollingTopToBottom:
        EyeRenderTimes[0]               = MidpointTime;
        EyeRenderTimes[1]               = MidpointTime;
        TimeWarpStartEndTimes[0][0]     = nextFrameBase;
        TimeWarpStartEndTimes[0][1]     = nextFrameBase + frameDelta;
        TimeWarpStartEndTimes[1][0]     = nextFrameBase;
        TimeWarpStartEndTimes[1][1]     = nextFrameBase + frameDelta;
        break;
    case HmdShutter_RollingLeftToRight:
        EyeRenderTimes[0]               = nextFrameBase + frameDelta * 0.25;
        EyeRenderTimes[1]               = nextFrameBase + frameDelta * 0.75;

        /*
        // TBD: MA: It is probably better if mesh sets it up per-eye.
        // Would apply if screen is 0 -> 1 for each eye mesh
        TimeWarpStartEndTimes[0][0]     = nextFrameBase;
        TimeWarpStartEndTimes[0][1]     = MidpointTime;
        TimeWarpStartEndTimes[1][0]     = MidpointTime;
        TimeWarpStartEndTimes[1][1]     = nextFrameBase + frameDelta;
        */

        // Mesh is set up to vary from Edge of scree 0 -> 1 across both eyes
        TimeWarpStartEndTimes[0][0]     = nextFrameBase;
        TimeWarpStartEndTimes[0][1]     = nextFrameBase + frameDelta;
        TimeWarpStartEndTimes[1][0]     = nextFrameBase;
        TimeWarpStartEndTimes[1][1]     = nextFrameBase + frameDelta;

        break;
    case HmdShutter_RollingRightToLeft:

        EyeRenderTimes[0]               = nextFrameBase + frameDelta * 0.75;
        EyeRenderTimes[1]               = nextFrameBase + frameDelta * 0.25;
        
        // This is *Correct* with Tom's distortion mesh organization.
        TimeWarpStartEndTimes[0][0]     = nextFrameBase ;
        TimeWarpStartEndTimes[0][1]     = nextFrameBase + frameDelta;
        TimeWarpStartEndTimes[1][0]     = nextFrameBase ;
        TimeWarpStartEndTimes[1][1]     = nextFrameBase + frameDelta;
        break;
    case HmdShutter_Global:
        // TBD
        EyeRenderTimes[0]               = MidpointTime;
        EyeRenderTimes[1]               = MidpointTime;
        TimeWarpStartEndTimes[0][0]     = MidpointTime;
        TimeWarpStartEndTimes[0][1]     = MidpointTime;
        TimeWarpStartEndTimes[1][0]     = MidpointTime;
        TimeWarpStartEndTimes[1][1]     = MidpointTime;
        break;
    default:
        break;
    }
}

  
double FrameTimeManager::BeginFrame(unsigned frameIndex)
{    
    RenderIMUTimeSeconds = 0.0;
    TimewarpIMUTimeSeconds = 0.0;

    // TPH - putting an assert so this doesn't remain a hidden problem.
    OVR_ASSERT(FrameTiming.Inputs.ScreenDelay != 0);

    // ThisFrameTime comes from the end of last frame, unless it it changed.
    double thisFrameTime = (FrameTiming.NextFrameTime != 0.0) ?
                           FrameTiming.NextFrameTime : ovr_GetTimeInSeconds();
    
    // We are starting to process a new frame...
    FrameTiming.InitTimingFromInputs(FrameTiming.Inputs, RenderInfo.Shutter.Type,
                                     thisFrameTime, frameIndex);

    return FrameTiming.ThisFrameTime;
}


void FrameTimeManager::EndFrame()
{
    // Record timing since last frame; must be called after Present & sync.
    FrameTiming.NextFrameTime = ovr_GetTimeInSeconds();    
    if (FrameTiming.ThisFrameTime > 0.0)
    {
    //Revisit dynamic pre-Timewarp delay adjustment logic
    /*
        double actualFrameDelta = FrameTiming.NextFrameTime - FrameTiming.ThisFrameTime;

        if (VsyncEnabled)
            TimewarpAdjuster.UpdateTimewarpWaitIfSkippedFrames(this, actualFrameDelta,
                                                               FrameTiming.NextFrameTime);

        FrameTimeDeltas.AddTimeDelta(actualFrameDelta);
    */
        FrameTimeDeltas.AddTimeDelta(FrameTiming.NextFrameTime - FrameTiming.ThisFrameTime);
        FrameTiming.Inputs.FrameDelta = calcFrameDelta();
    }

    // Write to Lock-less
    LocklessTiming.SetState(FrameTiming);
}


// Thread-safe function to query timing for a future frame

FrameTimeManager::Timing FrameTimeManager::GetFrameTiming(unsigned frameIndex)
{
    Timing frameTiming = LocklessTiming.GetState();

    if (frameTiming.ThisFrameTime != 0.0)
    {
        // If timing hasn't been initialized, starting based on "now" is the best guess.
        frameTiming.InitTimingFromInputs(frameTiming.Inputs, RenderInfo.Shutter.Type,
                                         ovr_GetTimeInSeconds(), frameIndex);
    }
    
    else if (frameIndex > frameTiming.FrameIndex)
    {
        unsigned frameDelta    = frameIndex - frameTiming.FrameIndex;
        double   thisFrameTime = frameTiming.NextFrameTime +
                                 double(frameDelta-1) * frameTiming.Inputs.FrameDelta;
        // Don't run away too far into the future beyond rendering.
        OVR_ASSERT(frameDelta < 6);

        frameTiming.InitTimingFromInputs(frameTiming.Inputs, RenderInfo.Shutter.Type,
                                         thisFrameTime, frameIndex);
    }    
     
    return frameTiming;
}


double FrameTimeManager::GetEyePredictionTime(ovrEyeType eye)
{
    if (VsyncEnabled)
    {
        return FrameTiming.EyeRenderTimes[eye];
    }

    // No VSync: Best guess for the near future
    return ovr_GetTimeInSeconds() + ScreenSwitchingDelay + NoVSyncToScanoutDelay;
}

Posef FrameTimeManager::GetEyePredictionPose(ovrHmd hmd, ovrEyeType eye)
{
    double           eyeRenderTime = GetEyePredictionTime(eye);
    ovrTrackingState eyeState      = ovrHmd_GetTrackingState(hmd, eyeRenderTime);
        
    // Record view pose sampling time for Latency reporting.
    if (RenderIMUTimeSeconds == 0.0)
    {
        // TODO: Figure out why this are not as accurate as ovr_GetTimeInSeconds()
        //RenderIMUTimeSeconds = eyeState.RawSensorData.TimeInSeconds;
        RenderIMUTimeSeconds = ovr_GetTimeInSeconds();
    }

    return eyeState.HeadPose.ThePose;
}


void FrameTimeManager::GetTimewarpPredictions(ovrEyeType eye, double timewarpStartEnd[2])
{
    if (VsyncEnabled)
    {
        timewarpStartEnd[0] = FrameTiming.TimeWarpStartEndTimes[eye][0];
        timewarpStartEnd[1] = FrameTiming.TimeWarpStartEndTimes[eye][1];
        return;
    }    

    // Free-running, so this will be displayed immediately.
    // Unfortunately we have no idea which bit of the screen is actually going to be displayed.
    // TODO: guess which bit of the screen is being displayed!
    // (e.g. use DONOTWAIT on present and see when the return isn't WASSTILLWAITING?)

    // We have no idea where scan-out is currently, so we can't usefully warp the screen spatially.
    timewarpStartEnd[0] = ovr_GetTimeInSeconds() + ScreenSwitchingDelay + NoVSyncToScanoutDelay;
    timewarpStartEnd[1] = timewarpStartEnd[0];
}


void FrameTimeManager::GetTimewarpMatrices(ovrHmd hmd, ovrEyeType eyeId,
                                           ovrPosef renderPose, ovrMatrix4f twmOut[2])
{
    if (!hmd)
    {
        return;
    }

    double timewarpStartEnd[2] = { 0.0, 0.0 };    
    GetTimewarpPredictions(eyeId, timewarpStartEnd);
      
    //HMDState* p = (HMDState*)hmd;
    ovrTrackingState startState = ovrHmd_GetTrackingState(hmd, timewarpStartEnd[0]);
    ovrTrackingState endState   = ovrHmd_GetTrackingState(hmd, timewarpStartEnd[1]);

    if (TimewarpIMUTimeSeconds == 0.0)
    {
        // TODO: Figure out why this are not as accurate as ovr_GetTimeInSeconds()
        //TimewarpIMUTimeSeconds = startState.RawSensorData.TimeInSeconds;
        TimewarpIMUTimeSeconds = ovr_GetTimeInSeconds();
    }

    Quatf quatFromStart = startState.HeadPose.ThePose.Orientation;
    Quatf quatFromEnd   = endState.HeadPose.ThePose.Orientation;
    Quatf quatFromEye   = renderPose.Orientation; //EyeRenderPoses[eyeId].Orientation;
    quatFromEye.Invert();
    
    Quatf timewarpStartQuat = quatFromEye * quatFromStart;
    Quatf timewarpEndQuat   = quatFromEye * quatFromEnd;

    Matrix4f timewarpStart(timewarpStartQuat);
    Matrix4f timewarpEnd(timewarpEndQuat);
    

    // The real-world orientations have:                                  X=right, Y=up,   Z=backwards.
    // The vectors inside the mesh are in NDC to keep the shader simple: X=right, Y=down, Z=forwards.
    // So we need to perform a similarity transform on this delta matrix.
    // The verbose code would look like this:
    /*
    Matrix4f matBasisChange;
    matBasisChange.SetIdentity();
    matBasisChange.M[0][0] =  1.0f;
    matBasisChange.M[1][1] = -1.0f;
    matBasisChange.M[2][2] = -1.0f;
    Matrix4f matBasisChangeInv = matBasisChange.Inverted();
    matRenderFromNow = matBasisChangeInv * matRenderFromNow * matBasisChange;
    */
    // ...but of course all the above is a constant transform and much more easily done.
    // We flip the signs of the Y&Z row, then flip the signs of the Y&Z column,
    // and of course most of the flips cancel:
    // +++                        +--                     +--
    // +++ -> flip Y&Z columns -> +-- -> flip Y&Z rows -> -++
    // +++                        +--                     -++
    timewarpStart.M[0][1] = -timewarpStart.M[0][1];
    timewarpStart.M[0][2] = -timewarpStart.M[0][2];
    timewarpStart.M[1][0] = -timewarpStart.M[1][0];
    timewarpStart.M[2][0] = -timewarpStart.M[2][0];

    timewarpEnd  .M[0][1] = -timewarpEnd  .M[0][1];
    timewarpEnd  .M[0][2] = -timewarpEnd  .M[0][2];
    timewarpEnd  .M[1][0] = -timewarpEnd  .M[1][0];
    timewarpEnd  .M[2][0] = -timewarpEnd  .M[2][0];

    twmOut[0] = timewarpStart;
    twmOut[1] = timewarpEnd;
}


// Used by renderer to determine if it should time distortion rendering.
bool  FrameTimeManager::NeedDistortionTimeMeasurement() const
{
    if (!VsyncEnabled)
        return false;
    return DistortionRenderTimes.GetCount() < DistortionRenderTimes.Capacity;
}


void  FrameTimeManager::AddDistortionTimeMeasurement(double distortionTimeSeconds)
{
    DistortionRenderTimes.AddTimeDelta(distortionTimeSeconds);

    //Revisit dynamic pre-Timewarp delay adjustment logic
    //updateTimewarpTiming();

    // If timewarp timing changes based on this sample, update it.
    double newTimewarpWaitDelta = calcTimewarpWaitDelta();
    if (newTimewarpWaitDelta != FrameTiming.Inputs.TimewarpWaitDelta)
    {
        FrameTiming.Inputs.TimewarpWaitDelta = newTimewarpWaitDelta;
        LocklessTiming.SetState(FrameTiming);
    }
}


void FrameTimeManager::UpdateFrameLatencyTrackingAfterEndFrame(
                                    unsigned char frameLatencyTestColor[3],
                                    const Util::FrameTimeRecordSet& rs)
{    
    // FrameTiming.NextFrameTime in this context (after EndFrame) is the end frame time.
    ScreenLatencyTracker.SaveDrawColor(frameLatencyTestColor[0],
                                       FrameTiming.NextFrameTime,
                                       RenderIMUTimeSeconds,
                                       TimewarpIMUTimeSeconds);

    ScreenLatencyTracker.MatchRecord(rs);

    // If screen delay changed, update timing.
    double newScreenDelay = calcScreenDelay();
    if (newScreenDelay != FrameTiming.Inputs.ScreenDelay)
    {
        FrameTiming.Inputs.ScreenDelay = newScreenDelay;
        LocklessTiming.SetState(FrameTiming);
    }
}


//-----------------------------------------------------------------------------------
//Revisit dynamic pre-Timewarp delay adjustment logic
/*
void FrameTimeManager::TimewarpDelayAdjuster::Reset()    
{
    State                           = State_WaitingToReduceLevel;
    DelayLevel                      = 0;
    InitialFrameCounter             = 0;
    TimewarpDelayReductionSeconds   = 0.0;
    DelayLevelFinishTime            = 0.0;

    memset(WaitTimeIndexForLevel, 0, sizeof(WaitTimeIndexForLevel));
    // If we are at level 0, waits are infinite.
    WaitTimeIndexForLevel[0] = MaxTimeIndex;
}


void FrameTimeManager::TimewarpDelayAdjuster::
        UpdateTimewarpWaitIfSkippedFrames(FrameTimeManager* manager,
                                          double measuredFrameDelta, double nextFrameTime)
{    
    // Times in seconds
    const static double delayTimingTiers[7] = { 1.0, 5.0, 15.0, 30.0, 60.0, 120.0, 1000000.0 };

    const double currentFrameDelta = manager->FrameTiming.Inputs.FrameDelta;


    // Once we detected frame spike, we skip several frames before testing again.    
    if (InitialFrameCounter > 0)
    {
        InitialFrameCounter --;
        return;
    }

    // Skipped frame would usually take 2x longer then regular frame
    if (measuredFrameDelta > currentFrameDelta * 1.8)
    {        
        if (State == State_WaitingToReduceLevel)
        {
            // If we got here, escalate the level again. 
            if (DelayLevel < MaxDelayLevel)
            {
                DelayLevel++;
                InitialFrameCounter = 3;
            }
        }

        else if (State == State_VerifyingAfterReduce)
        {
            // So we went down to this level and tried to wait to see if there was
            // as skipped frame and there is -> go back up a level and incrment its timing tier
            if (DelayLevel < MaxDelayLevel)
            {
                DelayLevel++;
                State = State_WaitingToReduceLevel;
                
                // For higher level delays reductions, i.e. more then half a frame,
                // we don't go into the infinite wait tier.
                int maxTimingTier = MaxTimeIndex;
                if (DelayLevel > MaxInfiniteTimingLevel)
                    maxTimingTier--;

                if (WaitTimeIndexForLevel[DelayLevel] < maxTimingTier )
                    WaitTimeIndexForLevel[DelayLevel]++;
            }
        }

        DelayLevelFinishTime          = nextFrameTime +
                                        delayTimingTiers[WaitTimeIndexForLevel[DelayLevel]];
        TimewarpDelayReductionSeconds = currentFrameDelta * 0.125 * DelayLevel;
        manager->updateTimewarpTiming();

    }

    else if (nextFrameTime > DelayLevelFinishTime)
    {        
        if (State == State_WaitingToReduceLevel)
        {
            if (DelayLevel > 0)
            {
                DelayLevel--;
                State = State_VerifyingAfterReduce;
                // Always use 1 sec to see if "down sampling mode" caused problems
                DelayLevelFinishTime = nextFrameTime + 1.0f; 
            }
        }
        else if (State == State_VerifyingAfterReduce)
        {
            // Prior display level successfully reduced,
            // try to see we we could go down further after wait.
            WaitTimeIndexForLevel[DelayLevel+1] = 0;            
            State                               = State_WaitingToReduceLevel;
            DelayLevelFinishTime                = nextFrameTime +
                                                  delayTimingTiers[WaitTimeIndexForLevel[DelayLevel]];
        }

        // TBD: Update TimeWarpTiming
        TimewarpDelayReductionSeconds = currentFrameDelta * 0.125 * DelayLevel;
        manager->updateTimewarpTiming();
    }


    //static int oldDelayLevel = 0;

    //if (oldDelayLevel != DelayLevel)
    //{
        //OVR_DEBUG_LOG(("DelayLevel:%d tReduction = %0.5f ", DelayLevel, TimewarpDelayReductionSeconds));
        //oldDelayLevel = DelayLevel;
    //}
    }
    */

//-----------------------------------------------------------------------------------
// ***** TimeDeltaCollector

void TimeDeltaCollector::AddTimeDelta(double timeSeconds)
{
    // avoid adding invalid timing values
    if(timeSeconds < 0.0f)
        return;

    if (Count == Capacity)
    {
        for(int i=0; i< Count-1; i++)
            TimeBufferSeconds[i] = TimeBufferSeconds[i+1];
        Count--;
    }
    TimeBufferSeconds[Count++] = timeSeconds;

    ReCalcMedian = true;
}

double TimeDeltaCollector::GetMedianTimeDelta() const
{
    if(ReCalcMedian)
    {
        double  SortedList[Capacity];
        bool    used[Capacity];

        memset(used, 0, sizeof(used));
        SortedList[0] = 0.0; // In case Count was 0...

        // Probably the slowest way to find median...
        for (int i=0; i<Count; i++)
        {
            double smallestDelta = 1000000.0;
            int    index = 0;

            for (int j = 0; j < Count; j++)
            {
                if (!used[j])
                {                
                    if (TimeBufferSeconds[j] < smallestDelta)
                    {
                        smallestDelta = TimeBufferSeconds[j];
                        index = j;
                    }
                }
            }

            // Mark as used
            used[index]   = true;
            SortedList[i] = smallestDelta;
        }

        // FIRMWARE HACK: Don't take the actual median, but err on the low time side
        Median = SortedList[Count/4];
        ReCalcMedian = false;
    }

    return Median;
}
      

}} // namespace OVR::CAPI

