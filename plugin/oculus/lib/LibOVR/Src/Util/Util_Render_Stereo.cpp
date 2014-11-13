/************************************************************************************

Filename    :   Util_Render_Stereo.cpp
Content     :   Stereo rendering configuration implementation
Created     :   October 22, 2012
Authors     :   Michael Antonov, Andrew Reisse, Tom Forsyth

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

*************************************************************************************/

#include "Util_Render_Stereo.h"

namespace OVR { namespace Util { namespace Render {

using namespace OVR::Tracking;


//-----------------------------------------------------------------------------------
// **** Useful debug functions.

char const* GetDebugNameEyeCupType ( EyeCupType eyeCupType )
{
    switch ( eyeCupType )
    {
    case EyeCup_DK1A:           return "DK1 A";             break;
    case EyeCup_DK1B:           return "DK1 B";             break;
    case EyeCup_DK1C:           return "DK1 C";             break;
    case EyeCup_DKHD2A:         return "DKHD2 A";           break;
    case EyeCup_OrangeA:        return "Orange A";          break;
    case EyeCup_RedA:           return "Red A";             break;
    case EyeCup_PinkA:          return "Pink A";            break;
    case EyeCup_BlueA:          return "Blue A";            break;
    case EyeCup_Delilah1A:      return "Delilah 1 A";       break;
    case EyeCup_Delilah2A:      return "Delilah 2 A";       break;
    case EyeCup_JamesA:         return "James A";           break;
    case EyeCup_SunMandalaA:    return "Sun Mandala A";     break;
    case EyeCup_DK2A:           return "DK2 A";             break;
    case EyeCup_LAST:           return "LAST";              break;
    default: OVR_ASSERT ( false ); return "Error"; break;
    }
}

char const* GetDebugNameHmdType ( HmdTypeEnum hmdType )
{
    switch ( hmdType )
    {
    case HmdType_None:              return "None";                   break;
    case HmdType_DK1:               return "DK1";                    break;
    case HmdType_DKProto:           return "DK1 prototype";          break;
    case HmdType_DKHDProto:         return "DK HD prototype 1";      break;
    case HmdType_DKHDProto566Mi:    return "DK HD prototype 566 Mi"; break;
    case HmdType_DKHD2Proto:        return "DK HD prototype 585";    break;
    case HmdType_CrystalCoveProto:  return "Crystal Cove";           break;
    case HmdType_DK2:               return "DK2";                    break;
    case HmdType_Unknown:           return "Unknown";                break;
    case HmdType_LAST:              return "LAST";                   break;
    default: OVR_ASSERT ( false ); return "Error"; break;
    }
}


//-----------------------------------------------------------------------------------
// **** Internal pipeline functions.

struct DistortionAndFov
{
    DistortionRenderDesc    Distortion;
    FovPort                 Fov; 
};

static DistortionAndFov CalculateDistortionAndFovInternal ( StereoEye eyeType, HmdRenderInfo const &hmd,
                                                            LensConfig const *pLensOverride = NULL,
                                                            FovPort const *pTanHalfFovOverride = NULL,
                                                            float extraEyeRotationInRadians = OVR_DEFAULT_EXTRA_EYE_ROTATION )
{
    // pLensOverride can be NULL, which means no override.

    DistortionRenderDesc localDistortion  = CalculateDistortionRenderDesc ( eyeType, hmd, pLensOverride );
    FovPort              fov              = CalculateFovFromHmdInfo ( eyeType, localDistortion, hmd, extraEyeRotationInRadians );
    // Here the app or the user would optionally clamp this visible fov to a smaller number if
    // they want more perf or resolution and are willing to give up FOV.
    // They may also choose to clamp UDLR differently e.g. to get cinemascope-style views.
    if ( pTanHalfFovOverride != NULL )
    {
        fov = *pTanHalfFovOverride;
    }

    // Here we could call ClampToPhysicalScreenFov(), but we do want people
    // to be able to play with larger-than-screen views.
    // The calling app can always do the clamping itself.
    DistortionAndFov result;
    result.Distortion = localDistortion;
    result.Fov        = fov;

    return result;
}


static Recti CalculateViewportInternal ( StereoEye eyeType,
                                            Sizei const actualRendertargetSurfaceSize,
                                            Sizei const requestedRenderedPixelSize,
                                            bool bRendertargetSharedByBothEyes,
                                            bool bMonoRenderingMode = false )
{
    Recti renderedViewport;
    if ( bMonoRenderingMode || !bRendertargetSharedByBothEyes || (eyeType == StereoEye_Center) )
    {
        // One eye per RT.
        renderedViewport.x = 0;
        renderedViewport.y = 0;
        renderedViewport.w = Alg::Min ( actualRendertargetSurfaceSize.w, requestedRenderedPixelSize.w );
        renderedViewport.h = Alg::Min ( actualRendertargetSurfaceSize.h, requestedRenderedPixelSize.h );
    }
    else
    {
        // Both eyes share the RT.
        renderedViewport.x = 0;
        renderedViewport.y = 0;
        renderedViewport.w = Alg::Min ( actualRendertargetSurfaceSize.w/2, requestedRenderedPixelSize.w );
        renderedViewport.h = Alg::Min ( actualRendertargetSurfaceSize.h,  requestedRenderedPixelSize.h );
        if ( eyeType == StereoEye_Right )
        {
            renderedViewport.x = (actualRendertargetSurfaceSize.w+1)/2;      // Round up, not down.
        }
    }
    return renderedViewport;
}

static Recti CalculateViewportDensityInternal ( StereoEye eyeType,
                                                   DistortionRenderDesc const &distortion,
                                                   FovPort const &fov,
                                                   Sizei const &actualRendertargetSurfaceSize,
                                                   bool bRendertargetSharedByBothEyes,
                                                   float desiredPixelDensity = 1.0f,
                                                   bool bMonoRenderingMode = false )
{
    OVR_ASSERT ( actualRendertargetSurfaceSize.w > 0 );
    OVR_ASSERT ( actualRendertargetSurfaceSize.h > 0 );

    // What size RT do we need to get 1:1 mapping?
    Sizei idealPixelSize = CalculateIdealPixelSize ( eyeType, distortion, fov, desiredPixelDensity );
    // ...but we might not actually get that size.
    return CalculateViewportInternal ( eyeType,
                                       actualRendertargetSurfaceSize,
                                       idealPixelSize,
                                       bRendertargetSharedByBothEyes, bMonoRenderingMode );
}

static ViewportScaleAndOffset CalculateViewportScaleAndOffsetInternal (
                                                          ScaleAndOffset2D const &eyeToSourceNDC,
                                                          Recti const &renderedViewport,
                                                          Sizei const &actualRendertargetSurfaceSize )
{
    ViewportScaleAndOffset result;
    result.RenderedViewport = renderedViewport;
    result.EyeToSourceUV = CreateUVScaleAndOffsetfromNDCScaleandOffset(
                                            eyeToSourceNDC, renderedViewport, actualRendertargetSurfaceSize );
    return result;
}


static StereoEyeParams CalculateStereoEyeParamsInternal ( StereoEye eyeType, HmdRenderInfo const &hmd,
                                                          DistortionRenderDesc const &distortion,
                                                          FovPort const &fov,
                                                          Sizei const &actualRendertargetSurfaceSize,
                                                          Recti const &renderedViewport,
                                                          bool bRightHanded = true, float zNear = 0.01f, float zFar = 10000.0f,
                                                          bool bMonoRenderingMode = false,
                                                          float zoomFactor = 1.0f )
{
    // Generate the projection matrix for intermediate rendertarget.
    // Z range can also be inserted later by the app (though not in this particular case)
    float fovScale = 1.0f / zoomFactor;
    FovPort zoomedFov = fov;
    zoomedFov.LeftTan  *= fovScale;
    zoomedFov.RightTan *= fovScale;
    zoomedFov.UpTan    *= fovScale;
    zoomedFov.DownTan  *= fovScale;
    Matrix4f projection = CreateProjection ( bRightHanded, zoomedFov, zNear, zFar );

    // Find the mapping from TanAngle space to target NDC space.
    // Note this does NOT take the zoom factor into account because
    // this is the mapping of actual physical eye FOV (and our eyes do not zoom!)
    // to screen space.
    ScaleAndOffset2D eyeToSourceNDC = CreateNDCScaleAndOffsetFromFov ( fov );
    
    // The size of the final FB, which is fixed and determined by the physical size of the device display.
    Recti distortedViewport   = GetFramebufferViewport ( eyeType, hmd );
    Vector3f virtualCameraOffset = CalculateEyeVirtualCameraOffset(hmd, eyeType, bMonoRenderingMode);

    StereoEyeParams result;
    result.Eye                  = eyeType;
    result.ViewAdjust           = Matrix4f::Translation(virtualCameraOffset);
    result.Distortion           = distortion;
    result.DistortionViewport   = distortedViewport;
    result.Fov                  = fov;
    result.RenderedProjection   = projection;
    result.EyeToSourceNDC       = eyeToSourceNDC;
    ViewportScaleAndOffset vsao = CalculateViewportScaleAndOffsetInternal ( eyeToSourceNDC, renderedViewport, actualRendertargetSurfaceSize );
    result.RenderedViewport     = vsao.RenderedViewport;
    result.EyeToSourceUV        = vsao.EyeToSourceUV;

    return result;
}


Vector3f CalculateEyeVirtualCameraOffset(HmdRenderInfo const &hmd,
                                         StereoEye eyeType, bool bmonoRenderingMode)
{
    Vector3f virtualCameraOffset(0);

    if (!bmonoRenderingMode)
    {
        float eyeCenterRelief = hmd.GetEyeCenter().ReliefInMeters;

        if (eyeType == StereoEye_Left)
        {
            virtualCameraOffset.x = hmd.EyeLeft.NoseToPupilInMeters;
            virtualCameraOffset.z = eyeCenterRelief - hmd.EyeLeft.ReliefInMeters;
        }
        else if (eyeType == StereoEye_Right)
        {
            virtualCameraOffset.x = -hmd.EyeRight.NoseToPupilInMeters;
            virtualCameraOffset.z = eyeCenterRelief - hmd.EyeRight.ReliefInMeters;
        }
    }

    return virtualCameraOffset;
}


//-----------------------------------------------------------------------------------
// **** Higher-level utility functions.

Sizei CalculateRecommendedTextureSize ( HmdRenderInfo const &hmd,
                                        bool bRendertargetSharedByBothEyes,
                                        float pixelDensityInCenter /*= 1.0f*/ )
{
    Sizei idealPixelSize[2];
    for ( int eyeNum = 0; eyeNum < 2; eyeNum++ )
    {
        StereoEye eyeType = ( eyeNum == 0 ) ? StereoEye_Left : StereoEye_Right;

        DistortionAndFov distortionAndFov = CalculateDistortionAndFovInternal ( eyeType, hmd, NULL, NULL, OVR_DEFAULT_EXTRA_EYE_ROTATION );

        idealPixelSize[eyeNum] = CalculateIdealPixelSize ( eyeType,
                                        distortionAndFov.Distortion,
                                        distortionAndFov.Fov,
                                        pixelDensityInCenter );
    }

    Sizei result;
    result.w = Alg::Max ( idealPixelSize[0].w, idealPixelSize[1].w );
    result.h = Alg::Max ( idealPixelSize[0].h, idealPixelSize[1].h );
    if ( bRendertargetSharedByBothEyes )
    {
        result.w *= 2;
    }
    return result;
}

StereoEyeParams CalculateStereoEyeParams ( HmdRenderInfo const &hmd,
                                           StereoEye eyeType,
                                           Sizei const &actualRendertargetSurfaceSize,
                                           bool bRendertargetSharedByBothEyes,
                                           bool bRightHanded /*= true*/,
                                           float zNear /*= 0.01f*/, float zFar /*= 10000.0f*/,
										   Sizei const *pOverrideRenderedPixelSize /* = NULL*/,
                                           FovPort const *pOverrideFovport /*= NULL*/,
                                           float zoomFactor /*= 1.0f*/ )
{
    DistortionAndFov distortionAndFov = CalculateDistortionAndFovInternal ( eyeType, hmd, NULL, NULL, OVR_DEFAULT_EXTRA_EYE_ROTATION );
    if ( pOverrideFovport != NULL )
    {
        distortionAndFov.Fov = *pOverrideFovport;
    }

    Recti viewport;
    if ( pOverrideRenderedPixelSize != NULL )
    {
        viewport = CalculateViewportInternal ( eyeType, actualRendertargetSurfaceSize, *pOverrideRenderedPixelSize, bRendertargetSharedByBothEyes, false );
    }
    else
    {
        viewport = CalculateViewportDensityInternal ( eyeType,
                                                      distortionAndFov.Distortion,
                                                      distortionAndFov.Fov,
                                                      actualRendertargetSurfaceSize, bRendertargetSharedByBothEyes, 1.0f, false );
    }

    return CalculateStereoEyeParamsInternal (
                                eyeType, hmd,
                                distortionAndFov.Distortion,
                                distortionAndFov.Fov,
                                actualRendertargetSurfaceSize, viewport,
                                bRightHanded, zNear, zFar, false, zoomFactor );
}


FovPort CalculateRecommendedFov ( HmdRenderInfo const &hmd,
                                  StereoEye eyeType,
                                  bool bMakeFovSymmetrical /* = false */ )
{
    DistortionAndFov distortionAndFov = CalculateDistortionAndFovInternal ( eyeType, hmd, NULL, NULL, OVR_DEFAULT_EXTRA_EYE_ROTATION );
    FovPort fov = distortionAndFov.Fov;
    if ( bMakeFovSymmetrical )
    {
        // Deal with engines that cannot support an off-center projection.
        // Unfortunately this means they will be rendering pixels that the user can't actually see.
        float fovTanH = Alg::Max ( fov.LeftTan, fov.RightTan );
        float fovTanV = Alg::Max ( fov.UpTan, fov.DownTan );
        fov.LeftTan = fovTanH;
        fov.RightTan = fovTanH;
        fov.UpTan = fovTanV;
        fov.DownTan = fovTanV;
    }
    return fov;
}

ViewportScaleAndOffset ModifyRenderViewport ( StereoEyeParams const &params,
                                              Sizei const &actualRendertargetSurfaceSize,
                                              Recti const &renderViewport )
{
    return CalculateViewportScaleAndOffsetInternal ( params.EyeToSourceNDC, renderViewport, actualRendertargetSurfaceSize );
}

ViewportScaleAndOffset ModifyRenderSize ( StereoEyeParams const &params,
                                          Sizei const &actualRendertargetSurfaceSize,
                                          Sizei const &requestedRenderSize,
                                          bool bRendertargetSharedByBothEyes /*= false*/ )
{
    Recti renderViewport = CalculateViewportInternal ( params.Eye, actualRendertargetSurfaceSize, requestedRenderSize, bRendertargetSharedByBothEyes, false );
    return CalculateViewportScaleAndOffsetInternal ( params.EyeToSourceNDC, renderViewport, actualRendertargetSurfaceSize );
}

ViewportScaleAndOffset ModifyRenderDensity ( StereoEyeParams const &params,
                                             Sizei const &actualRendertargetSurfaceSize,
                                             float pixelDensity /*= 1.0f*/,
                                             bool bRendertargetSharedByBothEyes /*= false*/ )
{
    Recti renderViewport = CalculateViewportDensityInternal ( params.Eye, params.Distortion, params.Fov, actualRendertargetSurfaceSize, bRendertargetSharedByBothEyes, pixelDensity, false );
    return CalculateViewportScaleAndOffsetInternal ( params.EyeToSourceNDC, renderViewport, actualRendertargetSurfaceSize );
}


//-----------------------------------------------------------------------------------
// **** StereoConfig Implementation

StereoConfig::StereoConfig(StereoMode mode)
    : Mode(mode),
      DirtyFlag(true)
{
    // Initialize "fake" default HMD values for testing without HMD plugged in.
    // These default values match those returned by DK1
    // (at least they did at time of writing - certainly good enough for debugging)
    Hmd.HmdType                                         = HmdType_None;
    Hmd.ResolutionInPixels                              = Sizei(1280, 800);
    Hmd.ScreenSizeInMeters                              = Sizef(0.1498f, 0.0936f);
    Hmd.ScreenGapSizeInMeters                           = 0.0f;
    Hmd.CenterFromTopInMeters                           = 0.0468f;
    Hmd.LensSeparationInMeters                          = 0.0635f;
    Hmd.LensDiameterInMeters                            = 0.035f;
    Hmd.LensSurfaceToMidplateInMeters                   = 0.025f;
    Hmd.EyeCups                                         = EyeCup_DK1A;
    Hmd.Shutter.Type                                    = HmdShutter_RollingTopToBottom;
    Hmd.Shutter.VsyncToNextVsync                        = ( 1.0f / 60.0f );
    Hmd.Shutter.VsyncToFirstScanline                    = 0.000052f;
    Hmd.Shutter.FirstScanlineToLastScanline             = 0.016580f;
    Hmd.Shutter.PixelSettleTime                         = 0.015f;
    Hmd.Shutter.PixelPersistence                        = ( 1.0f / 60.0f );
    Hmd.EyeLeft.Distortion.SetToIdentity();
    Hmd.EyeLeft.Distortion.MetersPerTanAngleAtCenter    = 0.043875f;
    Hmd.EyeLeft.Distortion.Eqn                          = Distortion_RecipPoly4;
    Hmd.EyeLeft.Distortion.K[0]                         = 1.0f;
    Hmd.EyeLeft.Distortion.K[1]                         = -0.3999f;
    Hmd.EyeLeft.Distortion.K[2]                         = 0.2408f;
    Hmd.EyeLeft.Distortion.K[3]                         = -0.4589f;
    Hmd.EyeLeft.Distortion.MaxR                         = 1.0f;
	Hmd.EyeLeft.Distortion.ChromaticAberration[0]		= 0.006f;
	Hmd.EyeLeft.Distortion.ChromaticAberration[1]		= 0.0f;
	Hmd.EyeLeft.Distortion.ChromaticAberration[2]		= -0.014f;
	Hmd.EyeLeft.Distortion.ChromaticAberration[3]		= 0.0f;
    Hmd.EyeLeft.NoseToPupilInMeters                     = 0.62f;
    Hmd.EyeLeft.ReliefInMeters                          = 0.013f;
    Hmd.EyeRight = Hmd.EyeLeft;

    SetViewportMode = SVPM_Density;
    SetViewportPixelsPerDisplayPixel = 1.0f;
    // Not used in this mode, but init them anyway.
    SetViewportSize[0] = Sizei(0,0);
    SetViewportSize[1] = Sizei(0,0);
    SetViewport[0] = Recti(0,0,0,0);
    SetViewport[1] = Recti(0,0,0,0);

    OverrideLens = false;
    OverrideTanHalfFov = false;
    OverrideZeroIpd = false;
    ExtraEyeRotationInRadians = OVR_DEFAULT_EXTRA_EYE_ROTATION;
    IsRendertargetSharedByBothEyes = true;
    RightHandedProjection = true;

    // This should cause an assert if the app does not call SetRendertargetSize()
    RendertargetSize = Sizei ( 0, 0 );

    ZNear = 0.01f;
    ZFar = 10000.0f;

    Set2DAreaFov(DegreeToRad(85.0f));
}

void StereoConfig::SetHmdRenderInfo(const HmdRenderInfo& hmd)
{
    Hmd = hmd;
    DirtyFlag = true;
}

void StereoConfig::Set2DAreaFov(float fovRadians)
{
    Area2DFov = fovRadians;
    DirtyFlag = true;
}

const StereoEyeParamsWithOrtho& StereoConfig::GetEyeRenderParams(StereoEye eye)
{
    if ( DirtyFlag )
    {
        UpdateComputedState();
    }

    static const uint8_t eyeParamIndices[3] = { 0, 0, 1 };

    OVR_ASSERT(eye < sizeof(eyeParamIndices));
    return EyeRenderParams[eyeParamIndices[eye]];
}

void StereoConfig::SetLensOverride ( LensConfig const *pLensOverrideLeft  /*= NULL*/,
                                     LensConfig const *pLensOverrideRight /*= NULL*/ )
{
    if ( pLensOverrideLeft == NULL )
    {
        OverrideLens = false;
    }
    else
    {
        OverrideLens = true;
        LensOverrideLeft = *pLensOverrideLeft;
        LensOverrideRight = *pLensOverrideLeft;
        if ( pLensOverrideRight != NULL )
        {
            LensOverrideRight = *pLensOverrideRight;
        }
    }
    DirtyFlag = true;
}

void StereoConfig::SetRendertargetSize (Size<int> const rendertargetSize,
                                        bool rendertargetIsSharedByBothEyes )
{
    RendertargetSize = rendertargetSize;
    IsRendertargetSharedByBothEyes = rendertargetIsSharedByBothEyes;
    DirtyFlag = true;
}

void StereoConfig::SetFov ( FovPort const *pfovLeft  /*= NULL*/,
                            FovPort const *pfovRight /*= NULL*/ )
{
    DirtyFlag = true;
    if ( pfovLeft == NULL )
    {
        OverrideTanHalfFov = false;
    }
    else
    {
        OverrideTanHalfFov = true;
        FovOverrideLeft  = *pfovLeft;
        FovOverrideRight = *pfovLeft;
        if ( pfovRight != NULL )
        {
            FovOverrideRight = *pfovRight;
        }
    }
}


void StereoConfig::SetZeroVirtualIpdOverride ( bool enableOverride )
{
    DirtyFlag = true;
    OverrideZeroIpd = enableOverride;
}


void StereoConfig::SetZClipPlanesAndHandedness ( float zNear /*= 0.01f*/, float zFar /*= 10000.0f*/, bool rightHandedProjection /*= true*/ )
{
    DirtyFlag = true;
    ZNear = zNear;
    ZFar = zFar;
    RightHandedProjection = rightHandedProjection;
}

void StereoConfig::SetExtraEyeRotation ( float extraEyeRotationInRadians )
{
    DirtyFlag = true;
    ExtraEyeRotationInRadians = extraEyeRotationInRadians;
}

Sizei StereoConfig::CalculateRecommendedTextureSize ( bool rendertargetSharedByBothEyes,
                                                      float pixelDensityInCenter /*= 1.0f*/ )
{
    return Render::CalculateRecommendedTextureSize ( Hmd, rendertargetSharedByBothEyes, pixelDensityInCenter );
}



void StereoConfig::UpdateComputedState()
{
    int numEyes = 2;
    StereoEye eyeTypes[2];

    switch ( Mode )
    {
    case Stereo_None:
        numEyes         = 1;
        eyeTypes[0]     = StereoEye_Center;
        break;

    case Stereo_LeftRight_Multipass:
        numEyes         = 2;
        eyeTypes[0]     = StereoEye_Left;
        eyeTypes[1]     = StereoEye_Right;
        break;

    default:
        numEyes = 0;
        OVR_ASSERT( false );
        break;
    }

    // If either of these fire, you've probably forgotten to call SetRendertargetSize()
    OVR_ASSERT ( RendertargetSize.w > 0 );
    OVR_ASSERT ( RendertargetSize.h > 0 );

    for ( int eyeNum = 0; eyeNum < numEyes; eyeNum++ )
    {
        StereoEye eyeType = eyeTypes[eyeNum];
        LensConfig *pLensOverride = NULL;
        if ( OverrideLens )
        {
            if ( eyeType == StereoEye_Right )
            {
                pLensOverride = &LensOverrideRight;
            }
            else
            {
                pLensOverride = &LensOverrideLeft;
            }
        }

        FovPort *pTanHalfFovOverride = NULL;
        if ( OverrideTanHalfFov )
        {
            if ( eyeType == StereoEye_Right )
            {
                pTanHalfFovOverride = &FovOverrideRight;
            }
            else
            {
                pTanHalfFovOverride = &FovOverrideLeft;
            }
        }

        DistortionAndFov distortionAndFov =
            CalculateDistortionAndFovInternal ( eyeType, Hmd,
                                                pLensOverride, pTanHalfFovOverride,
                                                ExtraEyeRotationInRadians );

        EyeRenderParams[eyeNum].StereoEye.Distortion = distortionAndFov.Distortion;
        EyeRenderParams[eyeNum].StereoEye.Fov        = distortionAndFov.Fov;
    }

    if ( OverrideZeroIpd )
    {
        // Take the union of the calculated eye FOVs.
        FovPort fov;
        fov.UpTan    = Alg::Max ( EyeRenderParams[0].StereoEye.Fov.UpTan   , EyeRenderParams[1].StereoEye.Fov.UpTan    );
        fov.DownTan  = Alg::Max ( EyeRenderParams[0].StereoEye.Fov.DownTan , EyeRenderParams[1].StereoEye.Fov.DownTan  );
        fov.LeftTan  = Alg::Max ( EyeRenderParams[0].StereoEye.Fov.LeftTan , EyeRenderParams[1].StereoEye.Fov.LeftTan  );
        fov.RightTan = Alg::Max ( EyeRenderParams[0].StereoEye.Fov.RightTan, EyeRenderParams[1].StereoEye.Fov.RightTan );
        EyeRenderParams[0].StereoEye.Fov = fov;
        EyeRenderParams[1].StereoEye.Fov = fov;
    }

    for ( int eyeNum = 0; eyeNum < numEyes; eyeNum++ )
    {
        StereoEye eyeType = eyeTypes[eyeNum];

        DistortionRenderDesc localDistortion = EyeRenderParams[eyeNum].StereoEye.Distortion;
        FovPort              fov             = EyeRenderParams[eyeNum].StereoEye.Fov;

        // Use a placeholder - will be overridden later.
        Recti tempViewport = Recti ( 0, 0, 1, 1 );

        EyeRenderParams[eyeNum].StereoEye = CalculateStereoEyeParamsInternal (
                                        eyeType, Hmd, localDistortion, fov,
                                        RendertargetSize, tempViewport,
                                        RightHandedProjection, ZNear, ZFar,
                                        OverrideZeroIpd );

        // We want to create a virtual 2D surface we can draw debug text messages to.
        // We'd like it to be a fixed distance (OrthoDistance) away,
        // and to cover a specific FOV (Area2DFov). We need to find the projection matrix for this,
        // and also to know how large it is in pixels to achieve a 1:1 mapping at the center of the screen.
        float orthoDistance = 0.8f;
        float orthoHalfFov = tanf ( Area2DFov * 0.5f );
        Vector2f unityOrthoPixelSize = localDistortion.PixelsPerTanAngleAtCenter * ( orthoHalfFov * 2.0f );
        float localInterpupillaryDistance = Hmd.EyeLeft.NoseToPupilInMeters + Hmd.EyeRight.NoseToPupilInMeters;
        if ( OverrideZeroIpd )
        {
            localInterpupillaryDistance = 0.0f;
        }
        Matrix4f ortho = CreateOrthoSubProjection ( true, eyeType,
                                                    orthoHalfFov, orthoHalfFov,
                                                    unityOrthoPixelSize.x, unityOrthoPixelSize.y,
                                                    orthoDistance, localInterpupillaryDistance,
                                                    EyeRenderParams[eyeNum].StereoEye.RenderedProjection );
        EyeRenderParams[eyeNum].OrthoProjection = ortho;
    }

    // ...and now set up the viewport, scale & offset the way the app wanted.
    setupViewportScaleAndOffsets();

    if ( OverrideZeroIpd )
    {
        // Monocular rendering has some fragile parts... don't break any by accident.
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.Fov.UpTan                   == EyeRenderParams[1].StereoEye.Fov.UpTan    );
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.Fov.DownTan                 == EyeRenderParams[1].StereoEye.Fov.DownTan  );
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.Fov.LeftTan                 == EyeRenderParams[1].StereoEye.Fov.LeftTan  );
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.Fov.RightTan                == EyeRenderParams[1].StereoEye.Fov.RightTan );
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.RenderedProjection.M[0][0]  == EyeRenderParams[1].StereoEye.RenderedProjection.M[0][0] );
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.RenderedProjection.M[1][1]  == EyeRenderParams[1].StereoEye.RenderedProjection.M[1][1] );
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.RenderedProjection.M[0][2]  == EyeRenderParams[1].StereoEye.RenderedProjection.M[0][2] );
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.RenderedProjection.M[1][2]  == EyeRenderParams[1].StereoEye.RenderedProjection.M[1][2] );
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.RenderedViewport            == EyeRenderParams[1].StereoEye.RenderedViewport      );
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.EyeToSourceUV.Offset        == EyeRenderParams[1].StereoEye.EyeToSourceUV.Offset  );
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.EyeToSourceUV.Scale         == EyeRenderParams[1].StereoEye.EyeToSourceUV.Scale   );
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.EyeToSourceNDC.Offset       == EyeRenderParams[1].StereoEye.EyeToSourceNDC.Offset );
        OVR_ASSERT ( EyeRenderParams[0].StereoEye.EyeToSourceNDC.Scale        == EyeRenderParams[1].StereoEye.EyeToSourceNDC.Scale  );
        OVR_ASSERT ( EyeRenderParams[0].OrthoProjection.M[0][0]               == EyeRenderParams[1].OrthoProjection.M[0][0] );
        OVR_ASSERT ( EyeRenderParams[0].OrthoProjection.M[1][1]               == EyeRenderParams[1].OrthoProjection.M[1][1] );
        OVR_ASSERT ( EyeRenderParams[0].OrthoProjection.M[0][2]               == EyeRenderParams[1].OrthoProjection.M[0][2] );
        OVR_ASSERT ( EyeRenderParams[0].OrthoProjection.M[1][2]               == EyeRenderParams[1].OrthoProjection.M[1][2] );
    }

    DirtyFlag = false;
}



