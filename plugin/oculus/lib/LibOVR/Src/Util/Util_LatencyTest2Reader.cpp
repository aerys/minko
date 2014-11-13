/************************************************************************************

Filename    :   Util_LatencyTest2Reader.cpp
Content     :   Shared functionality for the DK2 latency tester
Created     :   July 8, 2014
Authors     :   Volga Aksoy, Chris Taylor

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

*************************************************************************************/

#include "Util_LatencyTest2Reader.h"

namespace OVR { namespace Util {


//// FrameTimeRecord

bool FrameTimeRecord::ColorToReadbackIndex(int *readbackIndex, unsigned char color)
{
    int compareColor = color - LT2_ColorIncrement/2;
    int index        = color / LT2_ColorIncrement;  // Use color without subtraction due to rounding.
    int delta        = compareColor - index * LT2_ColorIncrement;

    if ((delta < LT2_PixelTestThreshold) && (delta > -LT2_PixelTestThreshold))
    {
        *readbackIndex = index;
        return true;
    }
    return false;
}

unsigned char FrameTimeRecord::ReadbackIndexToColor(int readbackIndex)
{
    OVR_ASSERT(readbackIndex < LT2_IncrementCount);
    return (unsigned char)(readbackIndex * LT2_ColorIncrement + LT2_ColorIncrement/2);
}


//// FrameTimeRecordSet

FrameTimeRecordSet::FrameTimeRecordSet()
{
    NextWriteIndex = 0;
    memset(this, 0, sizeof(FrameTimeRecordSet));
}

void FrameTimeRecordSet::AddValue(int readValue, double timeSeconds)
{
    Records[NextWriteIndex].ReadbackIndex = readValue;
    Records[NextWriteIndex].TimeSeconds = timeSeconds;
    NextWriteIndex++;
    if (NextWriteIndex == RecordCount)
        NextWriteIndex = 0;
}
// Matching should be done starting from NextWrite index 
// until wrap-around

const FrameTimeRecord& FrameTimeRecordSet::operator [] (int i) const
{
    return Records[(NextWriteIndex + i) & RecordMask];
}

const FrameTimeRecord& FrameTimeRecordSet::GetMostRecentFrame()
{
    return Records[(NextWriteIndex - 1) & RecordMask];
}

// Advances I to  absolute color index
bool FrameTimeRecordSet::FindReadbackIndex(int* i, int readbackIndex) const
{
    for (; *i < RecordCount; (*i)++)
    {
        if ((*this)[*i].ReadbackIndex == readbackIndex)
            return true;
    }
    return false;
}

bool FrameTimeRecordSet::IsAllZeroes() const
{
    for (int i = 0; i < RecordCount; i++)
        if (Records[i].ReadbackIndex != 0)
            return false;
    return true;
}


//// RecordStateReader

void RecordStateReader::GetRecordSet(FrameTimeRecordSet& recordset)
{
    if(!Updater)
    {
        return;
    }
        
    recordset = Updater->SharedLatencyTestState.GetState();
    return;
}


}} // namespace OVR::Util
