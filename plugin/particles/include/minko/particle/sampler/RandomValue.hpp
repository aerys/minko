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

#pragma once

#include "minko/ParticlesCommon.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/random.hpp"

namespace minko
{
    namespace particle
    {
        namespace sampler
        {
            template <class T>
            class RandomValue : public Sampler<T>
            {
            public:
                typedef std::shared_ptr<RandomValue<T> > Ptr;

            private:
                T _min;
                T _delta;

            public:
                static
                Ptr
                create(T min, T max)
                {
                    Ptr sampler = std::shared_ptr<RandomValue>(new RandomValue(min, max));

                    return sampler;
                };

                inline
                void
                min(T value)
                {
                    _min = value;
                };

                inline
                void
                max(T value)
                {
                    _delta = value - _min;
                };

            public:
                virtual
                T value(float time) const
                {
                    return _min + _delta * tools::rand01();
                };

                virtual
                void
                set(T& value, float time) const
                {
                    value = _min + _delta * tools::rand01();
                };

                virtual
                T
                max() const
                {
                    return _min + _delta;
                };

                virtual
                T
                min() const
                {
                    return _min;
                };

            protected:
                RandomValue(T min, T max):
                    _min(min),
                    _delta(max - min)
                {
                }
            };
        }
    }
}