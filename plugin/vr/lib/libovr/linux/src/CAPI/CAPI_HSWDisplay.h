/************************************************************************************

Filename    :   CAPI_HSWDisplay.h
Content     :   Implements Health and Safety Warning system.
Created     :   July 3, 2014
Authors     :   Paul Pedriana

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

#ifndef OVR_CAPI_HSWDisplay_h
#define OVR_CAPI_HSWDisplay_h

#include "../OVR_CAPI.h"
#include "CAPI_HMDRenderState.h"
#include <time.h>



//-------------------------------------------------------------------------------------
// ***** HSWDISPLAY_LOG
//
// Debug log wrapper.

#if !defined(HSWDISPLAY_LOG_ENABLED)
    #ifdef OVR_BUILD_DEBUG
        #define HSWDISPLAY_LOG_ENABLED 1
    #else
        #define HSWDISPLAY_LOG_ENABLED 0
    #endif
#endif

#if HSWDISPLAY_LOG_ENABLED
    #define HSWDISPLAY_LOG(...) OVR_DEBUG_LOG(__VA_ARGS__)
#else
    #define HSWDISPLAY_LOG(...)
#endif


//-------------------------------------------------------------------------------------
// ***** HSWDISPLAY_DISTANCE
//
// Floating point value in the range of ~0.75 to ~3.0 which controls the distance 
// (in meters) of the display from the viewer.

#ifndef HSWDISPLAY_DISTANCE
    #define HSWDISPLAY_DISTANCE 1.5f
#endif


//-------------------------------------------------------------------------------------
// ***** HSWDISPLAY_SCALE
//
// Floating point value in the range of ~0.1 to ~2.0 which controls the size scale of the 
// SDK-rendered HSW display. The value is an arbitrary relative value, though this may 
// change in future SDK versions.

#ifndef HSWDISPLAY_SCALE
    #define HSWDISPLAY_SCALE 0.92f
#endif




//-------------------------------------------------------------------------------------
// ***** Experimental C API functions
//
// These are currently not formally supported and may be promoted to the formal C API
// or may be removed in the future.

extern "C"
{
    // Normally if an application uses SDK-based distortion rendering 
    // (ovrHmd_BeginFrame / ovrHmd_EndFrame) then the SDK also takes care of 
    // drawing the health and safety warning. If an application is using 
    // SDK-based rendering but wants to draw the warning display itself, 
    // it call this function with enabled set to false.
    OVR_EXPORT void ovrhmd_EnableHSWDisplaySDKRender(ovrHmd hmd, ovrBool enabled);
}


namespace OVR { namespace CAPI {


//-------------------------------------------------------------------------------------
// ***** CAPI::HSWDisplay
//
// Note: This will be renamed to HSWDisplay in the future.
//
// Implements the logic for the Health and Safety (HAS) warning display. Primarily this
// is two things: providing information about whether the warning needs to be currently
// displayed, and implementing the display itself. 
//
// An HSWDisplay is associated 1:1 with an HMD. There can be at most one HSWDisplay 
// being displayed on an HMD at a time. If a warning needs to be displayed while an  
// existing one is present, it replaces the existing one. 
//
// Notes
//    Warnings are displayed per HMD (head mounted display).
//    The app can have multiple HMDs.
//    There can be multiple users of a given HMD over time, with each identified by a different user profile.
//    There can be multiple apps using HMDs.
//
//    Shows upon first entering a VR application (or VR mode in an application) when in Event Mode (e.g. trade show).
//    Shows upon each wearing of the HMD.
//    If the user profile is switched while display is active, the display must restart.
//    Doesn't show in app when app was started by a launcher app.
//
//    First display ever (per profile): 15 seconds until the display can be dismissed.
//    Subsequent displays: 6 seconds until the display can be dismissed. Per profile.
//    Dismissing occurs via HMD tap, designated keypress, gaze detection on OK button for N seconds, 
//        and possibly via an input gesture in the future.
//
//    If the warning fades out upon completion, the fade out should begin after the full display time has elapsed, 
//        but it needs to avoid interfering (e.g. obscuring) with the application. This likely means the application 
//        would need to put in a couple seconds delay to allow the fade to complete.
//    Ideally we'd handle the case of a user switching HMDs and not needing to see the warning again.

class HSWDisplay : public RefCountBase<HSWDisplay>
{
public:
    HSWDisplay(ovrRenderAPIType api, ovrHmd hmd, const HMDRenderState& renderState);

    virtual ~HSWDisplay();

    // Must be called after construction and before use.
    virtual bool Initialize(const ovrRenderAPIConfig*)
        { return true; }

    // Must be called before destruction.
    virtual void Shutdown() {}

    // Enables or disables the HSW display system. It may be disabled only for development uses.
    // It is enabled by default. 
    void Enable(bool enable);

    // Enables or disables our internal rendering when Render is called. If set to false then the 
    // application is expected to implement drawing of the display when Displayed is true.
    // It is enabled by default. 
    void EnableRender(bool enable);

    // Triggers a display of the HSW display for the associated HMD. Restarts the display if 
    // the warning is already being displayed. 
    void Display();

    // This function should be called per HMD every frame in order to give this class processing time. 
    // Writes the new state to newHSWDisplayState if it's non-NULL.
    // The graphicsContext argument indicates if the Tick is occurring within a graphics context and
    // thus if graphics operations are allowed during the TickState call.
    // Returns true if the new state results in a required warning display (ovrHSWDisplayState::Displayed became true).
    bool TickState(ovrHSWDisplayState *newHSWDisplayState = NULL, bool graphicsContext = false);

    // Gets the current state of the HSW display. 
    // Corresponds to ovrhmd_GetHSWDisplayState.
    void GetState(ovrHSWDisplayState *hasWarningState) const;

    // Removes the HSW display display if the minimum dismissal time has occurred. 
    // Returns true if the warning display could be dissmissed or was not displayed at the time of the call.
    // Corresponds to ovrhmd_DismissHSWDisplay.
    bool Dismiss();

    // Returns true if the HMD appears to be currently mounted and in a state that a 
    // warning display would be viewable.
    bool IsDisplayViewable() const;

    // Draws the warning to the eye texture(s). This must be done at the end of a 
    // frame but prior to executing the distortion rendering of the eye textures. 
    virtual void Render(ovrEyeType, const ovrTexture*);

    // Resets the current profile's HAS settings (e.g. to act as if the user has never seen the HSW display before).
    void ResetProfileData();

    // Returns the ovrRenderAPIType. This is essentially the same as RTTI, as it's indicating what subclass
    // is being used for this.
    ovrRenderAPIType GetRenderAPIType() const // e.g. ovrRenderAPI_D3D11
        { return RenderAPIType; }

    // Returns the required HSW display text for the current profile's locale. 
    // Useful for implementing custom warning displays. Returns the required strlen 
    // of the text, and thus success is indicated by a return value < strCapacity.
    // size_t GetText(char *str, size_t strCapacity);

    // Creates and constructs an instance of an HSWDisplay subclass based on the API type.
    static HSWDisplay* Factory(ovrRenderAPIType apiType, ovrHmd hmd, const HMDRenderState& renderState);

private:
    OVR_NON_COPYABLE(HSWDisplay)

protected:
    virtual void DisplayInternal() {}
    virtual void DismissInternal() {}
    virtual void RenderInternal(ovrEyeType, const ovrTexture*) {}
    virtual void UnloadGraphics() {}
    virtual void LoadGraphics() {}

    // Profile functionality
    time_t GetCurrentProfileLastHSWTime() const;
    void   SetCurrentProfileLastHSWTime(time_t t);

    // Generates an appropriate stereo ortho projection matrix.
    static void GetOrthoProjection(const HMDRenderState& RenderState, Matrix4f OrthoProjection[2]);

    // Returns the default HSW display texture data.
    static const uint8_t* GetDefaultTexture(size_t& TextureSize);

protected:
    bool                   Enabled;                 // If true then the HSW display system is enabled. True by default.
    bool                   Displayed;               // If true then the warning is currently visible and the following variables have meaning. Else there is no warning being displayed for this application on the given HMD.
    bool                   SDKRendered;             // If true then the display is being rendered by the SDK as opposed to the application.
    bool                   DismissRequested;        // If true then the warning has been requested to be hidden.
    bool                   RenderEnabled;           // If true then we handle rendering when Render is called. Else we skip it and assume the application is otherwise handling it itself.
    bool                   UnloadGraphicsRequested; // If true then an unload of graphics was requested. This acts as a message from the main thread to the drawing thread so that the unload happens in the expected thread.
    double                 StartTime;               // Absolute time when the warning was first displayed. See ovr_GetTimeInSeconds().
    double                 DismissibleTime;         // Absolute time when the warning can be dismissed.
    double                 LastPollTime;            // Used to prevent us from polling the required display state every frame but rather more like every 200 milliseconds.
    const ovrHmd           HMD;                     // The HMDState this HSWDisplay instance corresponds to.
    mutable bool           HMDMounted;              // True if the HMD was most recently found to be mounted. We need this in order to maintain HMDNewlyMounted.
    mutable bool           HMDNewlyMounted;         // True if HMDMounted has transitioned from false to true. We need this in order to tell if the HMD was recently mounted so we can display the HSW display.
    const ovrRenderAPIType RenderAPIType;           // e.g. ovrRenderAPI_D3D11
    const HMDRenderState&  RenderState;             // Information about the rendering setup.

    // Settings cache
    mutable String         LastProfileName;
    mutable int            LastHSWTime;

}; // class HSWDisplay



}} // namespace OVR::CAPI


#endif // OVR_CAPI_HSWDisplay_h
