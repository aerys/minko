/************************************************************************************

PublicHeader:   OVR
Filename    :   OVR_System.h
Content     :   General kernel initialization/cleanup, including that
                of the memory allocator.
Created     :   September 19, 2012
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

#ifndef OVR_System_h
#define OVR_System_h

#include "OVR_Allocator.h"
#include "OVR_Log.h"

namespace OVR {

// ***** System Core Initialization class

// System initialization must take place before any other OVR_Kernel objects are used;
// this is done my calling System::Init(). Among other things, this is necessary to
// initialize the memory allocator. Similarly, System::Destroy must be
// called before program exist for proper cleanup. Both of these tasks can be achieved by
// simply creating System object first, allowing its constructor/destructor do the work.

// TBD: Require additional System class for Oculus Rift API?

class System
{
public:

    // System constructor expects allocator to be specified, if it is being substituted.
    System(Log* log = Log::ConfigureDefaultLog(LogMask_Debug),
           Allocator* palloc = DefaultAllocator::InitSystemSingleton())
    {
        Init(log, palloc);
    }

    ~System()
    {
        Destroy();
    }

    // Returns 'true' if system was properly initialized.
    static bool OVR_CDECL IsInitialized();

    // Initializes System core.  Users can override memory implementation by passing
    // a different Allocator here.
    static void OVR_CDECL Init(Log* log = Log::ConfigureDefaultLog(LogMask_Debug),
                               Allocator *palloc = DefaultAllocator::InitSystemSingleton());

    // De-initializes System more, finalizing the threading system and destroying
    // the global memory allocator.
    static void OVR_CDECL Destroy();    
};

} // OVR

#endif
