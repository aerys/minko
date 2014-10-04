/************************************************************************************

Filename    :   Tracking_SensorStateReader.cpp
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

#include "Tracking_SensorStateReader.h"
#include "Tracking_PoseState.h"

namespace OVR { namespace Tracking {


//-------------------------------------------------------------------------------------

// This is a "perceptually tuned predictive filter", which means that it is optimized
// for improvements in the VR experience, rather than pure error.  In particular,
// jitter is more perceptible at lower speeds whereas latency is more perceptible
// after a high-speed motion.  Therefore, the prediction interval is dynamically
// adjusted based on speed.  Significant more research is needed to further improve
// this family of filters.
static Pose<double> calcPredictedPose(const PoseState<double>& poseState, double predictionDt)
{
	Pose<double> pose = poseState.ThePose;
	const double linearCoef = 1.0;
	Vector3d angularVelocity = poseState.AngularVelocity;
	double angularSpeed = angularVelocity.Length();

	// This could be tuned so that linear and angular are combined with different coefficients
	double speed = angularSpeed + linearCoef * poseState.LinearVelocity.Length();

	const double slope = 0.2; // The rate at which the dynamic prediction interval varies
	double candidateDt = slope * speed; // TODO: Replace with smoothstep function

	double dynamicDt = predictionDt;

	// Choose the candidate if it is shorter, to improve stability
	if (candidateDt < predictionDt)
	{
		dynamicDt = candidateDt;
	}

	if (angularSpeed > 0.001)
	{
		pose.Rotation = pose.Rotation * Quatd(angularVelocity, angularSpeed * dynamicDt);
	}

	pose.Translation += poseState.LinearVelocity * dynamicDt;

	return pose;
}


//// SensorStateReader

SensorStateReader::SensorStateReader() :
	Updater(NULL),
    LastLatWarnTime(0.)
{
}

void SensorStateReader::SetUpdater(const CombinedSharedStateUpdater* updater)
{
	Updater = updater;
}

void SensorStateReader::RecenterPose()
{
	if (!Updater)
	{
		return;
	}

	/*
		This resets position to center in x, y, z, and resets yaw to center.
		Other rotation components are not affected.
	*/

	const LocklessSensorState lstate = Updater->SharedSensorState.GetState();

	Posed worldFromCpf = lstate.WorldFromImu.ThePose * lstate.ImuFromCpf;
	double hmdYaw, hmdPitch, hmdRoll;
	worldFromCpf.Rotation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&hmdYaw, &hmdPitch, &hmdRoll);

	Posed worldFromCentered(Quatd(Axis_Y, hmdYaw), worldFromCpf.Translation);

	CenteredFromWorld = worldFromCentered.Inverted();
}

bool SensorStateReader::GetSensorStateAtTime(double absoluteTime, TrackingState& ss) const
{
	if (!Updater)
	{
        ss.StatusFlags = 0;
        return false;
	}

	const LocklessSensorState lstate = Updater->SharedSensorState.GetState();

    // Update time
	ss.HeadPose.TimeInSeconds = absoluteTime;

	// Update the status flags
	ss.StatusFlags = lstate.StatusFlags;
	// If no hardware is connected, override the tracking flags
	if (0 == (ss.StatusFlags & Status_HMDConnected))
	{
		ss.StatusFlags &= ~Status_TrackingMask;
	}
	if (0 == (ss.StatusFlags & Status_PositionConnected))
	{
		ss.StatusFlags &= ~(Status_PositionTracked | Status_CameraPoseTracked);
	}

	// If tracking info is invalid,
    if (0 == (ss.StatusFlags & Status_TrackingMask))
	{
        return false;
	}

    // Delta time from the last available data
	double pdt = absoluteTime - lstate.WorldFromImu.TimeInSeconds;
	static const double maxPdt = 0.1;

	// If delta went negative due to synchronization problems between processes or just a lag spike,
	if (pdt < 0.)
	{
		pdt = 0.;
	}
	else if (pdt > maxPdt)
	{
        if (LastLatWarnTime != lstate.WorldFromImu.TimeInSeconds)
        {
            LastLatWarnTime = lstate.WorldFromImu.TimeInSeconds;
            LogText("[SensorStateReader] Prediction interval too high: %f s, clamping at %f s\n", pdt, maxPdt);
        }
		pdt = maxPdt;
	}

	ss.HeadPose = PoseStatef(lstate.WorldFromImu);
	// Do prediction logic and ImuFromCpf transformation
	ss.HeadPose.ThePose = Posef(CenteredFromWorld * calcPredictedPose(lstate.WorldFromImu, pdt) * lstate.ImuFromCpf);

    ss.CameraPose = Posef(CenteredFromWorld * lstate.WorldFromCamera);

    Posed worldFromLeveledCamera = Posed(Quatd(), lstate.WorldFromCamera.Translation);
    ss.LeveledCameraPose = Posef(CenteredFromWorld * worldFromLeveledCamera);

    ss.RawSensorData = lstate.RawSensorData;
    ss.LastVisionProcessingTime = lstate.LastVisionProcessingTime;

	return true;
}

bool SensorStateReader::GetPoseAtTime(double absoluteTime, Posef& transform) const
{
	TrackingState ss;
	if (!GetSensorStateAtTime(absoluteTime, ss))
	{
		return false;
	}

	transform = ss.HeadPose.ThePose;

	return true;
}

uint32_t SensorStateReader::GetStatus() const
{
	if (!Updater)
	{
		return 0;
	}

	const LocklessSensorState lstate = Updater->SharedSensorState.GetState();

	// If invalid,
	if (0 == (lstate.StatusFlags & Status_TrackingMask))
	{
		// Return 0 indicating no orientation nor position tracking
		return 0;
	}

	return lstate.StatusFlags;
}

void SensorStateReader::LoadProfileCenteredFromWorld(Profile* profile)
{
    double camerastate[7];
    if (profile->GetDoubleValues(OVR_KEY_CAMERA_POSITION, camerastate, 7) == 0)
    {
        for (int i = 0; i < 7; i++) camerastate[i] = 0;
        camerastate[3] = 1;//no offset. by default, give the quaternion w component value 1
    }

    OVR::Quatd orientation = OVR::Quatd(camerastate[0], camerastate[1], camerastate[2], camerastate[3]);
    OVR::Vector3d position = OVR::Vector3d(camerastate[4], camerastate[5], camerastate[6]);

    CenteredFromWorld = OVR::Posed(orientation, position);

}

void SensorStateReader::SaveProfileCenteredFromWorld(Profile* profile)
{
    OVR::Quatd rot = CenteredFromWorld.Rotation;
    OVR::Vector3d trans = CenteredFromWorld.Translation;
    double vals[7] = { rot.x, rot.y, rot.z, rot.w, trans.x, trans.y, trans.z };
    profile->SetDoubleValues(OVR_KEY_CAMERA_POSITION, vals, 7);
}

}} // namespace OVR::Tracking
