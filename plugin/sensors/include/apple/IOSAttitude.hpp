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
#include "minko/sensors/AbstractAttitude.hpp"
#include "minko/log/Logger.hpp"

#import <CoreMotion/CoreMotion.h>

namespace minko
{
    namespace apple
    {
        namespace sensors
        {
            class IOSAttitude : public minko::sensors::AbstractAttitude
            {
            public:
                typedef std::shared_ptr<IOSAttitude> Ptr;

            private:
                IOSAttitude();

            public:
                static
                inline
                Ptr
                create()
                {
                    return std::shared_ptr<IOSAttitude>(new IOSAttitude());
                }

                void
                initialize();

                void
                startTracking();

                void
                stopTracking();
                
                const math::mat4&
                rotationMatrix();

                const math::vec4&
                quaternion();

            private:

                const math::mat4&
                getRotateEulerMatrix(float x, float y, float z);
                
                const math::mat4&
                glmMatrixFromCMRotationMatrix(CMRotationMatrix rotationMatrix);
                
                CMMotionManager *_manager;
            };
        }
    }
}