/************************************************************************************
 
PublicHeader:   None
Filename    :   dxgi_ovr_filter.h
Content     :   Shared usermode/kernel mode definitions for IOCTL functionality.
                Also used from LibOVR to access the driver.
Created     :   January 27, 2014
Authors     :   Dean Beeler
 
Copyright   :   Copyright 2013 Oculus, LLC. All Rights reserved.
 
Use of this software is subject to the terms of the Oculus LLC license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.
 
/************************************************************************************/

#ifndef OVR_dxgi_ovr_filter_h
#define OVR_dxgi_ovr_filter_h

#include <InitGuid.h>

#define USERMODE_TEST_ROTATION 1

#if USERMODE_TEST_ROTATION // Used only by the um test application
#define USERMODE_SURFACE_WIDTH 1920
#define USERMODE_SURFACE_HEIGHT 1080
#else
#define USERMODE_SURFACE_WIDTH 1080
#define USERMODE_SURFACE_HEIGHT 1920
#endif

#define TEST_ROTATION 0 // Kernel-mode parameters

#if TEST_ROTATION
#define SURFACE_WIDTH 1920
#define SURFACE_HEIGHT 1080
#else
#define SURFACE_WIDTH 1080
#define SURFACE_HEIGHT 1920
#endif

// {46231713-49FD-4922-84E3-9FF907C06803}
DEFINE_GUID(GUID_DEVINTERFACE_OVRRIFTMGR, 
	0x46231713, 0x49fd, 0x4922, 0x84, 0xe3, 0x9f, 0xf9, 0x7, 0xc0, 0x68, 0x3);

#define QUERYADAPTER_MAGICSIZE		17836
#define QUERYADAPTER_MAGICHEADER	0x4f565246 // OVRF
#define QUERYADAPTER_MAXPATH		2048

#define FUNCTION_INDEX 0xb800

#pragma pack(push,1)

#define OVR_RIFT_MODE_OFF			0	// Disabled
#define OVR_RIFT_MODE_ENABLED		1   // Enabled
#define OVR_RIFT_MODE_EXTEND		2   // Extending 2D displays. Without this flag
										//	2D displays are disabled when the Rift
										//	is active
#define OVR_RIFT_MODE_FRONTBUFFER	4	// Enable front buffer only for Rift
#define OVR_RIFT_MODE_LOCKMOUSE		8	// Prevent mouse from entering bounds

#define OVR_ESCAPE_TYPE_HANDLE		1   // Escape to notify driver of our collected handles

#define OVR_FlipImmediate			0x2
#define OVR_FlipOnNextVSync			0x4

//-----------------------------------------------------------------------------------
// Structures for application to UM driver

// Kernel32.dll functionality
typedef HMODULE  (WINAPI *WinLoadLibraryA) ( LPCSTR  );
typedef HMODULE  (WINAPI *WinLoadLibraryW) ( LPCWSTR  );
typedef HMODULE  (WINAPI *WinLoadLibraryExA) ( LPCSTR, HANDLE, DWORD  );
typedef HMODULE  (WINAPI *WinLoadLibraryExW) ( LPCWSTR, HANDLE, DWORD  );
typedef BOOL     (WINAPI *WinGetModuleHandleExA)( DWORD, LPCSTR, HMODULE* );
typedef BOOL     (WINAPI *WinGetModuleHandleExW)( DWORD, LPCWSTR, HMODULE* );

// Overridden DirectX 9 entry points
typedef void*	 (WINAPI *WinDirect3DCreate9)(UINT SDKVersion);
typedef HRESULT  (WINAPI *WinDirect3DCreate9Ex)(UINT SDKVersion, void** aDevice);

// Overridden DXGI entry points
typedef HRESULT (WINAPI *WinCreateDXGIFactory)(
	__in   REFIID riid,
	__out  void **ppFactory
	);

typedef HRESULT (WINAPI *WinCreateDXGIFactory1)(
	__in   REFIID riid,
	__out  void **ppFactory
	);

