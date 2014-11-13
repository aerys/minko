/************************************************************************************

Filename    :   Service_NetSessionCommon.cpp
Content     :   Server for service interface
Created     :   June 12, 2014
Authors     :   Kevin Jenkins, Chris Taylor

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

#include "Service_NetSessionCommon.h"
#include "../OVR_Stereo.h"

namespace OVR { namespace Service {


//// NetSessionCommon

NetSessionCommon::NetSessionCommon() :
    Terminated(false)
{
    pSession = new Net::Session;
    OVR_ASSERT(pSession != NULL);

    pRPC = new Net::Plugins::RPC1;
    OVR_ASSERT(pRPC != NULL);

    pSession->AddSessionListener(pRPC);
}

NetSessionCommon::~NetSessionCommon()
{
    if (pSession)
    {
        delete pSession;
        pSession = NULL;
    }
    if (pRPC)
    {
        delete pRPC;
        pRPC = NULL;
    }

    Terminated = true;

    OVR_ASSERT(IsFinished());
}

void NetSessionCommon::onSystemDestroy()
{
    Terminated = true;

    Join();

    Release();
}

void NetSessionCommon::onThreadDestroy()
{
    Terminated = true;
    if (pSession)
    {
        pSession->Shutdown();
    }
}

void NetSessionCommon::SerializeHMDInfo(Net::BitStream *bitStream, HMDInfo* hmdInfo)
{
    bitStream->Write(hmdInfo->ProductName);
    bitStream->Write(hmdInfo->Manufacturer);

    int32_t w = hmdInfo->Version;
    bitStream->Write(w);

    w = hmdInfo->HmdType;
    bitStream->Write(w);

    w = hmdInfo->ResolutionInPixels.w;
    bitStream->Write(w);

    w = hmdInfo->ResolutionInPixels.h;
    bitStream->Write(w);

    w = hmdInfo->ShimInfo.DeviceNumber;
    bitStream->Write(w);

    w = hmdInfo->ShimInfo.NativeWidth;
    bitStream->Write(w);

    w = hmdInfo->ShimInfo.NativeHeight;
    bitStream->Write(w);

    w = hmdInfo->ShimInfo.Rotation;
    bitStream->Write(w);

    bitStream->Write(hmdInfo->ScreenSizeInMeters.w);
    bitStream->Write(hmdInfo->ScreenSizeInMeters.h);
    bitStream->Write(hmdInfo->ScreenGapSizeInMeters);
    bitStream->Write(hmdInfo->CenterFromTopInMeters);
    bitStream->Write(hmdInfo->LensSeparationInMeters);

    w = hmdInfo->DesktopX;
    bitStream->Write(w);

    w = hmdInfo->DesktopY;
    bitStream->Write(w);

    w = hmdInfo->Shutter.Type;
    bitStream->Write(w);

    bitStream->Write(hmdInfo->Shutter.VsyncToNextVsync);
    bitStream->Write(hmdInfo->Shutter.VsyncToFirstScanline);
    bitStream->Write(hmdInfo->Shutter.FirstScanlineToLastScanline);
    bitStream->Write(hmdInfo->Shutter.PixelSettleTime);
    bitStream->Write(hmdInfo->Shutter.PixelPersistence);
    bitStream->Write(hmdInfo->DisplayDeviceName);

    w = hmdInfo->DisplayId;
    bitStream->Write(w);

    bitStream->Write(hmdInfo->PrintedSerial);

    uint8_t b = hmdInfo->InCompatibilityMode ? 1 : 0;
    bitStream->Write(b);

    w = hmdInfo->VendorId;
    bitStream->Write(w);

    w = hmdInfo->ProductId;
    bitStream->Write(w);

    bitStream->Write(hmdInfo->CameraFrustumFarZInMeters);
    bitStream->Write(hmdInfo->CameraFrustumHFovInRadians);
    bitStream->Write(hmdInfo->CameraFrustumNearZInMeters);
    bitStream->Write(hmdInfo->CameraFrustumVFovInRadians);

    w = hmdInfo->FirmwareMajor;
    bitStream->Write(w);

    w = hmdInfo->FirmwareMinor;
    bitStream->Write(w);
}

bool NetSessionCommon::DeserializeHMDInfo(Net::BitStream *bitStream, HMDInfo* hmdInfo)
{
    bitStream->Read(hmdInfo->ProductName);
    bitStream->Read(hmdInfo->Manufacturer);

    int32_t w = 0;
    if (!bitStream->Read(w))
    {
        // This indicates that no HMD could be found
        return false;
    }
    hmdInfo->Version = w;

    bitStream->Read(w);
    hmdInfo->HmdType = (HmdTypeEnum)w;

    bitStream->Read(w);
    hmdInfo->ResolutionInPixels.w = w;

    bitStream->Read(w);
    hmdInfo->ResolutionInPixels.h = w;

    bitStream->Read(w);
    hmdInfo->ShimInfo.DeviceNumber = w;

    bitStream->Read(w);
    hmdInfo->ShimInfo.NativeWidth = w;

    bitStream->Read(w);
    hmdInfo->ShimInfo.NativeHeight = w;

    bitStream->Read(w);
    hmdInfo->ShimInfo.Rotation = w;

    bitStream->Read(hmdInfo->ScreenSizeInMeters.w);
    bitStream->Read(hmdInfo->ScreenSizeInMeters.h);
    bitStream->Read(hmdInfo->ScreenGapSizeInMeters);
    bitStream->Read(hmdInfo->CenterFromTopInMeters);
    bitStream->Read(hmdInfo->LensSeparationInMeters);

    bitStream->Read(w);
    hmdInfo->DesktopX = w;

    bitStream->Read(w);
    hmdInfo->DesktopY = w;

    bitStream->Read(w);
    hmdInfo->Shutter.Type = (HmdShutterTypeEnum)w;

    bitStream->Read(hmdInfo->Shutter.VsyncToNextVsync);
    bitStream->Read(hmdInfo->Shutter.VsyncToFirstScanline);
    bitStream->Read(hmdInfo->Shutter.FirstScanlineToLastScanline);
    bitStream->Read(hmdInfo->Shutter.PixelSettleTime);
    bitStream->Read(hmdInfo->Shutter.PixelPersistence);
    bitStream->Read(hmdInfo->DisplayDeviceName);

    bitStream->Read(w);
    hmdInfo->DisplayId = w;

    bitStream->Read(hmdInfo->PrintedSerial);

    uint8_t b = 0;
    bitStream->Read(b);
    hmdInfo->InCompatibilityMode = (b != 0);

    bitStream->Read(w);
    hmdInfo->VendorId = w;

    bitStream->Read(w);
    hmdInfo->ProductId = w;

    bitStream->Read(hmdInfo->CameraFrustumFarZInMeters);
    bitStream->Read(hmdInfo->CameraFrustumHFovInRadians);
    bitStream->Read(hmdInfo->CameraFrustumNearZInMeters);
    bitStream->Read(hmdInfo->CameraFrustumVFovInRadians);

    bitStream->Read(w);
    hmdInfo->FirmwareMajor = w;

    if (!bitStream->Read(w))
    {
        OVR_ASSERT(false);
        return false;
    }
    hmdInfo->FirmwareMinor = w;

    return true;
}

// Prefix key names with this to pass through to server
static const char* BypassPrefix = "server:";

static const char* KeyNames[][NetSessionCommon::ENumTypes] = {
    /* EGetStringValue */ { "CameraSerial", "CameraUUID", 0 },
    /* EGetBoolValue */ { "ReleaseDK2Sensors", "ReleaseLegacySensors", 0 },
    /* EGetIntValue */ { 0 },
    /* EGetNumberValue */{ "CenterPupilDepth", 0 },
    /* EGetNumberValues */{ "NeckModelVector3f", 0 },
    /* ESetStringValue */ { 0 },
    /* ESetBoolValue */ { "ReleaseDK2Sensors", "ReleaseLegacySensors", 0 },
    /* ESetIntValue */ { 0 },
    /* ESetNumberValue */{ "CenterPupilDepth", 0 },
    /* ESetNumberValues */{ "NeckModelVector3f", 0 }
};

bool IsInStringArray(const char* a[], const char* key)
{
    for (int i = 0; a[i]; ++i)
    {
        if (OVR_strcmp(a[i], key) == 0)
            return true;
    }

    return false;
}

const char *NetSessionCommon::FilterKeyPrefix(const char* key)
{
    // If key starts with BypassPrefix,
    if (strstr(key, BypassPrefix) == key)
    {
        key += strlen(BypassPrefix);
    }

    return key;
}

bool NetSessionCommon::IsServiceProperty(EGetterSetters e, const char* key)
{
    if ((e >= 0 && e < ENumTypes) && IsInStringArray(KeyNames[e], key))
    {
        return true;
    }

    // If key starts with BypassPrefix,
    if (strstr(key, BypassPrefix) == key)
    {
        return true;
    }

    return false;
}


}} // namespace OVR::Service
