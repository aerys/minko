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

#include "minko/component/SpotLight.hpp"

using namespace minko;
using namespace minko::component;

SpotLight::SpotLight(float diffuse,
					 float specular,
                     float innerAngleRadians,
                     float outerAngleRadians,
                     float attenuationConstant,
                     float attenuationLinear,
                     float attenuationQuadratic) :
	AbstractDiscreteLight("spotLight", diffuse, specular)
{
    updateModelToWorldMatrix(math::mat4(1.f));

    attenuationCoefficients(math::vec3(attenuationConstant, attenuationLinear, attenuationQuadratic));
	innerConeAngle(innerAngleRadians);
	outerConeAngle(outerAngleRadians);
}

SpotLight::SpotLight(const SpotLight& spotlight, const CloneOption& option) :
	AbstractDiscreteLight("spotLights", spotlight.diffuse(), spotlight.specular())
{
    updateModelToWorldMatrix(math::mat4(1.f));

    auto test = spotlight.attenuationCoefficients();

	data()->set("attenuationCoeffs", spotlight.attenuationCoefficients());
	data()->set("cosInnerConeAngle", spotlight.innerConeAngle());
	data()->set("cosOuterConeAngle", spotlight.outerConeAngle());
}

AbstractComponent::Ptr
SpotLight::clone(const CloneOption& option)
{
	auto light = std::shared_ptr<SpotLight>(new SpotLight(*this, option));

	return light;
}

void
SpotLight::updateModelToWorldMatrix(const math::mat4& modelToWorld)
{
	data()
		->set("position",	(modelToWorld * math::vec4(0.f, 0.f, 0.f, 1.f)).xyz())
		->set("direction",	math::normalize(math::mat3(modelToWorld) * math::vec3(0.f, 0.f, -1.f)));
}

float
SpotLight::innerConeAngle() const
{
    return acos(data()->get<float>("cosInnerConeAngle"));
}

SpotLight&
SpotLight::innerConeAngle(float radians)
{
	data()->set<float>(
		"cosInnerConeAngle",
		cosf(std::max(0.0f, std::min(0.5f * math::pi<float>(), radians)))
	);

	return *this;
}

float
SpotLight::outerConeAngle() const
{
    return acos(data()->get<float>("cosOuterConeAngle"));
}

SpotLight&
SpotLight::outerConeAngle(float radians)
{
	data()->set<float>(
		"cosOuterConeAngle",
        cosf(std::max(0.0f, std::min(0.5f * math::pi<float>(), radians)))
	);

	return *this;
}

const math::vec3&
SpotLight::attenuationCoefficients() const
{
	return data()->get<math::vec3>("attenuationCoeffs");
}

SpotLight&
SpotLight::attenuationCoefficients(float constant, float linear, float quadratic)
{
	return attenuationCoefficients(math::vec3(constant, linear, quadratic));
}

SpotLight&
SpotLight::attenuationCoefficients(const math::vec3& value)
{
	data()->set("attenuationCoeffs", value);

	return *this;
}

bool
SpotLight::attenuationEnabled() const
{
	auto& coef = attenuationCoefficients();

	return !(coef.x < 0.0f || coef.y < 0.0f || coef.z < 0.0f);
}
