/************************************************************************************

PublicHeader:   OVR.h
Filename    :   OVR_DeviceConstants.h
Content     :   Device constants
Created     :   February 5, 2013
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

#ifndef OVR_DeviceConstants_h
#define OVR_DeviceConstants_h

namespace OVR {


//-------------------------------------------------------------------------------------
// Different device types supported by OVR; this type is reported by DeviceBase::GetType.
// 
enum DeviceType
{
    Device_None             = 0,
    Device_Manager          = 1,
    Device_HMD              = 2,
    Device_Sensor           = 3,
    Device_LatencyTester    = 4,
    Device_BootLoader       = 5,
    Device_All              = 0xFF // Set for enumeration only, to enumerate all device types.
};

} // namespace OVR

#endif
