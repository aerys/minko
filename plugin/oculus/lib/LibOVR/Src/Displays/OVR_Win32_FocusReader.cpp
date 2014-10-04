/************************************************************************************

Filename    :   OVR_Win32_FocusReader.cpp
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

#include "OVR_Win32_FocusReader.h"
#include "../Kernel/OVR_Log.h"
#include "../Service/Service_NetClient.h"

OVR_DEFINE_SINGLETON(OVR::Win32::RenderFocusReader);

namespace OVR { namespace Win32 {


HWND RenderFocusReader::ReadActiveWindow()
{
    FocusState = Reader.Get();
    if (!FocusState || NoSharedMemory)
    {
        if (!Reader.Open(OVR_FOCUS_OBSERVER_SHARE_NAME))
        {
            OVR_DEBUG_LOG(("[Win32ShimFunctions] Unable to open the shared memory space"));
            // Note: This should only warn and not assert because it is normal behavior when the server is not running.
            NoSharedMemory = true;
            return 0;
        }

        FocusState = Reader.Get();
        if (!FocusState)
        {
            OVR_DEBUG_LOG(("[Win32ShimFunctions] Unable to get the shared memory space"));
            NoSharedMemory = true;
            return 0;
        }
    }

    return (HWND)Ptr64ToPtr(FocusState->ActiveWindowHandle);
}

RenderFocusReader::RenderFocusReader()
{
	NoSharedMemory = false;

    PushDestroyCallbacks();
}

RenderFocusReader::~RenderFocusReader()
{
}

void RenderFocusReader::OnSystemDestroy()
{
    delete this;
}


}} // namespace OVR::Win32
