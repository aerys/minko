/************************************************************************************

Filename    :   OVR_Linux_Display.cpp
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

#include "OVR_Linux_Display.h"
#include "../Kernel/OVR_Log.h"

#include "EDID/edid.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xatom.h>

//-------------------------------------------------------------------------------------
// ***** Display enumeration Helpers

namespace OVR { 

static const uint8_t edid_v1_header[] = { 0x00, 0xff, 0xff, 0xff,
                                          0xff, 0xff, 0xff, 0x00 };

static const uint8_t edid_v1_descriptor_flag[] = { 0x00, 0x00 };

static const int DESCRIPTOR_DATA       =  5;
static const int UNKNOWN_DESCRIPTOR    = -1;
static const int DETAILED_TIMING_BLOCK = -2;

// The following three functions were pulled from OVR_Linux_Display.cpp
// and modified slightly.
static int blockType(uint8_t* block)
{
  if (!strncmp((const char*)edid_v1_descriptor_flag, (const char*)block, 2))
  {
    if (block[2] != 0)
    {
      return UNKNOWN_DESCRIPTOR;
    }
    else
    {
      return block[3];
    }
  }
  else
  {
    return DETAILED_TIMING_BLOCK;
  }
}

static char* getMonitorName(const uint8_t* block)
{
  static char     name[13];
  uint8_t const*  ptr = block + DESCRIPTOR_DATA;

  for (int i = 0; i < 13; i++, ptr++)
  {
    if (*ptr == 0xa)
    {
      name[i] = 0;
      return name;
    }

    name[i] = *ptr;
  }

  return name;
}

// Returns -1 on failure, 0 otherwise.
static int parseEdid(uint8_t* edid, Linux::DisplayEDID& edidResult)
{
  const int EDID_LENGTH                        = 0x80;
  const int EDID_HEADER                        = 0x00;
  const int EDID_HEADER_END                    = 0x07;

  // const int EDID_STRUCT_VERSION                = 0x12;
  // const int EDID_STRUCT_REVISION               = 0x13;

  const int MONITOR_NAME                       = 0xfc;
  // const int MONITOR_LIMITS                     = 0xfd;
  const int MONITOR_SERIAL                     = 0xff;

  // const int ESTABLISHED_TIMING_1               = 0x23;
  // const int ESTABLISHED_TIMING_2               = 0x24;
  // const int MANUFACTURERS_TIMINGS              = 0x25;

  const int DETAILED_TIMING_DESCRIPTIONS_START = 0x36;
  const int DETAILED_TIMING_DESCRIPTION_SIZE   = 18;
  const int NO_DETAILED_TIMING_DESCRIPTIONS    = 4;

  // const int DETAILED_TIMING_DESCRIPTION_1      = 0x36;
  // const int DETAILED_TIMING_DESCRIPTION_2      = 0x48;
  // const int DETAILED_TIMING_DESCRIPTION_3      = 0x5a;
  // const int DETAILED_TIMING_DESCRIPTION_4      = 0x6c;

  const char* monitorName = "Unknown";
  uint8_t*    block       = NULL;
  uint8_t     checksum    = 0;

  for (int i = 0; i < EDID_LENGTH; i++)
  {
    checksum += edid[i];
  }

  // Bad checksum, fail EDID
  if (checksum != 0)
  {
    return -1;
  }

  if (strncmp((const char*)edid + EDID_HEADER,
              (const char*)edid_v1_header, EDID_HEADER_END + 1))
  {
    // First bytes don't match EDID version 1 header
    return -1;
  }

  // Monitor name and timings
  char serialNumber[14];
  memset(serialNumber, 0, 14);

  block = edid + DETAILED_TIMING_DESCRIPTIONS_START;

  for (int i = 0; i < NO_DETAILED_TIMING_DESCRIPTIONS;
       i++, block += DETAILED_TIMING_DESCRIPTION_SIZE)
  {

    if (blockType(block) == MONITOR_NAME)
    {
      monitorName = getMonitorName(block);
    }

    if (blockType(block) == MONITOR_SERIAL)
    {
      memcpy(serialNumber, block + 5, 13);
      break;
    }
  }

  uint8_t vendorString[4] = {0};

  vendorString[0] = (edid[8] >> 2 & 31) + 64;
  vendorString[1] = (((edid[8] & 3) << 3) | (edid[9] >> 5)) + 64;
  vendorString[2] = (edid[9] & 31) + 64;

  edidResult.ModelNumber  = *(uint16_t*)&edid[10];
  edidResult.MonitorName  = monitorName;
  edidResult.VendorName   = reinterpret_cast<const char*>(vendorString);
  edidResult.SerialNumber = serialNumber;

  // FIXME: Get timings as well

  // std::cout << "# EDID version " << static_cast<int>(edid[EDID_STRUCT_VERSION])
  //           << " revision "      << static_cast<int>(edid[EDID_STRUCT_REVISION])
  //           << std::endl;

  return 0;
}


// Returns -1 in the case of failure, 0 otherwise.
// Parameters:
//  data    OUT This pointer is modified to point to the output from
//              XRRGetOutputProperty. You *must* call XFree on this pointer.
//  dataLen OUT The length of the data returned in 'data'.
static int getXRRProperty(_XDisplay* display, RROutput output, Atom atom,
                          uint8_t** data, int* dataLen)
{
  unsigned long nitems;
  unsigned long bytesAfter;
  int           actualFormat;
  Atom          actualType;

  int ret = XRRGetOutputProperty(display, output, atom, 0, 100,
                                 False, False, AnyPropertyType,
                                 &actualType, &actualFormat, &nitems,
                                 &bytesAfter, data);

  if (None != ret)
  {
    *dataLen = nitems;
    return 0;
  }
  else
  {
    return -1;
  }
}

static XRRModeInfo* findModeByXID(XRRScreenResources* screen, RRMode xid)
{
  for (int m = 0; m < screen->nmode; ++m)
  {
    XRRModeInfo* mode = &screen->modes[m];
    if (xid == mode->id)
    {
      return mode;
    }
  }
  return NULL;
}

static int discoverExtendedRifts(OVR::Linux::DisplayDesc* descriptorArray, int inputArraySize, bool /*edidInfo*/)
{
    int result = 0;

    _XDisplay* display = XOpenDisplay(NULL);

    if (display == NULL)
    {
        OVR::LogError("[Linux Display] Unable to open X Display!");
        return 0;
    }

    Atom EDIDAtom = XInternAtom(display, RR_PROPERTY_RANDR_EDID, False);
    int numScreens = XScreenCount(display);
    for (int i = 0; i < numScreens; ++i)
    {
        Window sr                       = XRootWindow(display, i);
        XRRScreenResources* screen      = XRRGetScreenResources(display, sr);

        for (int ii = 0; ii < screen->ncrtc; ++ii)
        {
            XRRCrtcInfo* crtcInfo = XRRGetCrtcInfo(display, screen, screen->crtcs[ii]);

            if (0 == crtcInfo->noutput)
            {
                XRRFreeCrtcInfo(crtcInfo);
                continue;
            }

            bool foundOutput = false;
            RROutput output = crtcInfo->outputs[0];
            for (int k = 0; k < crtcInfo->noutput; ++k)
            {
                XRROutputInfo* outputInfo =
                    XRRGetOutputInfo(display, screen, crtcInfo->outputs[k]);

                for (int kk = 0; kk < outputInfo->nmode; ++kk)
                {
                    if (outputInfo->modes[kk] == crtcInfo->mode)
                    {
                        output = crtcInfo->outputs[k];
                        foundOutput = true;
                        break;
                    }
                }
                XRRFreeOutputInfo(outputInfo);
                if (foundOutput)
                {
                    break;
                }
            }

            if (!foundOutput)
            {
                XRRFreeCrtcInfo(crtcInfo);
                continue;
            }

            XRROutputInfo* outputInfo = XRRGetOutputInfo(display, screen, output);
            if (RR_Connected != outputInfo->connection)
            {
                XRRFreeOutputInfo(outputInfo);
                XRRFreeCrtcInfo(crtcInfo);
                continue;
            }

            // Read EDID associated with crtc.
            uint8_t* data    = NULL;
            int      dataLen = 0;
            if (getXRRProperty(display, output, EDIDAtom, &data, &dataLen) != 0)
            {
                // Identify rifts based on EDID.
                Linux::DisplayEDID edid;
                parseEdid(data, edid);
                XFree(data);
                data = NULL;

                // TODO: Remove either this 3rdParty call to read EDID data
                //       or remove our own parsing of the EDID. Probably opt
                //       to remove our parsing.
                MonitorInfo* mi = read_edid_data(display, output);
                if (mi == NULL)
                {
                    XRRFreeOutputInfo(outputInfo);
                    XRRFreeCrtcInfo(crtcInfo);
                    continue;
                }

                if (edid.VendorName == "OVR")
                {
                    if( result >= inputArraySize )
                    {
                        delete mi;
                        XRRFreeOutputInfo(outputInfo);
                        XRRFreeCrtcInfo(crtcInfo);
                        return result;
                    }

                    XRRModeInfo* modeInfo = findModeByXID(screen, crtcInfo->mode);

                    int width = modeInfo->width;
                    int height = modeInfo->height;

                    if (   crtcInfo->rotation == RR_Rotate_90
                        || crtcInfo->rotation == RR_Rotate_270 )
                    {
                        width  = modeInfo->height;
                        height = modeInfo->width;
                    }

                    int x = crtcInfo->x;
                    int y = crtcInfo->y;

                    // Generate a device ID string similar Windows does it
                    char device_id[32];
                    OVR_sprintf(device_id, 32, "%s%04d-%d",
                                mi->manufacturer_code, mi->product_code,
                                screen->crtcs[ii]);

                    OVR::Linux::DisplayDesc& desc = descriptorArray[result++];
                    desc.DisplayID                 = device_id;
                    desc.ModelName                 = edid.MonitorName;
                    desc.EdidSerialNumber          = edid.SerialNumber;
                    desc.LogicalResolutionInPixels = Sizei(width, height);
                    desc.DesktopDisplayOffset      = Vector2i(x, y);

                    switch (mi->product_code)
                    {
                        case 3: desc.DeviceTypeGuess = HmdType_DK2;       break;
                        case 2: desc.DeviceTypeGuess = HmdType_DKHDProto; break;
                        case 1: desc.DeviceTypeGuess = HmdType_DK2;       break;

                        default:
                        case 0: desc.DeviceTypeGuess = HmdType_Unknown;   break;
                    }

                    // Hard-coded defaults in case the device doesn't have the
                    // data itself. DK2 prototypes (0003) or DK HD Prototypes (0002).
                    if (   desc.DeviceTypeGuess == HmdType_DK2
                        || desc.DeviceTypeGuess == HmdType_DKHDProto)
                    {
                        desc.LogicalResolutionInPixels = Sizei(1920, 1080);
                        desc.NativeResolutionInPixels  = Sizei(1080, 1920);
                    }
                    else
                    {
                        desc.LogicalResolutionInPixels = Sizei(width, height);
                        desc.NativeResolutionInPixels  = Sizei(width, height);
                    }
                }

                delete mi;
                mi = NULL;
            }

            XRRFreeOutputInfo(outputInfo);
            XRRFreeCrtcInfo(crtcInfo);
        }

        XRRFreeScreenResources(screen);
    }

    XCloseDisplay(display);

    return result;
}