ViewportScaleAndOffsetBothEyes StereoConfig::setupViewportScaleAndOffsets()
{
    for ( int eyeNum = 0; eyeNum < 2; eyeNum++ )
    {
        StereoEye eyeType = ( eyeNum == 0 ) ? StereoEye_Left : StereoEye_Right;

        DistortionRenderDesc localDistortion = EyeRenderParams[eyeNum].StereoEye.Distortion;
        FovPort              fov             = EyeRenderParams[eyeNum].StereoEye.Fov;

        Recti renderedViewport;
        switch ( SetViewportMode )
        {
        case SVPM_Density:
            renderedViewport = CalculateViewportDensityInternal (
                                    eyeType, localDistortion, fov,
                                    RendertargetSize, IsRendertargetSharedByBothEyes,
                                    SetViewportPixelsPerDisplayPixel, OverrideZeroIpd );
            break;
        case SVPM_Size:
            if ( ( eyeType == StereoEye_Right ) && !OverrideZeroIpd )
            {
                renderedViewport = CalculateViewportInternal (
                                        eyeType, RendertargetSize,
                                        SetViewportSize[1],
                                        IsRendertargetSharedByBothEyes, OverrideZeroIpd );
            }
            else
            {
                renderedViewport = CalculateViewportInternal (
                                        eyeType, RendertargetSize,
                                        SetViewportSize[0],
                                        IsRendertargetSharedByBothEyes, OverrideZeroIpd );
            }
            break;
        case SVPM_Viewport:
            if ( ( eyeType == StereoEye_Right ) && !OverrideZeroIpd )
            {
                renderedViewport = SetViewport[1];
            }
            else
            {
                renderedViewport = SetViewport[0];
            }
            break;
        default: OVR_ASSERT ( false ); break;
        }

        ViewportScaleAndOffset vpsao = CalculateViewportScaleAndOffsetInternal (
                                                EyeRenderParams[eyeNum].StereoEye.EyeToSourceNDC,
                                                renderedViewport,
                                                RendertargetSize );
        EyeRenderParams[eyeNum].StereoEye.RenderedViewport = vpsao.RenderedViewport;
        EyeRenderParams[eyeNum].StereoEye.EyeToSourceUV    = vpsao.EyeToSourceUV;
    }

    ViewportScaleAndOffsetBothEyes result;
    result.Left.EyeToSourceUV     = EyeRenderParams[0].StereoEye.EyeToSourceUV;
    result.Left.RenderedViewport  = EyeRenderParams[0].StereoEye.RenderedViewport;
    result.Right.EyeToSourceUV    = EyeRenderParams[1].StereoEye.EyeToSourceUV;
    result.Right.RenderedViewport = EyeRenderParams[1].StereoEye.RenderedViewport;
    return result;
}

