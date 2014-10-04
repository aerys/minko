/************************************************************************************

Filename    :   OVR_CAPI.cpp
Content     :   Experimental simple C interface to the HMD - version 1.
Created     :   November 30, 2013
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

#include "OVR_CAPI.h"
#include "Kernel/OVR_Timer.h"
#include "Kernel/OVR_Math.h"
#include "Kernel/OVR_System.h"
#include "OVR_Stereo.h"
#include "OVR_Profile.h"
#include "../Include/OVR_Version.h"

#include "CAPI/CAPI_HMDState.h"
#include "CAPI/CAPI_FrameTimeManager.h"

#include "Service/Service_NetClient.h"
#ifdef OVR_SINGLE_PROCESS
#include "Service/Service_NetServer.h"
#endif

#ifdef OVR_OS_WIN32
#include "Displays/OVR_Win32_ShimFunctions.h"
#endif


using namespace OVR;
using namespace OVR::Util::Render;
using namespace OVR::Tracking;

//-------------------------------------------------------------------------------------
// Math
namespace OVR {


// ***** FovPort

// C-interop support: FovPort <-> ovrFovPort
FovPort::FovPort(const ovrFovPort &src)
    : UpTan(src.UpTan), DownTan(src.DownTan), LeftTan(src.LeftTan), RightTan(src.RightTan)
{ }    

FovPort::operator ovrFovPort () const
{
    ovrFovPort result;
    result.LeftTan  = LeftTan;
    result.RightTan = RightTan;
    result.UpTan    = UpTan;
    result.DownTan  = DownTan;
    return result;
}

// Converts Fov Tan angle units to [-1,1] render target NDC space
Vector2f FovPort::TanAngleToRendertargetNDC(Vector2f const &tanEyeAngle)
{  
    ScaleAndOffset2D eyeToSourceNDC = CreateNDCScaleAndOffsetFromFov(*this);
    return tanEyeAngle * eyeToSourceNDC.Scale + eyeToSourceNDC.Offset;
}

// ***** SensorDataType

SensorDataType::SensorDataType(const ovrSensorData& s)
{
    Acceleration = s.Accelerometer;
    RotationRate = s.Gyro;
    MagneticField = s.Magnetometer;
    Temperature = s.Temperature;
    AbsoluteTimeSeconds = s.TimeInSeconds;
}

SensorDataType::operator ovrSensorData () const
{
    ovrSensorData result;
    result.Accelerometer = Acceleration;
    result.Gyro = RotationRate;
    result.Magnetometer = MagneticField;
    result.Temperature = Temperature;
    result.TimeInSeconds = (float) AbsoluteTimeSeconds;
    return result;
}


// ***** SensorState

TrackingState::TrackingState(const ovrTrackingState& s)
{
    HeadPose    = s.HeadPose;
    CameraPose  = s.CameraPose;
    LeveledCameraPose = s.LeveledCameraPose;
    RawSensorData = s.RawSensorData;
    StatusFlags = s.StatusFlags;
    LastVisionProcessingTime = s.LastVisionProcessingTime;
}

TrackingState::operator ovrTrackingState() const
{
    ovrTrackingState result;
    result.HeadPose     = HeadPose;
    result.CameraPose   = CameraPose;
    result.LeveledCameraPose = LeveledCameraPose;
    result.RawSensorData  = RawSensorData;
    result.StatusFlags  = StatusFlags;
    result.LastVisionProcessingTime = LastVisionProcessingTime;
    return result;
}


} // namespace OVR

//-------------------------------------------------------------------------------------

using namespace OVR::CAPI;

#ifdef __cplusplus 
extern "C" {
#endif


// Used to generate projection from ovrEyeDesc::Fov
OVR_EXPORT ovrMatrix4f ovrMatrix4f_Projection(ovrFovPort fov, float znear, float zfar, ovrBool rightHanded)
{
    return CreateProjection(rightHanded ? true : false, fov, znear, zfar);
}


OVR_EXPORT ovrMatrix4f ovrMatrix4f_OrthoSubProjection(ovrMatrix4f projection, ovrVector2f orthoScale,
                                                      float orthoDistance, float eyeViewAdjustX)
{

    float orthoHorizontalOffset = eyeViewAdjustX / orthoDistance;

    // Current projection maps real-world vector (x,y,1) to the RT.
    // We want to find the projection that maps the range [-FovPixels/2,FovPixels/2] to
    // the physical [-orthoHalfFov,orthoHalfFov]
    // Note moving the offset from M[0][2]+M[1][2] to M[0][3]+M[1][3] - this means
    // we don't have to feed in Z=1 all the time.
    // The horizontal offset math is a little hinky because the destination is
    // actually [-orthoHalfFov+orthoHorizontalOffset,orthoHalfFov+orthoHorizontalOffset]
    // So we need to first map [-FovPixels/2,FovPixels/2] to
    //                         [-orthoHalfFov+orthoHorizontalOffset,orthoHalfFov+orthoHorizontalOffset]:
    // x1 = x0 * orthoHalfFov/(FovPixels/2) + orthoHorizontalOffset;
    //    = x0 * 2*orthoHalfFov/FovPixels + orthoHorizontalOffset;
    // But then we need the sam mapping as the existing projection matrix, i.e.
    // x2 = x1 * Projection.M[0][0] + Projection.M[0][2];
    //    = x0 * (2*orthoHalfFov/FovPixels + orthoHorizontalOffset) * Projection.M[0][0] + Projection.M[0][2];
    //    = x0 * Projection.M[0][0]*2*orthoHalfFov/FovPixels +
    //      orthoHorizontalOffset*Projection.M[0][0] + Projection.M[0][2];
    // So in the new projection matrix we need to scale by Projection.M[0][0]*2*orthoHalfFov/FovPixels and
    // offset by orthoHorizontalOffset*Projection.M[0][0] + Projection.M[0][2].

    Matrix4f ortho;
    ortho.M[0][0] = projection.M[0][0] * orthoScale.x;
    ortho.M[0][1] = 0.0f;
    ortho.M[0][2] = 0.0f;
    ortho.M[0][3] = -projection.M[0][2] + ( orthoHorizontalOffset * projection.M[0][0] );

    ortho.M[1][0] = 0.0f;
    ortho.M[1][1] = -projection.M[1][1] * orthoScale.y;       // Note sign flip (text rendering uses Y=down).
    ortho.M[1][2] = 0.0f;
    ortho.M[1][3] = -projection.M[1][2];

    /*
    if ( fabsf ( zNear - zFar ) < 0.001f )
    {
        ortho.M[2][0] = 0.0f;
        ortho.M[2][1] = 0.0f;
        ortho.M[2][2] = 0.0f;
        ortho.M[2][3] = zFar;
    }
    else
    {
        ortho.M[2][0] = 0.0f;
        ortho.M[2][1] = 0.0f;
        ortho.M[2][2] = zFar / (zNear - zFar);
        ortho.M[2][3] = (zFar * zNear) / (zNear - zFar);
    }
    */

    // MA: Undo effect of sign
    ortho.M[2][0] = 0.0f;
    ortho.M[2][1] = 0.0f;
    //ortho.M[2][2] = projection.M[2][2] * projection.M[3][2] * -1.0f; // reverse right-handedness
    ortho.M[2][2] = 0.0f;
    ortho.M[2][3] = 0.0f;
        //projection.M[2][3];

    // No perspective correction for ortho.
    ortho.M[3][0] = 0.0f;
    ortho.M[3][1] = 0.0f;
    ortho.M[3][2] = 0.0f;
    ortho.M[3][3] = 1.0f;

    return ortho;
}


