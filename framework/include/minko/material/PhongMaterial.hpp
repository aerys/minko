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

#include "minko/material/BasicMaterial.hpp"

namespace minko
{
	namespace material
	{
		class PhongMaterial:
			public BasicMaterial
		{
		public:
			typedef std::shared_ptr<PhongMaterial>	Ptr;

		public:
			inline static
			Ptr
			create()
			{
                auto instance = Ptr(new PhongMaterial());

                instance->initialize();

                return instance;
			}

			Ptr
			specularColor(const math::vec4&);

			Ptr
			specularColor(uint);

			math::vec4
			specularColor() const;

			Ptr
			shininess(float);

			float
			shininess() const;

			Ptr
			normalMap(AbsTexturePtr);

            render::ResourceId
			normalMap() const;

            Ptr
            normalMapWrapMode(render::WrapMode);

            render::WrapMode
            normalMapWrapMode() const;

            Ptr
            normalMapTextureFilter(render::TextureFilter);

            render::TextureFilter
            normalMapTextureFilter() const;

            Ptr
            normalMapMipFilter(render::MipFilter);

            render::MipFilter
            normalMapMipFilter() const;

			Ptr
			specularMap(AbsTexturePtr);

			render::ResourceId
			specularMap() const;

            Ptr
            specularMapWrapMode(render::WrapMode);

            render::WrapMode
            specularMapWrapMode() const;

            Ptr
            specularMapTextureFilter(render::TextureFilter);

            render::TextureFilter
            specularMapTextureFilter() const;

            Ptr
            specularMapMipFilter(render::MipFilter);

            render::MipFilter
            specularMapMipFilter() const;

			Ptr
			environmentMap(AbsTexturePtr, render::EnvironmentMap2dType type = render::EnvironmentMap2dType::Unset);

            Ptr
            environmentMapWrapMode(render::WrapMode);

            render::WrapMode
            environmentMapWrapMode() const;

            Ptr
            environmentMapTextureFilter(render::TextureFilter);

            render::TextureFilter
            environmentMapTextureFilter() const;

            Ptr
            environmentMapMipFilter(render::MipFilter);

            render::MipFilter
            environmentMapMipFilter() const;

            render::ResourceId
			environmentCubemap() const;

            Ptr
            environmentCubemapWrapMode(render::WrapMode);

            render::WrapMode
            environmentCubemapWrapMode() const;

            Ptr
            environmentCubemapTextureFilter(render::TextureFilter);

            render::TextureFilter
            environmentCubemapTextureFilter() const;

            Ptr
            environmentCubemapMipFilter(render::MipFilter);

            render::MipFilter
            environmentCubemapMipFilter() const;

            render::ResourceId
			environmentMap2d() const;

			render::EnvironmentMap2dType
			environmentMap2dType() const;

            Ptr
            environmentMap2dWrapMode(render::WrapMode);

            render::WrapMode
            environmentMap2dWrapMode() const;

            Ptr
            environmentMap2dTextureFilter(render::TextureFilter);

            render::TextureFilter
            environmentMap2dTextureFilter() const;

            Ptr
            environmentMap2dMipFilter(render::MipFilter);

            render::MipFilter
            environmentMap2dMipFilter() const;

			Ptr
			environmentAlpha(float);

			float
			environmentAlpha() const;

			Ptr
			alphaMap(AbsTexturePtr);

            render::ResourceId
			alphaMap() const;

            Ptr
            alphaMapWrapMode(render::WrapMode);

            render::WrapMode
            alphaMapWrapMode() const;

            Ptr
            alphaMapTextureFilter(render::TextureFilter);

            render::TextureFilter
            alphaMapTextureFilter() const;

            Ptr
            alphaMapMipFilter(render::MipFilter);

            render::MipFilter
            alphaMapMipFilter() const;

			Ptr
			alphaThreshold(float);

			float
			alphaThreshold() const;

        protected:
            void
            initialize();

		private:
			PhongMaterial();
		};
	}
}