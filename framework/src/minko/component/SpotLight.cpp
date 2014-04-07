/*
Copyright (c) 2013 Aerys

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

#include "minko/math/Vector3.hpp"
#include "minko/math/Matrix4x4.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;

SpotLight::SpotLight(float diffuse,
					 float specular,
					 float attenuationConstant,
					 float attenuationLinear,
					 float attenuationQuadratic) :
	AbstractDiscreteLight("spotLights", diffuse, specular),
	_attenuationCoeffs(Vector3::create(attenuationConstant, attenuationLinear, attenuationQuadratic)),
	_worldPosition(Vector3::create(0.0f, 0.0f, 0.0f)),
	_worldDirection(Vector3::create()->copyFrom(Vector3::forward()))
{

	data()->set("attenuationCoeffs",	_attenuationCoeffs);
	data()->set("position",				_worldPosition);
	data()->set("direction",			_worldDirection);
}

void 
SpotLight::initialize(float innerAngleRadians,
					  float outerAngleRadians)
{
	AbstractDiscreteLight::initialize();

	innerConeAngle(innerAngleRadians);
	outerConeAngle(std::max(outerAngleRadians, innerAngleRadians));
}

void
SpotLight::updateModelToWorldMatrix(std::shared_ptr<math::Matrix4x4> modelToWorld)
{
	modelToWorld->copyTranslation(_worldPosition);
	modelToWorld->deltaTransform(Vector3::forward(), _worldDirection);
}

SpotLight::Ptr
SpotLight::innerConeAngle(float radians)
{
	_cosInnerConeAngle = cosf(std::max(0.0f, std::min(0.5f * (float)PI, radians)));
	data()->set<float>("cosInnerConeAngle", _cosInnerConeAngle);

	return std::static_pointer_cast<SpotLight>(shared_from_this());
}

SpotLight::Ptr
SpotLight::outerConeAngle(float radians)
{
	_cosOuterConeAngle = cosf(std::max(0.0f, std::min(0.5f * (float)PI, radians)));
	data()->set<float>("cosOuterConeAngle", _cosOuterConeAngle);

	return std::static_pointer_cast<SpotLight>(shared_from_this());
}

Vector3::Ptr
SpotLight::attenuationCoefficients() const
{
	return data()->get<math::Vector3::Ptr>("attenuationCoeffs");
}

SpotLight::Ptr
SpotLight::attenuationCoefficients(float constant, float linear, float quadratic) 
{
	return attenuationCoefficients(Vector3::create(constant, linear, quadratic));
}

SpotLight::Ptr
SpotLight::attenuationCoefficients(Vector3::Ptr value)
{
	_attenuationCoeffs->copyFrom(value);
	data()->set<Vector3::Ptr>("attenuationCoeffs", _attenuationCoeffs);

	return std::static_pointer_cast<SpotLight>(shared_from_this());
}

bool
SpotLight::attenuationEnabled() const
{
	return !( _attenuationCoeffs->x() < 0.0f || _attenuationCoeffs->y() < 0.0f || _attenuationCoeffs->z() < 0.0f);
}