OVR_EXPORT double ovr_GetTimeInSeconds()
{
    return Timer::GetSeconds();
}

// Waits until the specified absolute time.
OVR_EXPORT double ovr_WaitTillTime(double absTime)
{
    double       initialTime = ovr_GetTimeInSeconds();
    double       newTime     = initialTime;
    
    while(newTime < absTime)
    {
        for (int j = 0; j < 5; j++)
            OVR_PROCESSOR_PAUSE();

        newTime = ovr_GetTimeInSeconds();
    }

    // How long we waited
    return newTime - initialTime;
}


//-------------------------------------------------------------------------------------

// 1. Init/shutdown.

static ovrBool CAPI_SystemInitCalled = 0;
static ovrBool CAPI_ovrInitializeCalled = 0;

static OVR::Service::NetClient* CAPI_pNetClient = 0;

OVR_EXPORT void ovr_InitializeRenderingShim()
{
    OVR::System::DirectDisplayInitialize();
}

OVR_EXPORT ovrBool ovr_Initialize()
{
    if (CAPI_ovrInitializeCalled)
        return 1;

    // We must set up the system for the plugin to work
    if (!OVR::System::IsInitialized())
    {
        OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));
        CAPI_SystemInitCalled = 1;
    }

    CAPI_pNetClient = NetClient::GetInstance();

#ifdef OVR_SINGLE_PROCESS

    // If the server could not start running,
    if (Service::NetServer::GetInstance()->IsInitialized())
    {
        CAPI_pNetClient->Connect(true);
    }
    else
    {
        // This normally will happen if the OVRService is running in the background,
        // or another SingleProcess-mode app is running in the background.
        // In this case, it's using the hardware and we should not also attempt to use
        // the hardware.
        LogError("{ERR-079} [LibOVR] Server is already running");
    }
#else
    CAPI_pNetClient->Connect(true);
#endif

    CAPI_ovrInitializeCalled = 1;

    return 1;
}

OVR_EXPORT void ovr_Shutdown()
{  
    // We should clean up the system to be complete
    if (OVR::System::IsInitialized() && CAPI_SystemInitCalled)
    {
        OVR::System::Destroy();
    }

    CAPI_SystemInitCalled = 0;
    CAPI_ovrInitializeCalled = 0;
}


// There is a thread safety issue with ovrHmd_Detect in that multiple calls from different
// threads can corrupt the global array state. This would lead to two problems:
//  a) Create(index) enumerator may miss or overshoot items. Probably not a big deal
//     as game logic can easily be written to only do Detect(s)/Creates in one place.
//     The alternative would be to return list handle.
//  b) TBD: Un-mutexed Detect access from two threads could lead to crash. We should
//         probably check this.
//

OVR_EXPORT int ovrHmd_Detect()
{
    if (!CAPI_ovrInitializeCalled)
        return 0;

    return CAPI_pNetClient->Hmd_Detect();
}


