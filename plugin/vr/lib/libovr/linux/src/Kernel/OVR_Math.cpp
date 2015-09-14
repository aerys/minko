/************************************************************************************

Filename    :   OVR_Math.h
Content     :   Implementation of 3D primitives such as vectors, matrices.
Created     :   September 4, 2012
Authors     :   Andrew Reisse, Michael Antonov, Anna Yershova

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

#include "OVR_Math.h"
#include "OVR_Log.h"

#include <float.h>


namespace OVR {


//-------------------------------------------------------------------------------------
// ***** Constants

template<>
const Vector3<float> Vector3<float>::ZERO = Vector3<float>();

template<>
const Vector3<double> Vector3<double>::ZERO = Vector3<double>();

template<>
const Matrix4<float> Matrix4<float>::IdentityValue = Matrix4<float>(1.0f, 0.0f, 0.0f, 0.0f, 
                                                                    0.0f, 1.0f, 0.0f, 0.0f, 
                                                                    0.0f, 0.0f, 1.0f, 0.0f,
                                                                    0.0f, 0.0f, 0.0f, 1.0f);

template<>
const Matrix4<double> Matrix4<double>::IdentityValue = Matrix4<double>(1.0, 0.0, 0.0, 0.0, 
                                                                       0.0, 1.0, 0.0, 0.0, 
                                                                       0.0, 0.0, 1.0, 0.0,
                                                                       0.0, 0.0, 0.0, 1.0);


} // Namespace OVR
