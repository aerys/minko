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

#include "minko/component/PointLight.hpp"

#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;

PointLight::PointLight(float diffuse,
                       float specular,
                       float attenuationConstant,
                       float attenuationLinear,
                       float attenuationQuadratic):
    AbstractDiscreteLight("pointLights", diffuse, specular),
    _attenuationCoeffs(Vector3::create(attenuationConstant, attenuationLinear, attenuationQuadratic)),
    _worldPosition(Vector3::create(0.0f, 0.0f, 0.0f))
{
    data()
        ->set("attenuationCoeffs",    _attenuationCoeffs)
        ->set("position",            _worldPosition);
}

void
PointLight::updateModelToWorldMatrix(std::shared_ptr<math::Matrix4x4> modelToWorld)
{
    modelToWorld->copyTranslation(_worldPosition);
}

Vector3::Ptr
PointLight::attenuationCoefficients() const
{
    return data()->get<math::Vector3::Ptr>("attenuationCoeffs");
}

PointLight::Ptr
PointLight::attenuationCoefficients(float constant, float linear, float quadratic)
{
    return attenuationCoefficients(Vector3::create(constant, linear, quadratic));
}

PointLight::Ptr
PointLight::attenuationCoefficients(Vector3::Ptr value)
{
    _attenuationCoeffs->copyFrom(value);
    data()->set<Vector3::Ptr>("attenuationCoeffs", _attenuationCoeffs);

    return std::static_pointer_cast<PointLight>(shared_from_this());
}

bool
PointLight::attenuationEnabled() const
{
    return !( _attenuationCoeffs->x() < 0.0f || _attenuationCoeffs->y() < 0.0f || _attenuationCoeffs->z() < 0.0f);
}