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

#include "minko/BulletCommon.hpp"

namespace minko
{
    namespace component
    {
        namespace bullet
        {
            class ColliderData: public std::enable_shared_from_this<ColliderData>
            {
            public:
                typedef std::shared_ptr<ColliderData> Ptr;

                typedef std::shared_ptr<scene::Node>            NodePtr;
                typedef std::shared_ptr<AbstractPhysicsShape>    AbsShapePtr;
                typedef std::shared_ptr<math::Matrix4x4>        Matrix4x4Ptr;
                typedef std::shared_ptr<math::Quaternion>        QuaternionPtr;
                typedef std::shared_ptr<math::Vector3>            Vector3Ptr;

            private:
                const float                                        _mass;
                AbsShapePtr                                        _shape;
                Vector3Ptr                                        _inertia;
                float                                            _restitution;       // from bullet: best simulation results using zero restitution.
                float                                            _friction;          // from bullet: best simulation results when friction is non-zero
                float                                            _rollingFriction;

            public:
                inline static
                Ptr
                create(float        mass,
                       AbsShapePtr    shape,
                       float        restitution        = 0.0f,
                       float        friction        = 0.5f,
                       float        rollingFriction    = 0.0f,
                       Vector3Ptr    inertia            = nullptr)
                {
                    return std::shared_ptr<ColliderData>(new ColliderData(
                        mass,
                        shape,
                        restitution,
                        friction,
                        rollingFriction,
                        inertia
                    ));
                }

                inline
                AbsShapePtr
                shape() const
                {
                    return _shape;
                }

                inline
                float
                mass() const
                {
                    return _mass;
                }

                inline
                bool
                isStatic() const
                {
                    return _mass < 1e-6f;
                }

                inline
                Vector3Ptr
                inertia() const
                {
                    return _inertia;
                }

                inline
                float
                restitution() const
                {
                    return _restitution;
                }

                inline
                float
                friction() const
                {
                    return _friction;
                }

                inline
                float
                rollingFriction() const
                {
                    return _rollingFriction;
                }

            private:
                inline
                ColliderData(float            mass,
                             AbsShapePtr    shape,
                             float            restitution,
                             float            friction,
                             float            rollingFriction,
                             Vector3Ptr        inertia):
                    _mass(mass),
                    _shape(shape),
                    _inertia(inertia),
                    _restitution(restitution),
                    _friction(friction),
                    _rollingFriction(rollingFriction)
                {

                }
            };
        }
    }
}
