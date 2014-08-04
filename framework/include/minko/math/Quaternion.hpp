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
        class Quaternion:
            public std::enable_shared_from_this<Quaternion>
        {
        public:
            typedef std::shared_ptr<Quaternion> Ptr;

            typedef std::shared_ptr<Matrix4x4>            Matrix4x4Ptr;
            typedef std::shared_ptr<const Matrix4x4>    Matrix4x4ConstPtr;
            typedef std::shared_ptr<Vector3>            Vector3Ptr;

        private:
            float _i;
            float _j;
            float _k;
            float _r;

        public:
            inline static
            Ptr
            create(float i = 0.0f, float j = 0.0f, float k = 0.0f, float r = 1.0f)
            {
                return std::shared_ptr<Quaternion>(new Quaternion(i, j, k, r));
            }

            inline
            Ptr
            copyFrom(Ptr value)
            {
                return setTo(value->i(), value->j(), value->k(), value->r());
            }

            inline
            Ptr
            setTo(float i, float j, float k, float r)
            {
                _i    = i;
                _j    = j;
                _k    = k;
                _r    = r;

                return std::static_pointer_cast<Quaternion>(shared_from_this());
            }

            Ptr
            initialize(float radians, Vector3Ptr axis);

            inline
            float
            i() const
            {
                return _i;
            }

            inline
            float
            j() const
            {
                return _j;
            }

            inline
            float
            k() const
            {
                return _k;
            }

            inline
            float
            r() const
            {
                return _r;
            }

            Ptr
            identity();

            Ptr
            invert();

            Ptr
            fromMatrix(Matrix4x4ConstPtr);

            Matrix4x4Ptr
            toMatrix(Matrix4x4Ptr output = nullptr) const;

            Ptr
            normalize();

            float
            length() const;

            Ptr
            slerp(Ptr target, float ratio);

            std::string
            toString()
            {
                std::stringstream stream;

                stream << "(" << _i << ", " << _j << ", " << _k << ", " << _r << ")";

                return stream.str();

            }
        private:
            Quaternion(float i = 1.0f, float j = 0.0f, float k = 0.0f, float r = 0.0f)
                : _i(i), _j(j), _k(k), _r(r)
            {
            }

            float
            lengthSquared() const;
        };
    }
}
