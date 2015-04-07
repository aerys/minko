/************************************************************************************

Filename    :   OVR_MessageIDTypes.h
Content     :   Enumeration list indicating what type of message is being sent
Created     :   July 3, 2014
Authors     :   Kevin Jenkins

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

namespace OVR { namespace Net {

/// First byte of a network message
typedef unsigned char MessageID;

enum DefaultMessageIDTypes
{
    OVRID_RPC1,
    OVRID_END = 128,
    OVRID_LATENCY_TESTER_1,
};

}} // namespace OVR::Net