// Specify a pixel density - how many rendered pixels per pixel in the physical display.
ViewportScaleAndOffsetBothEyes StereoConfig::SetRenderDensity ( float pixelsPerDisplayPixel )
{
    SetViewportMode  = SVPM_Density;
    SetViewportPixelsPerDisplayPixel = pixelsPerDisplayPixel;
    return setupViewportScaleAndOffsets();
}

// Supply the size directly. Will be clamped to the physical rendertarget size.
ViewportScaleAndOffsetBothEyes StereoConfig::SetRenderSize ( Sizei const &renderSizeLeft, Sizei const &renderSizeRight )
{
    SetViewportMode  = SVPM_Size;
    SetViewportSize[0] = renderSizeLeft;
    SetViewportSize[1] = renderSizeRight;
    return setupViewportScaleAndOffsets();
}

// Supply the viewport directly. This is not clamped to the physical rendertarget - careful now!
ViewportScaleAndOffsetBothEyes StereoConfig::SetRenderViewport ( Recti const &renderViewportLeft, Recti const &renderViewportRight )
{
    SetViewportMode  = SVPM_Viewport;
    SetViewport[0] = renderViewportLeft;
    SetViewport[1] = renderViewportRight;
    return setupViewportScaleAndOffsets();
}

Matrix4f StereoConfig::GetProjectionWithZoom ( StereoEye eye, float fovZoom ) const
{
    int eyeNum = ( eye == StereoEye_Right ) ? 1 : 0;
    float fovScale = 1.0f / fovZoom;
    FovPort fovPort = EyeRenderParams[eyeNum].StereoEye.Fov;
    fovPort.LeftTan  *= fovScale;
    fovPort.RightTan *= fovScale;
    fovPort.UpTan    *= fovScale;
    fovPort.DownTan  *= fovScale;
    return CreateProjection ( RightHandedProjection, fovPort, ZNear, ZFar );
}




