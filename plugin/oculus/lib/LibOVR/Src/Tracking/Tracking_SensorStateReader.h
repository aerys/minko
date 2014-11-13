/************************************************************************************

Filename    :   Tracking_SensorStateReader.h
Content     :   Separate reader component that is able to recover sensor pose
Created     :   June 4, 2014
Authors     :   Chris Taylor

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

#ifndef Tracking_SensorStateReader_h
#define Tracking_SensorStateReader_h

#include "../Kernel/OVR_Lockless.h"
#include "Tracking_SensorState.h"

#include "../OVR_Profile.h"

namespace OVR { namespace Tracking {


//-----------------------------------------------------------------------------
// SensorStateReader

// User interface to retrieve pose from the sensor fusion subsystem
class SensorStateReader : public NewOverrideBase
{
protected:
	const CombinedSharedStateUpdater *Updater;


    // Last latency warning time
    mutable double LastLatWarnTime;

    // Transform from real-world coordinates to centered coordinates
    Posed CenteredFromWorld; 

public:
	SensorStateReader();

	// Initialize the updater
    void         SetUpdater(const CombinedSharedStateUpdater *updater);

	// Re-centers on the current yaw (optionally pitch) and translation
	void		 RecenterPose();

	// Get the full dynamical system state of the CPF, which includes velocities and accelerations,
	// predicted at a specified absolute point in time.
	bool		 GetSensorStateAtTime(double absoluteTime, Tracking::TrackingState& state) const;

	// Get the predicted pose (orientation, position) of the center pupil frame (CPF) at a specific point in time.
	bool		 GetPoseAtTime(double absoluteTime, Posef& transform) const;

	// Get the sensor status (same as GetSensorStateAtTime(...).Status)
	uint32_t     GetStatus() const;

    void LoadProfileCenteredFromWorld(Profile* profile);
    void SaveProfileCenteredFromWorld(Profile* profile);
};


}} // namespace OVR::Tracking

#endif // Tracking_SensorStateReader_h
