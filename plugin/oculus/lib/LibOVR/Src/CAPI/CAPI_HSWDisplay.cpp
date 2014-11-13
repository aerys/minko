/************************************************************************************

Filename    :   CAPI_HSWDisplay.cpp
Content     :   Implements Health and Safety Warning system.
Created     :   July 3, 2014
Authors     :   Paul Pedriana

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

#include "CAPI_HSWDisplay.h"
#include "CAPI_HMDState.h"
#include "../Kernel/OVR_Log.h"
#include "../Kernel/OVR_String.h"
#include "Textures/healthAndSafety.tga.h" // TGA file as a C array declaration.


//-------------------------------------------------------------------------------------
// ***** HSWDISPLAY_DEBUGGING
//
// Defined as 0 or 1. Enables debugging features of this module.

#if !defined(HSWDISPLAY_DEBUGGING)
    #if defined(AUTHOR_PPEDRIANA)
        #define HSWDISPLAY_DEBUGGING 1
    #else
        #define HSWDISPLAY_DEBUGGING 0
    #endif
#endif

#if HSWDISPLAY_DEBUGGING
    OVR_DISABLE_ALL_MSVC_WARNINGS()
    #include <winsock2.h>
    #include <Windows.h>
    OVR_RESTORE_ALL_MSVC_WARNINGS()
#endif

OVR_DISABLE_MSVC_WARNING(4996) // "This function or variable may be unsafe..."


//-------------------------------------------------------------------------------------
// ***** HSWDISPLAY_DEFAULT_ENABLED
//
// Defined as 0 or 1. 1 is default. If 0 then by default HSWDisplay is disabled.
// Developers can set it to 0 to disable HSW display.
//
#if !defined(HSWDISPLAY_DEFAULT_ENABLED)
    #define HSWDISPLAY_DEFAULT_ENABLED 1
#endif



//-------------------------------------------------------------------------------------
// ***** Experimental C API functions
//

extern "C"
{
    OVR_EXPORT void ovrhmd_EnableHSWDisplaySDKRender(ovrHmd hmd, ovrBool enabled)
    {
        OVR::CAPI::HMDState* pHMDState = (OVR::CAPI::HMDState*)hmd->Handle;

	    if (pHMDState)
	    {
            OVR::CAPI::HSWDisplay* pHSWDisplay = pHMDState->pHSWDisplay;

            if(pHSWDisplay)
                pHSWDisplay->EnableRender((enabled == 0) ? false : true);
        }
    }
}



//-------------------------------------------------------------------------------------
// ***** HSWDisplay implementation
//

namespace OVR { namespace CAPI {


static const time_t HSWDisplayTimeNever = (time_t)0; // Constant which denotes the time of "never", as in the display has never been shown yet.

#define HSWDISPLAY_POLL_INTERVAL            0.400 // Seconds between polling for whether the display should be shown.
#define OVR_KEY_HSWDISPLAYLASTDISPLAYEDTIME "HASWLastDisplayedTime"


#if defined(OVR_BUILD_DEBUG)
    #define HSWDISPLAY_FIRST_DISMISSAL_TIME    4     // Earliest time in seconds until the user can dismiss the display.
    #define HSWDISPLAY_REGULAR_DISMISSAL_TIME  2
#else
    #define HSWDISPLAY_FIRST_DISMISSAL_TIME   15
    #define HSWDISPLAY_REGULAR_DISMISSAL_TIME  6
#endif


HSWDisplay::HSWDisplay(ovrRenderAPIType renderAPIType, ovrHmd hmd, const HMDRenderState& hmdRenderState)
  : Enabled(HSWDISPLAY_DEFAULT_ENABLED ? true : false),
    Displayed(false),
    SDKRendered(false),
    DismissRequested(false),
    RenderEnabled(true),
    StartTime(0.0),
    DismissibleTime(0.0),
    LastPollTime(0.0),
    HMD(hmd), 
    HMDMounted(false),
    HMDNewlyMounted(false),
    RenderAPIType(renderAPIType), 
    RenderState(hmdRenderState)
{
}


HSWDisplay::~HSWDisplay()
{
    // To consider: assert that we are already shut down.
    HSWDisplay::Shutdown();
}


void HSWDisplay::Enable(bool enable)
{
    Enabled = enable;

    if(!enable && Displayed) // If it's visible but should not be...
        Dismiss();
}


void HSWDisplay::EnableRender(bool enable)
{
    RenderEnabled = enable;
}


void HSWDisplay::Display()
{
    HSWDISPLAY_LOG(("[HSWDisplay] Display()"));

    DisplayInternal();

    HMDNewlyMounted = false;
    Displayed = true;
    SDKRendered = RenderEnabled;
    StartTime = ovr_GetTimeInSeconds();

    const time_t lastDisplayedTime = HSWDisplay::GetCurrentProfileLastHSWTime();
    DismissibleTime = StartTime + ((lastDisplayedTime == HSWDisplayTimeNever) ? HSWDISPLAY_FIRST_DISMISSAL_TIME : HSWDISPLAY_REGULAR_DISMISSAL_TIME);

    SetCurrentProfileLastHSWTime(time(NULL));
}


bool HSWDisplay::IsDisplayViewable() const
{
    // This function is called IsDisplayViewable, but currently it refers only to whether the 
    // HMD is mounted on the user's head. 

    return HMDMounted;
}


bool HSWDisplay::Dismiss()
{
    #if HSWDISPLAY_DEBUGGING
        if(GetKeyState(VK_SCROLL) & 0x0001) // If the scroll lock key is toggled on...
            return false;                   // Make it so that the display doesn't dismiss, so we can debug this.
    #endif

    // If dismissal is not requested yet, mark it as such.
    bool newlyRequested = false;

    if(!DismissRequested)
    {
        DismissRequested = true;
        newlyRequested = true;
    }

    // If displayed and time has elapsed, do the dismissal.
    OVR_ASSERT(DismissibleTime <= (ovr_GetTimeInSeconds() + HSWDISPLAY_FIRST_DISMISSAL_TIME)); // Make sure the dismissal time is sane.
    if (Displayed && (ovr_GetTimeInSeconds() >= DismissibleTime))
    {
        DismissInternal();
        Displayed = false;
        DismissRequested = false;
        SDKRendered = false;
        return true;
    }

    if(newlyRequested)
        { HSWDISPLAY_LOG(("[HSWDisplay] Dismiss(): Not permitted yet. Queued for timeout in %.1f seconds.", DismissibleTime - ovr_GetTimeInSeconds())); }

    return false; // Cannot dismiss yet.
}


bool HSWDisplay::TickState(ovrHSWDisplayState *hswDisplayState)
{
    bool newlyDisplayed = false;
    const double currentTime = ovr_GetTimeInSeconds();

    // See if we need to be currently displayed. By design we automatically display but don't automatically dismiss.
    if (Displayed)
    {
        if (DismissRequested) // If dismiss was previously requested, see if it can be executed.
            Dismiss();

        if (Displayed) // If not already dismissed above...
        {
            // We currently have the debug behavior that we permit dismiss very soon after launch.
            #if defined(OVR_BUILD_DEBUG)
                if(currentTime >= (StartTime + 2))
                {
                    DismissibleTime = StartTime;
                    //Dismiss();
                }
            #endif
        }

        if (Displayed) // If not already dismissed above...
        {
            const ovrTrackingState ts = ((OVR::CAPI::HMDState*)HMD->Handle)->PredictedTrackingState(currentTime);

            if (ts.StatusFlags & ovrStatus_OrientationTracked) // If the Accelerometer data is valid...
            {
				const OVR::Vector3f v(ts.HeadPose.LinearAcceleration.x, ts.HeadPose.LinearAcceleration.y, ts.HeadPose.LinearAcceleration.z);

                const float minTapMagnitude = 350.0f; // Empirically determined by some testing.

                if (v.LengthSq() > minTapMagnitude)
                    Dismiss(); // This will do nothing if the display is not present.
            }
        }
    }
    else
    {
        if (Enabled && (currentTime >= (LastPollTime + HSWDISPLAY_POLL_INTERVAL)))
        {
            LastPollTime = currentTime;

            // We need to display if any of the following are true:
            //     - The application is just started in Event Mode while the HMD is mounted (warning display would be viewable) and this app was not spawned from a launcher.
            //     - The current user has never seen the display yet while the HMD is mounted (warning display would be viewable).
            //     - The HMD is newly mounted (or the warning display is otherwise newly viewable).
            //     - The warning display hasn't shown in 24 hours (need to verify this as a requirement).
            // Event Mode refers to when the app is being run in a public demo event such as a trade show.

            OVR::CAPI::HMDState* pHMDState = (OVR::CAPI::HMDState*)HMD->Handle;

            if(pHMDState)
            {
                const time_t lastDisplayedTime = HSWDisplay::GetCurrentProfileLastHSWTime();

                // We currently unilaterally set HMDMounted to true because we don't yet have the ability to detect this. To do: Implement this when possible.
                const bool previouslyMounted = HMDMounted;
                HMDMounted = true;
                HMDNewlyMounted = (!previouslyMounted && HMDMounted); // We set this back to false in the Display function or if the HMD is unmounted before then.

                if((lastDisplayedTime == HSWDisplayTimeNever) || HMDNewlyMounted)
                {
                    if(IsDisplayViewable()) // If the HMD is mounted and otherwise being viewed by the user...
                    {
                        Display();
                        newlyDisplayed = true;
                    }
                }
            }
        }
    }

    if(hswDisplayState)
        GetState(hswDisplayState);

    return newlyDisplayed;
}


void HSWDisplay::GetState(ovrHSWDisplayState *hswDisplayState) const
{
    // Return the state to the caller.
    OVR_ASSERT(hswDisplayState != NULL);
    hswDisplayState->Displayed = Displayed;
    hswDisplayState->StartTime = StartTime;
    hswDisplayState->DismissibleTime = DismissibleTime;
}


void HSWDisplay::Render(ovrEyeType eye, const ovrTexture* eyeTexture)
{
    SDKRendered = true;
    RenderInternal(eye, eyeTexture);
}

// Persist the HSW settings on the server, since it needs to be synchronized across all applications.
// Note that the profile manager singleton cannot be used for this task because it overwrites the global
// settings for which the rift config tool is supposed to be authoritative.  That also would step on the
// settings generated by other rift apps.  The server settings, however, are synchronized for all apps
// and so are appropriate for this task.
static String getHSWTimeKey(const char* userName)
{
    String keyName = "server:";
    keyName += OVR_KEY_HSWDISPLAYLASTDISPLAYEDTIME;
    keyName += ":";
    if (userName)
    {
        keyName += userName;
    }
    return keyName;
}

// Returns HSWDisplayTimeNever (0) if there is no profile or this is the first time we are seeing this profile.
time_t HSWDisplay::GetCurrentProfileLastHSWTime() const
{
    // We store the timeout value in HMDState's pProfile.
    HMDState* pHMDState = (HMDState*)HMD->Handle;

    if (pHMDState)
    {
        const char* profileName = pHMDState->pProfile ? pHMDState->pProfile->GetValue(OVR_KEY_USER) : NULL;

        if (profileName)
        {
            if (LastProfileName == profileName)
            {
                return LastHSWTime;
            }

            LastProfileName = profileName;
            String timeKey = getHSWTimeKey(profileName);
            int lastTime = pHMDState->getIntValue(timeKey.ToCStr(), (int)HSWDisplayTimeNever);

            LastHSWTime = lastTime;
            return lastTime;
        }
    }

    return HSWDisplayTimeNever;
}

void HSWDisplay::SetCurrentProfileLastHSWTime(time_t t)
{
    // The timeout value is stored in HMDState's pProfile.
    HMDState* pHMDState = (HMDState*)HMD->Handle;

    if (pHMDState)
    {
        const char* profileName = pHMDState->pProfile ? pHMDState->pProfile->GetValue(OVR_KEY_USER) : NULL;

        if (profileName)
        {
            LastProfileName = profileName;
            LastHSWTime = (int)t;

            String timeKey = getHSWTimeKey(profileName);
            pHMDState->setIntValue(timeKey.ToCStr(), (int)t);
        }
    }
}


// Generates an appropriate stereo ortho projection matrix.
void HSWDisplay::GetOrthoProjection(const HMDRenderState& RenderState, Matrix4f OrthoProjection[2])
{
    Matrix4f perspectiveProjection[2];
    perspectiveProjection[0] = ovrMatrix4f_Projection(RenderState.EyeRenderDesc[0].Fov, 0.01f, 10000.f, true);
    perspectiveProjection[1] = ovrMatrix4f_Projection(RenderState.EyeRenderDesc[1].Fov, 0.01f, 10000.f, true);

    const float    orthoDistance = HSWDISPLAY_DISTANCE; // This is meters from the camera (viewer) that we place the ortho plane.
    const Vector2f orthoScale0   = Vector2f(1.f) / Vector2f(RenderState.EyeRenderDesc[0].PixelsPerTanAngleAtCenter);
    const Vector2f orthoScale1   = Vector2f(1.f) / Vector2f(RenderState.EyeRenderDesc[1].PixelsPerTanAngleAtCenter);
    
    OrthoProjection[0] = ovrMatrix4f_OrthoSubProjection(perspectiveProjection[0], orthoScale0, orthoDistance, RenderState.EyeRenderDesc[0].ViewAdjust.x);
    OrthoProjection[1] = ovrMatrix4f_OrthoSubProjection(perspectiveProjection[1], orthoScale1, orthoDistance, RenderState.EyeRenderDesc[1].ViewAdjust.x);
}


const uint8_t* HSWDisplay::GetDefaultTexture(size_t& TextureSize)
{
    TextureSize = sizeof(healthAndSafety_tga);
    return healthAndSafety_tga;
}



}} // namespace OVR::CAPI




//-------------------------------------------------------------------------------------
// ***** HSWDisplay factory
//

#if defined (OVR_OS_WIN32)
    #define OVR_D3D_VERSION 9
    #include "D3D1X/CAPI_D3D9_HSWDisplay.h"
    #undef  OVR_D3D_VERSION

    #define OVR_D3D_VERSION 10
    #include "D3D1X/CAPI_D3D10_HSWDisplay.h"
    #undef  OVR_D3D_VERSION

    #define OVR_D3D_VERSION 11
    #include "D3D1X/CAPI_D3D11_HSWDisplay.h"
    #undef  OVR_D3D_VERSION
#endif

#include "GL/CAPI_GL_HSWDisplay.h"


OVR::CAPI::HSWDisplay* OVR::CAPI::HSWDisplay::Factory(ovrRenderAPIType apiType, ovrHmd hmd, const OVR::CAPI::HMDRenderState& renderState)
{
    OVR::CAPI::HSWDisplay* pHSWDisplay = NULL;

    switch (apiType)
    {
        case ovrRenderAPI_None:
            pHSWDisplay = new OVR::CAPI::HSWDisplay(apiType, hmd, renderState);
            break;

        case ovrRenderAPI_OpenGL:
            pHSWDisplay = new OVR::CAPI::GL::HSWDisplay(apiType, hmd, renderState);
            break;

    #if defined(OVR_OS_WIN32)
        case ovrRenderAPI_D3D9:
            pHSWDisplay = new OVR::CAPI::D3D9::HSWDisplay(apiType, hmd, renderState);
            break;

        case ovrRenderAPI_D3D10:
            pHSWDisplay = new OVR::CAPI::D3D10::HSWDisplay(apiType, hmd, renderState);
            break;

        case ovrRenderAPI_D3D11:
            pHSWDisplay = new OVR::CAPI::D3D11::HSWDisplay(apiType, hmd, renderState);
            break;
    #else
        case ovrRenderAPI_D3D9:
        case ovrRenderAPI_D3D10:
        case ovrRenderAPI_D3D11: // Fall through
    #endif

        // Handle unsupported cases.
        case ovrRenderAPI_Android_GLES:
        case ovrRenderAPI_Count: // This is not actually a type.
        default:
            break;
    }

    return pHSWDisplay;
}