// ovrHmd_Create us explicitly separated from ConfigureTracking and ConfigureRendering to allow creation of 
// a relatively light-weight handle that would reference the device going forward and would 
// survive future ovrHmd_Detect calls. That is once ovrHMD is returned, index is no longer
// necessary and can be changed by a ovrHmd_Detect call.
OVR_EXPORT ovrHmd ovrHmd_Create(int index)
{
    if (!CAPI_ovrInitializeCalled)
        return 0;

    double t0 = Timer::GetSeconds();
    HMDNetworkInfo netInfo;

    // There may be some delay before the HMD is fully detected.
    // Since we are also trying to create the HMD immediately it may lose this race and
    // get "NO HMD DETECTED."  Wait a bit longer to avoid this.
    while (!CAPI_pNetClient->Hmd_Create(index, &netInfo) ||
           netInfo.NetId == InvalidVirtualHmdId)
    {
        // If two seconds elapse and still no HMD detected,
        if (Timer::GetSeconds() - t0 > 2.)
        {
            if (!NetClient::GetInstance()->IsConnected(false, false))
            {
                NetClient::GetInstance()->SetLastError("Not connected to service");
            }
            else
            {
                NetClient::GetInstance()->SetLastError("No HMD Detected");
            }

            return 0;
        }
    }

    // Create HMD State object
    HMDState* hmds = HMDState::CreateHMDState(CAPI_pNetClient, netInfo);
    if (!hmds)
    {
        CAPI_pNetClient->Hmd_Release(netInfo.NetId);

        NetClient::GetInstance()->SetLastError("Unable to create HMD state");
        return 0;
    }

    // Reset frame timing so that FrameTimeManager values are properly initialized in AppRendered mode.
    ovrHmd_ResetFrameTiming(hmds->pHmdDesc, 0);

    return hmds->pHmdDesc;
}


OVR_EXPORT ovrBool ovrHmd_AttachToWindow( ovrHmd hmd, void* window,
                                         const ovrRecti* destMirrorRect,
                                         const ovrRecti* sourceRenderTargetRect )
{
    OVR_UNUSED( destMirrorRect );
    OVR_UNUSED( sourceRenderTargetRect );

    if (!CAPI_ovrInitializeCalled)
        return false;

    if (!hmd || !hmd->Handle)
        return false;

#ifdef OVR_OS_WIN32
    HMDState* hmds = (HMDState*)hmd->Handle;
    CAPI_pNetClient->Hmd_AttachToWindow(hmds->GetNetId(), window);
    hmds->pWindow = window;

    Win32::DisplayShim::GetInstance().hWindow = (HWND)window;
#else
    OVR_UNUSED(window);
#endif

    return true;
}

OVR_EXPORT ovrHmd ovrHmd_CreateDebug(ovrHmdType type)
{
    if (!CAPI_ovrInitializeCalled)
        return 0;

    HMDState* hmds = HMDState::CreateHMDState(type);

    return hmds->pHmdDesc;
}

OVR_EXPORT void ovrHmd_Destroy(ovrHmd hmddesc)
{
    if (!hmddesc || !hmddesc->Handle)
        return;
    
    // TBD: Any extra shutdown?
    HMDState* hmds = (HMDState*)hmddesc->Handle;
        
    {   // Thread checker in its own scope, to avoid access after 'delete'.
        // Essentially just checks that no other RenderAPI function is executing.
        ThreadChecker::Scope checkScope(&hmds->RenderAPIThreadChecker, "ovrHmd_Destroy");
    }    

#ifdef OVR_OS_WIN32
    if (hmds->pWindow)
    {
        // ? ok to call
        //CAPI_pNetClient->Hmd_AttachToWindow(hmds->GetNetId(), 0);
        hmds->pWindow = 0;
        Win32::DisplayShim::GetInstance().hWindow = (HWND)0;
    }    
#endif

    delete (HMDState*)hmddesc->Handle;
}


OVR_EXPORT const char* ovrHmd_GetLastError(ovrHmd hmddesc)
{
    if (!CAPI_ovrInitializeCalled)
    {
        return "System initialize not called";
    }

    VirtualHmdId netId = InvalidVirtualHmdId;

    if (hmddesc && hmddesc->Handle)
    {
        HMDState* p = (HMDState*)hmddesc->Handle;
        netId = p->GetNetId();
    }

    return CAPI_pNetClient->Hmd_GetLastError(netId);
}

#define OVR_VERSION_LIBOVR_PFX "libOVR:"

// Returns version string representing libOVR version. Static, so
// string remains valid for app lifespan
OVR_EXPORT const char* ovr_GetVersionString()
{
	static const char* version = OVR_VERSION_LIBOVR_PFX OVR_VERSION_STRING;
    return version + sizeof(OVR_VERSION_LIBOVR_PFX) - 1;
}



//-------------------------------------------------------------------------------------

// Returns capability bits that are enabled at this time; described by ovrHmdCapBits.
// Note that this value is different font ovrHmdDesc::Caps, which describes what
// capabilities are available.
OVR_EXPORT unsigned int ovrHmd_GetEnabledCaps(ovrHmd hmddesc)
{
    HMDState* p = (HMDState*)hmddesc->Handle;
    return p ? p->EnabledHmdCaps : 0;
}

// Modifies capability bits described by ovrHmdCapBits that can be modified,
// such as ovrHmd_LowPersistance.
OVR_EXPORT void ovrHmd_SetEnabledCaps(ovrHmd hmddesc, unsigned int capsBits)
{
    HMDState* p = (HMDState*)hmddesc->Handle;
    if (p)
    {
        p->SetEnabledHmdCaps(capsBits);
    }
}


