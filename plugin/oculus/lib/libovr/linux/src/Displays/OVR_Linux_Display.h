/************************************************************************************

Filename    :   OVR_Linux_Display.h
Content     :   Linux-specific Display declarations
Created     :   July 2, 2014
Authors     :   James Hughes

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

*************************************************************************************/

#ifndef OVR_Linux_Display_h
#define OVR_Linux_Display_h

#include "OVR_Display.h"

namespace OVR { namespace Linux {


//-------------------------------------------------------------------------------------
// DisplayDesc

// Display information enumerable through OS .
// TBD: Should we just move this to public header, so it's a const member of Display?
struct DisplayDesc
{
    HmdTypeEnum DeviceTypeGuess;
    String      DisplayID; // This is the device identifier string from MONITORINFO (for app usage)
    String      ModelName; // This is a "DK2" type string
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
    DisplayEDID() :
        ModelNumber(0)
    {}

    String MonitorName;
    UInt16 ModelNumber;
    String VendorName;
    String SerialNumber;
};


//-------------------------------------------------------------------------------------
// Linux Display Search Handle
class LinuxDisplaySearchHandle : public DisplaySearchHandle
{
public:
    LinuxDisplaySearchHandle() :
        extended(false),
        application(false),
        extendedDisplayCount(0),
        applicationDisplayCount(0),
        displayCount(0)
    {}
    virtual ~LinuxDisplaySearchHandle()   {}

    static const int DescArraySize = 16;

    Linux::DisplayDesc  cachedDescriptorArray[DescArraySize];
    bool                extended;
    bool                application;
    int                 extendedDisplayCount;
    int                 applicationDisplayCount;
    int                 displayCount;
};

//-------------------------------------------------------------------------------------
// LinuxDisplayGeneric

// Describes Linux display in Compatibility mode, containing basic data
class LinuxDisplayGeneric : public Display
{
public:
    LinuxDisplayGeneric( const DisplayDesc& dd ) :
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

    virtual ~LinuxDisplayGeneric()
    {
    }

    virtual bool InCompatibilityMode() const
    {
        return true;
    }

    // Generic displays are not capable of mirroring
    virtual MirrorMode SetMirrorMode( MirrorMode newMode ) 
    { 
        OVR_UNUSED( newMode ); 
        return MirrorDisabled; 
    } 
};

}} // namespace OVR::Linux

#endif // OVR_Linux_Display_h
