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
#include "minko/Signal.hpp"

namespace minko
{
    namespace component
    {
        namespace bullet
        {
            class AbstractPhysicsShape:
                public std::enable_shared_from_this<AbstractPhysicsShape>
            {
            public:
                typedef std::shared_ptr<AbstractPhysicsShape> Ptr;

                enum Type
                {
                    SPHERE,
                    BOX,
                    CONE,
                    CYLINDER
                };

            private:
                typedef std::shared_ptr<math::Vector3>                Vector3Ptr;
                typedef std::shared_ptr<math::Quaternion>            QuaternionPtr;
                typedef std::shared_ptr<math::Matrix4x4>            Matrix4x4Ptr;
                typedef std::shared_ptr<geometry::LineGeometry>        LineGeometryPtr;
                typedef std::shared_ptr<render::AbstractContext>    AbsContextPtr;

            protected:
                Type            _type;
                float            _margin;
                Matrix4x4Ptr    _deltaTransform;
                Matrix4x4Ptr    _deltaTransformInverse;
                Vector3Ptr        _localScaling;
                float            _volumeScaling;

            private:
                std::shared_ptr<Signal<Ptr>> _shapeChanged;

            public:
                AbstractPhysicsShape(Type);

                virtual
                ~AbstractPhysicsShape()
                {

                }

                void
                initialize(Matrix4x4Ptr deltaTransform, Matrix4x4Ptr graphicsStartTransform);

                virtual
                float
                volume() const = 0;

                virtual
                LineGeometryPtr
                getGeometry(AbsContextPtr) const = 0;

                inline
                Type
                type() const
                {
                    return _type;
                }

                inline
                float
                margin() const
                {
                    return _margin;
                }

                inline
                void
                margin(float margin)
                {
                    const bool needsUpdate    = fabsf(margin - _margin) > 1e-6f;
                    _margin    = margin;
                    if (needsUpdate)
                        shapeChanged()->execute(shared_from_this());
                }

                inline
                Vector3Ptr
                localScaling() const
                {
                    return _localScaling;
                }

                void
                localScaling(float x, float y, float z);

                inline
                Matrix4x4Ptr
                deltaTransform() const
                {
                    return _deltaTransform;
                }

                inline
                Matrix4x4Ptr
                deltaTransformInverse() const
                {
                    return _deltaTransformInverse;
                }

                inline
                Signal<Ptr>::Ptr
                shapeChanged()
                {
                    return _shapeChanged;
                }
            };
        }
    }
}