//-------------------------------------------------------------------------------------
// *** Sensor

// Sensor APIs are separated from Create & Configure for several reasons:
//  - They need custom parameters that control allocation of heavy resources
//    such as Vision tracking, which you don't want to create unless necessary.
//  - A game may want to switch some sensor settings based on user input, 
//    or at lease enable/disable features such as Vision for debugging.
//  - The same or syntactically similar sensor interface is likely to be used if we 
//    introduce controllers.
//
//  - Sensor interface functions are all Thread-safe, unlike the frame/render API
//    functions that have different rules (all frame access functions
//    must be on render thread)

OVR_EXPORT ovrBool ovrHmd_ConfigureTracking(ovrHmd hmddesc, unsigned int supportedCaps,
                                                            unsigned int requiredCaps)
{
    if (hmddesc)
    {
        HMDState* p = (HMDState*)hmddesc->Handle;
        return p->ConfigureTracking(supportedCaps, requiredCaps);
    }

    return 0;
}

OVR_EXPORT void ovrHmd_RecenterPose(ovrHmd hmddesc)
{
    if (hmddesc)
    {
        HMDState* p = (HMDState*)hmddesc->Handle;
        p->TheSensorStateReader.RecenterPose();
    }
}

OVR_EXPORT ovrTrackingState ovrHmd_GetTrackingState(ovrHmd hmddesc, double absTime)
{
    ovrTrackingState result = {0};

    if (hmddesc)
    {
        HMDState* p = (HMDState*)hmddesc->Handle;
        result = p->PredictedTrackingState(absTime);
    }

#ifdef OVR_OS_WIN32
        // Set up display code for Windows
        Win32::DisplayShim::GetInstance().Active = (result.StatusFlags & ovrStatus_HmdConnected) != 0;
#endif

    return result;
}


//-------------------------------------------------------------------------------------
// *** General Setup

// Per HMD -> calculateIdealPixelSize
OVR_EXPORT ovrSizei ovrHmd_GetFovTextureSize(ovrHmd hmddesc, ovrEyeType eye, ovrFovPort fov,
                                             float pixelsPerDisplayPixel)
{
    ovrHmdStruct *  hmd = hmddesc->Handle;
    if (!hmd) return Sizei(0);
    
    HMDState* hmds = (HMDState*)hmd;
    return hmds->RenderState.GetFOVTextureSize(eye, fov, pixelsPerDisplayPixel);
}


//-------------------------------------------------------------------------------------


OVR_EXPORT 
ovrBool ovrHmd_ConfigureRendering( ovrHmd hmddesc,
                                   const ovrRenderAPIConfig* apiConfig,
                                   unsigned int distortionCaps,
                                   const ovrFovPort eyeFovIn[2],
                                   ovrEyeRenderDesc eyeRenderDescOut[2] )
{
    ovrHmdStruct *  hmd = hmddesc->Handle;
    if (!hmd) return 0;
    return ((HMDState*)hmd)->ConfigureRendering(eyeRenderDescOut, eyeFovIn,
                                                apiConfig, distortionCaps);
}



// TBD: MA - Deprecated, need alternative
void ovrHmd_SetVsync(ovrHmd hmddesc, ovrBool vsync)
{
    ovrHmdStruct *  hmd = hmddesc->Handle;
    if (!hmd) return;

    return ((HMDState*)hmd)->TimeManager.SetVsync(vsync? true : false);
}


OVR_EXPORT ovrFrameTiming ovrHmd_BeginFrame(ovrHmd hmddesc, unsigned int frameIndex)
{           
    HMDState* hmds = (HMDState*)hmddesc->Handle;
    if (!hmds)
    {
        ovrFrameTiming f;
        memset(&f, 0, sizeof(f));
        return f;
    }

    // Check: Proper configure and threading state for the call.
    hmds->checkRenderingConfigured("ovrHmd_BeginFrame");
    OVR_ASSERT_LOG(hmds->BeginFrameCalled == false, ("ovrHmd_BeginFrame called multiple times."));
    ThreadChecker::Scope checkScope(&hmds->RenderAPIThreadChecker, "ovrHmd_BeginFrame");
    
    hmds->BeginFrameCalled   = true;
    hmds->BeginFrameThreadId = OVR::GetCurrentThreadId();

    return ovrHmd_BeginFrameTiming(hmddesc, frameIndex);
}


