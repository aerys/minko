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
#include "minko/math/Vector3.hpp"

namespace minko
{
    namespace math
    {
        class Vector4 :
            public Vector3,
            public Convertible<Vector4>
        {
        public:
            typedef std::shared_ptr<Vector4>    Ptr;

        protected:
            float _w;

        public:
            inline static
            Ptr
            create(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f)
            {
                return std::shared_ptr<Vector4>(new Vector4(x, y, z, w));
            }

            inline static
            Ptr
            create(Ptr v)
            {
                return create(v->x(), v->y(), v->z(), v->w());
            }

            inline
            float
            w()
            {
                return _w;
            }

            inline
            void
            w(float w)
            {
                _w = w;
            }

            inline
            Ptr
            copyFrom(Ptr value)
            {
                return setTo(value->_x, value->_y, value->_z, value->_w);
            }

            inline
            Ptr
            setTo(float x, float y, float z, float w)
            {
                _x = x;
                _y = y;
                _z = z;
                _w = w;

                return std::static_pointer_cast<Vector4>(shared_from_this());
            }

            inline
            Ptr
            normalize()
            {
                float l = sqrtf(_x * _x + _y * _y + _z * _z + _w * _w);

                if (l != 0.)
                {
                    _x /= l;
                    _y /= l;
                    _z /= l;
                    _w /= l;
                }

                return std::static_pointer_cast<Vector4>(shared_from_this());
            }

            inline
            Ptr
            cross(Ptr value)
            {
                float x = _y * value->_z - _z * value->_y;
                float y = _z * value->_w - _w * value->_z;
                float z = _w * value->_x - _x * value->_w;
                float w = _x * value->_y - _y * value->_x;

                _x = x;
                _y = y;
                _z = z;
                _w = w;

                return std::static_pointer_cast<Vector4>(shared_from_this());
            }

            inline
            float
            dot(Ptr value) const
            {
                return _x * value->_x + _y * value->_y + _z * value->_z + _w * value->_w;
            }

            std::string
            toString()
            {
                std::stringstream stream;

                stream << "(" << _x << ", " << _y << ", " << _z << ", " << _w << ")";

                return stream.str();
            }

            inline
            Ptr
            operator-()
            {
                return create(-_x, -_y, -_z, -_w);
            }

            inline
            Ptr
            operator-(Ptr value)
            {
                return create(_x - value->_x, _y - value->_y, _z - value->_z, _w - value->_w);
            }

            inline
            Ptr
            operator+(Ptr value)
            {
                return create(_x + value->_x, _y + value->_y, _z + value->_z, _w + value->_w);
            }

            inline
            Ptr
            operator+=(Ptr value)
            {
                _x += value->_x;
                _y += value->_y;
                _z += value->_z;
                _w += value->_w;

                return std::static_pointer_cast<Vector4>(shared_from_this());
            }

            inline
            Ptr
            operator-=(Ptr value)
            {
                _x -= value->_x;
                _y -= value->_y;
                _z -= value->_z;
                _w -= value->_w;

                return std::static_pointer_cast<Vector4>(shared_from_this());
            }

            inline
            Ptr
            operator*(float value)
            {
                _x *= value;
                _y *= value;
                _z *= value;
                _w *= value;

                return std::static_pointer_cast<Vector4>(shared_from_this());
            }

            inline
            Ptr
            lerp(Ptr target, float ratio)
            {
                return setTo(
                    _x + (target->_x - _x) * ratio,
                    _y + (target->_y - _y) * ratio,
                    _z + (target->_z - _z) * ratio,
                    _w + (target->_w - _w) * ratio
                );
            }

            inline
            float
            lengthSquared() const
            {
                return _x * _x + _y * _y + _z * _z + _w * _w;
            }

            inline
            float
            length() const
            {
                return sqrtf(lengthSquared());
            }

            bool
            equals(std::shared_ptr<Vector4> vect4)
            {
                return vect4->x() == _x &&
                       vect4->y() == _y &&
                       vect4->z() == _z &&
                       vect4->w() == _w;
            }

        protected:
            Vector4(float x, float y, float z, float w) :
                Vector3(x, y, z),
                _w(w)
            {
            }
        };

    }
}
