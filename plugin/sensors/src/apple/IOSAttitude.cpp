/*
Copyright (c) 2015 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/Common.hpp"
#include "apple/IOSAttitude.hpp"

using namespace minko;
using namespace apple::sensors;

IOSAttitude::IOSAttitude() :
    _manager(nullptr),
    _rotationMatrix(math::mat4()),
    _quaternion(math::quat()),
    _deviceToDisplay(math::mat4()),
    _worldToInertialReferenceFrame(math::mat4())
{
}

void IOSAttitude::initialize()
{
    _manager = [[CMMotionManager alloc] init];

    // The inertial reference frame has z up and x forward, while the world has z out and x right
    _worldToInertialReferenceFrame = getRotateEulerMatrix(-90.f, 0.f, 90.f);
    
    // This assumes the device is landscape with the home button on the right
    _deviceToDisplay = getRotateEulerMatrix(0.f, 0.f, -90.f);
}

void
IOSAttitude::startTracking()
{
    LOG_INFO("Start tracking");
    
    if (_manager.isDeviceMotionAvailable && !_manager.isDeviceMotionActive)
    {
        [_manager startDeviceMotionUpdatesUsingReferenceFrame:CMAttitudeReferenceFrameXArbitraryZVertical];
    }
}

void
IOSAttitude::stopTracking()
{
    LOG_INFO("Stop tracking");
    
    [_manager stopDeviceMotionUpdates];
}

const math::mat4&
IOSAttitude::rotationMatrix()
{
    CMDeviceMotion *motion = _manager.deviceMotion;
    CMRotationMatrix rotationMatrix = motion.attitude.rotationMatrix;
    
    auto inertialReferenceFrameToDevice = math::transpose(glmMatrixFromCMRotationMatrix(rotationMatrix));
    
    auto worldToDevice = inertialReferenceFrameToDevice * _worldToInertialReferenceFrame;
    auto worldToDisplay = _deviceToDisplay * worldToDevice;
    
    _rotationMatrix = worldToDisplay;
    
    return _rotationMatrix;
}

const math::quat&
IOSAttitude::quaternion()
{
    CMDeviceMotion *motion = _manager.deviceMotion;
    CMQuaternion quaternion = motion.attitude.quaternion;
    
    _quaternion = math::quat();
    _quaternion.x = quaternion.x;
    _quaternion.y = quaternion.y;
    _quaternion.z = quaternion.z;
    _quaternion.w = quaternion.w;
    
    // TODO: Perform the same operations than into rotationMatrix()?
    
    return _quaternion;
}

math::mat4
IOSAttitude::glmMatrixFromCMRotationMatrix(CMRotationMatrix rotationMatrix)
{
    math::mat4 glmRotationMatrix(
        rotationMatrix.m11, rotationMatrix.m12, rotationMatrix.m13, 0.0f,
        rotationMatrix.m21, rotationMatrix.m22, rotationMatrix.m23, 0.0f,
        rotationMatrix.m31, rotationMatrix.m32, rotationMatrix.m33, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    return glmRotationMatrix;
}

math::mat4
IOSAttitude::getRotateEulerMatrix(float x, float y, float z)
{
    x *= (float)(M_PI / 180.0f);
    y *= (float)(M_PI / 180.0f);
    z *= (float)(M_PI / 180.0f);
    
    float cx = (float) cos(x);
    float sx = (float) sin(x);
    float cy = (float) cos(y);
    float sy = (float) sin(y);
    float cz = (float) cos(z);
    float sz = (float) sin(z);
    float cxsy = cx * sy;
    float sxsy = sx * sy;
    
    math::mat4 matrix;
    
    matrix[0][0] = cy * cz;
    matrix[0][1] = -cy * sz;
    matrix[0][2] = sy;
    matrix[0][3] = 0.0f;
    matrix[1][0] = cxsy * cz + cx * sz;
    matrix[1][1] = -cxsy * sz + cx * cz;
    matrix[1][2] = -sx * cy;
    matrix[1][3] = 0.0f;
    matrix[2][0] = -sxsy * cz + sx * sz;
    matrix[2][1] = sxsy * sz + sx * cz;
    matrix[2][2] = cx * cy;
    matrix[2][3] = 0.0f;
    matrix[3][0] = 0.0f;
    matrix[3][1] = 0.0f;
    matrix[3][2] = 0.0f;
    matrix[3][3] = 1.0f;
    
    return matrix;
}