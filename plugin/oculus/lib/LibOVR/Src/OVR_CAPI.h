/************************************************************************************

Filename    :   OVR_CAPI.h
Content     :   C Interface to Oculus tracking and rendering.
Created     :   November 23, 2013
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

/// @file OVR_CAPI.h
/// Exposes all general Rift functionality.
/// 
/// @mainpage Overview
/// Welcome to the Oculus SDK Reference Manual. All SDK functionality is exposed through a simple and portable C API, found in OVR_CAPI.h.
///
/// Please see the Oculus Developer Guide for detailed information about using the SDK in your native applications.

#ifndef OVR_CAPI_h
#define OVR_CAPI_h

#include <stdint.h>

typedef char ovrBool;

//-----------------------------------------------------------------------------------
// ***** OVR_EXPORT definition

#if !defined(OVR_EXPORT)
    #ifdef OVR_OS_WIN32
        #define OVR_EXPORT __declspec(dllexport)        
    #else
        #define OVR_EXPORT
    #endif
#endif

//#define ENABLE_LATENCY_TESTER

//-----------------------------------------------------------------------------------
// ***** Simple Math Structures

/// A 2D vector with integer components.
typedef struct ovrVector2i_
{
    int x, y;
} ovrVector2i;
/// A 2D size with integer components.
typedef struct ovrSizei_
{
    int w, h;
} ovrSizei;
/// A 2D rectangle with a position and size.
/// All components are integers.
typedef struct ovrRecti_
{
    ovrVector2i Pos;
    ovrSizei    Size;
} ovrRecti;

/// A quaternion rotation.
typedef struct ovrQuatf_
{
    float x, y, z, w;  
} ovrQuatf;
/// A 2D vector with float components.
typedef struct ovrVector2f_
{
    float x, y;
} ovrVector2f;
/// A 3D vector with float components.
typedef struct ovrVector3f_
{
    float x, y, z;
} ovrVector3f;
/// A 4x4 matrix with float elements.
typedef struct ovrMatrix4f_
{
    float M[4][4];
} ovrMatrix4f;
/// Position and orientation together.
typedef struct ovrPosef_
{
    ovrQuatf     Orientation;
    ovrVector3f  Position;    
} ovrPosef;

/// A full pose (rigid body) configuration with first and second derivatives.
typedef struct ovrPoseStatef_
{
    ovrPosef     ThePose;
    ovrVector3f  AngularVelocity;
    ovrVector3f  LinearVelocity;
    ovrVector3f  AngularAcceleration;
    ovrVector3f  LinearAcceleration;
    double       TimeInSeconds;         // Absolute time of this state sample.
} ovrPoseStatef;

/// Field Of View (FOV) in tangent of the angle units.
/// As an example, for a standard 90 degree vertical FOV, we would 
/// have: { UpTan = tan(90 degrees / 2), DownTan = tan(90 degrees / 2) }.
typedef struct ovrFovPort_
{
    /// The tangent of the angle between the viewing vector and the top edge of the field of view.
    float UpTan;
    /// The tangent of the angle between the viewing vector and the bottom edge of the field of view.
    float DownTan;
    /// The tangent of the angle between the viewing vector and the left edge of the field of view.
    float LeftTan;
    /// The tangent of the angle between the viewing vector and the right edge of the field of view.
    float RightTan;
} ovrFovPort;


//-----------------------------------------------------------------------------------
// ***** HMD Types

/// Enumerates all HMD types that we support.
typedef enum
{
    ovrHmd_None             = 0,    
    ovrHmd_DK1              = 3,
    ovrHmd_DKHD             = 4,    
    ovrHmd_DK2              = 6,
    ovrHmd_Other             // Some HMD other then the one in the enumeration.
} ovrHmdType;

/// HMD capability bits reported by device.
typedef enum
{
    // Read-only flags.
    ovrHmdCap_Present           = 0x0001,   ///  The HMD is plugged in and detected by the system.
    ovrHmdCap_Available         = 0x0002,   ///  The HMD and its sensor is available for ownership use.
											///  i.e. it is not already owned by another application.
    ovrHmdCap_Captured          = 0x0004,   ///  Set to 'true' if we captured ownership of this HMD.

    // These flags are intended for use with the new driver display mode.
    ovrHmdCap_ExtendDesktop     = 0x0008,   /// (read only) Means the display driver is in compatibility mode.

    // Modifiable flags (through ovrHmd_SetEnabledCaps).
    ovrHmdCap_NoMirrorToWindow  = 0x2000,   /// Disables mirroring of HMD output to the window. This may improve 
											/// rendering performance slightly (only if 'ExtendDesktop' is off).
    ovrHmdCap_DisplayOff        = 0x0040,   /// Turns off HMD screen and output (only if 'ExtendDesktop' is off).

    ovrHmdCap_LowPersistence    = 0x0080,   ///  HMD supports low persistence mode.
    ovrHmdCap_DynamicPrediction = 0x0200,   ///  Adjust prediction dynamically based on internally measured latency.
    /// Support rendering without VSync for debugging.
    ovrHmdCap_NoVSync           = 0x1000,

    // These bits can be modified by ovrHmd_SetEnabledCaps.
    ovrHmdCap_Writable_Mask     = 0x33F0,
    /// These flags are currently passed into the service. May change without notice.
    ovrHmdCap_Service_Mask      = 0x23F0
} ovrHmdCaps;


/// Tracking capability bits reported by the device.
/// Used with ovrHmd_ConfigureTracking.
typedef enum
{
    ovrTrackingCap_Orientation      = 0x0010,   ///  Supports orientation tracking (IMU).
    ovrTrackingCap_MagYawCorrection = 0x0020,   ///  Supports yaw drift correction via a magnetometer or other means.
    ovrTrackingCap_Position         = 0x0040,   ///  Supports positional tracking.
    ///  Overrides the other flags. Indicates that the application
    ///  doesn't care about tracking settings. This is the internal
    ///  default before ovrHmd_ConfigureTracking is called.
    ovrTrackingCap_Idle             = 0x0100,
} ovrTrackingCaps;

/// Distortion capability bits reported by device.
/// Used with ovrHmd_ConfigureRendering and ovrHmd_CreateDistortionMesh.
typedef enum
{        
    ovrDistortionCap_Chromatic	    = 0x01,		///	Supports chromatic aberration correction.
    ovrDistortionCap_TimeWarp	    = 0x02,		///	Supports timewarp.
    ovrDistortionCap_Vignette	    = 0x08,		///	Supports vignetting around the edges of the view.
    ovrDistortionCap_NoRestore      = 0x10,		///  Do not save and restore the graphics state when rendering distortion.
    ovrDistortionCap_FlipInput      = 0x20,		///  Flip the vertical texture coordinate of input images.
    ovrDistortionCap_SRGB           = 0x40,		///  Assume input images are in sRGB gamma-corrected color space.
    ovrDistortionCap_Overdrive      = 0x80,		///  Overdrive brightness transitions to reduce artifacts on DK2+ displays
    ovrDistortionCap_HqDistortion   = 0x100,	/// High-quality sampling of distortion buffer for anti-aliasing

    ovrDistortionCap_ProfileNoTimewarpSpinWaits = 0x10000,  /// Use when profiling with timewarp to remove false positives
} ovrDistortionCaps;


/// Specifies which eye is being used for rendering.
/// This type explicitly does not include a third "NoStereo" option, as such is
/// not required for an HMD-centered API.
typedef enum
{
    ovrEye_Left  = 0,
    ovrEye_Right = 1,
    ovrEye_Count = 2
} ovrEyeType;



/// This is a complete descriptor of the HMD.
typedef struct ovrHmdDesc_
{
    /// Internal handle of this HMD.
    struct ovrHmdStruct* Handle;

    /// This HMD's type.
    ovrHmdType  Type;
    
    /// Name string describing the product: "Oculus Rift DK1", etc.
    const char* ProductName;    
    const char* Manufacturer;
    
    /// HID Vendor and ProductId of the device.
    short       VendorId;
    short       ProductId;
    /// Sensor (and display) serial number.
    char        SerialNumber[24];
    /// Sensor firmware version.
    short       FirmwareMajor;
    short       FirmwareMinor;
    /// External tracking camera frustum dimensions (if present).
    float       CameraFrustumHFovInRadians;
    float       CameraFrustumVFovInRadians;
    float       CameraFrustumNearZInMeters;
    float       CameraFrustumFarZInMeters;

    /// Capability bits described by ovrHmdCaps.
    unsigned int HmdCaps;
	/// Capability bits described by ovrTrackingCaps.
    unsigned int TrackingCaps;
    /// Capability bits described by ovrDistortionCaps.
    unsigned int DistortionCaps;

    /// These define the recommended and maximum optical FOVs for the HMD.    
    ovrFovPort  DefaultEyeFov[ovrEye_Count];
    ovrFovPort  MaxEyeFov[ovrEye_Count];

    /// Preferred eye rendering order for best performance.
    /// Can help reduce latency on sideways-scanned screens.
    ovrEyeType  EyeRenderOrder[ovrEye_Count];

    /// Resolution of the full HMD screen (both eyes) in pixels.
    ovrSizei    Resolution;
    /// Location of the application window on the desktop (or 0,0).
    ovrVector2i WindowsPos;     

    /// Display that the HMD should present on.
    /// TBD: It may be good to remove this information relying on WindowPos instead.
    /// Ultimately, we may need to come up with a more convenient alternative,
    /// such as API-specific functions that return adapter, or something that will
    /// work with our monitor driver.
    /// Windows: (e.g. "\\\\.\\DISPLAY3", can be used in EnumDisplaySettings/CreateDC).
    const char* DisplayDeviceName;
    /// MacOS:
    int         DisplayId;
  
} ovrHmdDesc;


/// Simple type ovrHmd is used in ovrHmd_* calls.
typedef const ovrHmdDesc * ovrHmd;



/// Bit flags describing the current status of sensor tracking.
typedef enum
{
    ovrStatus_OrientationTracked    = 0x0001,   /// Orientation is currently tracked (connected and in use).
    ovrStatus_PositionTracked       = 0x0002,   /// Position is currently tracked (false if out of range).
    ovrStatus_CameraPoseTracked     = 0x0004,   /// Camera pose is currently tracked.
    ovrStatus_PositionConnected     = 0x0020,   /// Position tracking hardware is connected.
    ovrStatus_HmdConnected          = 0x0080    /// HMD Display is available and connected.
} ovrStatusBits;

/// Specifies a reading we can query from the sensor.
typedef struct ovrSensorData_
{
    ovrVector3f    Accelerometer;    /// Acceleration reading in m/s^2.
    ovrVector3f    Gyro;             /// Rotation rate in rad/s.
    ovrVector3f    Magnetometer;     /// Magnetic field in Gauss.
    float          Temperature;      /// Temperature of the sensor in degrees Celsius.
    float          TimeInSeconds;    /// Time when the reported IMU reading took place, in seconds.
} ovrSensorData;


/// Tracking state at a given absolute time (describes predicted HMD pose etc).
/// Returned by ovrHmd_GetTrackingState.
typedef struct ovrTrackingState_
{
    /// Predicted head pose (and derivatives) at the requested absolute time.
    /// The look-ahead interval is equal to (HeadPose.TimeInSeconds - RawSensorData.TimeInSeconds).
    ovrPoseStatef  HeadPose;

    /// Current pose of the external camera (if present).
    /// This pose includes camera tilt (roll and pitch). For a leveled coordinate
    /// system use LeveledCameraPose.
    ovrPosef       CameraPose;

    /// Camera frame aligned with gravity.
    /// This value includes position and yaw of the camera, but not roll and pitch.
    /// It can be used as a reference point to render real-world objects in the correct location.
    ovrPosef       LeveledCameraPose;

    /// The most recent sensor data received from the HMD.
    ovrSensorData  RawSensorData;

    /// Tracking status described by ovrStatusBits.
    unsigned int   StatusFlags;

    //// 0.4.1

    /// Time spent processing the last vision frame
    double         LastVisionProcessingTime;
} ovrTrackingState;


/// Frame timing data reported by ovrHmd_BeginFrameTiming() or ovrHmd_BeginFrame().
typedef struct ovrFrameTiming_
{
    /// The amount of time that has passed since the previous frame's
	/// ThisFrameSeconds value (usable for movement scaling).
    /// This will be clamped to no more than 0.1 seconds to prevent
    /// excessive movement after pauses due to loading or initialization.
    float			DeltaSeconds;

    /// It is generally expected that the following holds:
    /// ThisFrameSeconds < TimewarpPointSeconds < NextFrameSeconds < 
    /// EyeScanoutSeconds[EyeOrder[0]] <= ScanoutMidpointSeconds <= EyeScanoutSeconds[EyeOrder[1]].

    /// Absolute time value when rendering of this frame began or is expected to
    /// begin. Generally equal to NextFrameSeconds of the previous frame. Can be used
    /// for animation timing.
    double			ThisFrameSeconds;
    /// Absolute point when IMU expects to be sampled for this frame.
    double			TimewarpPointSeconds;
    /// Absolute time when frame Present followed by GPU Flush will finish and the next frame begins.
    double			NextFrameSeconds;

    /// Time when when half of the screen will be scanned out. Can be passed as an absolute time
	/// to ovrHmd_GetTrackingState() to get the predicted general orientation.
    double		    ScanoutMidpointSeconds;
    /// Timing points when each eye will be scanned out to display. Used when rendering each eye.
    double			EyeScanoutSeconds[2];    
} ovrFrameTiming;



/// Rendering information for each eye. Computed by either ovrHmd_ConfigureRendering()
/// or ovrHmd_GetRenderDesc() based on the specified FOV. Note that the rendering viewport 
/// is not included here as it can be specified separately and modified per frame through:
///    (a) ovrHmd_GetRenderScaleAndOffset in the case of client rendered distortion,
/// or (b) passing different values via ovrTexture in the case of SDK rendered distortion.
typedef struct ovrEyeRenderDesc_
{    
    ovrEyeType  Eye;
    ovrFovPort  Fov;
	ovrRecti	DistortedViewport; 	        /// Distortion viewport.
    ovrVector2f PixelsPerTanAngleAtCenter;  /// How many display pixels will fit in tan(angle) = 1.
    ovrVector3f ViewAdjust;  		        /// Translation to be applied to view matrix.
} ovrEyeRenderDesc;


//-----------------------------------------------------------------------------------
// ***** Platform-independent Rendering Configuration

/// These types are used to hide platform-specific details when passing
/// render device, OS, and texture data to the API.
///
/// The benefit of having these wrappers versus platform-specific API functions is
/// that they allow game glue code to be portable. A typical example is an
/// engine that has multiple back ends, say GL and D3D. Portable code that calls
/// these back ends may also use LibOVR. To do this, back ends can be modified
/// to return portable types such as ovrTexture and ovrRenderAPIConfig.
typedef enum
{
    ovrRenderAPI_None,
    ovrRenderAPI_OpenGL,
    ovrRenderAPI_Android_GLES,  // May include extra native window pointers, etc.
    ovrRenderAPI_D3D9,
    ovrRenderAPI_D3D10,
    ovrRenderAPI_D3D11,
    ovrRenderAPI_Count
} ovrRenderAPIType;

/// Platform-independent part of rendering API-configuration data.
/// It is a part of ovrRenderAPIConfig, passed to ovrHmd_Configure.
typedef struct ovrRenderAPIConfigHeader_
{
    ovrRenderAPIType API;
    ovrSizei         RTSize;
    int              Multisample;
} ovrRenderAPIConfigHeader;

/// Contains platform-specific information for rendering.
typedef struct ovrRenderAPIConfig_
{
    ovrRenderAPIConfigHeader Header;
    uintptr_t                PlatformData[8];
} ovrRenderAPIConfig;

/// Platform-independent part of the eye texture descriptor.
/// It is a part of ovrTexture, passed to ovrHmd_EndFrame.
/// If RenderViewport is all zeros then the full texture will be used.
typedef struct ovrTextureHeader_
{
    ovrRenderAPIType API;
    ovrSizei         TextureSize;
    ovrRecti         RenderViewport;  // Pixel viewport in texture that holds eye image.
} ovrTextureHeader;

/// Contains platform-specific information about a texture.
typedef struct ovrTexture_
{
    ovrTextureHeader Header;
    uintptr_t        PlatformData[8];
} ovrTexture;


// -----------------------------------------------------------------------------------
// ***** API Interfaces

// Basic steps to use the API:
//
// Setup:
//  1. ovrInitialize()
//  2. ovrHMD hmd = ovrHmd_Create(0)
//  3. Use hmd members and ovrHmd_GetFovTextureSize() to determine graphics configuration.
//  4. Call ovrHmd_ConfigureTracking() to configure and initialize tracking.
//  5. Call ovrHmd_ConfigureRendering() to setup graphics for SDK rendering,
//     which is the preferred approach.
//     Please refer to "Client Distorton Rendering" below if you prefer to do that instead.
//  6. If the ovrHmdCap_ExtendDesktop flag is not set, then use ovrHmd_AttachToWindow to 
//     associate the relevant application window with the hmd.
//  5. Allocate render target textures as needed.
//
// Game Loop:
//  6. Call ovrHmd_BeginFrame() to get the current frame timing information.
//  7. Render each eye using ovrHmd_GetEyePose to get the predicted head pose.
//  8. Call ovrHmd_EndFrame() to render the distorted textures to the back buffer
//     and present them on the hmd.
//
// Shutdown:
//  9. ovrHmd_Destroy(hmd)
//  10. ovr_Shutdown()
//

#ifdef __cplusplus 
extern "C" {
#endif

// ovr_InitializeRenderingShim initializes the rendering shim appart from everything
// else in LibOVR. This may be helpful if the application prefers to avoid
// creating any OVR resources (allocations, service connections, etc) at this point.
// ovr_InitializeRenderingShim does not bring up anything within LibOVR except the
// necessary hooks to enable the Direct-to-Rift functionality.
//
// Either ovr_InitializeRenderingShim() or ovr_Initialize() must be called before any
// Direct3D or OpenGL initilization is done by applictaion (creation of devices, etc).
// ovr_Initialize() must still be called after to use the rest of LibOVR APIs.
OVR_EXPORT void     ovr_InitializeRenderingShim();

// Library init/shutdown, must be called around all other OVR code.
// No other functions calls besides ovr_InitializeRenderingShim are allowed
// before ovr_Initialize succeeds or after ovr_Shutdown.
/// Initializes all Oculus functionality.
OVR_EXPORT ovrBool  ovr_Initialize();
/// Shuts down all Oculus functionality.
OVR_EXPORT void     ovr_Shutdown();

/// Returns version string representing libOVR version. Static, so
/// string remains valid for app lifespan
OVR_EXPORT const char* ovr_GetVersionString();



/// Detects or re-detects HMDs and reports the total number detected.
/// Users can get information about each HMD by calling ovrHmd_Create with an index.
OVR_EXPORT int      ovrHmd_Detect();


/// Creates a handle to an HMD which doubles as a description structure.
/// Index can [0 .. ovrHmd_Detect()-1]. Index mappings can cange after each ovrHmd_Detect call.
/// If not null, then the returned handle must be freed with ovrHmd_Destroy.
OVR_EXPORT ovrHmd   ovrHmd_Create(int index);
OVR_EXPORT void     ovrHmd_Destroy(ovrHmd hmd);

/// Creates a 'fake' HMD used for debugging only. This is not tied to specific hardware,
/// but may be used to debug some of the related rendering.
OVR_EXPORT ovrHmd   ovrHmd_CreateDebug(ovrHmdType type);


/// Returns last error for HMD state. Returns null for no error.
/// String is valid until next call or GetLastError or HMD is destroyed.
/// Pass null hmd to get global errors (during create etc).
OVR_EXPORT const char* ovrHmd_GetLastError(ovrHmd hmd);

/// Platform specific function to specify the application window whose output will be 
/// displayed on the HMD. Only used if the ovrHmdCap_ExtendDesktop flag is false.
///   Windows: SwapChain associated with this window will be displayed on the HMD.
///            Specify 'destMirrorRect' in window coordinates to indicate an area
///            of the render target output that will be mirrored from 'sourceRenderTargetRect'.
///            Null pointers mean "full size".
/// @note Source and dest mirror rects are not yet implemented.
OVR_EXPORT ovrBool ovrHmd_AttachToWindow(ovrHmd hmd, void* window,
										 const ovrRecti* destMirrorRect,
										 const ovrRecti* sourceRenderTargetRect);

//-------------------------------------------------------------------------------------

/// Returns capability bits that are enabled at this time as described by ovrHmdCaps.
/// Note that this value is different font ovrHmdDesc::HmdCaps, which describes what
/// capabilities are available for that HMD.
OVR_EXPORT unsigned int ovrHmd_GetEnabledCaps(ovrHmd hmd);

/// Modifies capability bits described by ovrHmdCaps that can be modified,
/// such as ovrHmd_LowPersistance.
OVR_EXPORT void         ovrHmd_SetEnabledCaps(ovrHmd hmd, unsigned int hmdCaps);


//-------------------------------------------------------------------------------------
// ***** Tracking Interface

/// All tracking interface functions are thread-safe, allowing tracking state to be sampled
/// from different threads.
/// ConfigureTracking starts sensor sampling, enabling specified capabilities,
///    described by ovrTrackingCaps.
///  - supportedTrackingCaps specifies support that is requested. The function will succeed 
///	  even if these caps are not available (i.e. sensor or camera is unplugged). Support
///    will automatically be enabled if such device is plugged in later. Software should
///    check ovrTrackingState.StatusFlags for real-time status.
///  - requiredTrackingCaps specify sensor capabilities required at the time of the call.
///    If they are not available, the function will fail. Pass 0 if only specifying
///    supportedTrackingCaps.
///  - Pass 0 for both supportedTrackingCaps and requiredTrackingCaps to disable tracking.
OVR_EXPORT ovrBool  ovrHmd_ConfigureTracking(ovrHmd hmd, unsigned int supportedTrackingCaps,
													     unsigned int requiredTrackingCaps);

/// Re-centers the sensor orientation.
/// Normally this will recenter the (x,y,z) translational components and the yaw 
/// component of orientation.
OVR_EXPORT void     ovrHmd_RecenterPose(ovrHmd hmd);

/// Returns tracking state reading based on the specified absolute system time.
/// Pass an absTime value of 0.0 to request the most recent sensor reading. In this case
/// both PredictedPose and SamplePose will have the same value.
/// ovrHmd_GetEyePose relies on this internally.
/// This may also be used for more refined timing of FrontBuffer rendering logic, etc.
OVR_EXPORT ovrTrackingState ovrHmd_GetTrackingState(ovrHmd hmd, double absTime);


//-------------------------------------------------------------------------------------
// ***** Graphics Setup


/// Calculates the recommended texture size for rendering a given eye within the HMD
/// with a given FOV cone. Higher FOV will generally require larger textures to 
/// maintain quality.
///  - pixelsPerDisplayPixel specifies the ratio of the number of render target pixels 
///    to display pixels at the center of distortion. 1.0 is the default value. Lower
///    values can improve performance.
OVR_EXPORT ovrSizei ovrHmd_GetFovTextureSize(ovrHmd hmd, ovrEyeType eye, ovrFovPort fov,
                                             float pixelsPerDisplayPixel);



//-------------------------------------------------------------------------------------
// *****  Rendering API Thread Safety

//  All of rendering functions including the configure and frame functions
// are *NOT thread safe*. It is ok to use ConfigureRendering on one thread and handle
//  frames on another thread, but explicit synchronization must be done since
//  functions that depend on configured state are not reentrant.
//
//  As an extra requirement, any of the following calls must be done on
//  the render thread, which is the same thread that calls ovrHmd_BeginFrame
//  or ovrHmd_BeginFrameTiming.
//    - ovrHmd_EndFrame
//    - ovrHmd_GetEyePose
//    - ovrHmd_GetEyeTimewarpMatrices


//-------------------------------------------------------------------------------------
// *****  SDK Distortion Rendering Functions

// These functions support rendering of distortion by the SDK through direct
// access to the underlying rendering API, such as D3D or GL.
// This is the recommended approach since it allows better support for future
// Oculus hardware, and enables a range of low-level optimizations.


/// Configures rendering and fills in computed render parameters.
/// This function can be called multiple times to change rendering settings.
/// eyeRenderDescOut is a pointer to an array of two ovrEyeRenderDesc structs
/// that are used to return complete rendering information for each eye.
///
///  - apiConfig provides D3D/OpenGL specific parameters. Pass null
///    to shutdown rendering and release all resources.
///  - distortionCaps describe desired distortion settings.
///
OVR_EXPORT ovrBool ovrHmd_ConfigureRendering( ovrHmd hmd,
                                              const ovrRenderAPIConfig* apiConfig,
                                              unsigned int distortionCaps,
                                              const ovrFovPort eyeFovIn[2],
                                              ovrEyeRenderDesc eyeRenderDescOut[2] );


/// Begins a frame, returning timing information.
/// This should be called at the beginning of the game rendering loop (on the render thread).
/// Pass 0 for the frame index if not using ovrHmd_GetFrameTiming.
OVR_EXPORT ovrFrameTiming ovrHmd_BeginFrame(ovrHmd hmd, unsigned int frameIndex);

/// Ends a frame, submitting the rendered textures to the frame buffer.
/// - RenderViewport within each eyeTexture can change per frame if necessary.
/// - 'renderPose' will typically be the value returned from ovrHmd_GetEyePose, 
///   but can be different if a different head pose was used for rendering.
/// - This may perform distortion and scaling internally, assuming is it not 
///   delegated to another thread. 
/// - Must be called on the same thread as BeginFrame.
/// - *** This Function will call Present/SwapBuffers and potentially wait for GPU Sync ***.
OVR_EXPORT void     ovrHmd_EndFrame(ovrHmd hmd,
                                    const ovrPosef renderPose[2],
                                    const ovrTexture eyeTexture[2]);


/// Returns the predicted head pose to use when rendering the specified eye.
/// - Must be called between ovrHmd_BeginFrameTiming and ovrHmd_EndFrameTiming.
/// - If the pose is used for rendering the eye, it should be passed to ovrHmd_EndFrame.
OVR_EXPORT ovrPosef ovrHmd_GetEyePose(ovrHmd hmd, ovrEyeType eye);



//-------------------------------------------------------------------------------------
// *****  Client Distortion Rendering Functions

// These functions provide the distortion data and render timing support necessary to allow
// client rendering of distortion. Client-side rendering involves the following steps:
//
//  1. Setup ovrEyeDesc based on the desired texture size and FOV.
//     Call ovrHmd_GetRenderDesc to get the necessary rendering parameters for each eye.
// 
//  2. Use ovrHmd_CreateDistortionMesh to generate the distortion mesh.
//
//  3. Use ovrHmd_BeginFrameTiming, ovrHmd_GetEyePose, and ovrHmd_BeginFrameTiming
//     in the rendering loop to obtain timing and predicted head orientation when
//     rendering each eye.
//      - When using timewarp, use ovr_WaitTillTime after the rendering and gpu flush, followed
//        by ovrHmd_GetEyeTimewarpMatrices to obtain the timewarp matrices used 
//        by the distortion pixel shader. This will minimize latency.
//

/// Computes the distortion viewport, view adjust, and other rendering parameters for 
/// the specified eye. This can be used instead of ovrHmd_ConfigureRendering to do 
/// setup for client rendered distortion.
OVR_EXPORT ovrEyeRenderDesc ovrHmd_GetRenderDesc(ovrHmd hmd,
                                                 ovrEyeType eyeType, ovrFovPort fov);


/// Describes a vertex used by the distortion mesh. This is intended to be converted into
/// the engine-specific format. Some fields may be unused based on the ovrDistortionCaps 
/// flags selected. TexG and TexB, for example, are not used if chromatic correction is 
/// not requested.
typedef struct ovrDistortionVertex_
{
    ovrVector2f ScreenPosNDC;    // [-1,+1],[-1,+1] over the entire framebuffer.
    float       TimeWarpFactor;  // Lerp factor between time-warp matrices. Can be encoded in Pos.z.
    float       VignetteFactor;  // Vignette fade factor. Can be encoded in Pos.w.
    ovrVector2f TanEyeAnglesR;
    ovrVector2f TanEyeAnglesG;
    ovrVector2f TanEyeAnglesB;    
} ovrDistortionVertex;

/// Describes a full set of distortion mesh data, filled in by ovrHmd_CreateDistortionMesh.
/// Contents of this data structure, if not null, should be freed by ovrHmd_DestroyDistortionMesh.
typedef struct ovrDistortionMesh_
{
    ovrDistortionVertex* pVertexData;
    unsigned short*      pIndexData;
    unsigned int         VertexCount;
    unsigned int         IndexCount;
} ovrDistortionMesh;

/// Generate distortion mesh per eye.
/// Distortion capabilities will depend on 'distortionCaps' flags. Users should 
/// render using the appropriate shaders based on their settings.
/// Distortion mesh data will be allocated and written into the ovrDistortionMesh data structure,
/// which should be explicitly freed with ovrHmd_DestroyDistortionMesh.
/// Users should call ovrHmd_GetRenderScaleAndOffset to get uvScale and Offset values for rendering.
/// The function shouldn't fail unless theres is a configuration or memory error, in which case
/// ovrDistortionMesh values will be set to null.
/// This is the only function in the SDK reliant on eye relief, currently imported from profiles, 
/// or overriden here.
OVR_EXPORT ovrBool  ovrHmd_CreateDistortionMesh( ovrHmd hmd,
                                                 ovrEyeType eyeType, ovrFovPort fov,
                                                 unsigned int distortionCaps,
                                                 ovrDistortionMesh *meshData);

/// Used to free the distortion mesh allocated by ovrHmd_GenerateDistortionMesh. meshData elements
/// are set to null and zeroes after the call.
OVR_EXPORT void     ovrHmd_DestroyDistortionMesh( ovrDistortionMesh* meshData );

/// Computes updated 'uvScaleOffsetOut' to be used with a distortion if render target size or
/// viewport changes after the fact. This can be used to adjust render size every frame if desired.
OVR_EXPORT void     ovrHmd_GetRenderScaleAndOffset( ovrFovPort fov,
                                                    ovrSizei textureSize, ovrRecti renderViewport,
                                                    ovrVector2f uvScaleOffsetOut[2] );


/// Thread-safe timing function for the main thread. Caller should increment frameIndex
/// with every frame and pass the index where applicable to functions called on the 
/// rendering thread.
OVR_EXPORT ovrFrameTiming ovrHmd_GetFrameTiming(ovrHmd hmd, unsigned int frameIndex);

/// Called at the beginning of the frame on the rendering thread.
/// Pass frameIndex == 0 if ovrHmd_GetFrameTiming isn't being used. Otherwise,
/// pass the same frame index as was used for GetFrameTiming on the main thread.
OVR_EXPORT ovrFrameTiming ovrHmd_BeginFrameTiming(ovrHmd hmd, unsigned int frameIndex);

/// Marks the end of client distortion rendered frame, tracking the necessary timing information.
/// This function must be called immediately after Present/SwapBuffers + GPU sync. GPU sync is
/// important before this call to reduce latency and ensure proper timing.
OVR_EXPORT void     ovrHmd_EndFrameTiming(ovrHmd hmd);

/// Initializes and resets frame time tracking. This is typically not necessary, but
/// is helpful if game changes vsync state or video mode. vsync is assumed to be on if this
/// isn't called. Resets internal frame index to the specified number.
OVR_EXPORT void     ovrHmd_ResetFrameTiming(ovrHmd hmd, unsigned int frameIndex);


/// Computes timewarp matrices used by distortion mesh shader, these are used to adjust
/// for head orientation change since the last call to ovrHmd_GetEyePose when rendering
/// this eye. The ovrDistortionVertex::TimeWarpFactor is used to blend between the
/// matrices, usually representing two different sides of the screen.
/// Must be called on the same thread as ovrHmd_BeginFrameTiming.
OVR_EXPORT void     ovrHmd_GetEyeTimewarpMatrices(ovrHmd hmd, ovrEyeType eye,
                                                  ovrPosef renderPose, ovrMatrix4f twmOut[2]);


//-------------------------------------------------------------------------------------
// ***** Stateless math setup functions

/// Used to generate projection from ovrEyeDesc::Fov.
OVR_EXPORT ovrMatrix4f ovrMatrix4f_Projection( ovrFovPort fov,
                                               float znear, float zfar, ovrBool rightHanded );

/// Used for 2D rendering, Y is down
/// orthoScale = 1.0f / pixelsPerTanAngleAtCenter
/// orthoDistance = distance from camera, such as 0.8m
OVR_EXPORT ovrMatrix4f ovrMatrix4f_OrthoSubProjection(ovrMatrix4f projection, ovrVector2f orthoScale,
                                                      float orthoDistance, float eyeViewAdjustX);

/// Returns global, absolute high-resolution time in seconds. This is the same
/// value as used in sensor messages.
OVR_EXPORT double   ovr_GetTimeInSeconds();

/// Waits until the specified absolute time.
OVR_EXPORT double   ovr_WaitTillTime(double absTime);



// -----------------------------------------------------------------------------------
// ***** Latency Test interface

/// Does latency test processing and returns 'TRUE' if specified rgb color should
/// be used to clear the screen.
OVR_EXPORT ovrBool      ovrHmd_ProcessLatencyTest(ovrHmd hmd, unsigned char rgbColorOut[3]);

/// Returns non-null string once with latency test result, when it is available.
/// Buffer is valid until next call.
OVR_EXPORT const char*  ovrHmd_GetLatencyTestResult(ovrHmd hmd);

/// Returns the latency testing color in rgbColorOut to render when using a DK2
/// Returns false if this feature is disabled or not-applicable (e.g. using a DK1)
OVR_EXPORT ovrBool      ovrHmd_GetLatencyTest2DrawColor(ovrHmd hmddesc, unsigned char rgbColorOut[3]);


//-------------------------------------------------------------------------------------
// ***** Health and Safety Warning Display interface
//

/// Used by ovrhmd_GetHSWDisplayState to report the current display state.
typedef struct ovrHSWDisplayState_
{
    /// If true then the warning should be currently visible
    /// and the following variables have meaning. Else there is no
    /// warning being displayed for this application on the given HMD.
    ovrBool Displayed;
    double  StartTime;       /// Absolute time when the warning was first displayed. See ovr_GetTimeInSeconds().
    double  DismissibleTime; /// Earliest absolute time when the warning can be dismissed. May be a time in the past.
} ovrHSWDisplayState;

/// Returns the current state of the HSW display. If the application is doing the rendering of
/// the HSW display then this function serves to indicate that the the warning should be 
/// currently displayed. If the application is using SDK-based eye rendering then the SDK by 
/// default automatically handles the drawing of the HSW display. An application that uses 
/// application-based eye rendering should use this function to know when to start drawing the
/// HSW display itself and can optionally use it in conjunction with ovrhmd_DismissHSWDisplay
/// as described below.
///
/// Example usage for application-based rendering:
///    bool HSWDisplayCurrentlyDisplayed = false; // global or class member variable
///    ovrHSWDisplayState hswDisplayState;
///    ovrhmd_GetHSWDisplayState(Hmd, &hswDisplayState);
///
///    if (hswDisplayState.Displayed && !HSWDisplayCurrentlyDisplayed) {
///        <insert model into the scene that stays in front of the user>
///        HSWDisplayCurrentlyDisplayed = true;
///    }
OVR_EXPORT void ovrHmd_GetHSWDisplayState(ovrHmd hmd, ovrHSWDisplayState *hasWarningState);

/// Dismisses the HSW display if the warning is dismissible and the earliest dismissal time 
/// has occurred. Returns true if the display is valid and could be dismissed. The application 
/// should recognize that the HSW display is being displayed (via ovrhmd_GetHSWDisplayState)
/// and if so then call this function when the appropriate user input to dismiss the warning
/// occurs.
///
/// Example usage :
///    void ProcessEvent(int key) {
///        if(key == escape) {
///            ovrHSWDisplayState hswDisplayState;
///            ovrhmd_GetHSWDisplayState(hmd, &hswDisplayState);
///
///            if(hswDisplayState.Displayed && ovrhmd_DismissHSWDisplay(hmd)) {
///                <remove model from the scene>
///                HSWDisplayCurrentlyDisplayed = false;
///            }
///        }
///    }
OVR_EXPORT ovrBool ovrHmd_DismissHSWDisplay(ovrHmd hmd);





// -----------------------------------------------------------------------------------
// ***** Property Access

// NOTICE: This is experimental part of API that is likely to go away or change.

// These allow accessing different properties of the HMD and profile.
// Some of the properties may go away with profile/HMD versions, so software should
// use defaults and/or proper fallbacks.
// 

// For now, access profile entries; this will change.
#if !defined(OVR_KEY_USER)

#define OVR_KEY_USER                        "User"
#define OVR_KEY_NAME                        "Name"
#define OVR_KEY_GENDER                      "Gender"
#define OVR_KEY_PLAYER_HEIGHT               "PlayerHeight"
#define OVR_KEY_EYE_HEIGHT                  "EyeHeight"
#define OVR_KEY_IPD                         "IPD"
#define OVR_KEY_NECK_TO_EYE_DISTANCE        "NeckEyeDistance"
#define OVR_KEY_CAMERA_POSITION				"CenteredFromWorld"

// TODO: remove this duplication with OVR_Profile.h
// Default measurements empirically determined at Oculus to make us happy
// The neck model numbers were derived as an average of the male and female averages from ANSUR-88
// NECK_TO_EYE_HORIZONTAL = H22 - H43 = INFRAORBITALE_BACK_OF_HEAD - TRAGION_BACK_OF_HEAD
// NECK_TO_EYE_VERTICAL = H21 - H15 = GONION_TOP_OF_HEAD - ECTOORBITALE_TOP_OF_HEAD
// These were determined to be the best in a small user study, clearly beating out the previous default values
#define OVR_DEFAULT_GENDER                  "Unknown"
#define OVR_DEFAULT_PLAYER_HEIGHT           1.778f
#define OVR_DEFAULT_EYE_HEIGHT              1.675f
#define OVR_DEFAULT_IPD                     0.064f
#define OVR_DEFAULT_NECK_TO_EYE_HORIZONTAL  0.0805f
#define OVR_DEFAULT_NECK_TO_EYE_VERTICAL    0.075f
#define OVR_DEFAULT_EYE_RELIEF_DIAL         3
#define OVR_DEFAULT_CAMERA_POSITION			{0,0,0,1,0,0,0}

#endif

/// Get boolean property. Returns first element if property is a boolean array.
/// Returns defaultValue if property doesn't exist.
OVR_EXPORT ovrBool      ovrHmd_GetBool(ovrHmd hmd, const char* propertyName, ovrBool defaultVal);

/// Modify bool property; false if property doesn't exist or is readonly.
OVR_EXPORT ovrBool      ovrHmd_SetBool(ovrHmd hmd, const char* propertyName, ovrBool value);

/// Get integer property. Returns first element if property is an integer array.
/// Returns defaultValue if property doesn't exist.
OVR_EXPORT int          ovrHmd_GetInt(ovrHmd hmd, const char* propertyName, int defaultVal);

/// Modify integer property; false if property doesn't exist or is readonly.
OVR_EXPORT ovrBool      ovrHmd_SetInt(ovrHmd hmd, const char* propertyName, int value);

/// Get float property. Returns first element if property is a float array.
/// Returns defaultValue if property doesn't exist.
OVR_EXPORT float        ovrHmd_GetFloat(ovrHmd hmd, const char* propertyName, float defaultVal);

/// Modify float property; false if property doesn't exist or is readonly.
OVR_EXPORT ovrBool      ovrHmd_SetFloat(ovrHmd hmd, const char* propertyName, float value);

/// Get float[] property. Returns the number of elements filled in, 0 if property doesn't exist.
/// Maximum of arraySize elements will be written.
OVR_EXPORT unsigned int ovrHmd_GetFloatArray(ovrHmd hmd, const char* propertyName,
                                            float values[], unsigned int arraySize);

/// Modify float[] property; false if property doesn't exist or is readonly.
OVR_EXPORT ovrBool      ovrHmd_SetFloatArray(ovrHmd hmd, const char* propertyName,
                                             float values[], unsigned int arraySize);

/// Get string property. Returns first element if property is a string array.
/// Returns defaultValue if property doesn't exist.
/// String memory is guaranteed to exist until next call to GetString or GetStringArray, or HMD is destroyed.
OVR_EXPORT const char*  ovrHmd_GetString(ovrHmd hmd, const char* propertyName,
                                        const char* defaultVal);

/// Set string property
OVR_EXPORT ovrBool ovrHmd_SetString(ovrHmd hmddesc, const char* propertyName,
                                    const char* value);

#ifdef __cplusplus 
} // extern "C"
#endif


#endif	// OVR_CAPI_h