// Renders textures to frame buffer
OVR_EXPORT void ovrHmd_EndFrame(ovrHmd hmddesc,
                                const ovrPosef renderPose[2],
                                const ovrTexture eyeTexture[2])
{
    HMDState* hmds = (HMDState*)hmddesc->Handle;
    if (!hmds) return;

    hmds->SubmitEyeTextures(renderPose, eyeTexture);

    // Debug state checks: Must be in BeginFrame, on the same thread.
    hmds->checkBeginFrameScope("ovrHmd_EndFrame");
    ThreadChecker::Scope checkScope(&hmds->RenderAPIThreadChecker, "ovrHmd_EndFrame");  
    
    hmds->pRenderer->SetLatencyTestColor(hmds->LatencyTestActive ? hmds->LatencyTestDrawColor : NULL);

    ovrHmd_GetLatencyTest2DrawColor(hmddesc, NULL); // We don't actually need to draw color, so send NULL
    
    if (hmds->pRenderer)
    {
        hmds->pRenderer->SaveGraphicsState();

        // See if we need to show the HSWDisplay.
        if (hmds->pHSWDisplay) // Until we know that these are valid, assume any of them can't be.
        {
            ovrHSWDisplayState hswDisplayState;
            hmds->pHSWDisplay->TickState(&hswDisplayState);  // This may internally call HASWarning::Display.

            if (hswDisplayState.Displayed)
            {
                hmds->pHSWDisplay->Render(ovrEye_Left, &eyeTexture[ovrEye_Left]);
                hmds->pHSWDisplay->Render(ovrEye_Right, &eyeTexture[ovrEye_Right]);
            }
        }

        hmds->pRenderer->EndFrame(true);
        hmds->pRenderer->RestoreGraphicsState();
    }
    // Call after present
    ovrHmd_EndFrameTiming(hmddesc);
    
    // Out of BeginFrame
    hmds->BeginFrameThreadId = 0;
    hmds->BeginFrameCalled   = false;
}


// Not exposed as part of public API
OVR_EXPORT void ovrHmd_RegisterPostDistortionCallback(ovrHmd hmddesc, PostDistortionCallback callback)
{
    HMDState* hmds = (HMDState*)hmddesc->Handle;
    if (!hmds) return;

    if (hmds->pRenderer)
    {
        hmds->pRenderer->RegisterPostDistortionCallback(callback);
    }
}



//-------------------------------------------------------------------------------------
// ***** Frame Timing logic


OVR_EXPORT ovrFrameTiming ovrHmd_GetFrameTiming(ovrHmd hmddesc, unsigned int frameIndex)
{
    ovrHmdStruct *  hmd = hmddesc->Handle;
    ovrFrameTiming f;
    memset(&f, 0, sizeof(f));

    HMDState* hmds = (HMDState*)hmd;
    if (hmds)
    {
        FrameTimeManager::Timing frameTiming = hmds->TimeManager.GetFrameTiming(frameIndex);

        f.ThisFrameSeconds       = frameTiming.ThisFrameTime;
        f.NextFrameSeconds       = frameTiming.NextFrameTime;
        f.TimewarpPointSeconds   = frameTiming.TimewarpPointTime;
        f.ScanoutMidpointSeconds = frameTiming.MidpointTime;
        f.EyeScanoutSeconds[0]   = frameTiming.EyeRenderTimes[0];
        f.EyeScanoutSeconds[1]   = frameTiming.EyeRenderTimes[1];

         // Compute DeltaSeconds.
        f.DeltaSeconds = (hmds->LastGetFrameTimeSeconds == 0.0f) ? 0.0f :
                         (float) (f.ThisFrameSeconds - hmds->LastFrameTimeSeconds);    
        hmds->LastGetFrameTimeSeconds = f.ThisFrameSeconds;
        if (f.DeltaSeconds > 1.0f)
            f.DeltaSeconds = 1.0f;
    }
        
    return f;
}

OVR_EXPORT ovrFrameTiming ovrHmd_BeginFrameTiming(ovrHmd hmddesc, unsigned int frameIndex)
{
    ovrHmdStruct *  hmd = hmddesc->Handle;
    ovrFrameTiming f;
    memset(&f, 0, sizeof(f));

    HMDState* hmds = (HMDState*)hmd;
    if (!hmds) return f;

    // Check: Proper state for the call.    
    OVR_ASSERT_LOG(hmds->BeginFrameTimingCalled == false,
                    ("ovrHmd_BeginFrameTiming called multiple times."));    
    hmds->BeginFrameTimingCalled = true;

    double thisFrameTime = hmds->TimeManager.BeginFrame(frameIndex);        

    const FrameTimeManager::Timing &frameTiming = hmds->TimeManager.GetFrameTiming();

    f.ThisFrameSeconds      = thisFrameTime;
    f.NextFrameSeconds      = frameTiming.NextFrameTime;
    f.TimewarpPointSeconds  = frameTiming.TimewarpPointTime;
    f.ScanoutMidpointSeconds= frameTiming.MidpointTime;
    f.EyeScanoutSeconds[0]  = frameTiming.EyeRenderTimes[0];
    f.EyeScanoutSeconds[1]  = frameTiming.EyeRenderTimes[1];

    // Compute DeltaSeconds.
    f.DeltaSeconds = (hmds->LastFrameTimeSeconds == 0.0f) ? 0.0f :
                     (float) (thisFrameTime - hmds->LastFrameTimeSeconds);
    hmds->LastFrameTimeSeconds = thisFrameTime;
    if (f.DeltaSeconds > 1.0f)
        f.DeltaSeconds = 1.0f;

    return f;
}


OVR_EXPORT void ovrHmd_EndFrameTiming(ovrHmd hmddesc)
{
    HMDState* hmds = (HMDState*)hmddesc->Handle;
    if (!hmds) return;

    // Debug state checks: Must be in BeginFrameTiming, on the same thread.
    hmds->checkBeginFrameTimingScope("ovrHmd_EndTiming");
   // MA TBD: Correct chek or not?
   // ThreadChecker::Scope checkScope(&hmds->RenderAPIThreadChecker, "ovrHmd_EndFrame");

    hmds->TimeManager.EndFrame();   
    hmds->BeginFrameTimingCalled = false;

    bool dk2LatencyTest = (hmds->EnabledHmdCaps & ovrHmdCap_DynamicPrediction) != 0;
    if (dk2LatencyTest)
    {
        Util::FrameTimeRecordSet recordset;
        hmds->TheLatencyTestStateReader.GetRecordSet(recordset);
        hmds->TimeManager.UpdateFrameLatencyTrackingAfterEndFrame( hmds->LatencyTest2DrawColor,
            recordset);
    }
}


