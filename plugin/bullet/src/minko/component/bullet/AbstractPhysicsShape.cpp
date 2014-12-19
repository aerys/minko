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

#include "minko/component/bullet/AbstractPhysicsShape.hpp"
#include "minko/math/tools.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;

bullet::AbstractPhysicsShape::AbstractPhysicsShape(Type type):
    _type(type),
    _margin(0.0f),
    _localScaling(Vector3::create(1.0f, 1.0f, 1.0f)),
    _volumeScaling(1.0f),
    _deltaTransform(Matrix4x4::create()->identity()),
    _deltaTransformInverse(Matrix4x4::create()->identity()),
    _shapeChanged(Signal<Ptr>::create())
{

}

void
bullet::AbstractPhysicsShape::localScaling(float x, float y, float z)
{
    _localScaling->setTo(x, y, z);
    _volumeScaling = fabsf(x * y * z);
}

void
bullet::AbstractPhysicsShape::initialize(Matrix4x4::Ptr deltaTransform,
                                         Matrix4x4::Ptr graphicsStartTransform)
{
    auto deltaScaling = Matrix4x4::create();
    removeScalingShear(
        deltaTransform,
        _deltaTransform,
        deltaScaling
    );

    _deltaTransformInverse
        ->copyFrom(_deltaTransform)
        ->invert();

    localScaling(deltaScaling->values()[0], deltaScaling->values()[5], deltaScaling->values()[10]);

#ifdef DEBUG_PHYSICS
    PhysicsWorld::print(std::cout << "- delta\t=\n", _deltaTransform) << std::endl;
    std::cout << "- local scaling\t= [ " << _localScaling->x() << " " << _localScaling->y() << " " << _localScaling->z() << " ]" << std::endl;
#endif // DEBUG_PHYSICS
}