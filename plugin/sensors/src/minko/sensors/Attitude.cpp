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

#include "minko/sensors/Attitude.hpp"

#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
# include "android/AndroidAttitude.hpp"
#endif

#if MINKO_PLATFORM == MINKO_PLATFORM_IOS
# include "apple/IOSAttitude.hpp"
#endif

using namespace minko;
using namespace sensors;

std::shared_ptr<Attitude> Attitude::_instance = nullptr;

Attitude::Attitude() :
    AbstractAttitude(),
    _attitudeManager(nullptr)
{
#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    _attitudeManager = android::sensors::AndroidAttitude::create();
#elif MINKO_PLATFORM == MINKO_PLATFORM_IOS
    _attitudeManager = apple::sensors::IOSAttitude::create();
#endif
}

void
Attitude::initialize()
{
    _attitudeManager->initialize();
}

void
Attitude::startTracking()
{
    _attitudeManager->startTracking();
}

void
Attitude::stopTracking()
{
    _attitudeManager->stopTracking();
}

const math::mat4& 
Attitude::rotationMatrix()
{
    return _attitudeManager->rotationMatrix();
}

const math::quat&
Attitude::quaternion()
{
    return _attitudeManager->quaternion();
}

float
Attitude::yaw()
{
    return _attitudeManager->yaw();
}

float
Attitude::pitch()
{
    return _attitudeManager->pitch();
}

float
Attitude::roll()
{
    return _attitudeManager->roll();
}

bool
Attitude::isSupported()
{
    return _attitudeManager->isSupported();
}