typedef HRESULT (WINAPI *WinCreateDXGIFactory2)(
	__in   UINT flags,
	__in   const IID &riid,
	__out  void **ppFactory
	);

// Application usermode callbacks from usermode driver. These 
// functions are all provided by the calling application that uses
// the filter mode driver

// IsInitializingDisplay is used at runtime to validate that
// the created resource (RT or bind_present) matches the resolution
// of our expected backbuffer. If the application returns true,
// our usermode driver will convert this to a primary
typedef BOOL  (WINAPI *IsInitializingDisplay) ( PVOID, UINT, UINT );
// RiftForContext is a function that will return the Rift device of
// the concerned context. This is for targeting a particular
// device instance with a particular Rift for rendering
typedef ULONG (WINAPI *RiftForContext)( PVOID, HANDLE );
// CloseRiftForContext is a function that informs the application
// the created device is shutting down and the context
// can freedly disassociate with the particular
typedef BOOL (WINAPI *CloseRiftForContext)( PVOID, HANDLE, ULONG );
typedef BOOL (WINAPI *WindowDisplayResolution)( PVOID, UINT*, UINT*, UINT*, UINT*, BOOL* );
// IsCreatingBackBuffer is a function directed at the runtime shim
// to confirm that the runtime is actively creating the additional
// swapchain for rotation and display out to the rift.
// When creating the original swapchain this function should return false
// so the orignal swapchain isn't inadvertantly coopted.
typedef BOOL (WINAPI *IsCreatingBackBuffer)( PVOID );
// Callback from the usermode driver to obtain the desire to see debug statements from
// the usermode drivers on the output console. Only called one per usermode driver shim
// and usermode runtime.
typedef BOOL (WINAPI *ShouldEnableDebug)( VOID );
// Callback from the usermode driver to the runtime obtain the vsync status
typedef BOOL (WINAPI *ShouldVSync)( VOID );
// Callback from usermode mode and runtime driver to obtain expected native width, 
// height and degrees rotation of the rift
typedef BOOL (WINAPI *ExpectedResolution)( PVOID, UINT*, UINT*, UINT* );
// Usermode callback that reports whether or not mirroring is enabled
typedef BOOL (WINAPI *MirroringEnabled)( PVOID );
// Callback from the shim for Unity and other plugins used to
// report the swapchain that was created by the application
typedef void* (WINAPI *GetDX11SwapChain)( PVOID );
// Callback to report the HWND associated with this context
typedef HWND (WINAPI* GetWindowForContext)( PVOID );
// Should present Rift on context
typedef BOOL (WINAPI* PresentRiftOnContext)( PVOID );
// Used by a pre-loaded shim (d3d9, dxgi, opengl32) to
// identify which api version we loaded
// 1 = OpenGL
// 9 = DirectX 9
// 10 = DirectX 1X
typedef int (WINAPI* ActiveAPIVersion)( PVOID );

#pragma warning(push)
#pragma warning(disable: 4201)

typedef struct _LINK_APPLICATION_DRIVER
{
	UINT32                      version;
	PVOID						context;

	union 
	{
		struct  
		{
			IsInitializingDisplay		pfnInitializingDisplay;
			RiftForContext				pfnRiftForContext;
			CloseRiftForContext			pfnCloseRiftForContext;
			WindowDisplayResolution		pfnWindowDisplayResolution;
			IsCreatingBackBuffer		pfnIsCreatingBackBuffer;
			ShouldEnableDebug			pfnShouldEnableDebug;
			ShouldVSync					pfnShouldVSync;
			ExpectedResolution			pfnExpectedResolution;
			MirroringEnabled			pfnMirroringEnabled;
			GetDX11SwapChain			pfnGetDX11SwapChain;
			GetWindowForContext			pfnGetWindowForContext;
			PresentRiftOnContext		pfnPresentRiftOnContext;
			ActiveAPIVersion			pfnActiveAPIVersion;
		};

		PROC	placeholders[128];
	};


	// Used by Runtime filter for linking with original libraries
	WinDirect3DCreate9			pfnDirect3DCreate9;
	WinDirect3DCreate9Ex		pfnDirect3DCreate9Ex;
	WinCreateDXGIFactory		pfnCreateDXGIFactory;
	WinCreateDXGIFactory1		pfnCreateDXGIFactory1;
	WinCreateDXGIFactory2		pfnCreateDXGIFactory2;
} LINK_APPLICATION_DRIVER, *PLINK_APPLICATION_DRIVER;

