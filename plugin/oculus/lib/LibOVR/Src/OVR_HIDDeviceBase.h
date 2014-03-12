/************************************************************************************

PublicHeader:   OVR.h
Filename    :   OVR_HIDDeviceBase.h
Content     :   Definition of HID device interface.
Created     :   March 11, 2013
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

#ifndef OVR_HIDDeviceBase_h
#define OVR_HIDDeviceBase_h

#include "Kernel/OVR_Types.h"

namespace OVR {

//-------------------------------------------------------------------------------------
// ***** HIDDeviceBase

// Base interface for HID devices.
class HIDDeviceBase
{
public:

    virtual ~HIDDeviceBase() { }

    virtual bool SetFeatureReport(UByte* data, UInt32 length) = 0;
    virtual bool GetFeatureReport(UByte* data, UInt32 length) = 0;
};

} // namespace OVR

#endif
