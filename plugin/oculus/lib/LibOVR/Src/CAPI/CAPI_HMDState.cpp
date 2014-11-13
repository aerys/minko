/************************************************************************************

Filename    :   CAPI_HMDState.cpp
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

#include "CAPI_HMDState.h"
#include "../OVR_Profile.h"
#include "../Service/Service_NetClient.h"
#ifdef OVR_OS_WIN32
#include "../Displays/OVR_Win32_ShimFunctions.h"
#endif


namespace OVR { namespace CAPI {


//-------------------------------------------------------------------------------------
// ***** HMDState

HMDState::HMDState(const OVR::Service::HMDNetworkInfo& netInfo,
				   const OVR::HMDInfo& hmdInfo,
				   Profile* profile,
				   Service::NetClient* client) :
    pProfile(profile),
    pHmdDesc(0),
    pWindow(0),
    pClient(client),
    NetId(netInfo.NetId),
    NetInfo(netInfo),
    OurHMDInfo(hmdInfo),
    pLastError(NULL),
    EnabledHmdCaps(0),
    EnabledServiceHmdCaps(0),
    SharedStateReader(),
    TheSensorStateReader(),
    TheLatencyTestStateReader(),
    LatencyTestActive(false),
  //LatencyTestDrawColor(),
    LatencyTest2Active(false),
  //LatencyTest2DrawColor(),
    TimeManager(true),
    RenderState(),
    pRenderer(),
    pHSWDisplay(),
    LastFrameTimeSeconds(0.),
    LastGetFrameTimeSeconds(0.),
  //LastGetStringValue(),
    RenderingConfigured(false),
    BeginFrameCalled(false),
    BeginFrameThreadId(),
    RenderAPIThreadChecker(),
    BeginFrameTimingCalled(false)
{
    sharedInit(profile);
}


HMDState::HMDState(const OVR::HMDInfo& hmdInfo, Profile* profile) :
    pProfile(profile),
    pHmdDesc(0),
    pWindow(0),
    pClient(0),
    NetId(InvalidVirtualHmdId),
    NetInfo(),
    OurHMDInfo(hmdInfo),
    pLastError(NULL),
    EnabledHmdCaps(0),
    EnabledServiceHmdCaps(0),
    SharedStateReader(),
    TheSensorStateReader(),
    TheLatencyTestStateReader(),
    LatencyTestActive(false),
  //LatencyTestDrawColor(),
    LatencyTest2Active(false),
  //LatencyTest2DrawColor(),
    TimeManager(true),
    RenderState(),
    pRenderer(),
    pHSWDisplay(),
    LastFrameTimeSeconds(0.),
    LastGetFrameTimeSeconds(0.),
  //LastGetStringValue(),
    RenderingConfigured(false),
    BeginFrameCalled(false),
    BeginFrameThreadId(),
    RenderAPIThreadChecker(),
    BeginFrameTimingCalled(false)
{
    sharedInit(profile);
}

HMDState::~HMDState()
{
    if (pClient)
    {
		pClient->Hmd_Release(NetId);
		pClient = 0;
    }

    ConfigureRendering(0,0,0,0);

    if (pHmdDesc)
    {
        OVR_FREE(pHmdDesc);
        pHmdDesc = NULL;
    }
}

void HMDState::sharedInit(Profile* profile)
{
    // TBD: We should probably be looking up the default profile for the given
    // device type + user if profile == 0.    
    pLastError = 0;

    RenderState.OurHMDInfo = OurHMDInfo;

    UpdateRenderProfile(profile);

    OVR_ASSERT(!pHmdDesc);
    pHmdDesc         = (ovrHmdDesc*)OVR_ALLOC(sizeof(ovrHmdDesc));
    *pHmdDesc        = RenderState.GetDesc();
    pHmdDesc->Handle = this;

    RenderState.ClearColor[0] = 0.0f;
    RenderState.ClearColor[1] = 0.0f;
    RenderState.ClearColor[2] = 0.0f;
    RenderState.ClearColor[3] = 0.0f;

    RenderState.EnabledHmdCaps = 0;

    TimeManager.Init(RenderState.RenderInfo);

    /*
    LatencyTestDrawColor[0] = 0;
    LatencyTestDrawColor[1] = 0;
    LatencyTestDrawColor[2] = 0;
    */

    RenderingConfigured = false;
    BeginFrameCalled   = false;
    BeginFrameThreadId = 0;
    BeginFrameTimingCalled = false;

    TheSensorStateReader.LoadProfileCenteredFromWorld(profile);

    // Construct the HSWDisplay. We will later reconstruct it with a specific ovrRenderAPI type if the application starts using SDK-based rendering.
    if(!pHSWDisplay)
        pHSWDisplay = *OVR::CAPI::HSWDisplay::Factory(ovrRenderAPI_None, pHmdDesc, RenderState);
}