#pragma warning(pop)


// OVRDisplay.dll functionality
typedef HRESULT (WINAPI *PreloadLibraryFn) ( WinLoadLibraryA , LPCSTR, PLINK_APPLICATION_DRIVER appDriver );
typedef HRESULT (WINAPI *PreloadLibraryRTFn) ( PLINK_APPLICATION_DRIVER appDriver );

//-----------------------------------------------------------------------------------
// Structures for UM driver to KM driver

typedef struct _QUERY_KM_DRIVER
{
	UINT32 magic;								// Friend or foe identifier for our filter driver
												// See: QUERYADAPTER_MAGICHEADER
	UINT32 maxVidPnSources;						// Returns the maximum number of video present network sources
} QUERY_KM_DRIVER, *PQUERY_KM_DRIVER;

#ifndef _D3DUKMDT_H_
typedef UINT D3DKMT_HANDLE;
#endif

typedef struct _HandleNotepad
{
	// These are assigned around CreateResource
	HANDLE			hUsermodeInResource;
	HANDLE			hUsermodeOutResource;

	// These are assigned within the kernel with
	// DxgkDdiCreateAllocation and
	// DxgkDdiOpenAllocation
	D3DKMT_HANDLE	hAllocation;
	PVOID			hDeviceSpecificHandle;
	PVOID			hKernelDriverHandle;

	// These are assigned around pfnAllocateCb
	HANDLE			hUsermodeSharedResource;
	D3DKMT_HANDLE	hKernelModeSharedResource;

	ULONG			childUid;

	UINT			pitch;

} HandleNotepad, *PHandleNotepad;


typedef struct _ALLOC_PRIVATE_STRUCTURE
{
	UINT32 magic;								// Friend or foe identifier for our filter driver

	PVOID originalPrivataDataPtr;				// Location in usermode of the original private data structure
	UINT  originalPrivateSize;					// Size of private data structure at the end of this header

	PVOID hAllocationHandle;					// User-mode-assigned allocation handle for CreateAllocation
	PVOID hDeviceSpecificHandle;				// Assigned in kernal OpenAllocation
	PVOID hInternalHandle;						// Assigned in kernal CreateAllocation
	UINT  pitch;								// Hinted surface pitch

	BYTE originalPrivateData[1];				// Variable length


} ALLOC_PRIVATE_STRUCTURE, *PALLOC_PRIVATE_STRUCTURE;

typedef struct _ESCAPE_STRUCTURE
{
	UINT32 magic;								// Friend or foe identifier for our filter driver

	UINT32 escapeType;							// Specifier for individual type of escape message
												// Type 1 for notepad
	union {
		HandleNotepad notepad;
	};
} ESCAPE_STRUCTURE, *PESCAPE_STRUCTURE;

// Structures for internal operation of KM driver

typedef struct _RIFT_SYNC
{
	ULONG	childUid;				// ChildUid as reported by RIFT_STATUS
	ULONG   vsync;					// 1 for vsync, 0 for immediate
} RIFT_SYNC, *PRIFT_SYNC;

typedef struct _RIFT_MODE
{
	ULONG	childUid;				// ChildUid as reported by RIFT_STATUS
	ULONG	mode;					// Bitmap of mode values, defined by OVR_RIFT_HOME_*
	HANDLE	userModeHandle;			// Handle of render target created in user mode
									// that's usable as a primary 
} RIFT_MODE, *PRIFT_MODE;

