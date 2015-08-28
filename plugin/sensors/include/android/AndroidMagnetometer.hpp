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

#pragma once

#include "minko/Minko.hpp"
#include "minko/sensors/AbstractMagnetometer.hpp"
#include "minko/log/Logger.hpp"
#include "minko/Signal.hpp"

#include <jni.h>

namespace minko
{
    namespace android
    {
        namespace sensors
        {
            class AndroidMagnetometer : public minko::sensors::AbstractMagnetometer
            {
            public:
                typedef std::shared_ptr<AndroidMagnetometer> Ptr;
                typedef std::shared_ptr<AbstractMagnetometer> AbstractMagnetometerPtr;

            private:
                AndroidMagnetometer();

            public:
                static
                inline
                Ptr
                create()
                {
                    return std::shared_ptr<AndroidMagnetometer>(new AndroidMagnetometer());
                }

                void
                initialize();

                void
                startTracking();

                void
                stopTracking();

                bool
                isSupported();

                inline
                const math::vec3&
                getSensorValue()
                {
                    return sensorValue;
                }

                inline
                Signal<float, float, float>::Ptr
                onSensorChanged()
                {
                    return sensorChanged;
                }

                static Signal<float, float, float>::Ptr sensorChanged;
                static math::vec3                       sensorValue;
            private:

                // JNI part

                // Java Objects
                jobject _magnetometer = nullptr;

                // Java Method IDs
                jmethodID _startTrackingMethod = nullptr;
                jmethodID _stopTrackingMethod = nullptr;
                jmethodID _isSupportedMethod = nullptr;

            };
        }
    }
}