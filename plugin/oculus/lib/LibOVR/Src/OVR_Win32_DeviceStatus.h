/************************************************************************************

Filename    :   OVR_Win32_DeviceStatus.h
Content     :   Win32-specific DeviceStatus header.
Created     :   January 24, 2013
Authors     :   Lee Cooper

Copyright   :   Copyright 2013 Oculus VR, Inc. All Rights reserved.

Licensed under the Oculus VR SDK License Version 2.0 (the "License"); 
you may not use the Oculus VR SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*************************************************************************************/

#ifndef OVR_Win32_DeviceStatus_h
#define OVR_Win32_DeviceStatus_h

#include <windows.h>
#include "Kernel/OVR_String.h"
#include "Kernel/OVR_RefCount.h"
#include "Kernel/OVR_Array.h"

namespace OVR { namespace Win32 {

//-------------------------------------------------------------------------------------
// ***** DeviceStatus
//
// DeviceStatus abstracts the handling of windows messages of interest for
// example the WM_DEVICECHANGED message which occurs when a device is plugged/unplugged.
// The device manager thread creates an instance of this class and passes its pointer
// in the constructor. That thread is also responsible for periodically calling 'ProcessMessages'
// to process queued windows messages. The client is notified via the 'OnMessage' method
// declared in the 'DeviceMessages::Notifier' interface.
class DeviceStatus : public RefCountBase<DeviceStatus>
{
public:

	// Notifier used for device messages.
	class Notifier  
	{
	public:
		enum MessageType
		{
			DeviceAdded     = 0,
			DeviceRemoved   = 1,
		};

		virtual bool OnMessage(MessageType type, const String& devicePath) 
        { OVR_UNUSED2(type, devicePath); return true; }
	};

	DeviceStatus(Notifier* const pClient);
	~DeviceStatus();

	void operator = (const DeviceStatus&);	// No assignment implementation.

	bool Initialize();
	void ShutDown();

	void ProcessMessages();

private:	
    enum 
    { 
        MaxUSBRecoveryAttempts  = 20,
        USBRecoveryTimeInterval = 500   // ms
    };
    struct RecoveryTimerDesc
    {
        UINT_PTR    TimerId;
        String      DevicePath;
        unsigned    NumAttempts;
    };

	static LRESULT CALLBACK WindowsMessageCallback( HWND hwnd, 
                                                    UINT message, 
                                                    WPARAM wParam, 
                                                    LPARAM lParam);

	bool MessageCallback(WORD messageType, const String& devicePath);

    void CleanupRecoveryTimer(UPInt index);
    RecoveryTimerDesc* FindRecoveryTimer(UINT_PTR timerId, UPInt* pindex);
    void FindAndCleanupRecoveryTimer(const String& devicePath);

private: // data
    Notifier* const     pNotificationClient;	// Don't reference count a back-pointer.

    HWND                hMessageWindow;
    HDEVNOTIFY          hDeviceNotify;

    UINT_PTR            LastTimerId;
    Array<RecoveryTimerDesc> RecoveryTimers;

    GUID                HidGuid;
};

}} // namespace OVR::Win32

#endif // OVR_Win32_DeviceStatus_h