static Vector3f GetNeckModelFromProfile(Profile* profile)
{
    OVR_ASSERT(profile);

    float neckeye[2] = { OVR_DEFAULT_NECK_TO_EYE_HORIZONTAL, OVR_DEFAULT_NECK_TO_EYE_VERTICAL };
    profile->GetFloatValues(OVR_KEY_NECK_TO_EYE_DISTANCE, neckeye, 2);

    // Make sure these are vaguely sensible values.
    //OVR_ASSERT((neckeye[0] > 0.05f) && (neckeye[0] < 0.5f));
    //OVR_ASSERT((neckeye[1] > 0.05f) && (neckeye[1] < 0.5f));

    // Named for clarity
    float NeckToEyeHorizontal = neckeye[0];
    float NeckToEyeVertical = neckeye[1];

    // Store the neck model
    return Vector3f(0.0, NeckToEyeVertical, -NeckToEyeHorizontal);
}

static float GetCenterPupilDepthFromRenderInfo(HmdRenderInfo* hmdRenderInfo)
{
    OVR_ASSERT(hmdRenderInfo);

    // Find the distance from the center of the screen to the "center eye"
    // This center eye is used by systems like rendering & audio to represent the player,
    // and they will handle the offsets needed from there to each actual eye.

    // HACK HACK HACK
    // We know for DK1 the screen->lens surface distance is roughly 0.049f, and that the faceplate->lens is 0.02357f.
    // We're going to assume(!!!!) that all HMDs have the same screen->faceplate distance.
    // Crystal Cove was measured to be roughly 0.025 screen->faceplate which agrees with this assumption.
    // TODO: do this properly!  Update:  Measured this at 0.02733 with a CC prototype, CES era (PT7), on 2/19/14 -Steve
    float screenCenterToMidplate = 0.02733f;
    float centerEyeRelief = hmdRenderInfo->GetEyeCenter().ReliefInMeters;
    float CenterPupilDepth = screenCenterToMidplate + hmdRenderInfo->LensSurfaceToMidplateInMeters + centerEyeRelief;

    return CenterPupilDepth;
}

void HMDState::UpdateRenderProfile(Profile* profile)
{
    // Apply the given profile to generate a render context
    RenderState.RenderInfo = GenerateHmdRenderInfoFromHmdInfo(RenderState.OurHMDInfo, profile);
    RenderState.Distortion[0] = CalculateDistortionRenderDesc(StereoEye_Left, RenderState.RenderInfo, 0);
    RenderState.Distortion[1] = CalculateDistortionRenderDesc(StereoEye_Right, RenderState.RenderInfo, 0);

    if (pClient)
    {
        // Center pupil depth
        float centerPupilDepth = GetCenterPupilDepthFromRenderInfo(&RenderState.RenderInfo);
        pClient->SetNumberValue(GetNetId(), "CenterPupilDepth", centerPupilDepth);

        // Neck model
        Vector3f neckModel = GetNeckModelFromProfile(profile);
        double neckModelArray[3] = {
            neckModel.x,
            neckModel.y,
            neckModel.z
        };
        pClient->SetNumberValues(GetNetId(), "NeckModelVector3f", neckModelArray, 3);
    }
}

HMDState* HMDState::CreateHMDState(NetClient* client, const HMDNetworkInfo& netInfo)
{
    // HMDState works through a handle to service HMD....
    HMDInfo hinfo;
    if (!client->Hmd_GetHmdInfo(netInfo.NetId, &hinfo))
    {
        OVR_DEBUG_LOG(("[HMDState] Unable to get HMD info"));
        return NULL;
    }

#ifdef OVR_OS_WIN32
    OVR_DEBUG_LOG(("Setting up display shim"));

    // Initialize the display shim before reporting the display to the user code
    // so that this will happen before the D3D display object is created.
    Win32::DisplayShim::GetInstance().Update(&hinfo.ShimInfo);
#endif

    Ptr<Profile> pDefaultProfile = *ProfileManager::GetInstance()->GetDefaultUserProfile(&hinfo);
    OVR_DEBUG_LOG(("Using profile %s", pDefaultProfile->GetValue(OVR_KEY_USER)));

    HMDState* hmds = new HMDState(netInfo, hinfo, pDefaultProfile, client);

    if (!hmds->SharedStateReader.Open(netInfo.SharedMemoryName.ToCStr()))
    {
        delete hmds;
        return NULL;
    }

    hmds->TheSensorStateReader.SetUpdater(hmds->SharedStateReader.Get());
    hmds->TheLatencyTestStateReader.SetUpdater(hmds->SharedStateReader.Get());

    return hmds;
}