typedef struct _RIFT_STATUS
{
	ULONG childUid;				// Display driver assigned Uid for this display
	ULONG mode;					// Active rift mode, see OVR_RIFT_MODE_*
	ULONG serialNumber;			// Serial number as reported in the Rift's EDID
	ULONG textureHandle;		// Handle of shared render resource -- NULL if not shared 
} RIFT_STATUS, *PRIFT_STATUS;

typedef struct _RIFT_STATUS_ARRAY
{
	ULONG arraySize;			// Size of pre-allocated RIFT_STATUS structures. 
	RIFT_STATUS status[1];		// Array of status blocks containing connection information on each Rift
} RIFT_STATUS_ARRAY, *PRIFT_STATUS_ARRAY;

#pragma pack(pop)

// IOCTL for UM application to KM driver

#define OVR_STATUS_SUCCESS					 0
#define OVR_STATUS_FAIL						-1
#define OVR_STATUS_DRIVER_IN_USE			-2
#define OVR_STATUS_MODE_ALREADY_ACTIVE		-3
#define OVR_STATUS_RIFT_NOT_PRESENT			-4

//
// Returns the number of Rift displays attached to the video adapter
// If 0, no Rift displays have been connected.
// If greater than 0, use this size to pre-allocate space for an array
// of rift statuses
//
// Input Buffer: Nothing
// Output Buffer: LONG - count of Rift displays attached to video adapter
//
#define IOCTL_RIFTMGR_GET_RIFT_COUNT CTL_CODE(FILE_DEVICE_VIDEO, \
	FUNCTION_INDEX, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Fills out a pre-allocated array with information on the individually attached
// screens.
// 
// On Input, specify the arraySize as the size of the allocation.
//
// On Output, the arraySize will be updated with the actual number of Rifts
// reported. Use IOCTL_RIFTMGR_GET_RIFT_COUNT to query the number of Rifts.
// If the count changes (added or removed) between calls, the function will either fail
// due to the buffer being too small, or the arraySize count will be updated
// with a new count of devices along with their respective parameters.
//
// Input Buffer: PRIFT_STATUS - Pointer to allocated status array
// Output Buffer: LONG - Count of Rift displays reported in the structure. -1 if out of
//						 memory						
//
#define IOCTL_RIFTMGR_GET_RIFT_ARRAY CTL_CODE(FILE_DEVICE_VIDEO, \
	FUNCTION_INDEX + 1, METHOD_NEITHER, FILE_ANY_ACCESS)

// Changes the mode of an attached Rift (DEPRECATED)
// Input Buffer: PRIFT_MODE - Pointer to a mode structure specifying the childUid and
//							  mode for a particular Rift
// Output Buffer: LONG		- Non-zero on error, 0 on successful mode change
//
#define IOCTL_RIFTMGR_SET_RIFT_MODE CTL_CODE(FILE_DEVICE_VIDEO, \
	FUNCTION_INDEX + 2, METHOD_NEITHER, FILE_ANY_ACCESS)

// Lock the primary of the rift and obtain an address
// Input Buffer: ULONG			- ChildUid of a Rift as previously discovered
// Output Buffer: ULONG_PTR		- Pointer to a usermode mapped address of the primary
#define IOCTL_RIFTMGR_GET_RIFT_PRIMARY CTL_CODE(FILE_DEVICE_VIDEO, \
	FUNCTION_INDEX + 3, METHOD_NEITHER, FILE_ANY_ACCESS)

// Release Rift primary
// Input Buffer: PULONG_PTR		- ChildUid of a Rift as previously discovered and virtual pointer
// Output Buffer: NOTHING
#define IOCTL_RIFTMGR_RELEASE_RIFT_PRIMARY CTL_CODE(FILE_DEVICE_VIDEO, \
	FUNCTION_INDEX + 4, METHOD_NEITHER, FILE_ANY_ACCESS)


// Point the rift to another render target
// Input Buffer: PHANDLE		- Array of handles, rift and the render target resource
// Output Buffer: NOTHING
#define IOCTL_RIFTMGR_SETRIFTBUFFER CTL_CODE(FILE_DEVICE_VIDEO, \
	FUNCTION_INDEX + 5, METHOD_NEITHER, FILE_ANY_ACCESS)

