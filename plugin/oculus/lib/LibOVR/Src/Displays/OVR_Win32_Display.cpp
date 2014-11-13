/************************************************************************************

Filename    :   OVR_Win32_Display.cpp
Content     :   Win32 Display implementation
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

#include <Windows.h>

#include "OVR_Win32_Display.h"
#include "OVR_Win32_Dxgi_Display.h"

#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <stdlib.h>
#include <winioctl.h>
#include <SetupAPI.h>
#include <Mmsystem.h>
#include <conio.h>

#ifdef OVR_COMPAT_EDID_VIA_WMI
# pragma comment(lib, "wbemuuid.lib")
#include <wbemidl.h>
#else // OVR_COMPAT_EDID_VIA_WMI
#include <setupapi.h>
#include <initguid.h>
#endif // OVR_COMPAT_EDID_VIA_WMI

// WIN32_LEAN_AND_MEAN included in OVR_Atomic.h may break 'byte' declaration.
#ifdef WIN32_LEAN_AND_MEAN
 typedef unsigned char byte;
#endif

#include "../Kernel/OVR_String.h"
#include "../Kernel/OVR_Log.h"

typedef struct
{
	HANDLE	hDevice;
	UINT	ExpectedWidth;
	UINT	ExpectedHeight;
	HWND	hWindow;
	bool	CompatibilityMode;
    bool    HideDK1Mode;
} ContextStruct;

static ContextStruct GlobalDisplayContext = {0};

 
//-------------------------------------------------------------------------------------
// ***** Display enumeration Helpers

// THere are two ways to enumerate display: through our driver (DeviceIoControl)
// and through Win32 EnumDisplayMonitors (compatibility mode).


namespace OVR { 

ULONG getRiftCount( HANDLE hDevice )
{
	ULONG riftCount = 0;
	DWORD bytesReturned = 0;

	BOOL result = DeviceIoControl( hDevice, IOCTL_RIFTMGR_GET_RIFT_COUNT, NULL, 0,
                                   &riftCount, sizeof( ULONG ), &bytesReturned, NULL );

	if( result )
		return riftCount;
	else
		return 0;
}

ULONG getRift( HANDLE hDevice, int index )
{
	ULONG riftCount = getRiftCount( hDevice );
	DWORD bytesReturned = 0;
	BOOL result;

	if( riftCount >= (ULONG)index )
	{
		RIFT_STATUS riftStatus[16] = {0};

		result = DeviceIoControl( hDevice, IOCTL_RIFTMGR_GET_RIFT_ARRAY, riftStatus,
                                  riftCount * sizeof( RIFT_STATUS ), &riftCount,
                                  sizeof( ULONG ), &bytesReturned, NULL );
		if( result )
		{
			PRIFT_STATUS tmpRift;
			unsigned int i;
			for( i = 0, tmpRift = riftStatus; i < riftCount; ++i, ++tmpRift )
			{
				if( i == (unsigned int)index )
					return tmpRift->childUid;
			}
		}
		else
		{
			printf("Failed to get array of rift devices\n");
		}
	}

	return 0;
}

#define EDID_LENGTH                             0x80

#define EDID_HEADER                             0x00
#define EDID_HEADER_END                         0x07

#define ID_MANUFACTURER_NAME                    0x08
#define ID_MANUFACTURER_NAME_END                0x09

#define EDID_STRUCT_VERSION                     0x12
#define EDID_STRUCT_REVISION                    0x13

#define ESTABLISHED_TIMING_1                    0x23
#define ESTABLISHED_TIMING_2                    0x24
#define MANUFACTURERS_TIMINGS                   0x25

#define DETAILED_TIMING_DESCRIPTIONS_START      0x36
#define DETAILED_TIMING_DESCRIPTION_SIZE        18
#define NO_DETAILED_TIMING_DESCRIPTIONS         4

#define DETAILED_TIMING_DESCRIPTION_1           0x36
#define DETAILED_TIMING_DESCRIPTION_2           0x48
#define DETAILED_TIMING_DESCRIPTION_3           0x5a
#define DETAILED_TIMING_DESCRIPTION_4           0x6c

#define MONITOR_NAME            0xfc
#define MONITOR_LIMITS          0xfd
#define MONITOR_SERIAL			0xff

#define UNKNOWN_DESCRIPTOR      -1
#define DETAILED_TIMING_BLOCK   -2

#define DESCRIPTOR_DATA         5

const byte edid_v1_header[] = { 0x00, 0xff, 0xff, 0xff,
	                            0xff, 0xff, 0xff, 0x00 };

const byte edid_v1_descriptor_flag[] = { 0x00, 0x00 };


static int blockType( byte* block )
{
	if ( !strncmp( (const char*)edid_v1_descriptor_flag, (const char*)block, 2 ) )
	{
		// descriptor
		if ( block[ 2 ] != 0 )
			return UNKNOWN_DESCRIPTOR;
		return block[ 3 ];
	}
    else
    {		
		return DETAILED_TIMING_BLOCK;
	}
}

static char* getMonitorName( byte const* block )
{
	static char name[ 13 ];
	unsigned    i;
	byte const* ptr = block + DESCRIPTOR_DATA;

	for( i = 0; i < 13; i++, ptr++ )
	{
		if ( *ptr == 0xa )
		{
			name[ i ] = 0;
			return name;
		}

		name[ i ] = *ptr;
	}

	return name;
}


static bool parseEdid( byte* edid, OVR::Win32::DisplayEDID& edidResult )
{
	unsigned i;
	byte* block;
	char* monitor_name = "Unknown";
	byte checksum = 0;

	for( i = 0; i < EDID_LENGTH; i++ )
		checksum += edid[ i ];

	// Bad checksum, fail EDID
	if (  checksum != 0  )
		return false;

	if ( strncmp( (const char*)edid+EDID_HEADER, (const char*)edid_v1_header, EDID_HEADER_END+1 ) )
	{
		// First bytes don't match EDID version 1 header
		return false;
	}

	//printf( "\n# EDID version %d revision %d\n", (int)edid[EDID_STRUCT_VERSION],(int)edid[EDID_STRUCT_REVISION] );

	// Monitor name and timings 

	char serialNumber[14];
	memset( serialNumber, 0, 14 );

	block = edid + DETAILED_TIMING_DESCRIPTIONS_START;

	for( i = 0; i < NO_DETAILED_TIMING_DESCRIPTIONS; i++,
		block += DETAILED_TIMING_DESCRIPTION_SIZE )
	{

		if ( blockType( block ) == MONITOR_NAME )
		{
			monitor_name = getMonitorName( block );
		}

		if( blockType( block ) == MONITOR_SERIAL )
		{
			memcpy( serialNumber, block + 5, 13 );
			break;
		}
	}

	BYTE vendorString[4] = {0};

	vendorString[0] = (edid[8] >> 2 & 31) + 64;
	vendorString[1] = ((edid[8] & 3) << 3) | (edid[9] >> 5) + 64;
	vendorString[2] = (edid[9] & 31) + 64;

	edidResult.ModelNumber  = *(UINT16*)&edid[10];
	edidResult.MonitorName  = OVR::String(monitor_name);
	edidResult.VendorName   = OVR::String((const char*)vendorString);
	edidResult.SerialNumber = OVR::String(serialNumber);
	
#if 0
	printf( "\tIdentifier \"%s\"\n", monitor_name );
	printf( "\tVendorName \"%s\"\n", vendorString );
	printf( "\tModelName \"%s\"\n", monitor_name );
	printf( "\tModelNumber %d\n", modelNumber );
	printf( "\tSerialNumber \"%x\"\n", *serialPointer );
#endif

	// FIXME: Get timings as well, though they aren't very useful here
	// except for the vertical refresh rate, presumably

	return true;
}

static bool getEdid(HANDLE hDevice, ULONG uid, OVR::Win32::DisplayEDID& edidResult)
{
	ULONG       riftCount = 0;
	DWORD       bytesReturned = 0;
	RIFT_STATUS riftStatus[16] = {0};

	BOOL result = DeviceIoControl( hDevice, IOCTL_RIFTMGR_GET_RIFT_COUNT, NULL, 0,
                                   &riftCount, sizeof( ULONG ), &bytesReturned, NULL );

    if (!result)
    {
        return false;
    }

	result = DeviceIoControl( hDevice, IOCTL_RIFTMGR_GET_RIFT_ARRAY, &riftStatus,
                              riftCount * sizeof( RIFT_STATUS ), &riftCount, sizeof(ULONG),
                              &bytesReturned, NULL );
    if (!result)
    {
        return false;
    }

    for (ULONG i = 0; i < riftCount; ++i)
    {
		ULONG riftUid = riftStatus[i].childUid;
        if (riftUid == uid)
        {
            char edidBuffer[512];

            result = DeviceIoControl(hDevice, IOCTL_RIFTMGR_GETEDID, &riftUid, sizeof(ULONG),
                                     edidBuffer, 512, &bytesReturned, NULL);

            if (result)
            {
                if (parseEdid((byte*)edidBuffer, edidResult))
                {
                    return true;
                }
                else
                {
                    OVR_DEBUG_LOG(("[Win32Display] WARNING: The driver was not able to return EDID for a display"));
                }
            }

            break;
        }
	}

    return false;
}


// Used to capture all the active monitor handles
struct MonitorSet
{
    enum { MaxMonitors = 8 };
    HMONITOR Monitors[MaxMonitors];
    int      MonitorCount;
    int      PrimaryCount;
};

static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData)
{
	MonitorSet* monitorSet = (MonitorSet*)dwData;
	if (monitorSet->MonitorCount >= MonitorSet::MaxMonitors)
		return FALSE;

	monitorSet->Monitors[monitorSet->MonitorCount] = hMonitor;
	monitorSet->MonitorCount++;
	return TRUE;
};

#ifdef OVR_COMPAT_EDID_VIA_WMI

static bool getCompatDisplayEDID( WCHAR* displayName, String& serialNumberStr, String& userFriendlyNameStr )
{
	IWbemLocator *pLoc = NULL;
	IWbemServices *pSvc = NULL;
	HRESULT hres;

	static bool initialized = false;
	static bool selfInitialized = true;
	if (!initialized)
	{
		hres = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (FAILED(hres))
		{
            
            LogError("{ERR-082} [Display] WARNING: Failed to initialize COM library. Error code = 0x%x", hres);
            OVR_ASSERT(false);
            return false;
		}

		hres = CoInitializeSecurity(
			NULL,
			-1,                          // COM authentication
			NULL,                        // Authentication services
			NULL,                        // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
			RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
			NULL,                        // Authentication info
			EOAC_NONE,                   // Additional capabilities 
			NULL                         // Reserved
			);

		if (FAILED(hres))
		{
            LogError("{ERR-083} [Display] WARNING: Failed to initialize security. Error code = 0x%x", hres);
            OVR_ASSERT(false);
            selfInitialized = false;
		}

		initialized = true;
	}

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres))
	{
        LogError("{ERR-084} [Display] WARNING: Failed to create IWbemLocator object.Err code = 0x%x", hres);
        OVR_ASSERT(false);
        return false;
	}

	BSTR AbackB = SysAllocString(L"root\\WMI");
	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		AbackB, // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (e.g. Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
		);
	SysFreeString(AbackB);

	if (FAILED(hres))
	{
        LogError("{ERR-085} [Display] WARNING: Could not connect to root\\WMI. Error code = 0x%x", hres);
        OVR_ASSERT(false);
        pLoc->Release();
		return false;
	}

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres))
	{
        LogError("{ERR-086} [Display] WARNING: Could not set proxy blanket. Error code = 0x%x", hres);
        OVR_ASSERT(false);
        pSvc->Release();
		pLoc->Release();
		return false;
	}


	BSTR wql = SysAllocString(L"WQL");
	BSTR select = SysAllocString(L"SELECT * FROM WmiMonitorID");
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		wql,
		select,
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);
	SysFreeString(wql);
	SysFreeString(select);

	if (FAILED(hres))
	{
        LogError("{ERR-087} [Display] WARNING: Query for operating system name failed. Error code = 0x%x", hres);
        OVR_ASSERT(false);
        pSvc->Release();
		pLoc->Release();
		return false;
	}

    int enumeratedCount = 0;
    bool found = false;

	IWbemClassObject *pclsObj = 0;
	while (pEnumerator)
	{
        ULONG uReturn = 0;
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (FAILED(hr) || !uReturn)
        {
            break;
        }

        ++enumeratedCount;

		VARIANT vtProp;
		hr = pclsObj->Get(L"InstanceName", 0, &vtProp, 0, 0);

		WCHAR* instanceName = vtProp.bstrVal;
		WCHAR* nextToken = NULL;
		if (SUCCEEDED(hr) &&
            wcstok_s(instanceName, L"\\", &nextToken) != NULL)
		{
			WCHAR* aToken = wcstok_s(NULL, L"\\", &nextToken);

			if (aToken != NULL)
			{
				VariantClear(&vtProp);

				if (wcscmp(aToken, displayName) != 0)
				{
                    pclsObj->Release();
                    continue;
				}

				// Read serial

				hr = pclsObj->Get(L"SerialNumberID", 0, &vtProp, 0, 0);

                if (SUCCEEDED(hr))
                {
                    if (vtProp.vt != VT_NULL && vtProp.parray != NULL)
                    {
                        static const int MaxSerialBytes = 14;
                        char serialNumber[MaxSerialBytes] = { 0 };

                        UINT32* serialArray = (UINT32*)vtProp.parray->pvData;
                        for (int i = 0; i < MaxSerialBytes; ++i)
                        {
                            serialNumber[i] = (BYTE)(serialArray[i] & 0xff);
                        }

                        serialNumber[sizeof(serialNumber)-1] = '\0';
                        serialNumberStr = serialNumber;
                    }
                    else
                    {
                        LogError("{ERR-088} [Display] WARNING: Wrong data format for SerialNumberID");
                    }

                    VariantClear(&vtProp);
                }
                else
                {
                    LogError("{ERR-089} [Display] WARNING: Failure getting display SerialNumberID: %d", (int)hr);
                }

				// Read length of name

                int userFriendlyNameLen = 0;

				hr = pclsObj->Get(L"UserFriendlyNameLength", 0, &vtProp, 0, 0);

                if (SUCCEEDED(hr))
                {
                    if (vtProp.vt != VT_NULL)
                    {
                        userFriendlyNameLen = vtProp.iVal;

                        if (userFriendlyNameLen <= 0)
                        {
                            userFriendlyNameLen = 0;

                            LogError("{ERR-090} [Display] WARNING: UserFriendlyNameLength = 0");
                        }
                    }
                    else
                    {
                        LogError("{ERR-091} [Display] WARNING: Wrong data format for UserFriendlyNameLength");
                    }

                    VariantClear(&vtProp);
                }
                else
                {
                    LogError("{ERR-092} [Display] WARNING: Failure getting display UserFriendlyNameLength: %d", (int)hr);
                }

				// Read name

				hr = pclsObj->Get(L"UserFriendlyName", 0, &vtProp, 0, 0);

                if (SUCCEEDED(hr) && userFriendlyNameLen > 0)
                {
                    if (vtProp.vt != VT_NULL && vtProp.parray != NULL)
                    {
                        static const int MaxNameBytes = 64;
                        char userFriendlyName[MaxNameBytes] = { 0 };

                        UINT32* nameArray = (UINT32*)vtProp.parray->pvData;
                        for (int i = 0; i < MaxNameBytes && i < userFriendlyNameLen; ++i)
                        {
                            userFriendlyName[i] = (BYTE)(nameArray[i] & 0xff);
                        }

                        userFriendlyName[sizeof(userFriendlyName)-1] = '\0';
                        userFriendlyNameStr = userFriendlyName;
                    }
                    else
                    {
                        // See: https://developer.oculusvr.com/forums/viewtopic.php?f=34&t=10961
                        // This can happen if someone has an EDID override in the registry.
                        LogError("{ERR-093} [Display] WARNING: Wrong data format for UserFriendlyName");
                    }

                    VariantClear(&vtProp);
                }
                else
                {
                    LogError("{ERR-094} [Display] WARNING: Failure getting display UserFriendlyName: %d", (int)hr);
                }
			}

            found = true;
            pclsObj->Release();
            break;
        }

		pclsObj->Release();
	}

	HMODULE hModule = GetModuleHandleA("wbemuuid");
	if (hModule)
	{
		DisableThreadLibraryCalls(hModule);
	}

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();

    if (!found)
    {
        LogError("{ERR-095} [Display] WARNING: Unable to enumerate the rift via WMI (found %d monitors). This is not normally an issue. Running as a user with Administrator privileges has fixed this problem in the past.", enumeratedCount);
        OVR_ASSERT(false);
    }

	return found;
}

#else // OVR_COMPAT_EDID_VIA_WMI

#define NAME_SIZE 128

DEFINE_GUID(GUID_CLASS_MONITOR,
    0x4d36e96e, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1,
    0x03, 0x18);

static void truncateNonalphanum(char* str, int len)
{
    for (int i = 0; i < len; ++i)
    {
        char ch = str[i];

        if ((ch < 'A' || ch > 'Z') &&
            (ch < 'a' || ch > 'z') &&
            (ch < '0' || ch > '9') &&
            ch != ' ')
        {
            str[i] = '\0';
            break;
        }
    }
}

static bool AccessDeviceRegistry(IN HDEVINFO devInfo, IN PSP_DEVINFO_DATA devInfoData,
                                 const WCHAR* displayName, String& serialNumberStr, String& userFriendlyNameStr)
{
    // Match hardware id to display name
    WCHAR hardwareId[128];
    if (!SetupDiGetDeviceRegistryProperty(
        devInfo,
        devInfoData,
        SPDRP_HARDWAREID,
        NULL,
        (PBYTE)hardwareId,
        sizeof(hardwareId),
        NULL))
    {
        LogError("{ERR-096} [Display] WARNING: SetupDiGetDeviceRegistryProperty for SPDRP_HARDWAREID failed. LastErr=%d", GetLastError());
        OVR_ASSERT(false);
        return false;
    }
    hardwareId[127] = 0;

    // If the hardware id did not match,
    if (!wcsstr(hardwareId, displayName))
    {
        // Stop here
        return false;
    }

    // Grab hardware info registry key
    HKEY hDevRegKey = SetupDiOpenDevRegKey(
        devInfo,
        devInfoData,
        DICS_FLAG_GLOBAL,
        0,
        DIREG_DEV,
        KEY_READ); // Only read permissions so it can be run without Administrator privs

    if (hDevRegKey == INVALID_HANDLE_VALUE)
    {
        LogError("{ERR-097} [Display] WARNING: SetupDiOpenDevRegKey failed. LastErr=%d", GetLastError());
        OVR_ASSERT(false);
        return false;
    }

    // Enumerate keys in registry
    bool found = false;

    // For each key,
    for (int i = 0;; i++)
    {
        BYTE EDIDdata[1024];
        DWORD edidsize = sizeof(EDIDdata);

        DWORD dwType, ActualValueNameLength = NAME_SIZE;
        CHAR valueName[NAME_SIZE];

        // Read the key value data
        LSTATUS retValue = RegEnumValueA(
            hDevRegKey,
            i,
            &valueName[0],
            &ActualValueNameLength,
            NULL,
            &dwType,
            EDIDdata,
            &edidsize);

        if (FAILED(retValue))
        {
            if (retValue == ERROR_NO_MORE_ITEMS)
            {
                break;
            }

            LogError("{ERR-098} [Display] WARNING: RegEnumValueA failed to read a key. LastErr=%d", retValue);
            OVR_ASSERT(false);
        }
        else if (0 == strcmp(valueName, "EDID")) // Value is EDID:
        {
            // Tested working for DK1 and DK2:

            char friendlyString[9];
            memcpy(friendlyString, EDIDdata + 77, 8);
            truncateNonalphanum(friendlyString, 8);
            friendlyString[8] = '\0';

            char edidString[14];
            memcpy(edidString, EDIDdata + 95, 13);
            truncateNonalphanum(edidString, 13);
            edidString[13] = '\0';

            serialNumberStr = edidString;
            userFriendlyNameStr = friendlyString;

            found = true;
            break;
        }
    }

    RegCloseKey(hDevRegKey);

    return found;
}

static bool getCompatDisplayEDID(const WCHAR* displayName, String& serialNumberStr, String& userFriendlyNameStr)
{
    HDEVINFO devInfo = NULL;

    devInfo = SetupDiGetClassDevsEx(
        &GUID_CLASS_MONITOR, //class GUID
        NULL, //enumerator
        NULL, //HWND
        DIGCF_PRESENT, // Flags //DIGCF_ALLCLASSES|
        NULL, // device info, create a new one.
        NULL, // machine name, local machine
        NULL);// reserved

    if (NULL == devInfo)
    {
        return false;
    }

    DWORD lastError = 0;

    // For each display,
    for (int i = 0;; i++)
    {
        SP_DEVINFO_DATA devInfoData = {};
        devInfoData.cbSize = sizeof(devInfoData);

        // Grab device info
        if (SetupDiEnumDeviceInfo(devInfo, i, &devInfoData))
        {
            // Access device info from registry
            if (AccessDeviceRegistry(devInfo, &devInfoData, displayName, serialNumberStr, userFriendlyNameStr))
            {
                return true;
            }
        }
        else
        {
            lastError = GetLastError();

            // If no more items found,
            if (lastError != ERROR_NO_MORE_ITEMS)
            {
                LogError("{ERR-099} [Display] WARNING: SetupDiEnumDeviceInfo failed. LastErr=%d", lastError);
                OVR_ASSERT(false);
            }

            break;
        }
    }

    LogError("{ERR-100} [Display] WARNING: SetupDiEnumDeviceInfo did not return the rift display. LastErr=%d", lastError);
    OVR_ASSERT(false);

    return false;
}

#endif // OVR_COMPAT_EDID_VIA_WMI

// This is function that's used 
bool anyRiftsInExtendedMode()
{
	bool result = false;

	MonitorSet monitors;
	monitors.MonitorCount = 0;
	// Get all the monitor handles 
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&monitors);

	DISPLAY_DEVICE dd, ddm;
	UINT           i, j;

	for( i = 0;
		(ZeroMemory(&dd, sizeof(dd)), dd.cb = sizeof(dd),
		EnumDisplayDevices(0, i, &dd, 0)) != 0;  i++ )
	{
		for( j = 0;
			(ZeroMemory(&ddm, sizeof(ddm)), ddm.cb = sizeof(ddm),
			EnumDisplayDevices(dd.DeviceName, j, &ddm, 0)) != 0;  j++ )
		{
			// Our monitor hardware has string "RTD2205" in it
			// Nate's device "CVT0003"
			if( wcsstr(ddm.DeviceID, L"RTD2205") ||
				wcsstr(ddm.DeviceID, L"CVT0003") ||
				wcsstr(ddm.DeviceID, L"MST0030") ||
				wcsstr(ddm.DeviceID, L"OVR00") ) // Part of Oculus EDID.
			{
				result = true;
			}
		}
	}

	return result;
}

static int discoverExtendedRifts(OVR::Win32::DisplayDesc* descriptorArray, int inputArraySize, bool includeEDID)
{
	static bool reportDiscovery = true;

	int result = 0;

	MonitorSet monitors;
	monitors.MonitorCount = 0;
	// Get all the monitor handles 
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&monitors);

	DISPLAY_DEVICE dd, ddm;
	UINT           i, j;    

	for (i = 0; 
		(ZeroMemory(&dd, sizeof(dd)), dd.cb = sizeof(dd),
		EnumDisplayDevices(0, i, &dd, 0)) != 0;  i++)
	{
		for (j = 0; 
			(ZeroMemory(&ddm, sizeof(ddm)), ddm.cb = sizeof(ddm),
			EnumDisplayDevices(dd.DeviceName, j, &ddm, 0)) != 0;  j++)
		{
			if( result >= inputArraySize )
				return result;

			// Our monitor hardware has string "RTD2205" in it
			// Nate's device "CVT0003"
			if (wcsstr(ddm.DeviceID, L"RTD2205") || 
				wcsstr(ddm.DeviceID, L"CVT0003") || 
				wcsstr(ddm.DeviceID, L"MST0030") ||
				wcsstr(ddm.DeviceID, L"OVR00") ) // Part of Oculus EDID.
			{
				String   deviceId(ddm.DeviceID);
				String   displayDeviceName(ddm.DeviceName);				
                Vector2i desktopOffset(0, 0);
                Sizei    monitorResolution(1280, 800);   

                // Make a device type guess
                HmdTypeEnum deviceTypeGuess = HmdType_Unknown;
                if (wcsstr(ddm.DeviceID, L"OVR0003"))
                {
                    // DK2 prototypes and variants
                    deviceTypeGuess = HmdType_DK2;
                    // Could also be:
                    // HmdType_CrystalCoveProto
                }
                else if (wcsstr(ddm.DeviceID, L"OVR0002"))
                {   // HD Prototypes
                    deviceTypeGuess = HmdType_DKHDProto;
                    // Could also be:
                    // HmdType_DKHD2Proto
                    // HmdType_DKHDProto566Mi
                }
                else if (wcsstr(ddm.DeviceID, L"OVR0001"))
                {   // DK1
                    deviceTypeGuess = HmdType_DK1;
                }
                else if (wcsstr(ddm.DeviceID, L"OVR00"))
                {   // Future Oculus HMD devices
                    deviceTypeGuess = HmdType_Unknown;
                }
                else
                {
                    deviceTypeGuess = HmdType_DKProto;
                }

				// Find the matching MONITORINFOEX for this device so we can get the 
				// screen coordinates
				MONITORINFOEX info;
				for (int m=0; m < monitors.MonitorCount; m++)
				{
					info.cbSize = sizeof(MONITORINFOEX);
					GetMonitorInfo(monitors.Monitors[m], &info);
					if (_tcsstr(ddm.DeviceName, info.szDevice) == ddm.DeviceName)
					{   // If the device name starts with the monitor name
						// then we found the matching DISPLAY_DEVICE and MONITORINFO
						// so we can gather the monitor coordinates
                        desktopOffset = Vector2i(info.rcMonitor.left, info.rcMonitor.top);
						break;
					}
				}

				WCHAR* instanceBuffer = (WCHAR*)calloc(wcslen(ddm.DeviceID) + 1, sizeof(WCHAR));
				wcscpy_s(instanceBuffer, wcslen(ddm.DeviceID) + 1, ddm.DeviceID);
				WCHAR* instanceName = instanceBuffer;
				WCHAR* nextToken = NULL;
				if (wcstok_s(instanceName, L"\\", &nextToken))
				{
					WCHAR* aToken = wcstok_s(NULL, L"\\", &nextToken);

					if (aToken)
					{
						String serialNumberStr, userFriendlyNameStr;
                        if (!includeEDID || getCompatDisplayEDID(aToken, serialNumberStr, userFriendlyNameStr))
						{
							// Set descriptor
							OVR::Win32::DisplayDesc& desc = descriptorArray[result++];

                            // If not including EDID,
                            if (!includeEDID)
                            {
                                // If DK2 id,
                                if (wcsstr(ddm.DeviceID, L"OVR0003"))
                                {
                                    userFriendlyNameStr = "Rift DK2";
                                }
                                else // Assume DK1 for now:
                                {
                                    userFriendlyNameStr = "Rift DK1";
                                }
                            }

                            desc.DeviceTypeGuess = deviceTypeGuess;
                            desc.DisplayID = displayDeviceName;
							desc.ModelName = userFriendlyNameStr;
							desc.EdidSerialNumber = serialNumberStr;
							desc.LogicalResolutionInPixels = monitorResolution;
							desc.DesktopDisplayOffset = desktopOffset;

							// Hard-coded defaults in case the device doesn't have the data itself.
							// DK2 prototypes (0003) or DK HD Prototypes (0002)                
							if (wcsstr(ddm.DeviceID, L"OVR0003") || wcsstr(ddm.DeviceID, L"OVR0002"))
							{
								desc.LogicalResolutionInPixels = Sizei(1920, 1080);
								desc.NativeResolutionInPixels = Sizei(1080, 1920);
							}
							else
							{
								desc.LogicalResolutionInPixels = monitorResolution;
								desc.NativeResolutionInPixels = monitorResolution;
							}
						}
					}
				}

				if (reportDiscovery)
				{
					// Only report once per run
					OVR_DEBUG_LOG_TEXT(("Display Found %s - %s\n",
						deviceId.ToCStr(), displayDeviceName.ToCStr()));
					reportDiscovery = false;
				}

				free(instanceBuffer);

				break;
			}
		}
	}

	return result;
}


//-------------------------------------------------------------------------------------
// ***** Display 

bool Display::InCompatibilityMode( bool displaySearch )
{
	bool result = false;
	if( displaySearch )
	{
		OVR::Win32::DisplayDesc displayArray[8];

		int extendedRiftCount = discoverExtendedRifts(displayArray, 8, false);
		if( extendedRiftCount )
		{
			result = true;
		}
		else
		{
			result = GlobalDisplayContext.CompatibilityMode;
		}
	}
	else
	{
		result = GlobalDisplayContext.CompatibilityMode;
	}

	return result;
}

#define OVR_FLAG_COMPATIBILITY_MODE 1
#define OVR_FLAG_HIDE_DK1 2

bool Display::Initialize()
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;

	hDevice = CreateFile( L"\\\\.\\ovr_video" ,
		                  GENERIC_READ | GENERIC_WRITE, NULL,
		                  NULL, OPEN_EXISTING, NULL, NULL);

	if (hDevice != NULL && hDevice != INVALID_HANDLE_VALUE)
	{
		GlobalDisplayContext.hDevice             = hDevice;
		GlobalDisplayContext.CompatibilityMode = false;

		DWORD bytesReturned = 0;
		LONG compatiblityResult = OVR_STATUS_SUCCESS;

		BOOL result = DeviceIoControl( hDevice, IOCTL_RIFTMGR_GETCOMPATIBILITYMODE, NULL, 0,
                                       &compatiblityResult, sizeof( LONG ), &bytesReturned, NULL );
		if (result)
		{
            GlobalDisplayContext.CompatibilityMode = (compatiblityResult & OVR_FLAG_COMPATIBILITY_MODE) != 0;
            GlobalDisplayContext.HideDK1Mode = (compatiblityResult & OVR_FLAG_HIDE_DK1) != 0;
        }
		else
		{
			// If calling our driver fails in any way, assume compatibility mode as well
			GlobalDisplayContext.CompatibilityMode = true;
		}

		if (!GlobalDisplayContext.CompatibilityMode)
		{
			Ptr<DisplaySearchHandle> searchHandle = *Display::GetDisplaySearchHandle();

			// If a display is actually connected, bring up the shim layers so we can actually use it
			if (GetDisplayCount(searchHandle) > 0)
			{
				// FIXME: Initializing DX9 with landscape numbers rather than portrait
				GlobalDisplayContext.ExpectedWidth = 1080;
				GlobalDisplayContext.ExpectedHeight = 1920;
			}
			else
			{
				GlobalDisplayContext.CompatibilityMode = true;
			}

		}
	}
	else
	{
		GlobalDisplayContext.CompatibilityMode = true;
	}

	return true;
}

bool Display::GetDriverMode(bool& driverInstalled, bool& compatMode, bool& hideDK1Mode)
{
    if (GlobalDisplayContext.hDevice == NULL)
    {
        driverInstalled = false;
        compatMode = true;
        hideDK1Mode = false;
    }
    else
    {
        driverInstalled = true;
        compatMode = GlobalDisplayContext.CompatibilityMode;
        hideDK1Mode = GlobalDisplayContext.HideDK1Mode;
    }

    return true;
}

bool Display::SetDriverMode(bool compatMode, bool hideDK1Mode)
{
    // If device is not initialized,
    if (GlobalDisplayContext.hDevice == NULL)
    {
        OVR_ASSERT(false);
        return false;
    }

    // If no change,
    if ((compatMode == GlobalDisplayContext.CompatibilityMode) &&
        (hideDK1Mode == GlobalDisplayContext.HideDK1Mode))
    {
        return true;
    }

    LONG mode_flags = 0;
    if (compatMode)
    {
        mode_flags |= OVR_FLAG_COMPATIBILITY_MODE;
    }
    if (hideDK1Mode)
    {
        mode_flags |= OVR_FLAG_HIDE_DK1;
    }

    DWORD bytesReturned = 0;
    LONG err = 1;

    if (!DeviceIoControl(GlobalDisplayContext.hDevice,
                         IOCTL_RIFTMGR_SETCOMPATIBILITYMODE,
                         &mode_flags,
                         sizeof(LONG),
                         &err,
                         sizeof(LONG),
                         &bytesReturned,
                         NULL) ||
        (err != 0 && err != -3))
    {
        LogError("{ERR-001w} [Win32Display] Unable to set device mode to (compat=%d dk1hide=%d): err=%d",
            (int)compatMode, (int)hideDK1Mode, (int)err);
        return false;
    }

    OVR_DEBUG_LOG(("[Win32Display] Set device mode to (compat=%d dk1hide=%d)",
        (int)compatMode, (int)hideDK1Mode));

    GlobalDisplayContext.HideDK1Mode = hideDK1Mode;
    GlobalDisplayContext.CompatibilityMode = compatMode;
    return true;
}

DisplaySearchHandle* Display::GetDisplaySearchHandle()
{
	return new Win32::Win32DisplaySearchHandle();
}

// FIXME: The handle parameter will be used to unify GetDisplayCount and GetDisplay calls
// The handle will be written to the 64-bit value pointed and will store the enumerated
// display list. This will allow the indexes to be meaningful between obtaining
// the count. With a single handle the count should be stable
int Display::GetDisplayCount(DisplaySearchHandle* handle, bool extended, bool applicationOnly, bool extendedEDIDSerials)
{
	static int extendedCount = -1;
	static int applicationCount = -1;

	Win32::Win32DisplaySearchHandle* localHandle = (Win32::Win32DisplaySearchHandle*)handle;
	
	if( localHandle == NULL )
		return 0;

	if( extendedCount == -1 || extended )
	{
        extendedCount = discoverExtendedRifts(localHandle->cachedDescriptorArray, 16, extendedEDIDSerials);
	}

	localHandle->extended = true;
	localHandle->extendedDisplayCount = extendedCount;
	int totalCount = extendedCount;

	if( applicationCount == -1 || applicationOnly )
	{
		applicationCount = getRiftCount(GlobalDisplayContext.hDevice);
		localHandle->application = true;
	}

	totalCount += applicationCount;
	localHandle->applicationDisplayCount = applicationCount;
	localHandle->displayCount = totalCount;

	return totalCount;
}

Ptr<Display> Display::GetDisplay(int index, DisplaySearchHandle* handle)
{
	Ptr<Display> result;

	if( index < 0 )
		return result;

	Win32::Win32DisplaySearchHandle* localHandle = (Win32::Win32DisplaySearchHandle*)handle;

	if( localHandle == NULL )
		return NULL;

	if (localHandle->extended)
	{
		if (index >= 0 && index < (int)localHandle->extendedDisplayCount)
		{
			return *new Win32::Win32DisplayGeneric(localHandle->cachedDescriptorArray[index]);
		}

		index -= localHandle->extendedDisplayCount;
	}

	if(localHandle->application)
	{
		if (index >= 0 && index < (int)getRiftCount(GlobalDisplayContext.hDevice))
		{
            ULONG riftChildId = getRift(GlobalDisplayContext.hDevice, index);
            Win32::DisplayEDID dEdid;

            if (!getEdid(GlobalDisplayContext.hDevice, riftChildId, dEdid))
            {
                return NULL;
            }

            // FIXME: We have the EDID. Let's just use that instead.
            uint32_t nativeWidth = 1080, nativeHeight = 1920;
            uint32_t logicalWidth = 1920, logicalHeight = 1080;
			uint32_t rotation = 0;

            switch (dEdid.ModelNumber)
            {
			case 0:
			case 1:
				nativeWidth = 1280;
				nativeHeight = 800;
				logicalWidth = nativeWidth;
				logicalHeight = nativeHeight;
				break;
			case 2:
			case 3:
			default:
				rotation = 90;
				break;
			}

            HmdTypeEnum deviceTypeGuess = HmdType_Unknown;
            switch (dEdid.ModelNumber)
            {
            case 3: deviceTypeGuess = HmdType_DK2;       break;
            case 2: deviceTypeGuess = HmdType_DKHDProto; break;
            case 1: deviceTypeGuess = HmdType_DK1;       break;
            default: break;
            }

			result = *new Win32::Win32DisplayDriver( 
						deviceTypeGuess,
                        "",
						dEdid.MonitorName,
						dEdid.SerialNumber,
                        Sizei(logicalWidth, logicalHeight),
						Sizei(nativeWidth, nativeHeight),
						Vector2i(0),
                        dEdid,
						GlobalDisplayContext.hDevice,
						riftChildId,
						rotation);
		}
	}
	return result;
}

Display::MirrorMode Win32::Win32DisplayDriver::SetMirrorMode( Display::MirrorMode newMode )
{
	return newMode;
}

static bool SetDisplayPower(HANDLE hDevice, ULONG childId, int mode)
{
    ULONG_PTR longArray[2];

    longArray[0] = childId;
    longArray[1] = mode;

    ULONG localResult = 0;
    DWORD bytesReturned = 0;

    BOOL result = DeviceIoControl(hDevice,
                                  IOCTL_RIFTMGR_DISPLAYPOWER,
                                  longArray,
                                  2 * sizeof(ULONG_PTR),
                                  &localResult,
                                  sizeof(ULONG),
                                  &bytesReturned,
                                  NULL);

    // Note: bytesReturned does not seem to be set
    return result != FALSE /* && bytesReturned == sizeof(ULONG) */ && mode == (int)localResult;
}

bool Win32::Win32DisplayDriver::SetDisplaySleep(bool sleep)
{
    return SetDisplayPower(hDevice, ChildId, sleep ? 2 : 1);
}


} // namespace OVR
