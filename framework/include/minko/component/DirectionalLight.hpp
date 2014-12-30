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
			math::vec3                         _worldDirection;
            bool                               _shadowMappingEnabled;
            std::shared_ptr<render::Texture>   _shadowMap;
            std::shared_ptr<Renderer>          _renderer;
            math::mat4                         _view;
            math::mat4                         _projection;

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
            void
            shadowProjection(const minko::math::mat4& projection)
            {
                _projection = projection;
                updateWorldToScreenMatrix();
            }

            inline
            std::shared_ptr<render::Texture>
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

            void
            computeShadowProjection(const math::mat4& viewProjection);

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

            void
            initializeShadowMapping(std::shared_ptr<file::AssetLibrary> assets);

            void
            updateWorldToScreenMatrix();
	    };
    }
}