// Enable or disable vsync on Rift present
// Input Buffer: PRIFT_SYNC		- Pointer to a mode structure specifying the childUid and
//								  and sync
// Output Buffer: NOTHING
#define IOCTL_RIFTMGR_SETVSYNCMODE CTL_CODE(FILE_DEVICE_VIDEO, \
	FUNCTION_INDEX + 6, METHOD_NEITHER, FILE_ANY_ACCESS)

// Get scan line
// Input Buffer: ULONG			- ChildUid of a Rift as previously discovered
// Output Buffer: ULONG			- 31st bit is set if in vertical blank, high 15 bits has per second 
//								  frame number (0-74), low 16 bits has scanline (0-1919)
#define IOCTL_RIFTMGR_GETSCANLINE CTL_CODE(FILE_DEVICE_VIDEO, \
	FUNCTION_INDEX + 7, METHOD_NEITHER, FILE_ANY_ACCESS)

// Enable or disable compatibility mode. Entering compatibility mode will fail if
// the Rift is already actively scanning out a surface
// Input Buffer: LONG			- Bit assignments:
// LSB (bit 0) is a flag for compatibility mode itself.
//      1 means compatibility mode.
//      0 means application direct mode.
// Bit 1 means "Hide DK1's".
//      1 means operate DK1's in synchronous with the compatibility mode exactly.
//      0 means operate in DK1 legacy mode.
// Output Buffer: LONG			- Result value (see OVR statuses)
//								   0 = success
//								  -1 = general failure
//								  -2 = failure, rift scanning out
//                                -3 = already active
//                                -4 = rift not present
#define IOCTL_RIFTMGR_SETCOMPATIBILITYMODE CTL_CODE(FILE_DEVICE_VIDEO, \
	FUNCTION_INDEX + 8, METHOD_NEITHER, FILE_ANY_ACCESS)

// Call to obtain the current status of compatibility mode
// Input Buffer: NOTHING
// Output Buffer: LONG			- Bit assignments:
// LSB (bit 0) is a flag for compatibility mode itself.
//      1 means compatibility mode.
//      0 means application direct mode.
// Bit 1 means "Hide DK1's".
//      1 means operate DK1's in synchronous with the compatibility mode exactly.
//      0 means operate in DK1 legacy mode.
#define IOCTL_RIFTMGR_GETCOMPATIBILITYMODE CTL_CODE(FILE_DEVICE_VIDEO, \
	FUNCTION_INDEX + 9, METHOD_NEITHER, FILE_ANY_ACCESS)

// Call to set the power mode of a particular Rift
// Input Buffer: PULONG_PTR		- ChildUid of a Rift as previously discovered and ULONG value
//							      second ULONG has value of 
//								  0 to simply obtain the power status of the display
//								  1 to set the display into a full power state (needs a primary to fully scan out)
//								  2 to set the display into sleep mode
//								  3 to set the display into full power off mode (WARNING: Will potentially trash primary)
// Output Buffer: LONG			- Result value 
//								  0 = Failure to obtain power status
//								  1 = Full power
//								  2 = Sleep
//								  3 = Power off
#define IOCTL_RIFTMGR_DISPLAYPOWER CTL_CODE(FILE_DEVICE_VIDEO, \
	FUNCTION_INDEX + 10, METHOD_NEITHER, FILE_ANY_ACCESS)

// Return the EDID of the display in the output buffer. The driver
// will copy as many bytes as possible to fill the buffer.
// Input Buffer: ULONG			- ChildUid of a Rift as previously discovered
// Output Buffer: PCHAR			- Preallocated buffer of a variable size to store the EDID from the display
#define IOCTL_RIFTMGR_GETEDID CTL_CODE(FILE_DEVICE_VIDEO, \
	FUNCTION_INDEX + 11, METHOD_NEITHER, FILE_ANY_ACCESS)

#endif
