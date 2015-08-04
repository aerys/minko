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

#include "minko/Common.hpp"

namespace minko
{
	namespace math
	{
		class Ray
		{
		public:
			typedef std::shared_ptr<Ray>	Ptr;

		private:
			vec3	_origin;
			vec3	_direction;

		public:
			inline static
			Ptr
			create(const vec3& origin, const vec3& direction)
			{
				return std::shared_ptr<Ray>(new Ray(origin, direction));
			}

			inline static
			Ptr
			create()
			{
				return std::shared_ptr<Ray>(new Ray());
			}

			inline
			vec3&
			direction()
			{
				return _direction;
			}

            inline
            void
            direction(const vec3& direction)
            {
                _direction = direction;
            }

			inline
			vec3&
			origin()
			{
				return _origin;
			}

            inline
            void
            origin(const vec3& origin)
            {
                _origin = origin;
            }

		private:
			Ray(const vec3& origin, const vec3& direction) :
				_origin(origin),
				_direction(direction)
			{
			}

			Ray() :
				_origin(vec3(0.)),
				_direction(vec3(0.f, 0.f, -1.f))
			{
			}
		};
	}
}