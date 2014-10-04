/************************************************************************************

Filename    :   CAPI_HMDState.h
Content     :   State associated with a single HMD
Created     :   January 24, 2014
Authors     :   Michael Antonov

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

#ifndef OVR_CAPI_HMDState_h
#define OVR_CAPI_HMDState_h

#include "../Kernel/OVR_Math.h"
#include "../Kernel/OVR_List.h"
#include "../Kernel/OVR_Log.h"
#include "../OVR_CAPI.h"

#include "CAPI_FrameTimeManager.h"
#include "CAPI_HMDRenderState.h"
#include "CAPI_DistortionRenderer.h"
#include "CAPI_HSWDisplay.h"

#include "../Service/Service_NetClient.h"
#include "../Net/OVR_NetworkTypes.h"
#include "../Util/Util_LatencyTest2Reader.h"

struct ovrHmdStruct { };

namespace OVR { namespace CAPI {


using namespace OVR::Util::Render;
using namespace OVR::Service;
using namespace OVR::Net;


//-------------------------------------------------------------------------------------
// ***** ThreadChecker

// This helper class is used to verify that the API is used according to supported
// thread safety constraints (is not re-entrant for this and related functions).
class ThreadChecker
{
public:

#ifndef OVR_BUILD_DEBUG

    // In release build, thread checks are disabled.
    ThreadChecker() { }
    void Begin(const char* functionName)    { OVR_UNUSED1(functionName); }
    void End()                              {  }

    // Add thread-re-entrancy check for function scope
    struct Scope
    {
        Scope(ThreadChecker*, const char *) { }
        ~Scope() { }
    };


#else // OVR_BUILD_DEBUG
    ThreadChecker() : pFunctionName(0), FirstThread(0)
    { }

    void Begin(const char* functionName)
    {        
        if (!pFunctionName)
        {
            pFunctionName = functionName;
            FirstThread   = GetCurrentThreadId();
        }
        else
        {
            // pFunctionName may be not null here if function is called internally on the same thread.
            OVR_ASSERT_LOG((FirstThread == GetCurrentThreadId()),
                ("%s (threadId=%p) called at the same times as %s (threadId=%p)\n",
                functionName, GetCurrentThreadId(), pFunctionName, FirstThread) );
        }        
    }
    void End()
    {
        pFunctionName = 0;
        FirstThread   = 0;
    }

    // Add thread-reentrancy check for function scope.
    struct Scope
    {
        Scope(ThreadChecker* threadChecker, const char *functionName) : pChecker(threadChecker)
        { pChecker->Begin(functionName); }
        ~Scope()
        { pChecker->End(); }
    private:
        ThreadChecker* pChecker;
    };

private:
    // If not 0, contains the name of the function that first entered the scope.
    const char * pFunctionName;
    ThreadId     FirstThread;

#endif // OVR_BUILD_DEBUG
};


//-------------------------------------------------------------------------------------
// ***** HMDState

// Describes a single HMD.
class HMDState : public ListNode<HMDState>,
                 public ovrHmdStruct, public NewOverrideBase 
{
    void operator=(const HMDState&) { } // Quiet warning.

protected:   
	HMDState(const OVR::Service::HMDNetworkInfo& netInfo,
			 const OVR::HMDInfo& hmdInfo,
			 Profile* profile,
			 Service::NetClient* client);
    HMDState(const HMDInfo& src, Profile* profile);

public:   
    virtual ~HMDState();

    static HMDState* CreateHMDState(Service::NetClient* client, const HMDNetworkInfo& netInfo);
    static HMDState* CreateHMDState(ovrHmdType hmdType); // Used for debug mode

    // *** Sensor Setup

    bool            ConfigureTracking(unsigned supportedCaps, unsigned requiredCaps);    
    void            ResetTracking();
	void			RecenterPose();
    ovrTrackingState PredictedTrackingState(double absTime);

    // Changes HMD Caps.
    // Capability bits that are not directly or logically tied to one system (such as sensor)
    // are grouped here. ovrHmdCap_VSync, for example, affects rendering and timing.
    void            SetEnabledHmdCaps(unsigned caps);
    unsigned        SetEnabledHmdCaps();

    bool            ProcessLatencyTest(unsigned char rgbColorOut[3]);

    // *** Rendering Setup
    bool        ConfigureRendering(ovrEyeRenderDesc eyeRenderDescOut[2],
                                   const ovrFovPort eyeFovIn[2],
                                   const ovrRenderAPIConfig* apiConfig,                                  
                                   unsigned distortionCaps);  
    
    void        UpdateRenderProfile(Profile* profile);


    void        SubmitEyeTextures(const ovrPosef renderPose[2],
                                  const ovrTexture eyeTexture[2]);


    void sharedInit ( Profile *profile );

    void applyProfileToSensorFusion();

    // INlines so that they can be easily compiled out.    
    // Does debug ASSERT checks for functions that require BeginFrame.
    // Also verifies that we are on the right thread.
    void checkBeginFrameScope(const char* functionName)
    {
        OVR_UNUSED1(functionName); // for Release build.
        OVR_ASSERT_LOG(BeginFrameCalled == true,
                       ("%s called outside ovrHmd_BeginFrame.", functionName));
        OVR_ASSERT_LOG(BeginFrameThreadId == OVR::GetCurrentThreadId(),
                       ("%s called on a different thread then ovrHmd_BeginFrame.", functionName));
    }

    void checkRenderingConfigured(const char* functionName)
    {
        OVR_UNUSED1(functionName); // for Release build.
        OVR_ASSERT_LOG(RenderingConfigured == true,
                       ("%s called without ovrHmd_ConfigureRendering.", functionName));
    }

    void checkBeginFrameTimingScope(const char* functionName)
    {
        OVR_UNUSED1(functionName); // for Release build.
        OVR_ASSERT_LOG(BeginFrameTimingCalled == true,
                       ("%s called outside ovrHmd_BeginFrameTiming.", functionName));
    }

	// Get properties by name.
    bool     getBoolValue(const char* propertyName, bool defaultVal);
    bool     setBoolValue(const char* propertyName, bool value);
    int      getIntValue(const char* propertyName, int defaultVal);
    bool     setIntValue(const char* propertyName, int value);
    float    getFloatValue(const char* propertyName, float defaultVal);
    bool     setFloatValue(const char* propertyName, float value);
	unsigned getFloatArray(const char* propertyName, float values[], unsigned arraySize);
    bool     setFloatArray(const char* propertyName, float values[], unsigned arraySize);
    const char* getString(const char* propertyName, const char* defaultVal);
    bool        setString(const char* propertyName, const char* value);

	VirtualHmdId GetNetId() { return NetId; }

public:
	Ptr<Profile>            pProfile;
    // Descriptor that gets allocated and returned to the user as ovrHmd.
    ovrHmdDesc*             pHmdDesc;
    // Window handle passed in AttachWindow.
    void*                   pWindow;

	// Network
	Service::NetClient*     pClient;
	VirtualHmdId            NetId;
	HMDNetworkInfo          NetInfo;

    // HMDInfo shouldn't change, as its string pointers are passed out.    
    HMDInfo                 OurHMDInfo;

    const char*             pLastError;

    // Caps enabled for the HMD.
    unsigned                EnabledHmdCaps;
    
    // Caps actually sent to the Sensor Service
    unsigned                EnabledServiceHmdCaps;
    
    // These are the flags actually applied to the Sensor device,
    // used to track whether SetDisplayReport calls are necessary.
    //unsigned                HmdCapsAppliedToSensor;
    
    // *** Sensor
    Tracking::CombinedSharedStateReader SharedStateReader;
    Tracking::SensorStateReader         TheSensorStateReader;
    Util::RecordStateReader             TheLatencyTestStateReader;

    bool                    LatencyTestActive;
    unsigned char           LatencyTestDrawColor[3];

    bool                    LatencyTest2Active;
    unsigned char           LatencyTest2DrawColor[3];

    // Rendering part
    FrameTimeManager        TimeManager;
    HMDRenderState          RenderState;
    Ptr<DistortionRenderer> pRenderer;

    // Health and Safety Warning display.
    Ptr<HSWDisplay>         pHSWDisplay;

    // Last timing value reported by BeginFrame.
    double                  LastFrameTimeSeconds;    
    // Last timing value reported by GetFrameTime. These are separate since the intended
    // use is from different threads. TBD: Move to FrameTimeManager? Make atomic?
    double                  LastGetFrameTimeSeconds;

    // Last cached value returned by ovrHmd_GetString/ovrHmd_GetStringArray.
    char                    LastGetStringValue[256];
   
    // Debug flag set after ovrHmd_ConfigureRendering succeeds.
    bool                    RenderingConfigured;
    // Set after BeginFrame succeeds, and its corresponding thread id for debug checks.
    bool                    BeginFrameCalled;
    ThreadId                BeginFrameThreadId;    
    // Graphics functions are not re-entrant from other threads.
    ThreadChecker           RenderAPIThreadChecker;
    // 
    bool                    BeginFrameTimingCalled;
};




//I appreciate this isn't an idea place for this function prototype, but needed
//to be seen by OVR_CAPI.cpp and the various SDK renderers of CAPI,
//and have everything defined.  Please move to a better place if you know of one.
ovrBool ovrHmd_CreateDistortionMeshInternal( ovrHmdStruct *  hmd,
                                             ovrEyeType eyeType, ovrFovPort fov,
                                             unsigned int distortionCaps,
                                             ovrDistortionMesh *meshData,
											 float overrideEyeReliefIfNonZero=0 );




}} // namespace OVR::CAPI

#endif // OVR_CAPI_HMDState_h