OVR_EXPORT void ovrHmd_ResetFrameTiming(ovrHmd hmddesc,  unsigned int frameIndex) 
{
    HMDState* hmds = (HMDState*)hmddesc->Handle;
    if (!hmds) return;
    
    hmds->TimeManager.ResetFrameTiming(frameIndex, 
                                       false,
                                       hmds->RenderingConfigured);
    hmds->LastFrameTimeSeconds    = 0.0;
    hmds->LastGetFrameTimeSeconds = 0.0;
}


ovrPosef ovrHmd_GetEyePose(ovrHmd hmd, ovrEyeType eye)
{
    HMDState* hmds = (HMDState*)hmd->Handle;
    if (!hmds) return ovrPosef();    

    // This isn't a great place, but since we removed ovrHmd_BeginEyeRender...
    // Only process latency tester for drawing the left eye (assumes left eye is drawn first)
    if (hmds->pRenderer && eye == 0)
    {
        hmds->LatencyTestActive = hmds->ProcessLatencyTest(hmds->LatencyTestDrawColor);
    }

    hmds->checkBeginFrameTimingScope("ovrHmd_GetEyePose");
    return hmds->TimeManager.GetEyePredictionPose(hmd, eye);
}


OVR_EXPORT void ovrHmd_AddDistortionTimeMeasurement(ovrHmd hmddesc, double distortionTimeSeconds)
{
    if (!hmddesc)
        return;
    HMDState* hmds = (HMDState*)hmddesc->Handle;

    hmds->checkBeginFrameTimingScope("ovrHmd_GetTimewarpEyeMatrices");   
    hmds->TimeManager.AddDistortionTimeMeasurement(distortionTimeSeconds);
}



OVR_EXPORT void ovrHmd_GetEyeTimewarpMatrices(ovrHmd hmddesc, ovrEyeType eye,
                                              ovrPosef renderPose, ovrMatrix4f twmOut[2])
{
    if (!hmddesc)
        return;
    HMDState* hmds = (HMDState*)hmddesc->Handle;

    // Debug checks: BeginFrame was called, on the same thread.
    hmds->checkBeginFrameTimingScope("ovrHmd_GetTimewarpEyeMatrices");   

    hmds->TimeManager.GetTimewarpMatrices(hmddesc, eye, renderPose, twmOut);

    /*
    // MA: Took this out because new latency test approach just sames
    //     the sample times in FrameTimeManager.
    // TODO: if no timewarp, then test latency in begin eye render
    if (eye == 0)
    {        
        hmds->ProcessLatencyTest2(hmds->LatencyTest2DrawColor, -1.0f);
    }
    */
}



OVR_EXPORT ovrEyeRenderDesc ovrHmd_GetRenderDesc(ovrHmd hmddesc,
                                                 ovrEyeType eyeType, ovrFovPort fov)
{
    HMDState* hmds = (HMDState*)hmddesc->Handle;
    ovrEyeRenderDesc erd;
   
    if (!hmds)
    {
        memset(&erd, 0, sizeof(erd));
        return erd;
    }

    return hmds->RenderState.CalcRenderDesc(eyeType, fov);
}



#define OVR_OFFSET_OF(s, field) ((size_t)&((s*)0)->field)



OVR_EXPORT ovrBool ovrHmd_CreateDistortionMesh( ovrHmd hmddesc,
                                                ovrEyeType eyeType, ovrFovPort fov,
                                                unsigned int distortionCaps,
                                                ovrDistortionMesh *meshData)
{
    // The 'internal' function below can be found in CAPI_HMDState.
    // Not ideal, but navigating the convolutions of what compiles
    // where, meant they are in the few places which actually lets these compile.
    // Please relocate (if you wish) to a more meaningful place if you can navigate the compiler gymnastics :)
    return(ovrHmd_CreateDistortionMeshInternal( hmddesc->Handle,
                                                eyeType, fov,
                                                distortionCaps,
                                                meshData,
                                                0));

}



// Frees distortion mesh allocated by ovrHmd_GenerateDistortionMesh. meshData elements
// are set to null and 0s after the call.
OVR_EXPORT void ovrHmd_DestroyDistortionMesh(ovrDistortionMesh* meshData)
{
    if (meshData->pVertexData)
        DistortionMeshDestroy((DistortionMeshVertexData*)meshData->pVertexData,
                              meshData->pIndexData);
    meshData->pVertexData = 0;
    meshData->pIndexData  = 0;
    meshData->VertexCount = 0;
    meshData->IndexCount  = 0;
}



