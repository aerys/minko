/************************************************************************************

Filename    :   Util_SystemInfo.cpp
Content     :   Various operations to get information about the system
Created     :   September 26, 2014
Author      :   Kevin Jenkins

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

#include "Util_SystemInfo.h"
#include "../Kernel/OVR_Timer.h"
#include "../Kernel/OVR_Threads.h"
#include "../Kernel/OVR_Log.h"
#include "../Kernel/OVR_Array.h"

/*
// Disabled, can't link RiftConfigUtil
#ifdef OVR_OS_WIN32
#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>

# pragma comment(lib, "wbemuuid.lib")
#endif
*/


namespace OVR { namespace Util {

// From http://blogs.msdn.com/b/oldnewthing/archive/2005/02/01/364563.aspx
#if defined (OVR_OS_WIN64) || defined (OVR_OS_WIN32)

#pragma comment(lib, "version.lib")

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
BOOL Is64BitWindows()
{
#if defined(_WIN64)
    return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
    // 32-bit programs run on both 32-bit and 64-bit Windows
    // so must sniff
    BOOL f64 = FALSE;
    LPFN_ISWOW64PROCESS fnIsWow64Process;

    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(L"kernel32"), "IsWow64Process");
    if (NULL != fnIsWow64Process)
    {
        return fnIsWow64Process(GetCurrentProcess(), &f64) && f64;
    }
    return FALSE;
#else
    return FALSE; // Win64 does not support Win16
#endif
}
#endif

const char * OSAsString()
{
#if defined (OVR_OS_IPHONE)
    return "IPhone";
#elif defined (OVR_OS_DARWIN)
    return "Darwin";
#elif defined (OVR_OS_MAC)
    return "Mac";
#elif defined (OVR_OS_BSD)
    return "BSD";
#elif defined (OVR_OS_WIN64) || defined (OVR_OS_WIN32)
    if (Is64BitWindows())
        return "Win64";
    else
        return "Win32";
#elif defined (OVR_OS_ANDROID)
    return "Android";
#elif defined (OVR_OS_LINUX)
    return "Linux";
#elif defined (OVR_OS_BSD)
    return "BSD";
#else
    return "Other";
#endif
}

uint64_t GetGuidInt()
{
    uint64_t g = Timer::GetTicksNanos();

    uint64_t lastTime, thisTime;
    int j;
    // Sleep a small random time, then use the last 4 bits as a source of randomness
    for (j = 0; j < 8; j++)
    {
        lastTime = Timer::GetTicksNanos();
        Thread::MSleep(1);
        Thread::MSleep(0);
        thisTime = Timer::GetTicksNanos();
        uint64_t diff = thisTime - lastTime;
        unsigned int diff4Bits = (unsigned int)(diff & 15);
        diff4Bits <<= 32 - 4;
        diff4Bits >>= j * 4;
        ((char*)&g)[j] ^= diff4Bits;
    }

    return g;
}
String GetGuidString()
{
    uint64_t guid = GetGuidInt();

    char buff[64];
#if defined(OVR_CC_MSVC)
    OVR_sprintf(buff, sizeof(buff), "%I64u", guid);
#else
    OVR_sprintf(buff, sizeof(buff), "%llu", (unsigned long long) guid);
#endif
    return String(buff);
}

const char * GetProcessInfo()
{
	#if defined (OVR_CPU_X86_64	)
    return "64 bit";
#elif defined (OVR_CPU_X86)
    return "32 bit";
#else
    return "TODO";
#endif
}
#ifdef OVR_OS_WIN32


String OSVersionAsString()
{
    return GetSystemFileVersionString("\\kernel32.dll");
}
String GetSystemFileVersionString(String filePath)
{
    char strFilePath[MAX_PATH]; // Local variable
    UINT sysDirLen = GetSystemDirectoryA(strFilePath, ARRAYSIZE(strFilePath));
    if (sysDirLen != 0)
    {
        OVR_strcat(strFilePath, MAX_PATH, filePath.ToCStr());
        return GetFileVersionString(strFilePath);
    }
    else
    {
        return "GetSystemDirectoryA failed";
    }
}
// See http://stackoverflow.com/questions/940707/how-do-i-programatically-get-the-version-of-a-dll-or-exe-file
String GetFileVersionString(String filePath)
{
    String result;

    DWORD dwSize = GetFileVersionInfoSizeA(filePath.ToCStr(), NULL);
    if (dwSize == 0)
    {
        OVR_DEBUG_LOG(("Error in GetFileVersionInfoSizeA: %d (for %s)", GetLastError(), filePath.ToCStr()));
        result = filePath + " not found";
    }
    else
    {
        BYTE* pVersionInfo = new BYTE[dwSize];
        if (!pVersionInfo)
        {
            OVR_DEBUG_LOG(("Out of memory allocating %d bytes (for %s)", dwSize, filePath.ToCStr()));
            result = "Out of memory";
        }
        else
        {
            if (!GetFileVersionInfoA(filePath.ToCStr(), 0, dwSize, pVersionInfo))
            {
                OVR_DEBUG_LOG(("Error in GetFileVersionInfo: %d (for %s)", GetLastError(), filePath.ToCStr()));
                result = "Cannot get version info";
            }
            else
            {
                VS_FIXEDFILEINFO* pFileInfo = NULL;
                UINT              pLenFileInfo = 0;
                if (!VerQueryValue(pVersionInfo, TEXT("\\"), (LPVOID*)&pFileInfo, &pLenFileInfo))
                {
                    OVR_DEBUG_LOG(("Error in VerQueryValue: %d (for %s)", GetLastError(), filePath.ToCStr()));
                    result = "File has no version info";
                }
                else
                {
                    int major = (pFileInfo->dwFileVersionMS >> 16) & 0xffff;
                    int minor = (pFileInfo->dwFileVersionMS) & 0xffff;
                    int hotfix = (pFileInfo->dwFileVersionLS >> 16) & 0xffff;
                    int other = (pFileInfo->dwFileVersionLS) & 0xffff;

                    char str[128];
                    OVR::OVR_sprintf(str, 128, "%d.%d.%d.%d", major, minor, hotfix, other);

                    result = str;
                }
            }

            delete[] pVersionInfo;
        }
    }

    return result;
}


String GetDisplayDriverVersion()
{
    return GetSystemFileVersionString("\\OVRDisplay32.dll");
}
String GetCameraDriverVersion()
{
    return GetSystemFileVersionString("\\drivers\\OCUSBVID.sys");
}

// From http://stackoverflow.com/questions/9524309/enumdisplaydevices-function-not-working-for-me
void GetGraphicsCardList( Array< String > &gpus)
{
	gpus.Clear();

	DISPLAY_DEVICEA dd;

	dd.cb = sizeof(dd);

	DWORD deviceNum = 0;
	while( EnumDisplayDevicesA(NULL, deviceNum, &dd, 0) ){
        if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
		    gpus.PushBack(dd.DeviceString);
		deviceNum++;
	}
}
#else

// used for driver files

String GetFileVersionString(String /*filePath*/)
{
	return String();
}

String GetSystemFileVersionString(String /*filePath*/)
{
	return String();
}

String GetDisplayDriverVersion()
{
	return String();
}

String GetCameraDriverVersion()
{
	return String();
}

#ifdef OVR_OS_MAC
    //use objective c source
#else
    
//todo linux, this requires searching /var/ files
void GetGraphicsCardList(OVR::Array< OVR::String > &gpus)
{
	gpus.Clear();
}
String OSVersionAsString()
{
    return String();
}
#endif //OVR_OS_MAC
#endif // WIN32

} } // namespace OVR { namespace Util {
