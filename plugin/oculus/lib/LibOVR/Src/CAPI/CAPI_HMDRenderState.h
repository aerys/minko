/************************************************************************************

Filename    :   CAPI_HMDRenderState.h
Content     :   Combines all of the rendering state associated with the HMD
Created     :   February 2, 2014
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

#ifndef OVR_CAPI_HMDRenderState_h
#define OVR_CAPI_HMDRenderState_h

#include "../OVR_CAPI.h"
#include "../Kernel/OVR_Math.h"
#include "../Util/Util_Render_Stereo.h"
#include "../Service/Service_NetSessionCommon.h"

namespace OVR { namespace CAPI {

using namespace OVR::Util::Render;


//-------------------------------------------------------------------------------------
// ***** HMDRenderState

// Combines all of the rendering setup information about one HMD.
// This structure only ever exists inside HMDState, but this 
// declaration is in a separate file to reduce #include dependencies.
// All actual lifetime and update control is done by the surrounding HMDState.
struct HMDRenderState
{
    // Utility query functions.
    ovrHmdDesc          GetDesc() const;
    ovrSizei            GetFOVTextureSize(int eye, ovrFovPort fov, float pixelsPerDisplayPixel) const;
    ovrEyeRenderDesc    CalcRenderDesc(ovrEyeType eyeType, const ovrFovPort& fov) const;

    HMDInfo                 OurHMDInfo;

    HmdRenderInfo           RenderInfo;
    DistortionRenderDesc    Distortion[2];
    ovrEyeRenderDesc        EyeRenderDesc[2]; 

    // Clear color used for distortion
    float                   ClearColor[4];

    // Pose at which last time the eye was rendered, as submitted by EndEyeRender.
    ovrPosef                EyeRenderPoses[2];

    // Capabilities passed to Configure.
    unsigned                EnabledHmdCaps;
    unsigned                DistortionCaps;
};


}} // namespace OVR::CAPI

#endif // OVR_CAPI_HMDState_h
