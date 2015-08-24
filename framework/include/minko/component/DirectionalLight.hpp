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
	    class DirectionalLight :
            public AbstractDiscreteLight
	    {
	    public:
		    typedef std::shared_ptr<DirectionalLight> Ptr;

        private:
            typedef std::shared_ptr<render::Texture>    TexturePtr;
            typedef std::shared_ptr<Renderer>           RendererPtr;

        public:
            static const uint MAX_NUM_SHADOW_CASCADES;
            static const uint DEFAULT_NUM_SHADOW_CASCADES;
            static const uint MIN_SHADOWMAP_SIZE;
            static const uint MAX_SHADOWMAP_SIZE;
            static const uint DEFAULT_SHADOWMAP_SIZE;

		private:
			math::vec3                  _worldDirection;
            bool                        _shadowMappingEnabled;
            uint                        _shadowMapSize;
            TexturePtr                  _shadowMap;
            uint                        _numShadowCascades;
            std::array<RendererPtr, 4>  _shadowRenderers;
            std::array<math::mat4, 4>   _shadowProjections;
            math::mat4                  _view;

	    public:
		    inline static
		    Ptr
		    create(float diffuse = 1.0f, float specular	= 1.0f)
		    {
                return std::shared_ptr<DirectionalLight>(new DirectionalLight(diffuse, specular));
		    }

			AbstractComponent::Ptr
			clone(const CloneOption& option);

		    ~DirectionalLight()
		    {
		    }

            inline
            TexturePtr
            shadowMap()
            {
                return _shadowMap;
            }

            inline
            void
            shadowSpread(float spread)
            {
                data()->set("shadowSpread", spread);
            }

            inline
            const std::array<math::mat4, 4>&
            shadowProjections() const
            {
                return _shadowProjections;
            }

            inline
            bool
            shadowMappingEnabled() const
            {
                return _shadowMappingEnabled;
            }

            inline
            uint
            numShadowCascades() const
            {
                return _numShadowCascades;
            }

            inline
            void
            computeShadowProjection(const math::mat4&   view,
                                    const math::mat4&   projection)
            {
                computeShadowProjection(view, projection, std::numeric_limits<float>::max());
            }

            void
            computeShadowProjection(const math::mat4&   view,
                                    const math::mat4&   projection,
                                    float               zFar);

            void
            enableShadowMapping(uint shadowMapSize  = DEFAULT_SHADOWMAP_SIZE,
                                uint numCascades    = DEFAULT_NUM_SHADOW_CASCADES);

            void
            disableShadowMapping(bool disposeResources = false);

		protected:
			void
            updateModelToWorldMatrix(const math::mat4& modelToWorld);

            void
            updateRoot(std::shared_ptr<scene::Node> root);

            void
            targetRemoved(std::shared_ptr<scene::Node> target);

	    private:
            DirectionalLight(float diffuse, float specular);

			DirectionalLight(const DirectionalLight& directionalLight, const CloneOption& option);

            bool
            initializeShadowMapping();

            void
            updateWorldToScreenMatrix();

            std::pair<math::vec3, math::vec3>
            computeBox(const math::mat4& viewProjection);

            std::pair<math::vec3, float>
            computeBoundingSphere(const math::mat4& view, const math::mat4& projection);

            std::pair<math::vec3, float>
            minSphere(std::vector<math::vec3> pt, uint np, std::vector<math::vec3> bnd, uint nb);
	    };
    }
}
