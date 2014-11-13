/************************************************************************************

Filename    :   OVR_Win32_FocusReader.h
Content     :   Reader for current app with focus on Windows
Created     :   July 2, 2014
Authors     :   Chris Taylor

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

#ifndef OVR_Win32_FocusReader_h
#define OVR_Win32_FocusReader_h

#include "../Kernel/OVR_System.h"
#include "../Kernel/OVR_Lockless.h"
#include "../Kernel/OVR_Array.h"
#include "../Kernel/OVR_SharedMemory.h"

namespace OVR { namespace Win32 {


#define OVR_FOCUS_OBSERVER_SHARE_NAME "OVRAppFocus"

//-----------------------------------------------------------------------------
// LocklessFocusState

#pragma pack(push, 8)

// Focus state data
struct LocklessFocusState
{
    LocklessFocusState(DWORD pid = 0) :
        ActiveProcessId(pid),
        ActiveWindowHandle(NULL)
    {
    }

    DWORD ActiveProcessId;
    void * POINTER_64 ActiveWindowHandle;
};

#pragma pack(pop)

typedef SharedObjectWriter< LocklessFocusState > SharedFocusWriter;
typedef SharedObjectReader< LocklessFocusState > SharedFocusReader;


//-----------------------------------------------------------------------------
// RenderFocusReader

class RenderFocusReader : public OVR::SystemSingletonBase<RenderFocusReader>, public NewOverrideBase
{
    OVR_DECLARE_SINGLETON(RenderFocusReader);

    SharedFocusReader         Reader;         // Shared memory reader
    const LocklessFocusState* FocusState;     // Pointer to the current focus state
    bool                      NoSharedMemory; // Flag reporting that no shared memory has been detected;

public:
    HWND ReadActiveWindow();
};


}} // namespace OVR::Win32

#endif // OVR_Win32_FocusReader_h
