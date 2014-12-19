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
#include "minko/CloneOption.hpp"

using namespace minko;
using namespace minko::component;

PointLight::PointLight(float diffuse,
					   float specular,
					   float attenuationConstant,
					   float attenuationLinear,
					   float attenuationQuadratic):
	AbstractDiscreteLight("pointLight", diffuse, specular),
	_attenuationCoeffs(math::vec3(attenuationConstant, attenuationLinear, attenuationQuadratic)),
	_worldPosition(math::vec3(0.f))
{
	data()->set("attenuationCoeffs", _attenuationCoeffs);
    updateModelToWorldMatrix(math::mat4(1.f));
}

PointLight::PointLight(const PointLight& pointLight, const CloneOption& option) :
	AbstractDiscreteLight("pointLights", pointLight.diffuse(), pointLight.specular()),
	_attenuationCoeffs(pointLight.attenuationCoefficients())
{
    updateModelToWorldMatrix(math::mat4(1.f));
}

AbstractComponent::Ptr
PointLight::clone(const CloneOption& option)
{
	auto light = std::shared_ptr<PointLight>(new PointLight(*this, option));

	return light;
}

void
PointLight::updateModelToWorldMatrix(const math::mat4& modelToWorld)
{
    data()->set("position", (modelToWorld * math::vec4(0.f, 0.f, 0.f, 1.f)).xyz());
}

const math::vec3&
PointLight::attenuationCoefficients() const
{
	return _attenuationCoeffs;
}

PointLight::Ptr
PointLight::attenuationCoefficients(float constant, float linear, float quadratic) 
{
	return attenuationCoefficients(math::vec3(constant, linear, quadratic));
}

PointLight::Ptr
PointLight::attenuationCoefficients(const math::vec3& value)
{
	data()->set("attenuationCoeffs", _attenuationCoeffs = value);

	return std::static_pointer_cast<PointLight>(shared_from_this());
}

bool
PointLight::attenuationEnabled() const
{
	return !( _attenuationCoeffs.x < 0.0f || _attenuationCoeffs.y < 0.0f || _attenuationCoeffs.z < 0.0f);
}