//-----------------------------------------------------------------------------------
// *****  Distortion Mesh Rendering


// Pow2 for the Morton order to work!
// 4 is too low - it is easy to see the "wobbles" in the HMD.
// 5 is realllly close but you can see pixel differences with even/odd frame checking.
// 6 is indistinguishable on a monitor on even/odd frames.
static const int DMA_GridSizeLog2   = 6;
static const int DMA_GridSize       = 1<<DMA_GridSizeLog2;
static const int DMA_NumVertsPerEye = (DMA_GridSize+1)*(DMA_GridSize+1);
static const int DMA_NumTrisPerEye  = (DMA_GridSize)*(DMA_GridSize)*2;



void DistortionMeshDestroy ( DistortionMeshVertexData *pVertices, uint16_t *pTriangleMeshIndices )
{
    OVR_FREE ( pVertices );
    OVR_FREE ( pTriangleMeshIndices );
}

void DistortionMeshCreate ( DistortionMeshVertexData **ppVertices, uint16_t **ppTriangleListIndices,
                            int *pNumVertices, int *pNumTriangles,
                            const StereoEyeParams &stereoParams, const HmdRenderInfo &hmdRenderInfo )
{
    bool    rightEye      = ( stereoParams.Eye == StereoEye_Right );
    int     vertexCount   = 0;
    int     triangleCount = 0;

    // Generate mesh into allocated data and return result.
    DistortionMeshCreate(ppVertices, ppTriangleListIndices, &vertexCount, &triangleCount,
                         rightEye, hmdRenderInfo, stereoParams.Distortion, stereoParams.EyeToSourceNDC);
    
    *pNumVertices  = vertexCount;
    *pNumTriangles = triangleCount;
}


