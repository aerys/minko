/*
Copyright (c) 2014 Aerys

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

namespace minko
{
    namespace component
    {
        class AbstractLodPriorityModifier
        {
        public:
            typedef std::shared_ptr<AbstractLodPriorityModifier> Ptr;

        protected:
            typedef std::shared_ptr<Surface>    SurfacePtr;

            typedef std::shared_ptr<math::Box>  BoxPtr;

        private:
            float _refreshRate;
            float _refreshTime;

            float _previousValue;
            float _targetValue;

            float _previousTime;

        public:
            virtual
            ~AbstractLodPriorityModifier() = default;

            inline
            float
            refreshRate() const
            {
                return _refreshRate;
            }

            inline
            void
            refreshRate(float value)
            {
                if (_refreshRate == value)
                    return;

                _refreshRate = value;

                if (_refreshRate > 0.f)
                {
                    _refreshTime = 1.f / _refreshRate;
                }
                else
                {
                    _refreshTime = 0.f;
                }
            }

            void
            initialize(SurfacePtr surface);

            float
            value(SurfacePtr          surface,
                  const math::vec3&   eyePosition,
                  const math::vec4&   viewport,
                  const math::mat4&   worldToScreenMatrix,
                  const math::mat4&   viewMatrix,
                  float               time);

        protected:
            AbstractLodPriorityModifier();

            virtual
            void
            doInitialize(SurfacePtr surface) = 0;

            virtual
            float
            computeValue(SurfacePtr          surface,
                         const math::vec3&   eyePosition,
                         const math::vec4&   viewport,
                         const math::mat4&   worldToScreenMatrix,
                         const math::mat4&   viewMatrix,
                         float               time) = 0;
        };
    }
}