HMDState* HMDState::CreateHMDState(ovrHmdType hmdType)
{
    HmdTypeEnum t = HmdType_None;
    if (hmdType == ovrHmd_DK1)
        t = HmdType_DK1;    
    else if (hmdType == ovrHmd_DK2)
        t = HmdType_DK2;

    // FIXME: This does not actually grab the right user..
    Ptr<Profile> pDefaultProfile = *ProfileManager::GetInstance()->GetDefaultProfile(t);

    return new HMDState(CreateDebugHMDInfo(t), pDefaultProfile);
}
    

//-------------------------------------------------------------------------------------
// *** Sensor 

bool HMDState::ConfigureTracking(unsigned supportedCaps, unsigned requiredCaps)
{
	return pClient ? pClient->Hmd_ConfigureTracking(NetId, supportedCaps, requiredCaps) : true;
}

void HMDState::ResetTracking()
{
	if (pClient) pClient->Hmd_ResetTracking(NetId);
}        

// Re-center the orientation.
void HMDState::RecenterPose()
{
    TheSensorStateReader.RecenterPose();
}

// Returns prediction for time.
ovrTrackingState HMDState::PredictedTrackingState(double absTime)
{    
	Tracking::TrackingState ss;
    TheSensorStateReader.GetSensorStateAtTime(absTime, ss);

    // Zero out the status flags
    if (!pClient || !pClient->IsConnected(false, false))
    {
        ss.StatusFlags = 0;
    }

    return ss;
}

void HMDState::SetEnabledHmdCaps(unsigned hmdCaps)
{
    if (OurHMDInfo.HmdType < HmdType_DK2)
    {
        // disable low persistence
        hmdCaps &= ~ovrHmdCap_LowPersistence;

        // disable dynamic prediction using the internal latency tester
        hmdCaps &= ~ovrHmdCap_DynamicPrediction;
    }

    if (OurHMDInfo.HmdType >= HmdType_DK2)
    {
        if ((EnabledHmdCaps ^ hmdCaps) & ovrHmdCap_DynamicPrediction)
        {
            // DynamicPrediction change
            TimeManager.ResetFrameTiming(TimeManager.GetFrameTiming().FrameIndex,
                                         (hmdCaps & ovrHmdCap_DynamicPrediction) ? true : false,
                                         RenderingConfigured);
        }
    }

    if ((EnabledHmdCaps ^ hmdCaps) & ovrHmdCap_NoVSync)
    {
        TimeManager.SetVsync((hmdCaps & ovrHmdCap_NoVSync) ? false : true);
    }

    if ((EnabledHmdCaps ^ hmdCaps) & ovrHmdCap_NoMirrorToWindow)
    {
#ifdef OVR_OS_WIN32
        Win32::DisplayShim::GetInstance().UseMirroring = (hmdCaps & ovrHmdCap_NoMirrorToWindow)  ?
                                                         false : true;
        if (pWindow)
        {   // Force window repaint so that stale mirrored image doesn't persist.
            ::InvalidateRect((HWND)pWindow, 0, true);
        }
#endif
    }

    // TBD: Should this include be only the rendering flags? Otherwise, bits that failed
    //      modification in Hmd_SetEnabledCaps may mis-match...
    EnabledHmdCaps             = hmdCaps & ovrHmdCap_Writable_Mask;
    RenderState.EnabledHmdCaps = EnabledHmdCaps;


    // If any of the modifiable service caps changed, call on the service.
    unsigned prevServiceCaps = EnabledServiceHmdCaps & ovrHmdCap_Writable_Mask;
    unsigned newServiceCaps  = hmdCaps & ovrHmdCap_Writable_Mask & ovrHmdCap_Service_Mask;

    if (prevServiceCaps ^ newServiceCaps)
	{
        EnabledServiceHmdCaps = pClient ? pClient->Hmd_SetEnabledCaps(NetId, newServiceCaps)
                                : newServiceCaps;
    }
}