// Generate distortion mesh for a eye.
void DistortionMeshCreate( DistortionMeshVertexData **ppVertices, uint16_t **ppTriangleListIndices,
                           int *pNumVertices, int *pNumTriangles,
                           bool rightEye,
                           const HmdRenderInfo &hmdRenderInfo, 
                           const DistortionRenderDesc &distortion, const ScaleAndOffset2D &eyeToSourceNDC )
{
    *pNumVertices  = DMA_NumVertsPerEye;
    *pNumTriangles = DMA_NumTrisPerEye;

    *ppVertices = (DistortionMeshVertexData*)
                      OVR_ALLOC( sizeof(DistortionMeshVertexData) * (*pNumVertices) );
    *ppTriangleListIndices  = (uint16_t*) OVR_ALLOC( sizeof(uint16_t) * (*pNumTriangles) * 3 );

    if (!*ppVertices || !*ppTriangleListIndices)
    {
        if (*ppVertices)
        {
            OVR_FREE(*ppVertices);
        }
        if (*ppTriangleListIndices)
        {
            OVR_FREE(*ppTriangleListIndices);
        }
        *ppVertices             = NULL;
        *ppTriangleListIndices  = NULL;
        *pNumTriangles          = 0;
        *pNumVertices           = 0;
        return;
    }

    // When does the fade-to-black edge start? Chosen heuristically.
    const float fadeOutBorderFraction = 0.075f;
      
    
    // Populate vertex buffer info
    float xOffset = 0.0f;
    float uOffset = 0.0f;
    OVR_UNUSED(uOffset);

    if (rightEye)
    {
        xOffset = 1.0f;
        uOffset = 0.5f;
    }

    // First pass - build up raw vertex data.
    DistortionMeshVertexData* pcurVert = *ppVertices;

    for ( int y = 0; y <= DMA_GridSize; y++ )
    {
        for ( int x = 0; x <= DMA_GridSize; x++ )
        {

            Vector2f sourceCoordNDC;
            // NDC texture coords [-1,+1]
            sourceCoordNDC.x = 2.0f * ( (float)x / (float)DMA_GridSize ) - 1.0f;
            sourceCoordNDC.y = 2.0f * ( (float)y / (float)DMA_GridSize ) - 1.0f;
            Vector2f tanEyeAngle = TransformRendertargetNDCToTanFovSpace ( eyeToSourceNDC, sourceCoordNDC );

            // Find a corresponding screen position.
            // Note - this function does not have to be precise - we're just trying to match the mesh tessellation
            // with the shape of the distortion to minimise the number of trianlges needed.
            Vector2f screenNDC = TransformTanFovSpaceToScreenNDC ( distortion, tanEyeAngle, false );
            // ...but don't let verts overlap to the other eye.
            screenNDC.x = Alg::Max ( -1.0f, Alg::Min ( screenNDC.x, 1.0f ) );
            screenNDC.y = Alg::Max ( -1.0f, Alg::Min ( screenNDC.y, 1.0f ) );

            // From those screen positions, we then need (effectively) RGB UVs.
            // This is the function that actually matters when doing the distortion calculation.
            Vector2f tanEyeAnglesR, tanEyeAnglesG, tanEyeAnglesB;
            TransformScreenNDCToTanFovSpaceChroma ( &tanEyeAnglesR, &tanEyeAnglesG, &tanEyeAnglesB,
                                                    distortion, screenNDC );
			
			pcurVert->TanEyeAnglesR = tanEyeAnglesR;
			pcurVert->TanEyeAnglesG = tanEyeAnglesG;
			pcurVert->TanEyeAnglesB = tanEyeAnglesB;
			
            HmdShutterTypeEnum shutterType = hmdRenderInfo.Shutter.Type;
            switch ( shutterType )
            {
            case HmdShutter_Global:
                pcurVert->TimewarpLerp = 0.0f;
                break;
            case HmdShutter_RollingLeftToRight:
                // Retrace is left to right - left eye goes 0.0 -> 0.5, then right goes 0.5 -> 1.0
                pcurVert->TimewarpLerp = screenNDC.x * 0.25f + 0.25f;
                if (rightEye)
                {
                    pcurVert->TimewarpLerp += 0.5f;
                }
                break;
            case HmdShutter_RollingRightToLeft:
                // Retrace is right to left - right eye goes 0.0 -> 0.5, then left goes 0.5 -> 1.0
                pcurVert->TimewarpLerp = 0.75f - screenNDC.x * 0.25f;
                if (rightEye)
                {
                    pcurVert->TimewarpLerp -= 0.5f;
                }
                break;
            case HmdShutter_RollingTopToBottom:
                // Retrace is top to bottom on both eyes at the same time.
                pcurVert->TimewarpLerp = screenNDC.y * 0.5f + 0.5f;
                break;
            default: OVR_ASSERT ( false ); break;
            }

            // Fade out at texture edges.
            // The furthest out will be the blue channel, because of chromatic aberration (true of any standard lens)
            Vector2f sourceTexCoordBlueNDC = TransformTanFovSpaceToRendertargetNDC ( eyeToSourceNDC, tanEyeAnglesB );
            float edgeFadeIn       = ( 1.0f / fadeOutBorderFraction ) *
                                     ( 1.0f - Alg::Max ( Alg::Abs ( sourceTexCoordBlueNDC.x ), Alg::Abs ( sourceTexCoordBlueNDC.y ) ) );
            // Also fade out at screen edges.
            float edgeFadeInScreen = ( 2.0f / fadeOutBorderFraction ) *
                                     ( 1.0f - Alg::Max ( Alg::Abs ( screenNDC.x ), Alg::Abs ( screenNDC.y ) ) );
            edgeFadeIn = Alg::Min ( edgeFadeInScreen, edgeFadeIn );

            pcurVert->Shade = Alg::Max ( 0.0f, Alg::Min ( edgeFadeIn, 1.0f ) );
            pcurVert->ScreenPosNDC.x = 0.5f * screenNDC.x - 0.5f + xOffset;
            pcurVert->ScreenPosNDC.y = -screenNDC.y;

            pcurVert++;
        }
    }


    // Populate index buffer info  
    uint16_t *pcurIndex = *ppTriangleListIndices;

    for ( int triNum = 0; triNum < DMA_GridSize * DMA_GridSize; triNum++ )
    {
        // Use a Morton order to help locality of FB, texture and vertex cache.
        // (0.325ms raster order -> 0.257ms Morton order)
        OVR_ASSERT ( DMA_GridSize <= 256 );
        int x = ( ( triNum & 0x0001 ) >> 0 ) |
                ( ( triNum & 0x0004 ) >> 1 ) |
                ( ( triNum & 0x0010 ) >> 2 ) |
                ( ( triNum & 0x0040 ) >> 3 ) |
                ( ( triNum & 0x0100 ) >> 4 ) |
                ( ( triNum & 0x0400 ) >> 5 ) |
                ( ( triNum & 0x1000 ) >> 6 ) |
                ( ( triNum & 0x4000 ) >> 7 );
        int y = ( ( triNum & 0x0002 ) >> 1 ) |
                ( ( triNum & 0x0008 ) >> 2 ) |
                ( ( triNum & 0x0020 ) >> 3 ) |
                ( ( triNum & 0x0080 ) >> 4 ) |
                ( ( triNum & 0x0200 ) >> 5 ) |
                ( ( triNum & 0x0800 ) >> 6 ) |
                ( ( triNum & 0x2000 ) >> 7 ) |
                ( ( triNum & 0x8000 ) >> 8 );
        int FirstVertex = x * (DMA_GridSize+1) + y;
        // Another twist - we want the top-left and bottom-right quadrants to
        // have the triangles split one way, the other two split the other.
        // +---+---+---+---+
        // |  /|  /|\  |\  |
        // | / | / | \ | \ |
        // |/  |/  |  \|  \|
        // +---+---+---+---+
        // |  /|  /|\  |\  |
        // | / | / | \ | \ |
        // |/  |/  |  \|  \|
        // +---+---+---+---+
        // |\  |\  |  /|  /|
        // | \ | \ | / | / |
        // |  \|  \|/  |/  |
        // +---+---+---+---+
        // |\  |\  |  /|  /|
        // | \ | \ | / | / |
        // |  \|  \|/  |/  |
        // +---+---+---+---+
        // This way triangle edges don't span long distances over the distortion function,
        // so linear interpolation works better & we can use fewer tris.
        if ( ( x < DMA_GridSize/2 ) != ( y < DMA_GridSize/2 ) )       // != is logical XOR
        {
            *pcurIndex++ = (uint16_t)FirstVertex;
            *pcurIndex++ = (uint16_t)FirstVertex+1;
            *pcurIndex++ = (uint16_t)FirstVertex+(DMA_GridSize+1)+1;

            *pcurIndex++ = (uint16_t)FirstVertex+(DMA_GridSize+1)+1;
            *pcurIndex++ = (uint16_t)FirstVertex+(DMA_GridSize+1);
            *pcurIndex++ = (uint16_t)FirstVertex;
        }
        else
        {
            *pcurIndex++ = (uint16_t)FirstVertex;
            *pcurIndex++ = (uint16_t)FirstVertex+1;
            *pcurIndex++ = (uint16_t)FirstVertex+(DMA_GridSize+1);

            *pcurIndex++ = (uint16_t)FirstVertex+1;
            *pcurIndex++ = (uint16_t)FirstVertex+(DMA_GridSize+1)+1;
            *pcurIndex++ = (uint16_t)FirstVertex+(DMA_GridSize+1);
        }
    }
}

