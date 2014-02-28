/************************************************************************************

Filename    :   OVR_String_FormatUtil.cpp
Content     :   String format functions.
Created     :   February 27, 2013
Notes       : 

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

************************************************************************************/

#include "OVR_String.h"
#include "OVR_Log.h"

namespace OVR {

void StringBuffer::AppendFormat(const char* format, ...)
{       
    va_list argList;

    va_start(argList, format);
    UPInt size = OVR_vscprintf(format, argList);
    va_end(argList);

    char* buffer = (char*) OVR_ALLOC(sizeof(char) * (size+1));

    va_start(argList, format);
    UPInt result = OVR_vsprintf(buffer, size+1, format, argList);
    OVR_UNUSED1(result);
    va_end(argList);
    OVR_ASSERT_LOG(result == size, ("Error in OVR_vsprintf"));

    AppendString(buffer);

    OVR_FREE(buffer);
}

} // OVR
