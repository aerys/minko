/************************************************************************************

Filename    :   OVR_Win32_ShimFunctions.cpp
Content     :   Client-side shim callbacks for usermode/rt hooks
Created     :   May 6, 2014
Authors     :   Dean Beeler

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

#include "OVR_Win32_Display.h"
#include "OVR_Win32_ShimFunctions.h"
#include "OVR_Win32_Dxgi_Display.h"
#include "../OVR_Stereo.h"
#include "OVR_Win32_FocusReader.h"

// Exported 
extern void checkUMDriverOverrides(void* context);
extern void clearUMDriverOverrides();

#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winioctl.h>
#include <SetupAPI.h>
#include <Mmsystem.h>
#include <conio.h>


//-------------------------------------------------------------------------------------
// ***** User-mode Callbacks
//
// See IsInitializingDisplay, etc in Dxgi_Display.h for details
//

extern LINK_APPLICATION_DRIVER appDriver;

BOOL WINAPI OVRIsInitializingDisplay(PVOID context, UINT width, UINT height)
{
	OVR::Win32::DisplayShim* con = (OVR::Win32::DisplayShim*)context;
	if (con->ExpectedWidth == (int)width && con->ExpectedHeight == (int)height)
		return TRUE;

	return FALSE;
}

BOOL WINAPI OVRExpectedResolution( PVOID context, UINT* width, UINT* height, UINT* rotationInDegrees )
{
	OVR::Win32::DisplayShim* con = (OVR::Win32::DisplayShim*)context;

	*width = con->ExpectedWidth;
	*height = con->ExpectedHeight;
	*rotationInDegrees = con->Rotation;
	return TRUE;
}

BOOL WINAPI OVRIsCreatingBackBuffer(PVOID context)
{
	OVR::Win32::DisplayShim* con = (OVR::Win32::DisplayShim*)context;
	if( con->ExpectedWidth != -1 && con->ExpectedHeight != -1 )
		return TRUE;

	return FALSE;
}

BOOL WINAPI OVRShouldVSync( )
{
	return FALSE;
}


ULONG WINAPI OVRRiftForContext(PVOID context, HANDLE driverHandle)
{
	OVR_UNUSED( driverHandle );
	OVR::Win32::DisplayShim* con = (OVR::Win32::DisplayShim*)context;

    return con->ChildUid;
}

HWND WINAPI OVRGetWindowForContext(PVOID context)
{
	OVR::Win32::DisplayShim* con = (OVR::Win32::DisplayShim*)context;

	if( con->Active )
	{
		return con->hWindow;
	}
	else
	{
		return 0;
	}
}

BOOL WINAPI OVRShouldPresentOnContext(PVOID context)
{
	OVR::Win32::DisplayShim* con = (OVR::Win32::DisplayShim*)context;

	return con->Active && ( con->hWindow == OVR::Win32::RenderFocusReader::GetInstance()->ReadActiveWindow() );
}

BOOL WINAPI OVRCloseRiftForContext( PVOID context, HANDLE driverHandle, ULONG rift )
{
	OVR_UNUSED( context ); OVR_UNUSED( driverHandle ); OVR_UNUSED( rift );
	// TODO
	return TRUE;
}

BOOL WINAPI OVRWindowDisplayResolution( PVOID context, UINT* width, UINT* height,
									   UINT* titleHeight, UINT* borderWidth,
									   BOOL* vsyncEnabled )
{
	OVR::Win32::DisplayShim* con = (OVR::Win32::DisplayShim*)context;
	void* handle = con->hWindow;

	if( handle )
	{
		RECT winRect = { 0 };
		GetWindowRect( (HWND)handle, &winRect );

		RECT rect = {0};
		if( GetClientRect( (HWND)handle, &rect ) )
		{
			LONG barHeight = (winRect.bottom - winRect.top) - (rect.bottom - rect.top);
			LONG borderSize = (winRect.right - winRect.left) - (rect.right - rect.left);

			*titleHeight = barHeight - borderSize + (borderSize / 2 );
			*borderWidth = borderSize / 2;
			*width = rect.right - rect.left + (borderSize / 2);
			*height = rect.bottom - rect.top + *titleHeight;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	*vsyncEnabled = TRUE;

	return TRUE;
}

BOOL WINAPI OVRShouldEnableDebug()
{
	return FALSE;
}

BOOL WINAPI OVRMirroringEnabled( PVOID context )
{
	OVR::Win32::DisplayShim* con = (OVR::Win32::DisplayShim*)context;

	return con->UseMirroring;
}

namespace OVR { namespace Win32 {

DisplayShim::DisplayShim() :
	ChildUid( 0 ),
	ExpectedWidth( 1280 ),
	ExpectedHeight( 800 ),
	Rotation( 0 ),
	hWindow( 0 ),
	UseMirroring( TRUE )
{

}

DisplayShim::~DisplayShim()
{

}

bool DisplayShim::Initialize( bool inCompatibility )
{
	if( !inCompatibility )
		checkUMDriverOverrides( this );

	return true;
}

bool DisplayShim::Shutdown()
{
    clearUMDriverOverrides();

	return true;
}

bool DisplayShim::Update(Win32ShimInfo* shimInfo)
{
	ChildUid = shimInfo->DeviceNumber;
	ExpectedWidth = shimInfo->NativeWidth;
	ExpectedHeight = shimInfo->NativeHeight;
	Rotation = shimInfo->Rotation;
	UseMirroring = shimInfo->UseMirroring != 0;
	return true;
}

void* DisplayShim::GetDX11SwapChain()
{
	if( appDriver.pfnGetDX11SwapChain )
	{
		return (*appDriver.pfnGetDX11SwapChain)(this);
	}

	return NULL;
}


} } // OVR::Win32