//-----------------------------------------------------------------------------------
// *****  Heightmap Mesh Rendering


static const int HMA_GridSizeLog2   = 7;
static const int HMA_GridSize       = 1<<HMA_GridSizeLog2;
static const int HMA_NumVertsPerEye = (HMA_GridSize+1)*(HMA_GridSize+1);
static const int HMA_NumTrisPerEye  = (HMA_GridSize)*(HMA_GridSize)*2;


void HeightmapMeshDestroy ( HeightmapMeshVertexData *pVertices, uint16_t *pTriangleMeshIndices )
{
    OVR_FREE ( pVertices );
    OVR_FREE ( pTriangleMeshIndices );
}

void HeightmapMeshCreate ( HeightmapMeshVertexData **ppVertices, uint16_t **ppTriangleListIndices,
    int *pNumVertices, int *pNumTriangles,
    const StereoEyeParams &stereoParams, const HmdRenderInfo &hmdRenderInfo )
{
    bool    rightEye      = ( stereoParams.Eye == StereoEye_Right );
    int     vertexCount   = 0;
    int     triangleCount = 0;

    // Generate mesh into allocated data and return result.
    HeightmapMeshCreate(ppVertices, ppTriangleListIndices, &vertexCount, &triangleCount,
        rightEye, hmdRenderInfo, stereoParams.EyeToSourceNDC);

    *pNumVertices  = vertexCount;
    *pNumTriangles = triangleCount;
}