unsigned HMDState::SetEnabledHmdCaps()
{
	unsigned serviceCaps = pClient ? pClient->Hmd_GetEnabledCaps(NetId) :
                                      EnabledServiceHmdCaps;
    
    return serviceCaps & ((~ovrHmdCap_Service_Mask) | EnabledHmdCaps);    
}


//-------------------------------------------------------------------------------------
// ***** Property Access

// FIXME: Remove the EGetBoolValue stuff and do it with a "Server:" prefix, so we do not
// need to keep a white-list of keys.  This is also way cool because it allows us to add
// new settings keys from outside CAPI that can modify internal server data.

bool HMDState::getBoolValue(const char* propertyName, bool defaultVal)
{
    if (NetSessionCommon::IsServiceProperty(NetSessionCommon::EGetBoolValue, propertyName))
    {
       return NetClient::GetInstance()->GetBoolValue(GetNetId(), propertyName, defaultVal);
    }
    else if (pProfile)
    {
        return pProfile->GetBoolValue(propertyName, defaultVal);
    }
    return defaultVal;
}

bool HMDState::setBoolValue(const char* propertyName, bool value)
{
    return NetClient::GetInstance()->SetBoolValue(GetNetId(), propertyName, value);
}

int HMDState::getIntValue(const char* propertyName, int defaultVal)
{
    if (NetSessionCommon::IsServiceProperty(NetSessionCommon::EGetIntValue, propertyName))
    {
        return NetClient::GetInstance()->GetIntValue(GetNetId(), propertyName, defaultVal);
    }
    else if (pProfile)
    {
        return pProfile->GetIntValue(propertyName, defaultVal);
    }
    return defaultVal;
}

bool HMDState::setIntValue(const char* propertyName, int value)
{
    return NetClient::GetInstance()->SetIntValue(GetNetId(), propertyName, value);
}

float HMDState::getFloatValue(const char* propertyName, float defaultVal)
{
    if (OVR_strcmp(propertyName, "LensSeparation") == 0)
    {
        return OurHMDInfo.LensSeparationInMeters;
    }
    else if (OVR_strcmp(propertyName, "VsyncToNextVsync") == 0) 
    {
        return OurHMDInfo.Shutter.VsyncToNextVsync;
    }
    else if (OVR_strcmp(propertyName, "PixelPersistence") == 0) 
    {
        return OurHMDInfo.Shutter.PixelPersistence;
    }
    else if (NetSessionCommon::IsServiceProperty(NetSessionCommon::EGetNumberValue, propertyName))
    {
       return (float)NetClient::GetInstance()->GetNumberValue(GetNetId(), propertyName, defaultVal);
    }
    else if (pProfile)
    {
        return pProfile->GetFloatValue(propertyName, defaultVal);
    }

    return defaultVal;
}

bool HMDState::setFloatValue(const char* propertyName, float value)
{
    return NetClient::GetInstance()->SetNumberValue(GetNetId(), propertyName, value);
}

static unsigned CopyFloatArrayWithLimit(float dest[], unsigned destSize,
                                        float source[], unsigned sourceSize)
{
    unsigned count = Alg::Min(destSize, sourceSize);
    for (unsigned i = 0; i < count; i++)
        dest[i] = source[i];
    return count;
}

unsigned HMDState::getFloatArray(const char* propertyName, float values[], unsigned arraySize)
{
	if (arraySize)
	{
		if (OVR_strcmp(propertyName, "ScreenSize") == 0)
		{
			float data[2] = { OurHMDInfo.ScreenSizeInMeters.w, OurHMDInfo.ScreenSizeInMeters.h };

            return CopyFloatArrayWithLimit(values, arraySize, data, 2);
		}
        else if (OVR_strcmp(propertyName, "DistortionClearColor") == 0)
        {
            return CopyFloatArrayWithLimit(values, arraySize, RenderState.ClearColor, 4);
        }
        else if (OVR_strcmp(propertyName, "DK2Latency") == 0)
        {
            if (OurHMDInfo.HmdType != HmdType_DK2)
            {
                return 0;
            }

            float data[3];            
            TimeManager.GetLatencyTimings(data);
            
            return CopyFloatArrayWithLimit(values, arraySize, data, 3);
        }
        else if (NetSessionCommon::IsServiceProperty(NetSessionCommon::EGetNumberValues, propertyName))
        {
            // Convert floats to doubles
            double* da = new double[arraySize];
            for (int i = 0; i < (int)arraySize; ++i)
            {
                da[i] = values[i];
            }

            int count = NetClient::GetInstance()->GetNumberValues(GetNetId(), propertyName, da, (int)arraySize);

            for (int i = 0; i < count; ++i)
            {
                values[i] = (float)da[i];
            }

            delete[] da;

            return count;
        }
		else if (pProfile)
		{        
			// TBD: Not quite right. Should update profile interface, so that
			//      we can return 0 in all conditions if property doesn't exist.
		
            return pProfile->GetFloatValues(propertyName, values, arraySize);
			}
		}

	return 0;
}

