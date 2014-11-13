/************************************************************************************

Filename    :   CAPI_FrameTimeManager.h
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

#ifndef OVR_CAPI_FrameTimeManager_h
#define OVR_CAPI_FrameTimeManager_h

#include "../OVR_CAPI.h"
#include "../Kernel/OVR_Timer.h"
#include "../Kernel/OVR_Math.h"
#include "../Util/Util_Render_Stereo.h"

namespace OVR { namespace CAPI {

//-------------------------------------------------------------------------------------

// Helper class to collect median times between frames, so that we know
// how long to wait. 
struct TimeDeltaCollector
{
    TimeDeltaCollector() : Median(-1.0), Count(0), ReCalcMedian(true) { }

    void    AddTimeDelta(double timeSeconds);    
    void    Clear() { Count = 0; }    

    double  GetMedianTimeDelta() const;

    double  GetCount() const { return Count; }

    enum { Capacity = 12 };
private:
	double  TimeBufferSeconds[Capacity];
	mutable double  Median;
	int     Count;
    mutable bool    ReCalcMedian;
};


//-------------------------------------------------------------------------------------
// ***** FrameLatencyTracker

// FrameLatencyTracker tracks frame Present to display Scan-out timing, as reported by
// the DK2 internal latency tester pixel read-back. The computed value is used in
// FrameTimeManager for prediction. View Render and TimeWarp to scan-out latencies are
// also reported for debugging.
//
// The class operates by generating color values from GetNextDrawColor() that must
// be rendered on the back end and then looking for matching values in FrameTimeRecordSet
// structure as reported by HW.

class FrameLatencyTracker
{
public:

    enum { FramesTracked = Util::LT2_IncrementCount-1 };

    FrameLatencyTracker();

    // DrawColor == 0 is special in that it doesn't need saving of timestamp
    unsigned char GetNextDrawColor();

    void SaveDrawColor(unsigned char drawColor, double endFrameTime,
                       double renderIMUTime, double timewarpIMUTime );

    void MatchRecord(const Util::FrameTimeRecordSet &r);   

    bool IsLatencyTimingAvailable();
    void GetLatencyTimings(float latencies[3]);

    void Reset();

public:

    struct FrameTimeRecordEx : public Util::FrameTimeRecord
    {
        bool    MatchedRecord;
        double  RenderIMUTimeSeconds;
        double  TimewarpIMUTimeSeconds;
    };

    // True if rendering read-back is enabled.
    bool                  TrackerEnabled;

    enum SampleWaitType {
        SampleWait_Zeroes, // We are waiting for a record with all zeros.
        SampleWait_Match   // We are issuing & matching colors.
    };
    
    SampleWaitType        WaitMode;
    int                   MatchCount;
    // Records of frame timings that we are trying to measure.
    FrameTimeRecordEx     FrameEndTimes[FramesTracked];
    int                   FrameIndex;
    // Median filter for (ScanoutTimeSeconds - PostPresent frame time)
    TimeDeltaCollector    FrameDeltas;
    // Latency reporting results
    double                RenderLatencySeconds;
    double                TimewarpLatencySeconds;
    double                LatencyRecordTime;
};



//-------------------------------------------------------------------------------------
// ***** FrameTimeManager

// FrameTimeManager keeps track of rendered frame timing and handles predictions for
// orientations and time-warp.

class FrameTimeManager
{
public:
    FrameTimeManager(bool vsyncEnabled);

    // Data that affects frame timing computation.
    struct TimingInputs
    {
        // Hard-coded value or dynamic as reported by FrameTimeDeltas.GetMedianTimeDelta().
        double              FrameDelta;
        // Screen delay from present to scan-out, as potentially reported by ScreenLatencyTracker.
        double              ScreenDelay;
        // Negative value of how many seconds before EndFrame we start timewarp. 0.0 if not used.
        double              TimewarpWaitDelta;
        
        TimingInputs()
            : FrameDelta(0), ScreenDelay(0), TimewarpWaitDelta(0)
        { }
    };

    // Timing values for a specific frame.
    struct Timing
    {
        TimingInputs        Inputs;
        
        // Index of a frame that started at ThisFrameTime.
        unsigned int        FrameIndex;
        // Predicted absolute times for when this frame will show up on screen.
        // Generally, all values will be >= NextFrameTime, since that's the time we expect next
        // vsync to succeed.
        double              ThisFrameTime;
        double              TimewarpPointTime;
        double              NextFrameTime;        
        double              MidpointTime;
        double              EyeRenderTimes[2];
        double              TimeWarpStartEndTimes[2][2];

        Timing()
        {
            memset(this, 0, sizeof(Timing));
        }

        void InitTimingFromInputs(const TimingInputs& inputs, HmdShutterTypeEnum shutterType,
                                  double thisFrameTime, unsigned int frameIndex);
    };

   
    // Called on startup to provided data on HMD timing.
    void    Init(HmdRenderInfo& renderInfo);

    // Called with each new ConfigureRendering.
    void    ResetFrameTiming(unsigned frameIndex,
                             bool dynamicPrediction, bool sdkRender);

    void    SetVsync(bool enabled) { VsyncEnabled = enabled; }

    // BeginFrame returns time of the call
    // TBD: Should this be a predicted time value instead ?
    double  BeginFrame(unsigned frameIndex);
    void    EndFrame();    

    // Thread-safe function to query timing for a future frame
    Timing  GetFrameTiming(unsigned frameIndex);
 
    double  GetEyePredictionTime(ovrEyeType eye);
    Posef   GetEyePredictionPose(ovrHmd hmd, ovrEyeType eye);

    void    GetTimewarpPredictions(ovrEyeType eye, double timewarpStartEnd[2]); 
    void    GetTimewarpMatrices(ovrHmd hmd, ovrEyeType eye, ovrPosef renderPose, ovrMatrix4f twmOut[2]);

    // Used by renderer to determine if it should time distortion rendering.
    bool    NeedDistortionTimeMeasurement() const;
    void    AddDistortionTimeMeasurement(double distortionTimeSeconds);

    
    // DK2 Latency test interface
    
    // Get next draw color for DK2 latency tester (3-byte RGB)
    void GetFrameLatencyTestDrawColor(unsigned char outColor[3])
    {
        outColor[0] = ScreenLatencyTracker.GetNextDrawColor();
        outColor[1] = ScreenLatencyTracker.IsLatencyTimingAvailable() ? 255 : 0;
        outColor[2] = ScreenLatencyTracker.IsLatencyTimingAvailable() ? 0 : 255;
    }

    // Must be called after EndFrame() to update latency tester timings.
    // Must pass color reported by NextFrameColor for this frame.
    void    UpdateFrameLatencyTrackingAfterEndFrame(unsigned char frameLatencyTestColor[3],
                                                    const Util::FrameTimeRecordSet& rs);

    void    GetLatencyTimings(float latencies[3])
    { return ScreenLatencyTracker.GetLatencyTimings(latencies); }

    const Timing& GetFrameTiming() const { return FrameTiming; }

#ifndef NO_SCREEN_TEAR_HEALING
    bool    IsScreenTearing() const { return ScreenTearing; };
    bool    ScreenTearingReaction();
#endif // NO_SCREEN_TEAR_HEALING

private:
    double  calcFrameDelta() const;
    double  calcScreenDelay() const;
    double  calcTimewarpWaitDelta() const;

    //Revisit dynamic pre-Timewarp delay adjustment logic
    /*
    void    updateTimewarpTiming();


    
    // TimewarpDelayAdjuster implements a simple state machine that reduces the amount
    // of time-warp waiting based on skipped frames. 
    struct TimewarpDelayAdjuster
    {
        enum StateInLevel
        {        
            // We are ok at this level, and will be waiting for some time before trying to reduce.
            State_WaitingToReduceLevel,  
            // After decrementing a level, we are verifying that this won't cause skipped frames.
            State_VerifyingAfterReduce
       };
    
        enum {
            MaxDelayLevel          = 5,
            MaxInfiniteTimingLevel = 3,
            MaxTimeIndex           = 6
        };

        StateInLevel State;   
        // Current level. Higher levels means larger delay reduction (smaller overall time-warp delay).
        int          DelayLevel;
        // Index for the amount of time we'd wait in this level. If attempt to decrease level fails,
        // with is incrementing causing the level to become "sticky". 
        int          WaitTimeIndexForLevel[MaxTimeIndex + 1];
        // We skip few frames after each escalation to avoid too rapid of a reduction.
        int          InitialFrameCounter;
        // What th currect "reduction" currently is.
        double       TimewarpDelayReductionSeconds;
        // When we should try changing the level again.
        double       DelayLevelFinishTime;

    public:
        TimewarpDelayAdjuster() { Reset(); }

        void    Reset();

        void    UpdateTimewarpWaitIfSkippedFrames(FrameTimeManager* manager,
                                                  double measuredFrameDelta,
                                                  double nextFrameTime);

        double  GetDelayReduction() const { return TimewarpDelayReductionSeconds; }
    };
    */

    
    HmdRenderInfo       RenderInfo;
    // Timings are collected through a median filter, to avoid outliers.
    TimeDeltaCollector  FrameTimeDeltas;
    TimeDeltaCollector  DistortionRenderTimes;
    FrameLatencyTracker ScreenLatencyTracker;

    // Timing changes if we have no Vsync (all prediction is reduced to fixed interval).
    bool                VsyncEnabled;
    // Set if we are rendering via the SDK, so DistortionRenderTimes is valid.
    bool                DynamicPrediction;
    // Set if SDk is doing the rendering.
    bool                SdkRender;
    // Direct to rift.
    bool                DirectToRift;
#ifndef NO_SCREEN_TEAR_HEALING
    // Screen tearing detection
    mutable bool        ScreenTearing;
    // Number of frames tearing has been observed
    mutable int         TearingFrameCount;
    // Number of frames of reaction
    mutable int         HealingFrameCount;
#endif // NO_SCREEN_TEAR_HEALING

    // Total frame delay due to VsyncToFirstScanline, persistence and settle time.
    // Computed from RenderInfor.Shutter.
    double              VSyncToScanoutDelay;
    double              NoVSyncToScanoutDelay;
    double              ScreenSwitchingDelay;

    //Revisit dynamic pre-Timewarp delay adjustment logic
    //TimewarpDelayAdjuster  TimewarpAdjuster;

    // Current (or last) frame timing info. Used as a source for LocklessTiming.
    Timing                  FrameTiming;
    // TBD: Don't we need NextFrame here as well?
    LocklessUpdater<Timing, Timing> LocklessTiming;


    // IMU Read timings
    double              RenderIMUTimeSeconds;
    double              TimewarpIMUTimeSeconds;
};


}} // namespace OVR::CAPI

#endif // OVR_CAPI_FrameTimeManager_h


