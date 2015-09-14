/************************************************************************************

Filename    :   OVR_String_FormatUtil.cpp
Content     :   String format functions.
Created     :   February 27, 2013
Notes       : 

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

#include "OVR_String.h"
#include "OVR_Log.h"

namespace OVR {

void StringBuffer::AppendFormat(const char* format, ...)
{       
    va_list argList;
    char    buffer[512];
    char*   bufferUsed = buffer;
    char*   bufferAllocated = NULL;

    va_start(argList, format);

    #if !defined(OVR_CC_MSVC) // Non-Microsoft compilers require you to save a copy of the va_list.
        va_list argListSaved;
        va_copy(argListSaved, argList);
    #endif

    int requiredStrlen = OVR_vsnprintf(bufferUsed, OVR_ARRAY_COUNT(buffer), format, argList); // The large majority of the time this will succeed.

    if(requiredStrlen >= (int)sizeof(buffer)) // If the initial capacity wasn't enough...
    {
        bufferAllocated = (char*)OVR_ALLOC(sizeof(char) * (requiredStrlen + 1));
        bufferUsed = bufferAllocated;
        if(bufferAllocated)
        {
            #if !defined(OVR_CC_MSVC)
                va_end(argList);
                va_copy(argList, argListSaved);
            #endif
            requiredStrlen = OVR_vsnprintf(bufferAllocated, (requiredStrlen + 1), format, argList);
        }
    }

    if(requiredStrlen < 0) // If there was a printf format error...
    {
        bufferUsed = NULL;
    }

    va_end(argList);

    if(bufferUsed)
        AppendString(bufferUsed);

    if(bufferAllocated)
        OVR_FREE(bufferAllocated);
}

} // OVR
