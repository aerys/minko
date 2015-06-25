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
#include "AbstractAttitude.hpp"

namespace minko
{
    namespace sensors
    {
        class Attitude : AbstractAttitude
        {
        public:
            typedef std::shared_ptr<Attitude> Ptr;
            
            static
            Ptr
            getInstance()
            {
                if (_instance == nullptr)
                    _instance = Ptr(new Attitude());

                return _instance;
            }
            
            void
            initialize() override;

            void
            startTracking() override;

            void
            stopTracking() override;
            
            const math::mat4&
            rotationMatrix() override;

            const math::quat&
            quaternion() override;
        private:
            Attitude();
            
            std::shared_ptr<AbstractAttitude> _attitudeManager;

            static Ptr _instance;
        };
    }
}

