/************************************************************************************

Filename    :   Util_LatencyTest2Reader.h
Content     :   Shared functionality for the DK2 latency tester
Created     :   July 8, 2014
Authors     :   Volga Aksoy, Chris Taylor

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

*************************************************************************************/

#ifndef OVR_Util_LatencyTest2_State_h
#define OVR_Util_LatencyTest2_State_h

#include "../Kernel/OVR_Lockless.h"

namespace OVR { namespace Util {


enum LatencyTester2Constants
{
    LT2_ColorIncrement                  = 32,
    LT2_PixelTestThreshold              = LT2_ColorIncrement / 3,
    LT2_IncrementCount                  = 256 / LT2_ColorIncrement,
    LT2_TimeoutWaitingForColorDetected  = 1000  // 1 second
};


//-------------------------------------------------------------------------------------
// FrameTimeRecord

// Describes frame scan-out time used for latency testing.
struct FrameTimeRecord
{
    int    ReadbackIndex;
    double TimeSeconds;

    // Utility functions to convert color to readBack indices and back.
    // The purpose of ReadbackIndex is to allow direct comparison by value.

    static bool ColorToReadbackIndex(int *readbackIndex, unsigned char color);
    static unsigned char ReadbackIndexToColor(int readbackIndex);
};


//-----------------------------------------------------------------------------
// FrameTimeRecordSet

// FrameTimeRecordSet is a container holding multiple consecutive frame timing records
// returned from the lock-less state. Used by FrameTimeManager. 
struct FrameTimeRecordSet
{
    enum {
        RecordCount = 4,
        RecordMask  = RecordCount - 1
    };
    FrameTimeRecord Records[RecordCount];    
    int             NextWriteIndex;

    FrameTimeRecordSet();

    void AddValue(int readValue, double timeSeconds);
    // Matching should be done starting from NextWrite index 
    // until wrap-around

    const FrameTimeRecord& operator [] (int i) const;

    const FrameTimeRecord& GetMostRecentFrame();

    // Advances I to  absolute color index
    bool FindReadbackIndex(int* i, int readbackIndex) const;

    bool IsAllZeroes() const;
};

typedef LocklessUpdater<FrameTimeRecordSet, FrameTimeRecordSet> LockessRecordUpdater;


}} // namespace OVR::Util

#endif // OVR_Util_LatencyTest2_State_h
