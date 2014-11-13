/************************************************************************************

Filename    :   OVR_CAPI_HMDRenderState.cpp
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

#include "CAPI_HMDRenderState.h"

namespace OVR { namespace CAPI {


//-------------------------------------------------------------------------------------
// ***** HMDRenderState

ovrHmdDesc HMDRenderState::GetDesc() const
{
    ovrHmdDesc d;
    memset(&d, 0, sizeof(d));
    
    d.Type = ovrHmd_Other;
     
    d.ProductName       = OurHMDInfo.ProductName;    
    d.Manufacturer      = OurHMDInfo.Manufacturer;
    d.Resolution.w      = OurHMDInfo.ResolutionInPixels.w;
    d.Resolution.h      = OurHMDInfo.ResolutionInPixels.h;
    d.WindowsPos.x      = OurHMDInfo.DesktopX;
    d.WindowsPos.y      = OurHMDInfo.DesktopY;
    d.DisplayDeviceName = OurHMDInfo.DisplayDeviceName;
    d.DisplayId         = OurHMDInfo.DisplayId;
    d.VendorId          = (short)OurHMDInfo.VendorId;
    d.ProductId         = (short)OurHMDInfo.ProductId;
    d.FirmwareMajor     = (short)OurHMDInfo.FirmwareMajor;
    d.FirmwareMinor     = (short)OurHMDInfo.FirmwareMinor;
    d.CameraFrustumFarZInMeters  = OurHMDInfo.CameraFrustumFarZInMeters;
    d.CameraFrustumHFovInRadians = OurHMDInfo.CameraFrustumHFovInRadians;
    d.CameraFrustumNearZInMeters = OurHMDInfo.CameraFrustumNearZInMeters;
    d.CameraFrustumVFovInRadians = OurHMDInfo.CameraFrustumVFovInRadians;

    OVR_strcpy(d.SerialNumber, sizeof(d.SerialNumber), OurHMDInfo.PrintedSerial.ToCStr());

    d.HmdCaps           = ovrHmdCap_Present | ovrHmdCap_NoVSync;
    d.TrackingCaps      = ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Orientation;
    d.DistortionCaps    = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp |
                          ovrDistortionCap_Vignette | ovrDistortionCap_SRGB |
                          ovrDistortionCap_FlipInput | ovrDistortionCap_ProfileNoTimewarpSpinWaits |
                          ovrDistortionCap_HqDistortion;

    if( OurHMDInfo.InCompatibilityMode )
        d.HmdCaps |= ovrHmdCap_ExtendDesktop;

    if (strstr(OurHMDInfo.ProductName, "DK1"))
    {
        d.Type = ovrHmd_DK1;        
    }
    else if (strstr(OurHMDInfo.ProductName, "DK2"))
    {
        d.Type        = ovrHmd_DK2;
        d.HmdCaps    |= ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction;
        d.TrackingCaps |= ovrTrackingCap_Position;
		d.DistortionCaps |= ovrDistortionCap_Overdrive;
    }
        
    const DistortionRenderDesc& leftDistortion  = Distortion[0];
    const DistortionRenderDesc& rightDistortion = Distortion[1];
  
    // The suggested FOV (assuming eye rotation)
    d.DefaultEyeFov[0] = CalculateFovFromHmdInfo(StereoEye_Left, leftDistortion, RenderInfo, OVR_DEFAULT_EXTRA_EYE_ROTATION);
    d.DefaultEyeFov[1] = CalculateFovFromHmdInfo(StereoEye_Right, rightDistortion, RenderInfo, OVR_DEFAULT_EXTRA_EYE_ROTATION);

    // FOV extended across the entire screen
    d.MaxEyeFov[0] = GetPhysicalScreenFov(StereoEye_Left, leftDistortion);
    d.MaxEyeFov[1] = GetPhysicalScreenFov(StereoEye_Right, rightDistortion);
    
    if (OurHMDInfo.Shutter.Type == HmdShutter_RollingRightToLeft)
    {
        d.EyeRenderOrder[0] = ovrEye_Right;
        d.EyeRenderOrder[1] = ovrEye_Left;
    }
    else
    {
        d.EyeRenderOrder[0] = ovrEye_Left;
        d.EyeRenderOrder[1] = ovrEye_Right;
    }    

    // MA: Taking this out on purpose.
	// Important information for those that are required to do their own timing,
    // because of shortfalls in timing code.
	//d.VsyncToNextVsync = OurHMDInfo.Shutter.VsyncToNextVsync;
	//d.PixelPersistence = OurHMDInfo.Shutter.PixelPersistence;

    return d;
}


ovrSizei HMDRenderState::GetFOVTextureSize(int eye, ovrFovPort fov, float pixelsPerDisplayPixel) const
{
    OVR_ASSERT((unsigned)eye < 2);
    StereoEye seye = (eye == ovrEye_Left) ? StereoEye_Left : StereoEye_Right;
    return CalculateIdealPixelSize(seye, Distortion[eye], fov, pixelsPerDisplayPixel);
}

ovrEyeRenderDesc HMDRenderState::CalcRenderDesc(ovrEyeType eyeType, const ovrFovPort& fov) const
{    
    const HmdRenderInfo&   hmdri = RenderInfo;
    StereoEye        eye   = (eyeType == ovrEye_Left) ? StereoEye_Left : StereoEye_Right;
    ovrEyeRenderDesc e0;
    
    e0.Eye                       = eyeType;
    e0.Fov                       = fov;
    e0.ViewAdjust                = CalculateEyeVirtualCameraOffset(hmdri, eye, false);
    e0.DistortedViewport         = GetFramebufferViewport(eye, hmdri);
    e0.PixelsPerTanAngleAtCenter = Distortion[0].PixelsPerTanAngleAtCenter;

    return e0;
}

}} // namespace OVR::CAPI