bool HMDState::setFloatArray(const char* propertyName, float values[], unsigned arraySize)
{
    if (!arraySize)
    {
        return false;
    }
    
    if (OVR_strcmp(propertyName, "DistortionClearColor") == 0)
    {
        CopyFloatArrayWithLimit(RenderState.ClearColor, 4, values, arraySize);
        return true;
    }

    double* da = new double[arraySize];
    for (int i = 0; i < (int)arraySize; ++i)
    {
        da[i] = values[i];
    }

    bool result = NetClient::GetInstance()->SetNumberValues(GetNetId(), propertyName, da, arraySize);

    delete[] da;

    return result;
}

const char* HMDState::getString(const char* propertyName, const char* defaultVal)
{
    if (NetSessionCommon::IsServiceProperty(NetSessionCommon::EGetStringValue, propertyName))
    {
        return NetClient::GetInstance()->GetStringValue(GetNetId(), propertyName, defaultVal);
    }

	if (pProfile)
	{
		LastGetStringValue[0] = 0;
		if (pProfile->GetValue(propertyName, LastGetStringValue, sizeof(LastGetStringValue)))
		{
			return LastGetStringValue;
		}
	}

	return defaultVal;
}

bool HMDState::setString(const char* propertyName, const char* value)
{
    return NetClient::GetInstance()->SetStringValue(GetNetId(), propertyName, value);
}


//-------------------------------------------------------------------------------------
// *** Latency Test

bool HMDState::ProcessLatencyTest(unsigned char rgbColorOut[3])
{    
    return NetClient::GetInstance()->LatencyUtil_ProcessInputs(Timer::GetSeconds(), rgbColorOut);
}

//-------------------------------------------------------------------------------------
// *** Rendering

bool HMDState::ConfigureRendering(ovrEyeRenderDesc eyeRenderDescOut[2],
                                  const ovrFovPort eyeFovIn[2],
                                  const ovrRenderAPIConfig* apiConfig,                                  
                                  unsigned distortionCaps)
{
    ThreadChecker::Scope checkScope(&RenderAPIThreadChecker, "ovrHmd_ConfigureRendering");

    // null -> shut down.
    if (!apiConfig)
    {
        if (pHSWDisplay)
        {
            pHSWDisplay->Shutdown();
            pHSWDisplay.Clear();
        }

        if (pRenderer)
            pRenderer.Clear();        
        RenderingConfigured = false; 
        return true;
    }

    if (pRenderer &&
        (apiConfig->Header.API != pRenderer->GetRenderAPI()))
    {
        // Shutdown old renderer.
        if (pHSWDisplay)
        {
            pHSWDisplay->Shutdown();
            pHSWDisplay.Clear();
        }

        if (pRenderer)
            pRenderer.Clear();
    }

	distortionCaps = distortionCaps & pHmdDesc->DistortionCaps;

    // Step 1: do basic setup configuration
    RenderState.EnabledHmdCaps = EnabledHmdCaps;     // This is a copy... Any cleaner way?
    RenderState.DistortionCaps = distortionCaps;
    RenderState.EyeRenderDesc[0] = RenderState.CalcRenderDesc(ovrEye_Left,  eyeFovIn[0]);
    RenderState.EyeRenderDesc[1] = RenderState.CalcRenderDesc(ovrEye_Right, eyeFovIn[1]);
    eyeRenderDescOut[0] = RenderState.EyeRenderDesc[0];
    eyeRenderDescOut[1] = RenderState.EyeRenderDesc[1];

    TimeManager.ResetFrameTiming(0,
                                 (EnabledHmdCaps & ovrHmdCap_DynamicPrediction) ? true : false,
                                 true);

    LastFrameTimeSeconds = 0.0f;

    // Set RenderingConfigured early to avoid ASSERTs in renderer initialization.
    RenderingConfigured = true;

    if (!pRenderer)
    {
        pRenderer = *DistortionRenderer::APICreateRegistry
                        [apiConfig->Header.API](pHmdDesc, TimeManager, RenderState);
    }

    if (!pRenderer ||
        !pRenderer->Initialize(apiConfig, distortionCaps))
    {
        RenderingConfigured = false;
        return false;
    }    

    // Setup the Health and Safety Warning display system.
    if(pHSWDisplay && (pHSWDisplay->GetRenderAPIType() != apiConfig->Header.API)) // If we need to reconstruct the HSWDisplay for a different graphics API type, delete the existing display.
    {
        pHSWDisplay->Shutdown();
        pHSWDisplay.Clear();
    }

    if(!pHSWDisplay) // Use * below because that for of operator= causes it to inherit the refcount the factory gave the object.
        pHSWDisplay = *OVR::CAPI::HSWDisplay::Factory(apiConfig->Header.API, pHmdDesc, RenderState);

    if (pHSWDisplay)
        pHSWDisplay->Initialize(apiConfig); // This is potentially re-initializing it with a new config.

    return true;
}


