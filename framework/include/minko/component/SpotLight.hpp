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

        private:
            float                            _cosInnerConeAngle;
            float                            _cosOuterConeAngle;
            std::shared_ptr<math::Vector3>    _attenuationCoeffs;
            std::shared_ptr<math::Vector3>    _worldPosition;
            std::shared_ptr<math::Vector3>    _worldDirection;

        public:
            inline static
            Ptr
            create(float innerAngleRadians        = float(M_PI) * 0.25f,
                   float outerAngleRadians        = -1.0f,
                   float diffuse                = 1.0f,
                   float specular                = 1.0f,
                   float attenuationConstant    = -1.0f,
                   float attenuationLinear        = -1.0f,
                   float attenuationQuadratic    = -1.0f)
            {
                auto light = std::shared_ptr<SpotLight>(
                    new SpotLight(
                        diffuse,
                        specular,
                        attenuationConstant,
                        attenuationLinear,
                        attenuationQuadratic
                    )
                );

                light->initialize(innerAngleRadians, outerAngleRadians);

                return light;
            }

			AbstractComponent::Ptr
			clone(const CloneOption& option);

            inline
            float
            cosInnerConeAngle() const
            {
                return _cosInnerConeAngle;
            }

            Ptr
            innerConeAngle(float radians);

            inline
            float
            cosOuterConeAngle() const
            {
                return _cosOuterConeAngle;
            }

            Ptr
            outerConeAngle(float radians);

            bool
            attenuationEnabled() const;

            std::shared_ptr<math::Vector3>
            attenuationCoefficients() const;

            Ptr
            attenuationCoefficients(float constant, float linear, float quadratic);

            Ptr
            attenuationCoefficients(std::shared_ptr<math::Vector3>);

			inline
			std::shared_ptr<math::Vector3>
			position() const
			{
				return data()->get<std::shared_ptr<math::Vector3>>("position");
			}

			inline
			Ptr
			position(std::shared_ptr<math::Vector3> position)
			{
				data()->set<std::shared_ptr<math::Vector3>>("position", position);

				return std::static_pointer_cast<SpotLight>(shared_from_this());
			}

			inline
			std::shared_ptr<math::Vector3>
			direction() const
			{
				return data()->get<std::shared_ptr<math::Vector3>>("direction");
			}

			inline
			Ptr
			direction(std::shared_ptr<math::Vector3> direction)
			{
				data()->set<std::shared_ptr<math::Vector3>>("direction", direction);

				return std::static_pointer_cast<SpotLight>(shared_from_this());
			}

        protected:
            void
            updateModelToWorldMatrix(std::shared_ptr<math::Matrix4x4> modelToWorld);

        private:
            SpotLight(float diffuse,
                      float specular,
                      float attenuationConstant,
                      float attenuationLinear,
                      float attenuationQuadratic);

			SpotLight(const SpotLight& spotlight, const CloneOption& option);

            void
            initialize(float innerAngleRadians,
                       float outerAngleRadians);
        };
    }
}
