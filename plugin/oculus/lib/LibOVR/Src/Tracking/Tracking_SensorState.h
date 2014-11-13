/************************************************************************************

Filename    :   Tracking_SensorState.h
Content     :   Sensor state information shared by tracking system with games
Created     :   May 13, 2014
Authors     :   Dov Katz, Chris Taylor

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

#ifndef Tracking_SensorState_h
#define Tracking_SensorState_h

#include "Tracking_PoseState.h"
#include "../Kernel/OVR_SharedMemory.h"
#include "../Kernel/OVR_Lockless.h"
#include "../Kernel/OVR_String.h"
#include "../Util/Util_LatencyTest2State.h"
#include "../Sensors/OVR_DeviceConstants.h"

// CAPI forward declarations.
struct ovrTrackingState_;
typedef struct ovrTrackingState_ ovrTrackingState;
struct ovrPoseStatef_;
typedef struct ovrPoseStatef_ ovrPoseStatef;

namespace OVR { namespace Tracking {


//-------------------------------------------------------------------------------------
// ***** Sensor State
// These values are reported as compatible with C API.

// Bit flags describing the current status of sensor tracking.
enum StatusBits
{
    // Tracked bits: Toggled by SensorFusion
	Status_OrientationTracked = 0x0001, // Orientation is currently tracked (connected and in use)
	Status_PositionTracked    = 0x0002, // Position is currently tracked (false if out of range)
    Status_CameraPoseTracked  = 0x0004, // Camera pose is currently tracked

    // Connected bits: Toggled by TrackingManager
    Status_PositionConnected  = 0x0020, // Position tracking HW is connected
	Status_HMDConnected       = 0x0080, // HMD is available & connected

    // Masks
    Status_AllMask = 0xffff,
    Status_TrackingMask = Status_PositionTracked | Status_OrientationTracked | Status_CameraPoseTracked,
    Status_ConnectedMask = Status_PositionConnected | Status_HMDConnected,
};


// Full state of of the sensor reported by GetSensorState() at a given absolute time.
class TrackingState
{
public:
	TrackingState() : StatusFlags(0) { }

	// C-interop support
	TrackingState(const ovrTrackingState& s);
	operator ovrTrackingState () const;

	// HMD pose information for the requested time.
	PoseStatef   HeadPose;

    // Orientation and position of the external camera, if present.
    Posef        CameraPose;
    // Orientation and position of the camera after alignment with gravity 
    Posef        LeveledCameraPose;

    // Most recent sensor data received from the HMD
    SensorDataType RawSensorData;

    // Sensor status described by ovrStatusBits.
	uint32_t     StatusFlags;

    //// 0.4.1

    // Time spent processing the last vision frame
    double LastVisionProcessingTime;
};


// -----------------------------------------------

#pragma pack(push, 8)

struct LocklessSensorStatePadding;

// State version stored in lockless updater "queue" and used for 
// prediction by GetPoseAtTime/GetSensorStateAtTime
struct LocklessSensorState
{
	PoseState<double> WorldFromImu;
    SensorDataType    RawSensorData;
    Pose<double>      WorldFromCamera;
	uint32_t          StatusFlags;
    uint32_t          _PAD_0_;

	// ImuFromCpf for HMD pose tracking
	Posed             ImuFromCpf;
    double            LastVisionProcessingTime;

	// Initialized to invalid state
	LocklessSensorState() :
		StatusFlags(0)
	{
	}

    LocklessSensorState& operator = (const LocklessSensorStatePadding& rhs);
};
    
// Padded out version stored in the updater slots
// Designed to be a larger fixed size to allow the data to grow in the future
// without breaking older compiled code.
struct LocklessSensorStatePadding
{
    uint64_t words[64];

    static const int DataWords = (sizeof(LocklessSensorState) + sizeof(uint64_t) - 1) / sizeof(uint64_t);

    // Just copy the low data words
    inline LocklessSensorStatePadding& operator=(const LocklessSensorState& rhs)
    {
        const uint64_t* src = (const uint64_t*)&rhs;

        // if this fires off, then increase words' array size
        OVR_ASSERT(sizeof(words) > sizeof(LocklessSensorState));

        for (int i = 0; i < DataWords; ++i)
        {
            words[i] = src[i];
        }

        return *this;
    }
};

// Just copy the low data words
inline LocklessSensorState& LocklessSensorState::operator = (const LocklessSensorStatePadding& rhs)
{
    uint64_t* dest = (uint64_t*)this;

    for (int i = 0; i < LocklessSensorStatePadding::DataWords; ++i)
    {
        dest[i] = rhs.words[i];
    }

    return *this;
}

#pragma pack(pop)

// A lockless updater for sensor state
typedef LocklessUpdater<LocklessSensorState, LocklessSensorStatePadding> SensorStateUpdater;


//// Combined state

struct CombinedSharedStateUpdater
{
    SensorStateUpdater         SharedSensorState;
    Util::LockessRecordUpdater SharedLatencyTestState;
};

typedef SharedObjectWriter< CombinedSharedStateUpdater > CombinedSharedStateWriter;
typedef SharedObjectReader< CombinedSharedStateUpdater > CombinedSharedStateReader;


}} // namespace OVR::Tracking

#endif
