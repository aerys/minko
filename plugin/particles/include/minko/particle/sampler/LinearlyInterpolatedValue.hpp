/*
Copyright (c) 2013 Aerys

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

namespace minko
{
	namespace particle
	{
		namespace sampler
		{
			template <class T>
			class LinearlyInterpolatedValue : public Sampler<T>
			{
			public:
				typedef std::shared_ptr<LinearlyInterpolatedValue<T> > Ptr;

			private:
				T       _startValue;
				T       _deltaValue;
                float   _startTime;
                float   _endTime;
                float   _invDeltaTime;
                    
			public:
				static inline
				Ptr
				create(T        startValue, 
                       T        endValue, 
                       float    startTime = 0.0f, 
                       float    endTime = 1.0f)
				{
					Ptr ptr = std::shared_ptr<LinearlyInterpolatedValue>(new LinearlyInterpolatedValue(startValue, endValue, startTime, endTime));

					return ptr;
				};

    //            inline
				//void
				//start(T value)
				//{
				//	_startValue = value;
				//};

    //            inline
				//void
				//end(T value)
				//{
				//	_delta = value - _startValue;
				//};

                inline
                T
                min() const
                {
                    return startValue();
                };

                inline
                T
                max() const
                {
                    return startValue();
                };

                inline
				T
				startValue() const
				{
					return _startValue;
				};

                inline
				T
				endValue() const
				{
					return _startValue + _deltaValue;
				};

                inline
                float
                startTime() const
                {
                    return _startTime;
                }

                inline
                float
                endTime() const
                {
                    return _endTime;
                }

                inline
				T
				value(float time) const
				{
                    const float t = std::max(0.0f, std::min(1.0f, (time - _startTime) * _invDeltaTime));

					return _startValue + _deltaValue * t;
				};

                inline
				void
				set(T& target, float time) const
				{
					target = value(time);
				};

			protected:
                inline
				LinearlyInterpolatedValue(T     startValue, 
                                          T     endValue, 
                                          float startTime, 
                                          float endTime): 
                    _startValue(startValue),
                    _deltaValue(endValue - startValue),
                    _startTime(std::min(startTime, endTime)),
                    _endTime(std::max(startTime, endTime)),
                    _invDeltaTime(0.0f)
				{
                    _startTime      = std::max(0.0f, std::min(1.0f, _startTime));
                    _endTime        = std::max(0.0f, std::min(1.0f, _endTime));
                    _invDeltaTime   = fabsf(_endTime - _startTime) < 1e-3f ? 0.0f : 1.0f / (_endTime - _startTime);
                };
			};
		}
	}
}