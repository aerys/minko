/************************************************************************************

Filename    :   Tracking_PoseState.h
Content     :   Describes the complete pose at a point in time, including derivatives
Created     :   May 13, 2014
Authors     :   Dov Katz

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

#ifndef Tracking_PoseState_h
#define Tracking_PoseState_h

#include "../Kernel/OVR_Math.h"

namespace OVR {

// PoseState describes the complete pose, or a rigid body configuration, at a
// point in time, including first and second derivatives. It is used to specify
// instantaneous location and movement of the headset.
// SensorState is returned as a part of the sensor state.

template<class T>
class PoseState
{
public:
	typedef typename CompatibleTypes<Pose<T> >::Type CompatibleType;

	PoseState() : TimeInSeconds(0.0) { }
    PoseState(Pose<T> pose, double time) : ThePose(pose), TimeInSeconds(time) { }

	// float <-> double conversion constructor.
	explicit PoseState(const PoseState<typename Math<T>::OtherFloatType> &src)
		: ThePose(src.ThePose),
		AngularVelocity(src.AngularVelocity), LinearVelocity(src.LinearVelocity),
		AngularAcceleration(src.AngularAcceleration), LinearAcceleration(src.LinearAcceleration),
		TimeInSeconds(src.TimeInSeconds)
	{ }

	// C-interop support: PoseStatef <-> ovrPoseStatef
	PoseState(const typename CompatibleTypes<PoseState<T> >::Type& src)
		: ThePose(src.ThePose),
		AngularVelocity(src.AngularVelocity), LinearVelocity(src.LinearVelocity),
		AngularAcceleration(src.AngularAcceleration), LinearAcceleration(src.LinearAcceleration),
		TimeInSeconds(src.TimeInSeconds)
	{ }

	operator typename CompatibleTypes<PoseState<T> >::Type() const
	{
		typename CompatibleTypes<PoseState<T> >::Type result;
		result.ThePose = ThePose;
		result.AngularVelocity = AngularVelocity;
		result.LinearVelocity = LinearVelocity;
		result.AngularAcceleration = AngularAcceleration;
		result.LinearAcceleration = LinearAcceleration;
		result.TimeInSeconds = TimeInSeconds;
		return result;
	}

	Pose<T> ThePose;
	Vector3<T>  AngularVelocity;
	Vector3<T>  LinearVelocity;
	Vector3<T>  AngularAcceleration;
	Vector3<T>  LinearAcceleration;
	// Absolute time of this state sample; always a double measured in seconds.
	double      TimeInSeconds;

	// ***** Helpers for Pose integration

	// Stores and integrates gyro angular velocity reading for a given time step.
	void StoreAndIntegrateGyro(Vector3d angVel, double dt);
	// Stores and integrates position/velocity from accelerometer reading for a given time step.
	void StoreAndIntegrateAccelerometer(Vector3d linearAccel, double dt);

	// Performs integration of state by adding next state delta to it
	// to produce a combined state change
	void AdvanceByDelta(const PoseState<T>& delta);
};


template<class T>
PoseState<T> operator*(const OVR::Pose<T>& trans, const PoseState<T>& poseState)
{
	PoseState<T> result;
	result.ThePose = trans * poseState.ThePose;
	result.LinearVelocity = trans.Rotate(poseState.LinearVelocity);
	result.LinearAcceleration = trans.Rotate(poseState.LinearAcceleration);
	result.AngularVelocity = trans.Rotate(poseState.AngularVelocity);
	result.AngularAcceleration = trans.Rotate(poseState.AngularAcceleration);
	return result;
}


// External API returns pose as float, but uses doubles internally for quaternion precision.
typedef PoseState<float>  PoseStatef;
typedef PoseState<double> PoseStated;


} // namespace OVR::Vision


namespace OVR {

	template<> struct CompatibleTypes<OVR::PoseState<float> > { typedef ovrPoseStatef Type; };
	template<> struct CompatibleTypes<OVR::PoseState<double> > { typedef ovrPoseStated Type; };

}

#endif // Tracking_PoseState_h