// Computes updated 'uvScaleOffsetOut' to be used with a distortion if render target size or
// viewport changes after the fact. This can be used to adjust render size every frame, if desired.
OVR_EXPORT void ovrHmd_GetRenderScaleAndOffset( ovrFovPort fov,
                                                ovrSizei textureSize, ovrRecti renderViewport,
                                                ovrVector2f uvScaleOffsetOut[2] )
{        
    // Find the mapping from TanAngle space to target NDC space.
    ScaleAndOffset2D  eyeToSourceNDC = CreateNDCScaleAndOffsetFromFov(fov);
    // Find the mapping from TanAngle space to textureUV space.
    ScaleAndOffset2D  eyeToSourceUV  = CreateUVScaleAndOffsetfromNDCScaleandOffset(
                                         eyeToSourceNDC,
                                         renderViewport, textureSize );

    uvScaleOffsetOut[0] = eyeToSourceUV.Scale;
    uvScaleOffsetOut[1] = eyeToSourceUV.Offset;
}


//-------------------------------------------------------------------------------------
// ***** Latency Test interface

OVR_EXPORT ovrBool ovrHmd_GetLatencyTestDrawColor(ovrHmd hmddesc, unsigned char rgbColorOut[3])
{
    HMDState* p = (HMDState*)hmddesc->Handle;
    rgbColorOut[0] = p->LatencyTestDrawColor[0];
    rgbColorOut[1] = p->LatencyTestDrawColor[1];
    rgbColorOut[2] = p->LatencyTestDrawColor[2];
    return p->LatencyTestActive;
}

OVR_EXPORT ovrBool ovrHmd_ProcessLatencyTest(ovrHmd hmddesc, unsigned char rgbColorOut[3])
{
    OVR_UNUSED(hmddesc);
    return NetClient::GetInstance()->LatencyUtil_ProcessInputs(Timer::GetSeconds(), rgbColorOut);
}

OVR_EXPORT const char*  ovrHmd_GetLatencyTestResult(ovrHmd hmddesc)
{
    OVR_UNUSED(hmddesc);
    return NetClient::GetInstance()->LatencyUtil_GetResultsString();
}

OVR_EXPORT ovrBool ovrHmd_GetLatencyTest2DrawColor(ovrHmd hmddesc, unsigned char rgbColorOut[3])
{
    HMDState* hmds = (HMDState*)hmddesc->Handle;
    if (!hmds) return false;

    // TBD: Move directly into renderer
    bool dk2LatencyTest = (hmds->EnabledHmdCaps & ovrHmdCap_DynamicPrediction) != 0;
    if (dk2LatencyTest)
    {
        hmds->TimeManager.GetFrameLatencyTestDrawColor(hmds->LatencyTest2DrawColor);
        if(rgbColorOut != NULL)
        {
            rgbColorOut[0] = hmds->LatencyTest2DrawColor[0];
            rgbColorOut[1] = hmds->LatencyTest2DrawColor[1];
            rgbColorOut[2] = hmds->LatencyTest2DrawColor[2];
        }

        if(hmds->pRenderer != NULL)
            hmds->pRenderer->SetLatencyTest2Color(hmds->LatencyTest2DrawColor);
    }
    else
    {
        if(hmds->pRenderer != NULL)
            hmds->pRenderer->SetLatencyTest2Color(NULL);
    }

    return dk2LatencyTest;
}


OVR_EXPORT double ovrHmd_GetMeasuredLatencyTest2(ovrHmd hmddesc)
{
    HMDState* p = (HMDState*)hmddesc->Handle;

    // MA Test
    float latencies[3];
    p->TimeManager.GetLatencyTimings(latencies);
    return latencies[2];
  //  return p->LatencyUtil2.GetMeasuredLatency();
}


//-------------------------------------------------------------------------------------
// ***** Health and Safety Warning Display interface
//

OVR_EXPORT void ovrHmd_GetHSWDisplayState(ovrHmd hmd, ovrHSWDisplayState *hswDisplayState)
{
    OVR::CAPI::HMDState* pHMDState = (OVR::CAPI::HMDState*)hmd->Handle;

    if (pHMDState)
    {
        OVR::CAPI::HSWDisplay* pHSWDisplay = pHMDState->pHSWDisplay;

        if(pHSWDisplay)
            pHSWDisplay->TickState(hswDisplayState); // This may internally call HSWDisplay::Display.
    }
}

OVR_EXPORT ovrBool ovrHmd_DismissHSWDisplay(ovrHmd hmd)
{
    OVR::CAPI::HMDState* pHMDState = (OVR::CAPI::HMDState*)hmd->Handle;

    if (pHMDState)
    {
        OVR::CAPI::HSWDisplay* pHSWDisplay = pHMDState->pHSWDisplay;

        if(pHSWDisplay)
            return (pHSWDisplay->Dismiss() ? 1 : 0);
    }

    return false;
}


// -----------------------------------------------------------------------------------
// ***** Property Access
OVR_EXPORT ovrBool ovrHmd_GetBool(ovrHmd hmddesc,
                                  const char* propertyName,
                                  ovrBool defaultVal)
{
    OVR_ASSERT(propertyName);
    if (hmddesc)
    {
        HMDState* hmds = (HMDState*)hmddesc->Handle;
        OVR_ASSERT(hmds);
        if (hmds)
        {
            return hmds->getBoolValue(propertyName, (defaultVal != 0));
        }
    }

    return NetClient::GetInstance()->GetBoolValue(InvalidVirtualHmdId, propertyName, (defaultVal != 0)) ? 1 : 0;
}