// Generate heightmap mesh for one eye.
void HeightmapMeshCreate( HeightmapMeshVertexData **ppVertices, uint16_t **ppTriangleListIndices,
    int *pNumVertices, int *pNumTriangles, bool rightEye,
    const HmdRenderInfo &hmdRenderInfo,
    const ScaleAndOffset2D &eyeToSourceNDC )
{
    *pNumVertices  = HMA_NumVertsPerEye;
    *pNumTriangles = HMA_NumTrisPerEye;

    *ppVertices = (HeightmapMeshVertexData*) OVR_ALLOC( sizeof(HeightmapMeshVertexData) * (*pNumVertices) );
    *ppTriangleListIndices  = (uint16_t*) OVR_ALLOC( sizeof(uint16_t) * (*pNumTriangles) * 3 );

    if (!*ppVertices || !*ppTriangleListIndices)
    {
        if (*ppVertices)
        {
            OVR_FREE(*ppVertices);
        }
        if (*ppTriangleListIndices)
        {
            OVR_FREE(*ppTriangleListIndices);
        }
        *ppVertices             = NULL;
        *ppTriangleListIndices  = NULL;
        *pNumTriangles          = 0;
        *pNumVertices           = 0;
        return;
    }

    // Populate vertex buffer info
    //float xOffset = 0.0f;  Not currently used.
    //float uOffset = 0.0f;

    //if (rightEye)
    //{
    //    xOffset = 1.0f;
    //    uOffset = 0.5f;
    //}

    // First pass - build up raw vertex data.
    HeightmapMeshVertexData* pcurVert = *ppVertices;

    for ( int y = 0; y <= HMA_GridSize; y++ )
    {
        for ( int x = 0; x <= HMA_GridSize; x++ )
        {
            Vector2f sourceCoordNDC;
            // NDC texture coords [-1,+1]
            sourceCoordNDC.x = 2.0f * ( (float)x / (float)HMA_GridSize ) - 1.0f;
            sourceCoordNDC.y = 2.0f * ( (float)y / (float)HMA_GridSize ) - 1.0f;
            Vector2f tanEyeAngle = TransformRendertargetNDCToTanFovSpace ( eyeToSourceNDC, sourceCoordNDC );
            
            pcurVert->TanEyeAngles = tanEyeAngle;

            HmdShutterTypeEnum shutterType = hmdRenderInfo.Shutter.Type;
            switch ( shutterType )
            {
            case HmdShutter_Global:
                pcurVert->TimewarpLerp = 0.0f;
                break;
            case HmdShutter_RollingLeftToRight:
                // Retrace is left to right - left eye goes 0.0 -> 0.5, then right goes 0.5 -> 1.0
                pcurVert->TimewarpLerp = sourceCoordNDC.x * 0.25f + 0.25f;
                if (rightEye)
                {
                    pcurVert->TimewarpLerp += 0.5f;
                }
                break;
            case HmdShutter_RollingRightToLeft:
                // Retrace is right to left - right eye goes 0.0 -> 0.5, then left goes 0.5 -> 1.0
                pcurVert->TimewarpLerp = 0.75f - sourceCoordNDC.x * 0.25f;
                if (rightEye)
                {
                    pcurVert->TimewarpLerp -= 0.5f;
                }
                break;
            case HmdShutter_RollingTopToBottom:
                // Retrace is top to bottom on both eyes at the same time.
                pcurVert->TimewarpLerp = sourceCoordNDC.y * 0.5f + 0.5f;
                break;
            default: OVR_ASSERT ( false ); break;
            }

            // Don't let verts overlap to the other eye.
            //sourceCoordNDC.x = Alg::Max ( -1.0f, Alg::Min ( sourceCoordNDC.x, 1.0f ) );
            //sourceCoordNDC.y = Alg::Max ( -1.0f, Alg::Min ( sourceCoordNDC.y, 1.0f ) );

            //pcurVert->ScreenPosNDC.x = 0.5f * sourceCoordNDC.x - 0.5f + xOffset;
            pcurVert->ScreenPosNDC.x = sourceCoordNDC.x;
            pcurVert->ScreenPosNDC.y = -sourceCoordNDC.y;

            pcurVert++;
        }
    }


    // Populate index buffer info  
    uint16_t *pcurIndex = *ppTriangleListIndices;

    for ( int triNum = 0; triNum < HMA_GridSize * HMA_GridSize; triNum++ )
    {
        // Use a Morton order to help locality of FB, texture and vertex cache.
        // (0.325ms raster order -> 0.257ms Morton order)
        OVR_ASSERT ( HMA_GridSize < 256 );
        int x = ( ( triNum & 0x0001 ) >> 0 ) |
                ( ( triNum & 0x0004 ) >> 1 ) |
                ( ( triNum & 0x0010 ) >> 2 ) |
                ( ( triNum & 0x0040 ) >> 3 ) |
                ( ( triNum & 0x0100 ) >> 4 ) |
                ( ( triNum & 0x0400 ) >> 5 ) |
                ( ( triNum & 0x1000 ) >> 6 ) |
                ( ( triNum & 0x4000 ) >> 7 );
        int y = ( ( triNum & 0x0002 ) >> 1 ) |
                ( ( triNum & 0x0008 ) >> 2 ) |
                ( ( triNum & 0x0020 ) >> 3 ) |
                ( ( triNum & 0x0080 ) >> 4 ) |
                ( ( triNum & 0x0200 ) >> 5 ) |
                ( ( triNum & 0x0800 ) >> 6 ) |
                ( ( triNum & 0x2000 ) >> 7 ) |
                ( ( triNum & 0x8000 ) >> 8 );
        int FirstVertex = x * (HMA_GridSize+1) + y;
        // Another twist - we want the top-left and bottom-right quadrants to
        // have the triangles split one way, the other two split the other.
        // +---+---+---+---+
        // |  /|  /|\  |\  |
        // | / | / | \ | \ |
        // |/  |/  |  \|  \|
        // +---+---+---+---+
        // |  /|  /|\  |\  |
        // | / | / | \ | \ |
        // |/  |/  |  \|  \|
        // +---+---+---+---+
        // |\  |\  |  /|  /|
        // | \ | \ | / | / |
        // |  \|  \|/  |/  |
        // +---+---+---+---+
        // |\  |\  |  /|  /|
        // | \ | \ | / | / |
        // |  \|  \|/  |/  |
        // +---+---+---+---+
        // This way triangle edges don't span long distances over the distortion function,
        // so linear interpolation works better & we can use fewer tris.
        if ( ( x < HMA_GridSize/2 ) != ( y < HMA_GridSize/2 ) )       // != is logical XOR
        {
            *pcurIndex++ = (uint16_t)FirstVertex;
            *pcurIndex++ = (uint16_t)FirstVertex+1;
            *pcurIndex++ = (uint16_t)FirstVertex+(HMA_GridSize+1)+1;

            *pcurIndex++ = (uint16_t)FirstVertex+(HMA_GridSize+1)+1;
            *pcurIndex++ = (uint16_t)FirstVertex+(HMA_GridSize+1);
            *pcurIndex++ = (uint16_t)FirstVertex;
        }
        else
        {
            *pcurIndex++ = (uint16_t)FirstVertex;
            *pcurIndex++ = (uint16_t)FirstVertex+1;
            *pcurIndex++ = (uint16_t)FirstVertex+(HMA_GridSize+1);

            *pcurIndex++ = (uint16_t)FirstVertex+1;
            *pcurIndex++ = (uint16_t)FirstVertex+(HMA_GridSize+1)+1;
            *pcurIndex++ = (uint16_t)FirstVertex+(HMA_GridSize+1);
        }
    }
}

//-----------------------------------------------------------------------------------
// ***** Prediction and timewarp.
//

// Calculates the values from the HMD info.
PredictionValues PredictionGetDeviceValues ( const HmdRenderInfo &hmdRenderInfo,
                                             bool withTimewarp /*= true*/,
                                             bool withVsync /*= true*/ )
{
    PredictionValues result;

    result.WithTimewarp = withTimewarp;
    result.WithVsync = withVsync;

    // For unclear reasons, most graphics systems add an extra frame of latency
    // somewhere along the way. In time we'll debug this and figure it out, but
    // for now this gets prediction a little bit better.
    const float extraFramesOfBufferingKludge = 1.0f;

    if ( withVsync )
    {
        // These are the times from the Present+Flush to when the middle of the scene is "averagely visible" (without timewarp)
        // So if you had no timewarp, this, plus the time until the next vsync, is how much to predict by.
        result.PresentFlushToRenderedScene  = extraFramesOfBufferingKludge * hmdRenderInfo.Shutter.FirstScanlineToLastScanline;
        // Predict to the middle of the screen being scanned out.
        result.PresentFlushToRenderedScene += hmdRenderInfo.Shutter.VsyncToFirstScanline + 0.5f * hmdRenderInfo.Shutter.FirstScanlineToLastScanline;
        // Time for pixels to get half-way to settling.
        result.PresentFlushToRenderedScene += hmdRenderInfo.Shutter.PixelSettleTime * 0.5f;
        // Predict to half-way through persistence
        result.PresentFlushToRenderedScene += hmdRenderInfo.Shutter.PixelPersistence * 0.5f;

        // The time from the Present+Flush to when the first scanline is "averagely visible".
        result.PresentFlushToTimewarpStart  = extraFramesOfBufferingKludge * hmdRenderInfo.Shutter.FirstScanlineToLastScanline;
        // Predict to the first line being scanned out.
        result.PresentFlushToTimewarpStart += hmdRenderInfo.Shutter.VsyncToFirstScanline;
        // Time for pixels to get half-way to settling.
        result.PresentFlushToTimewarpStart += hmdRenderInfo.Shutter.PixelSettleTime * 0.5f;
        // Predict to half-way through persistence
        result.PresentFlushToTimewarpStart += hmdRenderInfo.Shutter.PixelPersistence * 0.5f;

        // Time to the the last scanline.
        result.PresentFlushToTimewarpEnd    = result.PresentFlushToTimewarpStart + hmdRenderInfo.Shutter.FirstScanlineToLastScanline;

        // Ideal framerate.
        result.PresentFlushToPresentFlush   = hmdRenderInfo.Shutter.VsyncToNextVsync;
    }
    else
    {
        // Timewarp without vsync is a little odd.
        // Currently, we assume that without vsync, we have no idea which scanline
        // is currently being sent to the display. So we can't do lerping timewarp,
        // we can just do a full-screen late-stage fixup.

        // "PresentFlushToRenderedScene" means the time from the Present+Flush to when the middle of the scene is "averagely visible" (without timewarp)
        // So if you had no timewarp, this, plus the time until the next flush (which is usually the time to render the frame), is how much to predict by.
        // Time for pixels to get half-way to settling.
        result.PresentFlushToRenderedScene  = hmdRenderInfo.Shutter.PixelSettleTime * 0.5f;
        // Predict to half-way through persistence
        result.PresentFlushToRenderedScene += hmdRenderInfo.Shutter.PixelPersistence * 0.5f;

        // Without vsync, you don't know timings, and so can't do anything useful with lerped warping.
        result.PresentFlushToTimewarpStart  = result.PresentFlushToRenderedScene;
        result.PresentFlushToTimewarpEnd    = result.PresentFlushToRenderedScene;

        // There's no concept of "ideal" when vsync is off.
        result.PresentFlushToPresentFlush   = 0.0f;
    }

    return result;
}

