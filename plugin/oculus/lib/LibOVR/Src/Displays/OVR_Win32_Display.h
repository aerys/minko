/************************************************************************************

Filename    :   OVR_Win32_Display.h
Content     :   Win32-specific Display declarations
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

#ifndef OVR_Win32_Display_h
#define OVR_Win32_Display_h

#include "OVR_Display.h"

namespace OVR { namespace Win32 {


//-------------------------------------------------------------------------------------
// DisplayDesc

// Display information enumerable through Win32.
// TBD: Should we just move this to public header, so it's a const member of Display?
struct DisplayDesc
{
    HmdTypeEnum DeviceTypeGuess; // This is a guess about what type of HMD it is connected to
	String      DisplayID;       // This is the device identifier string from MONITORINFO (for app usage)
    String      ModelName;       // This is a "DK2" type string
    String      EdidSerialNumber;
    Sizei       LogicalResolutionInPixels;
    Sizei       NativeResolutionInPixels;
    Vector2i    DesktopDisplayOffset;
};


//-------------------------------------------------------------------------------------
// DisplayEDID

// Describes EDID information as reported from our display driver.
struct DisplayEDID
{
	String MonitorName;
	UINT16 ModelNumber;
	String VendorName;
	String SerialNumber;
};

class Win32DisplaySearchHandle : public DisplaySearchHandle
{
public:
    static const int ArraySize = 16;

    Win32::DisplayDesc cachedDescriptorArray[ArraySize];
    bool			   extended;
    bool			   application;
    int				   extendedDisplayCount;
    int				   applicationDisplayCount;
    int				   displayCount;

    Win32DisplaySearchHandle()
    {
    }
	virtual ~Win32DisplaySearchHandle()
    {
    }
};

//-------------------------------------------------------------------------------------
// Win32DisplayGeneric

// Describes Win32 display in Compatibility mode, containing basic data
class Win32DisplayGeneric : public Display
{
public:
	Win32DisplayGeneric( const DisplayDesc& dd ) :
		Display(dd.DeviceTypeGuess,
				dd.DisplayID,
				dd.ModelName,
				dd.EdidSerialNumber,
				dd.LogicalResolutionInPixels,
				dd.NativeResolutionInPixels,
				dd.DesktopDisplayOffset,
				0,
				0,
				false)
    {
	}

	virtual ~Win32DisplayGeneric()
	{
	}

	// Generic displays are not capable of mirroring
	virtual MirrorMode SetMirrorMode( MirrorMode newMode ) 
	{ 
		OVR_UNUSED( newMode ); 
		return MirrorDisabled; 
	} 
};


//-------------------------------------------------------------------------------------
// Win32DisplayDriver

// Oculus driver based display object.
class Win32DisplayDriver : public Display
{
	HANDLE		hDevice;
	ULONG		ChildId;
	DisplayEDID Edid;

public:
    Win32DisplayDriver(const HmdTypeEnum  deviceTypeGuess,
                       const String&      displayID,
					   const String&      modelName,
					   const String&      edidSerial,
                       const Sizei&       logicalRes,
					   const Sizei&       nativeRes,
					   const Vector2i&    displayOffset,
                       const DisplayEDID& edid,
					   HANDLE hdevice,
					   ULONG child,
					   uint32_t rotation) :
		Display(deviceTypeGuess,
				displayID,
				modelName,
				edidSerial,
				logicalRes,
				nativeRes,
				displayOffset,
				child,
				rotation,
				true),
		hDevice(hdevice),
		ChildId(child),
		Edid(edid)
    {
	}

	virtual ~Win32DisplayDriver()
	{
	}

	virtual MirrorMode SetMirrorMode( MirrorMode newMode );

    // Support sleep/wake
	virtual bool SetDisplaySleep(bool off);
};


}} // namespace OVR::Win32

#endif // OVR_Win32_Display_h