void  HMDState::SubmitEyeTextures(const ovrPosef renderPose[2],
                                  const ovrTexture eyeTexture[2])
{
    RenderState.EyeRenderPoses[0] = renderPose[0];
    RenderState.EyeRenderPoses[1] = renderPose[1];

    if (pRenderer)
    {
        pRenderer->SubmitEye(0, &eyeTexture[0]);
        pRenderer->SubmitEye(1, &eyeTexture[1]);
    }
}


// I appreciate this is not an idea place for this function, but it didn't seem to be
// being linked properly when in OVR_CAPI.cpp. 
// Please relocate if you know of a better place
ovrBool ovrHmd_CreateDistortionMeshInternal( ovrHmdStruct *  hmd,
                                             ovrEyeType eyeType, ovrFovPort fov,
                                             unsigned int distortionCaps,
                                             ovrDistortionMesh *meshData,
											 float overrideEyeReliefIfNonZero )
{
    if (!meshData)
        return 0;
    HMDState* hmds = (HMDState*)hmd;

    // Not used now, but Chromatic flag or others could possibly be checked for in the future.
    OVR_UNUSED1(distortionCaps); 
   
#if defined (OVR_OS_WIN32)
    OVR_COMPILER_ASSERT(sizeof(DistortionMeshVertexData) == sizeof(ovrDistortionVertex));
#endif
	
    // *** Calculate a part of "StereoParams" needed for mesh generation

    // Note that mesh distortion generation is invariant of RenderTarget UVs, allowing
    // render target size and location to be changed after the fact dynamically. 
    // eyeToSourceUV is computed here for convenience, so that users don't need
    // to call ovrHmd_GetRenderScaleAndOffset unless changing RT dynamically.

    const HmdRenderInfo&  hmdri          = hmds->RenderState.RenderInfo;    
    StereoEye             stereoEye      = (eyeType == ovrEye_Left) ? StereoEye_Left : StereoEye_Right;

    DistortionRenderDesc& distortion = hmds->RenderState.Distortion[eyeType];
	if (overrideEyeReliefIfNonZero)
	{
		distortion.Lens = GenerateLensConfigFromEyeRelief(overrideEyeReliefIfNonZero,hmdri);
	}

    // Find the mapping from TanAngle space to target NDC space.
    ScaleAndOffset2D      eyeToSourceNDC = CreateNDCScaleAndOffsetFromFov(fov);

    int triangleCount = 0;
    int vertexCount = 0;

    DistortionMeshCreate((DistortionMeshVertexData**)&meshData->pVertexData,
                         (uint16_t**)&meshData->pIndexData,
                          &vertexCount, &triangleCount,
                          (stereoEye == StereoEye_Right),
                          hmdri, distortion, eyeToSourceNDC);

    if (meshData->pVertexData)
    {
        // Convert to index
        meshData->IndexCount = triangleCount * 3;
        meshData->VertexCount = vertexCount;
        return 1;
    }

    return 0;
}



}} // namespace OVR::CAPI
