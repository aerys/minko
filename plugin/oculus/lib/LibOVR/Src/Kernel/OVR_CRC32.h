/************************************************************************************

PublicHeader:   OVR
Filename    :   OVR_CRC32.h
Content     :   CRC-32 with polynomial used for sensor devices
Created     :   June 20, 2014
Author      :   Chris Taylor

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

#ifndef OVR_CRC32_h
#define OVR_CRC32_h

#include "OVR_Types.h"

namespace OVR {


//-----------------------------------------------------------------------------------
// ***** CRC-32

// Polynomial used and algorithm details are proprietary to our sensor board
uint32_t CRC32_Calculate(const void* data, int bytes, uint32_t prevCRC = 0);


} // namespace OVR

#endif
