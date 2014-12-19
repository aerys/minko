/************************************************************************************

PublicHeader:   n/a
Filename    :   OVR_NetworkTypes.h
Content     :   Shared header for network types
Created     :   June 12, 2014
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

#ifndef OVR_NetworkTypes_h
#define OVR_NetworkTypes_h

#include "../Kernel/OVR_Types.h"

namespace OVR {	namespace Net {


typedef uint64_t NetworkID;
const NetworkID InvalidNetworkID = ~((NetworkID)0);


} } // OVR::Net

#endif
