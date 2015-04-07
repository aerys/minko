/************************************************************************************

PublicHeader:   None
Filename    :   OVR_Display.h
Content     :   Contains platform independent display management
Created     :   May 6, 2014
Notes       : 

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

#ifndef OVR_Display_h
#define OVR_Display_h

#include "../Sensors/OVR_DeviceConstants.h" // Required for HmdTypeEnum

#include "../Kernel/OVR_Types.h"
#include "../Kernel/OVR_Atomic.h"
#include "../Kernel/OVR_RefCount.h"
#include "../Kernel/OVR_Array.h"
#include "../Kernel/OVR_String.h"
#include "../Kernel/OVR_Math.h"

namespace OVR {


class DisplaySearchHandle : virtual public RefCountBaseV<DisplaySearchHandle>
{
public:
	DisplaySearchHandle() {}

	virtual ~DisplaySearchHandle() {}

	void operator= (const DisplaySearchHandle&) {}
};

//-------------------------------------------------------------------------------------
// ***** Display

// Display object describes an Oculus HMD screen in LibOVR, providing information such
// as EDID serial number and resolution in platform-independent manner.
//
// Display is an abstract base class to support OS and driver specific implementations.
// It support HMD screen enumeration through GetDisplayCount/GetDisplay static functions.
//
// Examples of implementations of Display are the following:
// Display_Win32_Generic - Compatibly mode implementation that maintains operation on
//						   systems without drivers.
// Display_Win32_Driver  - Driver-Based display
// Display_OSX_Generic   - Additional compatibility mode implementation for OS X

class Display : public RefCountBase<Display>
{
protected:
	enum MirrorMode
	{
		MirrorEnabled = 0,
		MirrorDisabled = 1
	};

	MirrorMode mirrorMode;

	Display(
            HmdTypeEnum deviceTypeGuess,
#ifdef OVR_OS_MAC
            uint32_t displayID,
#else
			const String& displayID,
#endif
			const String& modelName,
			const String& editSerial,
            const Sizei& logicalRes,
			const Sizei& nativeRes,
			const Vector2i& displayOffset, 
			const uint64_t devNumber,
			const uint32_t rotation,
			const bool appExclusive):
		mirrorMode(MirrorDisabled),
		DeviceTypeGuess(deviceTypeGuess),
        DisplayID(displayID),
		ModelName(modelName),
		EdidSerialNumber(editSerial),
		LogicalResolutionInPixels(logicalRes),
		NativeResolutionInPixels(nativeRes),
		DesktopDisplayOffset(displayOffset),
		DeviceNumber(devNumber),
		Rotation(rotation),
		ApplicationExclusive(appExclusive)
    {
	}

    void operator = (const Display&) { } // Quiet warning.

public:
	virtual ~Display() { }

	// ----- Platform specific static Display functionality -----

	// Mandatory function that sets up the display environment with
	// any necessary shimming and function hooks. This should be one
	// of the very first things your application does when it
	// initializes LibOVR
	static bool         Initialize();

	// Returns a count of the detected displays. These are Rift displays
	// attached directly to an active display port
	static int          GetDisplayCount( DisplaySearchHandle* handle = NULL, bool extended = true, bool applicationOnly = true, bool extendedEDIDSerials = false );
	// Returns a specific index of a display. Displays are sorted in no particular order.
	static Ptr<Display> GetDisplay( int index = 0, DisplaySearchHandle* handle = NULL ); 


    // Returns true if we are referencing the same display; useful for matching display
    // objects with the ones already detected.
    bool MatchDisplay(const Display* other)
    {
		// Note this is not checking the DeviceName, which corresponds to which monitor the device is.
		// This allows matching to match a display that has changed how it is plugged in.
		return (DisplayID == other->DisplayID) &&
               (EdidSerialNumber == other->EdidSerialNumber) &&
               (NativeResolutionInPixels == other->NativeResolutionInPixels) &&
               (DesktopDisplayOffset == other->DesktopDisplayOffset) &&
               (ApplicationExclusive == other->ApplicationExclusive);
    }


	// ----- Device independent instance based Display functionality -----

    // Device type guess based on display info.
    const HmdTypeEnum   DeviceTypeGuess;
#if defined(OVR_OS_MAC)
    // CGDirectDisplayID for the rift.
    const uint32_t      DisplayID; 
#else
	// A string denoting the display device name so that apps can recognize the monitor
	const String        DisplayID;
#endif
    // A literal string containing the name of the model, i.e. Rift DK2
    const String        ModelName;
    // Part of the serial number encoded in Edid, used for monitor <-> sensor matching.
    const String        EdidSerialNumber;
    // Logical resolution is the display resolution in presentation terms.
    // That is to say, the resolution that represents the orientation the
    // display is projected to the user. For DK2, while being a portrait display
    // the display is held in landscape and therefore the logical resolution
    // is 1920x1080
    const Sizei         LogicalResolutionInPixels;
    // Native resolution is the resolution reported by the EDID and represents the
    // exact hardware resolution of the Rift. For example, on DK2
    // this is 1080x1920
    // In theory, an OS rotated Rift's native and logical resolutions should match
    const Sizei         NativeResolutionInPixels;
    // For displays that are attached to the desktop, this return value has meaning.
    // Otherwise it should always return origin
    const Vector2i      DesktopDisplayOffset;
	// For Windows machines this value stores the ChildUid used to identify this display
	const uint64_t	    DeviceNumber;
	// Stores the device specific default rotation of the screen
	// E.g. DK2 is rotated 90 degrees as it is a portrait display
	const uint32_t	    Rotation;
	// Is set if the Display is capable in Application-Only mode
	const bool			ApplicationExclusive;

	// Functionality for rendering within the window
	virtual MirrorMode SetMirrorMode( MirrorMode newMode ) = 0;

	// Functionality for enabling/disabling display
    virtual bool SetDisplaySleep(bool off)
    {
        // Override to implement if supported
        OVR_UNUSED(off);
        return false;
    }

    // Check if right now the current rendering application should be in compatibility mode
    static bool InCompatibilityMode( bool displaySearch = true );

    // Get/set the mode for all applications
    static bool GetDriverMode(bool& driverInstalled, bool& compatMode, bool& hideDK1Mode);
    static bool SetDriverMode(bool compatMode, bool hideDK1Mode);

    static DisplaySearchHandle* GetDisplaySearchHandle();
};


} // namespace OVR

#endif