Matrix4f TimewarpComputePoseDelta ( Matrix4f const &renderedViewFromWorld, Matrix4f const &predictedViewFromWorld, Matrix4f const&eyeViewAdjust )
{
    Matrix4f worldFromPredictedView = (eyeViewAdjust * predictedViewFromWorld).InvertedHomogeneousTransform();
    Matrix4f matRenderFromNowStart = (eyeViewAdjust * renderedViewFromWorld) * worldFromPredictedView;

    // The sensor-predicted orientations have:                           X=right, Y=up,   Z=backwards.
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
    matRenderFromNowStart.M[0][1] = -matRenderFromNowStart.M[0][1];
    matRenderFromNowStart.M[0][2] = -matRenderFromNowStart.M[0][2];
    matRenderFromNowStart.M[1][0] = -matRenderFromNowStart.M[1][0];
    matRenderFromNowStart.M[2][0] = -matRenderFromNowStart.M[2][0];
    matRenderFromNowStart.M[1][3] = -matRenderFromNowStart.M[1][3];
    matRenderFromNowStart.M[2][3] = -matRenderFromNowStart.M[2][3];

    return matRenderFromNowStart;
}

Matrix4f TimewarpComputePoseDeltaPosition ( Matrix4f const &renderedViewFromWorld, Matrix4f const &predictedViewFromWorld, Matrix4f const&eyeViewAdjust )
{
    Matrix4f worldFromPredictedView = (eyeViewAdjust * predictedViewFromWorld).InvertedHomogeneousTransform();
    Matrix4f matRenderXform = (eyeViewAdjust * renderedViewFromWorld) * worldFromPredictedView;

    return matRenderXform.Inverted();
}

TimewarpMachine::TimewarpMachine()
{    
    for ( int i = 0; i < 2; i++ )
    {
        EyeRenderPoses[i] = Posef();
    }
    DistortionTimeCount = 0;
    VsyncEnabled = false;
}

void TimewarpMachine::Reset(HmdRenderInfo& renderInfo, bool vsyncEnabled, double timeNow)
{
    RenderInfo = renderInfo;
    VsyncEnabled = vsyncEnabled;
    CurrentPredictionValues = PredictionGetDeviceValues ( renderInfo, true, VsyncEnabled );
    PresentFlushToPresentFlushSeconds = 0.0f;
    DistortionTimeCount = 0;
    DistortionTimeAverage = 0.0f;
    LastFramePresentFlushTime = timeNow;
    AfterPresentAndFlush(timeNow);
}

void TimewarpMachine::AfterPresentAndFlush(double timeNow)
{
    PresentFlushToPresentFlushSeconds = (float)(timeNow - LastFramePresentFlushTime);
    LastFramePresentFlushTime = timeNow;
    NextFramePresentFlushTime = timeNow + (double)PresentFlushToPresentFlushSeconds;
}

double TimewarpMachine::GetViewRenderPredictionTime()
{
    // Note that PredictionGetDeviceValues() did all the vsync-dependent thinking for us.
    return NextFramePresentFlushTime + CurrentPredictionValues.PresentFlushToRenderedScene;
}

bool TimewarpMachine::GetViewRenderPredictionPose(SensorStateReader* reader, Posef& pose)
{
	return reader->GetPoseAtTime(GetViewRenderPredictionTime(), pose);
}

double TimewarpMachine::GetVisiblePixelTimeStart()
{
    // Note that PredictionGetDeviceValues() did all the vsync-dependent thinking for us.
    return NextFramePresentFlushTime + CurrentPredictionValues.PresentFlushToTimewarpStart;
}
double TimewarpMachine::GetVisiblePixelTimeEnd()
{
    // Note that PredictionGetDeviceValues() did all the vsync-dependent thinking for us.
    return NextFramePresentFlushTime + CurrentPredictionValues.PresentFlushToTimewarpEnd;
}
bool TimewarpMachine::GetPredictedVisiblePixelPoseStart(SensorStateReader* reader, Posef& pose)
{
	return reader->GetPoseAtTime(GetVisiblePixelTimeStart(), pose);
}
bool TimewarpMachine::GetPredictedVisiblePixelPoseEnd(SensorStateReader* reader, Posef& pose)
{
	return reader->GetPoseAtTime(GetVisiblePixelTimeEnd(), pose);
}
bool TimewarpMachine::GetTimewarpDeltaStart(SensorStateReader* reader, Posef const &renderedPose, Matrix4f& transform)
{
	Posef visiblePose;
	if (!GetPredictedVisiblePixelPoseStart(reader, visiblePose))
	{
		return false;
	}

    Matrix4f visibleMatrix(visiblePose);
    Matrix4f renderedMatrix(renderedPose);
    Matrix4f identity;  // doesn't matter for orientation-only timewarp
    transform = TimewarpComputePoseDelta ( renderedMatrix, visibleMatrix, identity );

	return true;
}
bool TimewarpMachine::GetTimewarpDeltaEnd(SensorStateReader* reader, Posef const &renderedPose, Matrix4f& transform)
{
	Posef visiblePose;
	if (!GetPredictedVisiblePixelPoseEnd(reader, visiblePose))
	{
		return false;
	}

    Matrix4f visibleMatrix(visiblePose);
    Matrix4f renderedMatrix(renderedPose);
    Matrix4f identity;  // doesn't matter for orientation-only timewarp
    transform = TimewarpComputePoseDelta ( renderedMatrix, visibleMatrix, identity );

	return true;
}


// What time should the app wait until before starting distortion?
double  TimewarpMachine::JustInTime_GetDistortionWaitUntilTime()
{
    if ( !VsyncEnabled || ( DistortionTimeCount < NumDistortionTimes ) )
    {
        // Don't wait.
        return LastFramePresentFlushTime;
    }

    const float fudgeFactor = 0.002f;      // Found heuristically - 1ms is too short because of timing granularity - may need further tweaking!
    float howLongBeforePresent = DistortionTimeAverage + fudgeFactor;
    // Subtlety here. Technically, the correct time is NextFramePresentFlushTime - howLongBeforePresent.
    // However, if the app drops a frame, this then perpetuates it,
    // i.e. if the display is running at 60fps, but the last frame was slow,
    // (e.g. because of swapping or whatever), then NextFramePresentFlushTime is
    // 33ms in the future, not 16ms. Since this function supplies the 
    // time to wait until, the app will indeed wait until 32ms, so the framerate
    // drops to 30fps and never comes back up!
    // So we return the *ideal* framerate, not the *actual* framerate.
    return LastFramePresentFlushTime + (float)( CurrentPredictionValues.PresentFlushToPresentFlush - howLongBeforePresent );
}


bool    TimewarpMachine::JustInTime_NeedDistortionTimeMeasurement() const
{
    if (!VsyncEnabled)
    {
        return false;
    }
    return ( DistortionTimeCount < NumDistortionTimes );
}

void    TimewarpMachine::JustInTime_BeforeDistortionTimeMeasurement(double timeNow)
{
    DistortionTimeCurrentStart = timeNow;
}

void    TimewarpMachine::JustInTime_AfterDistortionTimeMeasurement(double timeNow)
{
    float timeDelta = (float)( timeNow - DistortionTimeCurrentStart );
    if ( DistortionTimeCount < NumDistortionTimes )
    {
        DistortionTimes[DistortionTimeCount] = timeDelta;
        DistortionTimeCount++;
        if ( DistortionTimeCount == NumDistortionTimes )
        {
            // Median.
            float distortionTimeMedian = 0.0f;
            for ( int i = 0; i < NumDistortionTimes/2; i++ )
            {
                // Find the maximum time of those remaining.
                float maxTime = DistortionTimes[0];
                int maxIndex = 0;
                for ( int j = 1; j < NumDistortionTimes; j++ )
                {
                    if ( maxTime < DistortionTimes[j] )
                    {
                        maxTime = DistortionTimes[j];
                        maxIndex = j;
                    }
                }
                // Zero that max time, so we'll find the next-highest time.
                DistortionTimes[maxIndex] = 0.0f;
                distortionTimeMedian = maxTime;
            }
            DistortionTimeAverage = distortionTimeMedian;
        }
    }
    else
    {
        OVR_ASSERT ( !"Really didn't need more measurements, thanks" );
    }
}


}}}  // OVR::Util::Render

