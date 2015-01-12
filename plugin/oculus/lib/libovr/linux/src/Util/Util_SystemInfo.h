/************************************************************************************

Filename    :   Util_SystemInfo.h
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

#include "../Kernel/OVR_String.h"
#include "../Kernel/OVR_Types.h"
#include "../Kernel/OVR_Array.h"

namespace OVR { namespace Util {

const char * OSAsString();
String OSVersionAsString();
uint64_t GetGuidInt();
String GetGuidString();
const char * GetProcessInfo();
String GetFileVersionString(String filePath);
String GetSystemFileVersionString(String filePath);
String GetDisplayDriverVersion();
String GetCameraDriverVersion();
void GetGraphicsCardList(OVR::Array< OVR::String > &gpus);

} } // namespace OVR { namespace Util {