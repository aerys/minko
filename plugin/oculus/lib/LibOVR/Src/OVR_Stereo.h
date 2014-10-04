/************************************************************************************

Filename    :   OVR_Stereo.h
Content     :   Stereo rendering functions
Created     :   November 30, 2013
Authors     :   Tom Fosyth

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

#ifndef OVR_Stereo_h
#define OVR_Stereo_h

#include "Sensors/OVR_DeviceConstants.h"
#include "Displays/OVR_Display.h"
#include "OVR_Profile.h"

// CAPI Forward declaration.
typedef struct ovrFovPort_ ovrFovPort;
typedef struct ovrRecti_ ovrRecti;

namespace OVR {

class SensorDevice; // Opaque forward declaration


//-----------------------------------------------------------------------------------
// ***** Stereo Enumerations

// StereoEye specifies which eye we are rendering for; it is used to
// retrieve StereoEyeParams.
enum StereoEye
{
    StereoEye_Center,
    StereoEye_Left,
    StereoEye_Right    
};


//-----------------------------------------------------------------------------------
// ***** FovPort

// FovPort describes Field Of View (FOV) of a viewport.
// This class has values for up, down, left and right, stored in 
// tangent of the angle units to simplify calculations.
//
// As an example, for a standard 90 degree vertical FOV, we would 
// have: { UpTan = tan(90 degrees / 2), DownTan = tan(90 degrees / 2) }.
//
// CreateFromRadians/Degrees helper functions can be used to
// access FOV in different units.

struct FovPort
{
    float UpTan;
    float DownTan;
    float LeftTan;
    float RightTan;

    FovPort ( float sideTan = 0.0f ) :
        UpTan(sideTan), DownTan(sideTan), LeftTan(sideTan), RightTan(sideTan) { }
    FovPort ( float u, float d, float l, float r ) :
        UpTan(u), DownTan(d), LeftTan(l), RightTan(r) { }

    // C-interop support: FovPort <-> ovrFovPort (implementation in OVR_CAPI.cpp).
    FovPort(const ovrFovPort& src);
    operator ovrFovPort () const;

    static FovPort CreateFromRadians(float horizontalFov, float verticalFov)
    {
        FovPort result;
        result.UpTan    = tanf (   verticalFov * 0.5f );
        result.DownTan  = tanf (   verticalFov * 0.5f );
        result.LeftTan  = tanf ( horizontalFov * 0.5f );
        result.RightTan = tanf ( horizontalFov * 0.5f );
        return result;
    }

    static FovPort CreateFromDegrees(float horizontalFovDegrees,
                                     float verticalFovDegrees)
    {
        return CreateFromRadians(DegreeToRad(horizontalFovDegrees),
                                 DegreeToRad(verticalFovDegrees));
    }

    //  Get Horizontal/Vertical components of Fov in radians.
    float GetVerticalFovRadians() const     { return atanf(UpTan)    + atanf(DownTan); }
    float GetHorizontalFovRadians() const   { return atanf(LeftTan)  + atanf(RightTan); }
    //  Get Horizontal/Vertical components of Fov in degrees.
    float GetVerticalFovDegrees() const     { return RadToDegree(GetVerticalFovRadians()); }
    float GetHorizontalFovDegrees() const   { return RadToDegree(GetHorizontalFovRadians()); }

    // Compute maximum tangent value among all four sides.
    float GetMaxSideTan() const
    {
        return Alg::Max(Alg::Max(UpTan, DownTan), Alg::Max(LeftTan, RightTan));
    }

    // Converts Fov Tan angle units to [-1,1] render target NDC space
    Vector2f TanAngleToRendertargetNDC(Vector2f const &tanEyeAngle);


    // Compute per-channel minimum and maximum of Fov.
    static FovPort Min(const FovPort& a, const FovPort& b)
    {   
        FovPort fov( Alg::Min( a.UpTan   , b.UpTan    ),   
                     Alg::Min( a.DownTan , b.DownTan  ),
                     Alg::Min( a.LeftTan , b.LeftTan  ),
                     Alg::Min( a.RightTan, b.RightTan ) );
        return fov;
    }

    static FovPort Max(const FovPort& a, const FovPort& b)
    {   
        FovPort fov( Alg::Max( a.UpTan   , b.UpTan    ),   
                     Alg::Max( a.DownTan , b.DownTan  ),
                     Alg::Max( a.LeftTan , b.LeftTan  ),
                     Alg::Max( a.RightTan, b.RightTan ) );
        return fov;
    }
};


//-----------------------------------------------------------------------------------
// ***** ScaleAndOffset

struct ScaleAndOffset2D
{
    Vector2f Scale;
    Vector2f Offset;

    ScaleAndOffset2D(float sx = 0.0f, float sy = 0.0f, float ox = 0.0f, float oy = 0.0f)
      : Scale(sx, sy), Offset(ox, oy)        
    { }
};


//-----------------------------------------------------------------------------------
// ***** Misc. utility functions.

// Inputs are 4 points (pFitX[0],pFitY[0]) through (pFitX[3],pFitY[3])
// Result is four coefficients in pResults[0] through pResults[3] such that
//      y = pResult[0] + x * ( pResult[1] + x * ( pResult[2] + x * ( pResult[3] ) ) );
// passes through all four input points.
// Return is true if it succeeded, false if it failed (because two control points
// have the same pFitX value).
bool FitCubicPolynomial ( float *pResult, const float *pFitX, const float *pFitY );

//-----------------------------------------------------------------------------------
// ***** LensConfig

// LensConfig describes the configuration of a single lens in an HMD.
// - Eqn and K[] describe a distortion function.
// - MetersPerTanAngleAtCenter is the relationship between distance on a
//   screen (at the center of the lens), and the angle variance of the light after it
//   has passed through the lens.
// - ChromaticAberration is an array of parameters for controlling
//   additional Red and Blue scaling in order to reduce chromatic aberration
//   caused by the Rift lenses.
struct LensConfig
{
    // The result is a scaling applied to the distance from the center of the lens.
    float    DistortionFnScaleRadiusSquared (float rsq) const;
    // x,y,z components map to r,g,b scales.
    Vector3f DistortionFnScaleRadiusSquaredChroma (float rsq) const;

    // DistortionFn applies distortion to the argument.
    // Input: the distance in TanAngle/NIC space from the optical center to the input pixel.
    // Output: the resulting distance after distortion.
    float DistortionFn(float r) const
    {
        return r * DistortionFnScaleRadiusSquared ( r * r );
    }

    // DistortionFnInverse computes the inverse of the distortion function on an argument.
    float DistortionFnInverse(float r) const;

    // Also computes the inverse, but using a polynomial approximation. Warning - it's just an approximation!
    float DistortionFnInverseApprox(float r) const;
    // Sets up InvK[].
    void SetUpInverseApprox();

    // Sets a bunch of sensible defaults.
    void SetToIdentity();



    enum { NumCoefficients = 11 };

    DistortionEqnType   Eqn;
    float               K[NumCoefficients];
    float               MaxR;       // The highest R you're going to query for - the curve is unpredictable beyond it.

    float               MetersPerTanAngleAtCenter;

    // Additional per-channel scaling is applied after distortion:
    //  Index [0] - Red channel constant coefficient.
    //  Index [1] - Red channel r^2 coefficient.
    //  Index [2] - Blue channel constant coefficient.
    //  Index [3] - Blue channel r^2 coefficient.
    float               ChromaticAberration[4];

    float               InvK[NumCoefficients];
    float               MaxInvR;
};


// For internal use - storing and loading lens config data

// Returns true on success.
bool LoadLensConfig ( LensConfig *presult, uint8_t const *pbuffer, int bufferSizeInBytes );

// Returns number of bytes needed.
int SaveLensConfigSizeInBytes ( LensConfig const &config );
// Returns true on success.
bool SaveLensConfig ( uint8_t *pbuffer, int bufferSizeInBytes, LensConfig const &config );


//-----------------------------------------------------------------------------------
// ***** DistortionRenderDesc

// This describes distortion for a single eye in an HMD with a display, not just the lens by itself.
struct DistortionRenderDesc
{
    // The raw lens values.
    LensConfig          Lens;

    // These map from [-1,1] across the eye being rendered into TanEyeAngle space (but still distorted)
    Vector2f            LensCenter;
    Vector2f            TanEyeAngleScale;
    // Computed from device characteristics, IPD and eye-relief.
    // (not directly used for rendering, but very useful)
    Vector2f            PixelsPerTanAngleAtCenter;
};


//-------------------------------------------------------------------------------------
// ***** HMDInfo 

// This structure describes various aspects of the HMD allowing us to configure rendering.
//
//  Currently included data:
//   - Physical screen dimensions, resolution, and eye distances.
//     (some of these will be configurable with a tool in the future).
//     These arguments allow us to properly setup projection across HMDs.
//   - DisplayDeviceName for identifying HMD screen; system-specific interpretation.
//
// TBD:
//  - Power on/ off?
//  - Sensor rates and capabilities
//  - Distortion radius/variables    
//  - Screen update frequency
//  - Distortion needed flag
//  - Update modes:
//      Set update mode: Stereo (both sides together), mono (same in both eyes),
//                       Alternating, Alternating scan-lines.

// Win32 Oculus VR Display Driver Shim Information
struct Win32ShimInfo
{
	int DeviceNumber;
	int NativeWidth;
	int NativeHeight;
	int Rotation;
	int UseMirroring;

	Win32ShimInfo() :
		DeviceNumber(-1),
		NativeWidth(-1),
		NativeHeight(-1),
		Rotation(-1),
		UseMirroring(1)
	{
	}
};

class HMDInfo
{
public:
	// Name string describing the product: "Oculus Rift DK1", etc.
	String      ProductName;
	String      Manufacturer;

	unsigned    Version;

	// Characteristics of the HMD screen and enclosure
	HmdTypeEnum HmdType;
	Size<int>   ResolutionInPixels;
	Size<float> ScreenSizeInMeters;
	float       ScreenGapSizeInMeters;
	float       CenterFromTopInMeters;
	float       LensSeparationInMeters;

	// Timing & shutter data. All values in seconds.
	struct ShutterInfo
	{
		HmdShutterTypeEnum  Type;
		float   VsyncToNextVsync;                // 1/framerate
		float   VsyncToFirstScanline;            // for global shutter, vsync->shutter open.
		float   FirstScanlineToLastScanline;     // for global shutter, will be zero.
		float   PixelSettleTime;                 // estimated.
		float   PixelPersistence;                // Full persistence = 1/framerate.
	}           Shutter;

	// Desktop coordinate position of the screen (can be negative; may not be present on all platforms)
	int         DesktopX;
	int         DesktopY;

	// Windows:
	// "\\\\.\\DISPLAY3", etc. Can be used in EnumDisplaySettings/CreateDC.
	String      DisplayDeviceName;
	Win32ShimInfo ShimInfo;

	// MacOS:
	int         DisplayId;

	bool	    InCompatibilityMode;

	// Printed serial number for the HMD; should match external sticker
    String      PrintedSerial;

    // Tracker descriptor information:
    int         VendorId;
    int         ProductId;
    int         FirmwareMajor;
    int         FirmwareMinor;

    float   CameraFrustumHFovInRadians;
    float   CameraFrustumVFovInRadians;
    float   CameraFrustumNearZInMeters;
    float   CameraFrustumFarZInMeters;

	// Constructor initializes all values to 0s.
	// To create a "virtualized" HMDInfo, use CreateDebugHMDInfo instead.
	HMDInfo() :
		Version(0),
		HmdType(HmdType_None),
		ResolutionInPixels(0),
		ScreenSizeInMeters(0.0f),
		ScreenGapSizeInMeters(0.0f),
		CenterFromTopInMeters(0),
		LensSeparationInMeters(0),
		DisplayId(-1),
		InCompatibilityMode(false)
	{
		DesktopX = 0;
		DesktopY = 0;
		Shutter.Type = HmdShutter_LAST;
		Shutter.VsyncToNextVsync = 0.0f;
		Shutter.VsyncToFirstScanline = 0.0f;
		Shutter.FirstScanlineToLastScanline = 0.0f;
		Shutter.PixelSettleTime = 0.0f;
		Shutter.PixelPersistence = 0.0f;

        CameraFrustumHFovInRadians = 0;
        CameraFrustumVFovInRadians = 0;
        CameraFrustumNearZInMeters = 0;
        CameraFrustumFarZInMeters = 0;
    }

	// Operator = copies local fields only (base class must be correct already)
	void operator=(const HMDInfo& src)
	{
		ProductName = src.ProductName;
		Manufacturer = src.Manufacturer;
		Version = src.Version;
		HmdType = src.HmdType;
		ResolutionInPixels = src.ResolutionInPixels;
		ScreenSizeInMeters = src.ScreenSizeInMeters;
		ScreenGapSizeInMeters = src.ScreenGapSizeInMeters;
		CenterFromTopInMeters = src.CenterFromTopInMeters;
		LensSeparationInMeters = src.LensSeparationInMeters;
		DesktopX = src.DesktopX;
		DesktopY = src.DesktopY;
		Shutter = src.Shutter;
		DisplayDeviceName = src.DisplayDeviceName;
		ShimInfo = src.ShimInfo;
		DisplayId = src.DisplayId;
		InCompatibilityMode = src.InCompatibilityMode;
        VendorId = src.VendorId;
        ProductId = src.ProductId;
        FirmwareMajor = src.FirmwareMajor;
        FirmwareMinor = src.FirmwareMinor;
        PrintedSerial = src.PrintedSerial;
        CameraFrustumHFovInRadians = src.CameraFrustumHFovInRadians;
        CameraFrustumVFovInRadians = src.CameraFrustumVFovInRadians;
        CameraFrustumNearZInMeters = src.CameraFrustumNearZInMeters;
        CameraFrustumFarZInMeters = src.CameraFrustumFarZInMeters;
    }

	void SetScreenParameters(int hres, int vres,
							 float hsize, float vsize,
							 float vCenterFromTopInMeters, float lensSeparationInMeters,
							 bool compatibilityMode)
	{
		ResolutionInPixels = Sizei(hres, vres);
		ScreenSizeInMeters = Sizef(hsize, vsize);
		CenterFromTopInMeters = vCenterFromTopInMeters;
		LensSeparationInMeters = lensSeparationInMeters;
		InCompatibilityMode = compatibilityMode;
	}

	bool IsSameDisplay(const HMDInfo& o) const
	{
		return DisplayId == o.DisplayId &&
			DisplayDeviceName.CompareNoCase(o.DisplayDeviceName) == 0;
	}

	static bool CreateFromSensorAndDisplay(SensorDevice* sensor, Display* display, HMDInfo* hmdi);
};


//-----------------------------------------------------------------------------------
// ***** HmdRenderInfo

// All the parts of the HMD info that are needed to set up the rendering system.

struct HmdRenderInfo
{
    // The start of this structure is intentionally very similar to HMDInfo in OVER_Device.h
    // However to reduce interdependencies, one does not simply #include the other.

    HmdTypeEnum HmdType;

    // Size of the entire screen
    Size<int>   ResolutionInPixels;
    Size<float> ScreenSizeInMeters;
    float       ScreenGapSizeInMeters;

    // Characteristics of the lenses.
    float       CenterFromTopInMeters;
    float       LensSeparationInMeters;
    float       LensDiameterInMeters;
    float       LensSurfaceToMidplateInMeters;
    EyeCupType  EyeCups;

    // Timing & shutter data. All values in seconds.
    struct ShutterInfo
    {
        HmdShutterTypeEnum  Type;
        float               VsyncToNextVsync;                // 1/framerate
        float               VsyncToFirstScanline;            // for global shutter, vsync->shutter open.
        float               FirstScanlineToLastScanline;     // for global shutter, will be zero.
        float               PixelSettleTime;                 // estimated.
        float               PixelPersistence;                // Full persistence = 1/framerate.
    }           Shutter;


    // These are all set from the user's profile.
    struct EyeConfig
    {
        // Distance from center of eyeball to front plane of lens.
        float               ReliefInMeters;
        // Distance from nose (technically, center of Rift) to the middle of the eye.
        float               NoseToPupilInMeters;

        LensConfig          Distortion;
    } EyeLeft, EyeRight;


    HmdRenderInfo()
    {
        HmdType = HmdType_None;
        ResolutionInPixels.w = 0;
        ResolutionInPixels.h = 0;
        ScreenSizeInMeters.w = 0.0f;
        ScreenSizeInMeters.h = 0.0f;
        ScreenGapSizeInMeters = 0.0f;
        CenterFromTopInMeters = 0.0f;
        LensSeparationInMeters = 0.0f;
        LensDiameterInMeters = 0.0f;
        LensSurfaceToMidplateInMeters = 0.0f;
        Shutter.Type = HmdShutter_LAST;
        Shutter.VsyncToNextVsync = 0.0f;
        Shutter.VsyncToFirstScanline = 0.0f;
        Shutter.FirstScanlineToLastScanline = 0.0f;
        Shutter.PixelSettleTime = 0.0f;
        Shutter.PixelPersistence = 0.0f;
        EyeCups = EyeCup_DK1A;
        EyeLeft.ReliefInMeters = 0.0f;
        EyeLeft.NoseToPupilInMeters = 0.0f;
        EyeLeft.Distortion.SetToIdentity();
        EyeRight = EyeLeft;
    }

    // The "center eye" is the position the HMD tracking returns,
    // and games will also usually use it for audio, aiming reticles, some line-of-sight tests, etc.
    EyeConfig GetEyeCenter() const
    {
        EyeConfig result;
        result.ReliefInMeters = 0.5f * ( EyeLeft.ReliefInMeters + EyeRight.ReliefInMeters );
        result.NoseToPupilInMeters = 0.0f;
        result.Distortion.SetToIdentity();
        return result;
    }

};


//-----------------------------------------------------------------------------------

// Stateless computation functions, in somewhat recommended execution order.
// For examples on how to use many of them, see the StereoConfig::UpdateComputedState function.

const float OVR_DEFAULT_EXTRA_EYE_ROTATION = 30.0f * MATH_FLOAT_DEGREETORADFACTOR;

// Creates a dummy debug HMDInfo matching a particular HMD model.
// Useful for development without an actual HMD attached.
HMDInfo             CreateDebugHMDInfo(HmdTypeEnum hmdType);


// profile may be NULL, in which case it uses the hard-coded defaults.
// distortionType should be left at the default unless you require something specific for your distortion shaders.
// eyeCupOverride can be EyeCup_LAST, in which case it uses the one in the profile.
HmdRenderInfo       GenerateHmdRenderInfoFromHmdInfo ( HMDInfo const &hmdInfo,
                                                       Profile const *profile = NULL,
                                                       DistortionEqnType distortionType = Distortion_CatmullRom10,
                                                       EyeCupType eyeCupOverride = EyeCup_LAST );

LensConfig          GenerateLensConfigFromEyeRelief ( float eyeReliefInMeters, HmdRenderInfo const &hmd,
                                                      DistortionEqnType distortionType = Distortion_CatmullRom10 );

DistortionRenderDesc CalculateDistortionRenderDesc ( StereoEye eyeType, HmdRenderInfo const &hmd,
                                                     LensConfig const *pLensOverride = NULL );

FovPort             CalculateFovFromEyePosition ( float eyeReliefInMeters,
                                                  float offsetToRightInMeters,
                                                  float offsetDownwardsInMeters,
                                                  float lensDiameterInMeters,
                                                  float extraEyeRotationInRadians = OVR_DEFAULT_EXTRA_EYE_ROTATION);

FovPort             CalculateFovFromHmdInfo ( StereoEye eyeType,
                                              DistortionRenderDesc const &distortion,
                                              HmdRenderInfo const &hmd,
                                              float extraEyeRotationInRadians = OVR_DEFAULT_EXTRA_EYE_ROTATION );

FovPort             GetPhysicalScreenFov ( StereoEye eyeType, DistortionRenderDesc const &distortion );

FovPort             ClampToPhysicalScreenFov ( StereoEye eyeType, DistortionRenderDesc const &distortion,
                                               FovPort inputFovPort );

Sizei               CalculateIdealPixelSize ( StereoEye eyeType, DistortionRenderDesc const &distortion,
                                              FovPort fov, float pixelsPerDisplayPixel );

Recti               GetFramebufferViewport ( StereoEye eyeType, HmdRenderInfo const &hmd );

Matrix4f            CreateProjection ( bool rightHanded, FovPort fov,
                                       float zNear = 0.01f, float zFar = 10000.0f );

Matrix4f            CreateOrthoSubProjection ( bool rightHanded, StereoEye eyeType,
                                               float tanHalfFovX, float tanHalfFovY,
                                               float unitsX, float unitsY, float distanceFromCamera,
                                               float interpupillaryDistance, Matrix4f const &projection,
                                               float zNear = 0.0f, float zFar = 0.0f );

ScaleAndOffset2D    CreateNDCScaleAndOffsetFromFov ( FovPort fov );

ScaleAndOffset2D    CreateUVScaleAndOffsetfromNDCScaleandOffset ( ScaleAndOffset2D scaleAndOffsetNDC,
                                                                  Recti renderedViewport,
                                                                  Sizei renderTargetSize );


//-----------------------------------------------------------------------------------
// ***** StereoEyeParams

// StereoEyeParams describes RenderDevice configuration needed to render
// the scene for one eye. 
struct StereoEyeParams
{
    StereoEye               Eye;
    Matrix4f                ViewAdjust;             // Translation to be applied to view matrix.

    // Distortion and the VP on the physical display - the thing to run the distortion shader on.
    DistortionRenderDesc    Distortion;
    Recti                   DistortionViewport;

    // Projection and VP of a particular view (you could have multiple of these).
    Recti                   RenderedViewport;       // Viewport that we render the standard scene to.
    FovPort                 Fov;                    // The FOVs of this scene.
    Matrix4f                RenderedProjection;     // Projection matrix used with this eye.
    ScaleAndOffset2D        EyeToSourceNDC;         // Mapping from TanEyeAngle space to [-1,+1] on the rendered image.
    ScaleAndOffset2D        EyeToSourceUV;          // Mapping from TanEyeAngle space to actual texture UV coords.
};


//-----------------------------------------------------------------------------------
// A set of "forward-mapping" functions, mapping from framebuffer space to real-world and/or texture space.
Vector2f TransformScreenNDCToTanFovSpace ( DistortionRenderDesc const &distortion,
                                           const Vector2f &framebufferNDC );
void TransformScreenNDCToTanFovSpaceChroma ( Vector2f *resultR, Vector2f *resultG, Vector2f *resultB, 
                                             DistortionRenderDesc const &distortion,
                                             const Vector2f &framebufferNDC );
Vector2f TransformTanFovSpaceToRendertargetTexUV ( ScaleAndOffset2D const &eyeToSourceUV,
                                                   Vector2f const &tanEyeAngle );
Vector2f TransformTanFovSpaceToRendertargetNDC ( ScaleAndOffset2D const &eyeToSourceNDC,
                                                 Vector2f const &tanEyeAngle );
Vector2f TransformScreenPixelToScreenNDC( Recti const &distortionViewport,
                                          Vector2f const &pixel );
Vector2f TransformScreenPixelToTanFovSpace ( Recti const &distortionViewport,
                                             DistortionRenderDesc const &distortion,
                                             Vector2f const &pixel );
Vector2f TransformScreenNDCToRendertargetTexUV( DistortionRenderDesc const &distortion,
                                                StereoEyeParams const &eyeParams,
                                                Vector2f const &pixel );
Vector2f TransformScreenPixelToRendertargetTexUV( Recti const &distortionViewport,
                                                  DistortionRenderDesc const &distortion,
                                                  StereoEyeParams const &eyeParams,
                                                  Vector2f const &pixel );

// A set of "reverse-mapping" functions, mapping from real-world and/or texture space back to the framebuffer.
// Be aware that many of these are significantly slower than their forward-mapping counterparts.
Vector2f TransformTanFovSpaceToScreenNDC( DistortionRenderDesc const &distortion,
                                          const Vector2f &tanEyeAngle, bool usePolyApprox = false );
Vector2f TransformRendertargetNDCToTanFovSpace( const ScaleAndOffset2D &eyeToSourceNDC,
                                                const Vector2f &textureNDC );

// Handy wrappers.
inline Vector2f TransformTanFovSpaceToRendertargetTexUV ( StereoEyeParams const &eyeParams,
                                                          Vector2f const &tanEyeAngle )
{
    return TransformTanFovSpaceToRendertargetTexUV ( eyeParams.EyeToSourceUV, tanEyeAngle );
}
inline Vector2f TransformTanFovSpaceToRendertargetNDC ( StereoEyeParams const &eyeParams,
                                                        Vector2f const &tanEyeAngle )
{
    return TransformTanFovSpaceToRendertargetNDC ( eyeParams.EyeToSourceNDC, tanEyeAngle );
}

} //namespace OVR

#endif // OVR_Stereo_h
