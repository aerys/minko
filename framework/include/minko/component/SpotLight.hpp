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

#include "minko/component/AbstractDiscreteLight.hpp"

namespace minko
{
	namespace component
	{
		class SpotLight :
			public AbstractDiscreteLight
		{
		public:
			typedef std::shared_ptr<SpotLight> Ptr;

		public:
			inline static
			Ptr
			create(float diffuse				= 1.0f,
				   float specular				= 1.0f,
                   float innerAngleRadians      = M_PI * 0.20f,
                   float outerAngleRadians      = M_PI * 0.25f,
				   float attenuationConstant	= -1.0f,
				   float attenuationLinear		= -1.0f,
				   float attenuationQuadratic	= -1.0f)
			{
                return std::shared_ptr<SpotLight>(new SpotLight(
                    diffuse,
                    specular,
                    innerAngleRadians,
                    outerAngleRadians,
                    attenuationConstant,
                    attenuationLinear,
                    attenuationQuadratic
                ));
			}

			AbstractComponent::Ptr
			clone(const CloneOption& option);

            float
            innerConeAngle() const;

			SpotLight&
			innerConeAngle(float radians);

            float
            outerConeAngle() const;

			SpotLight&
			outerConeAngle(float radians);

			bool
			attenuationEnabled() const;

			const math::vec3&
			attenuationCoefficients() const;

			SpotLight&
			attenuationCoefficients(float constant, float linear, float quadratic);

			SpotLight&
			attenuationCoefficients(const math::vec3&);

            inline
            math::vec3
            position() const
            {
                return data()->get<math::vec3>("position");
            }

		protected:
			void
            updateModelToWorldMatrix(const math::mat4& modelToWorld);

		private:
			SpotLight(float diffuse,
                      float specular,
                      float innerAngleRadians,
                      float outerAngleRadians,
                      float attenuationConstant,
                      float attenuationLinear,
                      float attenuationQuadratic);

			SpotLight(const SpotLight& spotlight, const CloneOption& option);
		};
	}
}