//-------------------------------------------------------------------------------------
// ***** Display 

bool Display::Initialize()
{
    // Nothing to initialize. OS X only supports compatibility mode.
    return true;
}

bool Display::GetDriverMode(bool& driverInstalled, bool& compatMode, bool& hideDK1Mode)
{
    driverInstalled = false;
    compatMode = true;
    hideDK1Mode = false;
    return true;
}

bool Display::SetDriverMode(bool /*compatMode*/, bool /*hideDK1Mode*/)
{
    return false;
}

DisplaySearchHandle* Display::GetDisplaySearchHandle()
{
	return new Linux::LinuxDisplaySearchHandle();
}

bool Display::InCompatibilityMode( bool displaySearch )
{
	OVR_UNUSED( displaySearch );
    return true;
}

int Display::GetDisplayCount(DisplaySearchHandle* handle, bool extended, bool applicationOnly, bool edidInfo)
{
    OVR_UNUSED4(handle, extended, applicationOnly, edidInfo);

	static int extendedCount = -1;

	Linux::LinuxDisplaySearchHandle* localHandle = (Linux::LinuxDisplaySearchHandle*)handle;
    if (localHandle == NULL)
    {
        OVR::LogError("[Linux Display] No search handle passed into GetDisplayCount. Return 0 rifts.");
        return 0;
    }

    if (extendedCount == -1 || extended)
    {
        extendedCount = discoverExtendedRifts(localHandle->cachedDescriptorArray, Linux::LinuxDisplaySearchHandle::DescArraySize, edidInfo);
    }

	localHandle->extended = true;
	localHandle->extendedDisplayCount = extendedCount;
	int totalCount = extendedCount;

    /// FIXME: Implement application mode for OS X.
    localHandle->application = false;
    localHandle->applicationDisplayCount = 0;

    localHandle->displayCount = totalCount;

    return totalCount;
}


Ptr<Display> Display::GetDisplay( int index, DisplaySearchHandle* handle )
{
    Ptr<Display> result = NULL;

    if (index < 0)
    {
        OVR::LogError("[Linux Display] Invalid index given to GetDisplay.");
        return NULL;
    }

	Linux::LinuxDisplaySearchHandle* localHandle = (Linux::LinuxDisplaySearchHandle*)handle;
    if (localHandle == NULL)
    {
        OVR::LogError("[Linux Display] No search handle passed into GetDisplay. Return 0 rifts.");
        return NULL;
    }

    if (localHandle->extended)
    {
        if (index >= 0 && index < (int)localHandle->extendedDisplayCount)
        {
            return *new Linux::LinuxDisplayGeneric(localHandle->cachedDescriptorArray[index]);
        }

        index -= localHandle->extendedDisplayCount;
    }

    if (localHandle->application)
    {
        OVR::LogError("[Linux Display] Mac does not support application displays.");
    }

    return result;
}


} // namespace OVR