OVR_EXPORT ovrBool ovrHmd_SetBool(ovrHmd hmddesc,
                                  const char* propertyName,
                                  ovrBool value)
{
    OVR_ASSERT(propertyName);
    if (hmddesc)
    {
        HMDState* hmds = (HMDState*)hmddesc->Handle;
        OVR_ASSERT(hmds);
        if (hmds)
        {
            return hmds->setBoolValue(propertyName, value != 0) ? 1 : 0;
        }
    }

    return NetClient::GetInstance()->SetBoolValue(InvalidVirtualHmdId, propertyName, (value != 0)) ? 1 : 0;
}

OVR_EXPORT int ovrHmd_GetInt(ovrHmd hmddesc,
                             const char* propertyName,
                             int defaultVal)
{
    OVR_ASSERT(propertyName);
    if (hmddesc)
    {
        HMDState* hmds = (HMDState*)hmddesc->Handle;
        OVR_ASSERT(hmds);
        if (hmds)
        {
            return hmds->getIntValue(propertyName, defaultVal);
        }
    }

    return NetClient::GetInstance()->GetIntValue(InvalidVirtualHmdId, propertyName, defaultVal);
}

OVR_EXPORT ovrBool ovrHmd_SetInt(ovrHmd hmddesc,
                                 const char* propertyName,
                                 int value)
{
    OVR_ASSERT(propertyName);
    if (hmddesc)
    {
        HMDState* hmds = (HMDState*)hmddesc->Handle;
        OVR_ASSERT(hmds);
        if (hmds)
        {
            return hmds->setIntValue(propertyName, value) ? 1 : 0;
        }
    }

    return NetClient::GetInstance()->SetIntValue(InvalidVirtualHmdId, propertyName, value) ? 1 : 0;
}

OVR_EXPORT float ovrHmd_GetFloat(ovrHmd hmddesc,
                                 const char* propertyName,
                                 float defaultVal)
{
    OVR_ASSERT(propertyName);
    if (hmddesc)
    {
        HMDState* hmds = (HMDState*)hmddesc->Handle;
        OVR_ASSERT(hmds);
        if (hmds)
        {
            return hmds->getFloatValue(propertyName, defaultVal);
        }
    }

    return (float)NetClient::GetInstance()->GetNumberValue(InvalidVirtualHmdId, propertyName, defaultVal);
}

OVR_EXPORT ovrBool ovrHmd_SetFloat(ovrHmd hmddesc,
                                   const char* propertyName,
                                   float value)
{
    OVR_ASSERT(propertyName);
    if (hmddesc)
    {
        HMDState* hmds = (HMDState*)hmddesc->Handle;
        OVR_ASSERT(hmds);
        if (hmds)
        {
            return hmds->setFloatValue(propertyName, value) ? 1 : 0;
        }
    }

    return NetClient::GetInstance()->SetNumberValue(InvalidVirtualHmdId, propertyName, value) ? 1 : 0;
}

OVR_EXPORT unsigned int ovrHmd_GetFloatArray(ovrHmd hmddesc,
                                             const char* propertyName,
                                             float values[],
                                             unsigned int arraySize)
{
    OVR_ASSERT(propertyName);
    OVR_ASSERT(hmddesc);
    if (hmddesc)
    {
        HMDState* hmds = (HMDState*)hmddesc->Handle;
        OVR_ASSERT(hmds);
        if (hmds)
        {
            return hmds->getFloatArray(propertyName, values, arraySize);
        }
    }

    // FIXME: Currently it takes a few lines of code to do this, so just not supported for now.
    return 0;
}

// Modify float[] property; false if property doesn't exist or is readonly.
OVR_EXPORT ovrBool ovrHmd_SetFloatArray(ovrHmd hmddesc,
                                        const char* propertyName,
                                        float values[],
                                        unsigned int arraySize)
{
    OVR_ASSERT(propertyName);
    OVR_ASSERT(hmddesc);
    if (hmddesc)
    {
        HMDState* hmds = (HMDState*)hmddesc->Handle;
        OVR_ASSERT(hmds);
        if (hmds)
        {
            return hmds->setFloatArray(propertyName, values, arraySize) ? 1 : 0;
        }
    }

    // FIXME: Currently it takes a few lines of code to do this, so just not supported for now.
    return 0;
}

OVR_EXPORT const char* ovrHmd_GetString(ovrHmd hmddesc,
                                        const char* propertyName,
                                        const char* defaultVal)
{
    OVR_ASSERT(propertyName);
    if (hmddesc)
    {
        HMDState* hmds = (HMDState*)hmddesc->Handle;
        if (hmds)
        {
            return hmds->getString(propertyName, defaultVal);
        }
    }

    return NetClient::GetInstance()->GetStringValue(InvalidVirtualHmdId, propertyName, defaultVal);
}
 
OVR_EXPORT ovrBool ovrHmd_SetString(ovrHmd hmddesc,
                                    const char* propertyName,
                                    const char* value)
{
    OVR_ASSERT(propertyName);
    if (hmddesc)
    {
        HMDState* hmds = (HMDState*)hmddesc->Handle;
        if (hmds)
        {
            return hmds->setString(propertyName, value) ? 1 : 0;
        }
    }

    return NetClient::GetInstance()->SetStringValue(InvalidVirtualHmdId, propertyName, value) ? 1 : 0;
}


#ifdef __cplusplus 
} // extern "C"
#endif
