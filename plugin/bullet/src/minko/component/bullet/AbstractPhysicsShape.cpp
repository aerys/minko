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
    _localScaling(math::vec3(1.0f, 1.0f, 1.0f)),
    _volumeScaling(1.0f),
    _deltaTransform(math::mat4()),
    _deltaTransformInverse(math::mat4()),
    _shapeChanged(Signal<Ptr>::create())
{

}

void
bullet::AbstractPhysicsShape::localScaling(float x, float y, float z)
{
    _localScaling.x = x;
    _localScaling.y = y;
    _localScaling.z = z;
    _volumeScaling = fabsf(x * y * z);
}

void
bullet::AbstractPhysicsShape::initialize(const math::mat4& deltaTransform,
                                         const math::mat4& graphicsStartTransform)
{
    auto deltaScaling = math::mat4();

    _deltaTransform = removeScalingShear(deltaTransform, deltaScaling);

    _deltaTransformInverse = math::inverse(_deltaTransform);

    localScaling(deltaScaling[0][0], deltaScaling[1][1], deltaScaling[2][2]);

#ifdef DEBUG_PHYSICS
    std::cout << "- delta\t=\n" << std::to_string(_deltaTransform) << std::endl;
    std::cout << "- local scaling\t= [ " << _localScaling.x << " " << _localScaling.y << " " << _localScaling.z << " ]" << std::endl;
#endif // DEBUG_PHYSICS
}