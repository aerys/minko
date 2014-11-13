/************************************************************************************

Filename    :   Util_Render_Stereo.h
Content     :   Sample stereo rendering configuration classes.
Created     :   October 22, 2012
Authors     :   Michael Antonov, Tom Forsyth

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

#ifndef OVR_Util_Render_Stereo_h
#define OVR_Util_Render_Stereo_h

#include "../OVR_Stereo.h"
#include "../Tracking/Tracking_SensorStateReader.h"

namespace OVR { namespace Util { namespace Render {



//-----------------------------------------------------------------------------------
// **** Useful debug functions.
//
// Purely for debugging - the results are not very end-user-friendly.
char const* GetDebugNameEyeCupType ( EyeCupType eyeCupType );
char const* GetDebugNameHmdType ( HmdTypeEnum hmdType );



//-----------------------------------------------------------------------------------
// **** Higher-level utility functions.

Sizei CalculateRecommendedTextureSize    ( HmdRenderInfo const &hmd,
                                           bool bRendertargetSharedByBothEyes,
                                           float pixelDensityInCenter = 1.0f );

FovPort CalculateRecommendedFov          ( HmdRenderInfo const &hmd,
                                           StereoEye eyeType,
                                           bool bMakeFovSymmetrical = false);

StereoEyeParams CalculateStereoEyeParams ( HmdRenderInfo const &hmd,
                                           StereoEye eyeType,
                                           Sizei const &actualRendertargetSurfaceSize,
                                           bool bRendertargetSharedByBothEyes,
                                           bool bRightHanded = true,
                                           float zNear = 0.01f, float zFar = 10000.0f,
										   Sizei const *pOverrideRenderedPixelSize = NULL,
                                           FovPort const *pOverrideFovport = NULL,
                                           float zoomFactor = 1.0f );

Vector3f CalculateEyeVirtualCameraOffset(HmdRenderInfo const &hmd,
                                         StereoEye eyeType, bool bMonoRenderingMode );


// These are two components from StereoEyeParams that can be changed
// very easily without full recomputation of everything.
struct ViewportScaleAndOffset
{
    Recti               RenderedViewport;
    ScaleAndOffset2D    EyeToSourceUV;
};

// Three ways to override the size of the render view dynamically.
// None of these require changing the distortion parameters or the regenerating the distortion mesh,
// and can be called every frame if desired.
ViewportScaleAndOffset ModifyRenderViewport ( StereoEyeParams const &params,
                                              Sizei const &actualRendertargetSurfaceSize,
                                              Recti const &renderViewport );

ViewportScaleAndOffset ModifyRenderSize ( StereoEyeParams const &params,
                                          Sizei const &actualRendertargetSurfaceSize,
                                          Sizei const &requestedRenderSize,
                                          bool bRendertargetSharedByBothEyes = false );

ViewportScaleAndOffset ModifyRenderDensity ( StereoEyeParams const &params,
                                             Sizei const &actualRendertargetSurfaceSize,
                                             float pixelDensity = 1.0f,
                                             bool bRendertargetSharedByBothEyes = false );


//-----------------------------------------------------------------------------------
// *****  StereoConfig

// StereoConfig maintains a scene stereo state and allow switching between different
// stereo rendering modes. To support rendering, StereoConfig keeps track of HMD
// variables such as screen size, eye-to-screen distance and distortion, and computes
// extra data such as FOV and distortion center offsets based on it. Rendering
// parameters are returned though StereoEyeParams for each eye.
//
// Beyond regular 3D projection, this class supports rendering a 2D orthographic
// surface for UI and text. The 2D surface will be defined by CreateOrthoSubProjection().
// The (0,0) coordinate corresponds to eye center location.
// 
// Applications are not required to use this class, but they should be doing very
// similar sequences of operations, and it may be useful to start with this class
// and modify it.

struct StereoEyeParamsWithOrtho
{
    StereoEyeParams         StereoEye;
    Matrix4f                OrthoProjection;
};

struct ViewportScaleAndOffsetBothEyes
{
    ViewportScaleAndOffset  Left;
    ViewportScaleAndOffset  Right;
};

class StereoConfig
{
public:

    // StereoMode describes rendering modes that can be used by StereoConfig.
    // These modes control whether stereo rendering is used or not (Stereo_None),
    // and how it is implemented.
    enum StereoMode
    {
        Stereo_None                     = 0,        // Single eye
        Stereo_LeftRight_Multipass      = 1,        // One frustum per eye
    };


    StereoConfig(StereoMode mode = Stereo_LeftRight_Multipass);
 
    //---------------------------------------------------------------------------------------------
    // *** Core functions - every app MUST call these functions at least once.

    // Sets HMD parameters; also initializes distortion coefficients.
    void        SetHmdRenderInfo(const HmdRenderInfo& hmd);

    // Set the physical size of the rendertarget surface the app created,
    // and whether one RT is shared by both eyes, or each eye has its own RT:
    // true: both eyes are rendered to the same RT. Left eye starts at top-left, right eye starts at top-middle.
    // false: each eye is rendered to its own RT. Some GPU architectures prefer this arrangement.
    // Typically, the app would call CalculateRecommendedTextureSize() to suggest the choice of RT size.
    // This setting must be exactly the size of the actual RT created, or the UVs produced will be incorrect.
    // If the app wants to render to a subsection of the RT, it should use SetRenderSize()
    void        SetRendertargetSize (Size<int> const rendertargetSize,
                                     bool rendertargetIsSharedByBothEyes );

    // Returns full set of Stereo rendering parameters for the specified eye.
    const StereoEyeParamsWithOrtho& GetEyeRenderParams(StereoEye eye);



    //---------------------------------------------------------------------------------------------
    // *** Optional functions - an app may call these to override default behaviours.

    const HmdRenderInfo& GetHmdRenderInfo() const { return Hmd; }

    // Returns the recommended size of rendertargets.
    // If rendertargetIsSharedByBothEyes is true, this is the size of the combined buffer.
    // If rendertargetIsSharedByBothEyes is false, this is the size of each individual buffer.
    // pixelDensityInCenter may be set to any number - by default it will match the HMD resolution in the center of the image.
    // After creating the rendertargets, the application MUST call SetRendertargetSize() with the actual size created
    // (which can be larger or smaller as the app wishes, but StereoConfig needs to know either way)
    Sizei       CalculateRecommendedTextureSize ( bool rendertargetSharedByBothEyes,
                                                  float pixelDensityInCenter = 1.0f );

    // Sets a stereo rendering mode and updates internal cached
    // state (matrices, per-eye view) based on it.
    void        SetStereoMode(StereoMode mode)  { Mode = mode; DirtyFlag = true; }
    StereoMode  GetStereoMode() const           { return Mode; }

    // Sets the fieldOfView that the 2D coordinate area stretches to.
    void        Set2DAreaFov(float fovRadians);

    // Really only for science experiments - no normal app should ever need to override
    // the HMD's lens descriptors. Passing NULL removes the override.
    // Supply both = set left and right.
    // Supply just left = set both to the same.
    // Supply neither = remove override.
    void        SetLensOverride ( LensConfig const *pLensOverrideLeft  = NULL,
                                  LensConfig const *pLensOverrideRight = NULL );
 
    // Override the rendered FOV in various ways. All angles in tangent units.
    // This is not clamped to the physical FOV of the display - you'll need to do that yourself!
    // Supply both = set left and right.
    // Supply just left = set both to the same.
    // Supply neither = remove override.
    void        SetFov ( FovPort const *pfovLeft  = NULL,
					     FovPort const *pfovRight = NULL );
    
    void        SetFovPortRadians ( float horizontal, float vertical )
    {
        FovPort fov = FovPort::CreateFromRadians(horizontal, vertical);
        SetFov( &fov, &fov );
    }


    // This forces a "zero IPD" mode where there is just a single render with an FOV that
    //   is the union of the two calculated FOVs.
    // The calculated render is for the left eye. Any size & FOV overrides for the right
    //   eye will be ignored.
    // If you query the right eye's size, you will get the same render
    //   size & position as the left eye - you should not actually do the render of course!
    //   The distortion values will be different, because it goes to a different place on the framebuffer.
    // Note that if you do this, the rendertarget does not need to be twice the width of
    //   the render size any more.
    void        SetZeroVirtualIpdOverride ( bool enableOverride );

    // Allows the app to specify near and far clip planes and the right/left-handedness of the projection matrix.
    void        SetZClipPlanesAndHandedness ( float zNear = 0.01f, float zFar = 10000.0f,
                                              bool rightHandedProjection = true );

    // Allows the app to specify how much extra eye rotation to allow when determining the visible FOV.
    void        SetExtraEyeRotation ( float extraEyeRotationInRadians = 0.0f );

    // The dirty flag is set by any of the above calls. Just handy for the app to know
    // if e.g. the distortion mesh needs regeneration.
    void        SetDirty() { DirtyFlag = true; }
    bool        IsDirty() { return DirtyFlag; }

    // An app never needs to call this - GetEyeRenderParams will call it internally if
    // the state is dirty. However apps can call this explicitly to control when and where
    // computation is performed (e.g. not inside critical loops)
    void        UpdateComputedState();

    // This returns the projection matrix with a "zoom". Does not modify any internal state.
    Matrix4f    GetProjectionWithZoom ( StereoEye eye, float fovZoom ) const;


    //---------------------------------------------------------------------------------------------
    // The SetRender* functions are special.
    //
    // They do not require a full recalculation of state, and they do not change anything but the
    // ViewportScaleAndOffset data for the eyes (which they return), and do not set the dirty flag!
    // This means they can be called without regenerating the distortion mesh, and thus 
    // can happily be called every frame without causing performance problems. Dynamic rescaling 
    // of the rendertarget can help keep framerate up in demanding VR applications.
    // See the documentation for more details on their use.

    // Specify a pixel density - how many rendered pixels per pixel in the physical display.
    ViewportScaleAndOffsetBothEyes SetRenderDensity ( float pixelsPerDisplayPixel );

    // Supply the size directly. Will be clamped to the physical rendertarget size.
    ViewportScaleAndOffsetBothEyes SetRenderSize ( Sizei const &renderSizeLeft, Sizei const &renderSizeRight );

    // Supply the viewport directly. This is not clamped to the physical rendertarget - careful now!
    ViewportScaleAndOffsetBothEyes SetRenderViewport ( Recti const &renderViewportLeft, Recti const &renderViewportRight );

private:

    // *** Modifiable State

    StereoMode         Mode;
    HmdRenderInfo      Hmd;

    float              Area2DFov;           // FOV range mapping to the 2D area.

    // Only one of these three overrides can be true!
    enum SetViewportModeEnum
    {
        SVPM_Density,
        SVPM_Size,
        SVPM_Viewport,
    }                  SetViewportMode;
    // ...and depending which it is, one of the following are used.
    float              SetViewportPixelsPerDisplayPixel;
    Sizei              SetViewportSize[2];
    Recti           SetViewport[2];

    // Other overrides.
    bool               OverrideLens;
    LensConfig         LensOverrideLeft;
    LensConfig         LensOverrideRight;
    Sizei              RendertargetSize;
    bool               OverrideTanHalfFov;
    FovPort            FovOverrideLeft;
    FovPort            FovOverrideRight;
    bool               OverrideZeroIpd;
    float              ZNear;
    float              ZFar;
    float              ExtraEyeRotationInRadians;
    bool               IsRendertargetSharedByBothEyes;
    bool               RightHandedProjection;

    bool               DirtyFlag;   // Set when any if the modifiable state changed. Does NOT get set by SetRender*()

    // Utility function.
    ViewportScaleAndOffsetBothEyes setupViewportScaleAndOffsets();

    // *** Computed State

public:     // Small hack for the config tool. Normal code should never read EyeRenderParams directly - use GetEyeRenderParams() instead.
    // 0/1 = left/right main views.
    StereoEyeParamsWithOrtho    EyeRenderParams[2];
};


//-----------------------------------------------------------------------------------
// *****  Distortion Mesh Rendering
//

// Stores both texture UV coords, or tan(angle) values.
// Use whichever set of data the specific distortion algorithm requires.
// This struct *must* be binary compatible with CAPI ovrDistortionVertex.
struct DistortionMeshVertexData
{
    // [-1,+1],[-1,+1] over the entire framebuffer.
    Vector2f    ScreenPosNDC;
    // [0.0-1.0] interpolation value for timewarping - see documentation for details.
    float       TimewarpLerp;
    // [0.0-1.0] fade-to-black at the edges to reduce peripheral vision noise.
    float       Shade;        
    // The red, green, and blue vectors in tan(angle) space.
    // Scale and offset by the values in StereoEyeParams.EyeToSourceUV.Scale
    // and StereoParams.EyeToSourceUV.Offset to get to real texture UV coords.
    Vector2f    TanEyeAnglesR;
    Vector2f    TanEyeAnglesG;
    Vector2f    TanEyeAnglesB;    
};


void DistortionMeshCreate ( DistortionMeshVertexData **ppVertices, uint16_t **ppTriangleListIndices,
                            int *pNumVertices, int *pNumTriangles,
                            const StereoEyeParams &stereoParams, const HmdRenderInfo &hmdRenderInfo );

// Generate distortion mesh for a eye. This version requires less data then stereoParms, supporting
// dynamic change in render target viewport.
void DistortionMeshCreate( DistortionMeshVertexData **ppVertices, uint16_t **ppTriangleListIndices,
                           int *pNumVertices, int *pNumTriangles,
                           bool rightEye,
                           const HmdRenderInfo &hmdRenderInfo, 
                           const DistortionRenderDesc &distortion, const ScaleAndOffset2D &eyeToSourceNDC );

void DistortionMeshDestroy ( DistortionMeshVertexData *pVertices, uint16_t *pTriangleMeshIndices );


//-----------------------------------------------------------------------------------
// *****  Heightmap Mesh Rendering
//

// Stores both texture UV coords, or tan(angle) values.
// This struct *must* be binary compatible with CAPI ovrHeightmapVertex.
struct HeightmapMeshVertexData
{
    // [-1,+1],[-1,+1] over the entire framebuffer.
    Vector2f    ScreenPosNDC;
    // [0.0-1.0] interpolation value for timewarping - see documentation for details.
    float       TimewarpLerp;
    // The vectors in tan(angle) space.
    // Scale and offset by the values in StereoEyeParams.EyeToSourceUV.Scale
    // and StereoParams.EyeToSourceUV.Offset to get to real texture UV coords.
    Vector2f    TanEyeAngles;    
};


void HeightmapMeshCreate ( HeightmapMeshVertexData **ppVertices, uint16_t **ppTriangleListIndices,
    int *pNumVertices, int *pNumTriangles,
    const StereoEyeParams &stereoParams, const HmdRenderInfo &hmdRenderInfo );

// Generate heightmap mesh for a eye. This version requires less data then stereoParms, supporting
// dynamic change in render target viewport.
void HeightmapMeshCreate( HeightmapMeshVertexData **ppVertices, uint16_t **ppTriangleListIndices,
    int *pNumVertices, int *pNumTriangles, bool rightEye,
    const HmdRenderInfo &hmdRenderInfo, const ScaleAndOffset2D &eyeToSourceNDC );

void HeightmapMeshDestroy ( HeightmapMeshVertexData *pVertices, uint16_t *pTriangleMeshIndices );



//-----------------------------------------------------------------------------------
// ***** Prediction and timewarp.
//

struct PredictionValues
{
    // All values in seconds.
    // These are the times in seconds from a present+flush to the relevant display element.
    // The time is measured to the middle of that element's visibility window,
    // e.g. if the device is a full-persistence display, the element will be visible for
    // an entire frame, so the time measures to the middle of that period, i.e. half the frame time.
    float PresentFlushToRenderedScene;        // To the overall rendered 3D scene being visible.
    float PresentFlushToTimewarpStart;        // To when the first timewarped scanline will be visible.
    float PresentFlushToTimewarpEnd;          // To when the last timewarped scanline will be visible.
    float PresentFlushToPresentFlush;         // To the next present+flush, i.e. the ideal framerate.

    bool  WithTimewarp;
    bool  WithVsync;
};

// Calculates the values from the HMD info.
PredictionValues PredictionGetDeviceValues ( const HmdRenderInfo &hmdRenderInfo,
                                             bool withTimewarp = true,
                                             bool withVsync = true );

// Pass in an orientation used to render the scene, and then the predicted orientation
// (which may have been computed later on, and thus is more accurate), and this
// will return the matrix to pass to the timewarp distortion shader.
// TODO: deal with different handedness?
Matrix4f TimewarpComputePoseDelta ( Matrix4f const &renderedViewFromWorld, Matrix4f const &predictedViewFromWorld, Matrix4f const&eyeViewAdjust );
Matrix4f TimewarpComputePoseDeltaPosition ( Matrix4f const &renderedViewFromWorld, Matrix4f const &predictedViewFromWorld, Matrix4f const&eyeViewAdjust );



// TimewarpMachine helps keep track of rendered frame timing and
// handles predictions for time-warp rendering.
class TimewarpMachine
{
public:
    TimewarpMachine();
   
    // Call this on and every time something about the setup changes.
    void        Reset ( HmdRenderInfo& renderInfo, bool vsyncEnabled, double timeNow );

    // The only reliable time in most engines is directly after the frame-present and GPU flush-and-wait.
    // This call should be done right after that to give this system the timing info it needs.
    void        AfterPresentAndFlush(double timeNow);

    // The "average" time the rendered frame will show up,
    // and the predicted pose of the HMD at that time.
    // You usually only need to call one of these functions.
    double      GetViewRenderPredictionTime();
    bool        GetViewRenderPredictionPose(Tracking::SensorStateReader* reader, Posef& transform);


    // Timewarp prediction functions. You usually only need to call one of these three sets of functions.

    // The predicted times that the first and last pixel will be visible on-screen.
    double      GetVisiblePixelTimeStart();
    double      GetVisiblePixelTimeEnd();
    // Predicted poses of the HMD at those first and last pixels.
	bool        GetPredictedVisiblePixelPoseStart(Tracking::SensorStateReader* reader, Posef& transform);
	bool        GetPredictedVisiblePixelPoseEnd(Tracking::SensorStateReader* reader, Posef& transform);
    // The delta matrices to feed to the timewarp distortion code,
    // given the pose that was used for rendering.
    // (usually the one returned by GetViewRenderPredictionPose() earlier)
	bool        GetTimewarpDeltaStart(Tracking::SensorStateReader* reader, Posef const &renderedPose, Matrix4f& transform);
	bool        GetTimewarpDeltaEnd(Tracking::SensorStateReader* reader, Posef const &renderedPose, Matrix4f& transform);

    // Just-In-Time distortion aims to delay the second sensor reading & distortion
    // until the very last moment to improve prediction. However, it is a little scary,
    // since the delay might wait too long and miss the vsync completely!
    // Use of the JustInTime_* functions is entirely optional, and we advise allowing
    // users to turn it off in their video options to cope with odd machine configurations.

    // What time should the app wait until before starting distortion?
    double      JustInTime_GetDistortionWaitUntilTime();

    // Used to time the distortion rendering
    bool        JustInTime_NeedDistortionTimeMeasurement() const;
    void        JustInTime_BeforeDistortionTimeMeasurement(double timeNow);
    void        JustInTime_AfterDistortionTimeMeasurement(double timeNow);

private:
    bool                VsyncEnabled;
    HmdRenderInfo       RenderInfo;
    PredictionValues    CurrentPredictionValues;

    enum { NumDistortionTimes = 10 };
    int                 DistortionTimeCount;
    double              DistortionTimeCurrentStart;
    float               DistortionTimes[NumDistortionTimes];
    float               DistortionTimeAverage;

    // Pose at which last time the eye was rendered.
    Posef               EyeRenderPoses[2];

    // Absolute time of the last present+flush
    double              LastFramePresentFlushTime;
    // Seconds between present+flushes
    float               PresentFlushToPresentFlushSeconds;
    // Predicted absolute time of the next present+flush
    double              NextFramePresentFlushTime;

};



}}}  // OVR::Util::Render

#endif
