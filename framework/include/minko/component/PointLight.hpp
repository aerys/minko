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
		class PointLight:
			public AbstractDiscreteLight
		{
		public:
			typedef std::shared_ptr<PointLight> Ptr;
	
		private:
			math::vec3	_attenuationCoeffs;
			math::vec3	_worldPosition;

		public:
			inline static
			Ptr
			create(float diffuse				= 1.0f,
				   float specular				= 1.0f,
				   float attenuationConstant	= -1.0f,
				   float attenuationLinear		= -1.0f,
				   float attenuationQuadratic	= -1.0f)
			{
                return std::shared_ptr<PointLight>(new PointLight(
                    diffuse,
                    specular,
                    attenuationConstant,
                    attenuationLinear,
                    attenuationQuadratic
                ));;
			}
	
            ~PointLight() = default;

			AbstractComponent::Ptr
			clone(const CloneOption& option);

			bool
			attenuationEnabled() const;

			const math::vec3&
			attenuationCoefficients() const;

			Ptr
			attenuationCoefficients(float constant, float linear, float quadratic);

			Ptr
			attenuationCoefficients(const math::vec3& coef);

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
			PointLight(float diffuse,
					   float specular,
					   float attenuationConstant,
					   float attenuationLinear,
					   float attenuationQuadratic);

			PointLight(const PointLight& pointLight, const CloneOption& option);
		};
	}
}
