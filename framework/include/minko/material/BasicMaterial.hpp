/*
Copyright (c) 2022 Aerys

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

#include "minko/material/Material.hpp"
#include "minko/material/FogTechnique.hpp"

namespace minko
{
	namespace material
	{
		class BasicMaterial :
			public Material
		{
		public:
			typedef std::shared_ptr<BasicMaterial>				Ptr;

		protected:
			typedef std::shared_ptr<render::AbstractTexture>	AbsTexturePtr;
			typedef std::shared_ptr<render::Texture>			TexturePtr;
			typedef std::shared_ptr<render::CubeTexture>		CubeTexturePtr;
			typedef std::shared_ptr<render::States>				RenderStatesPtr;

		public:
			inline static
			Ptr
			create(const std::string& name = "BasicMaterial")
			{
				return Ptr(new BasicMaterial(name));
			}

			inline static
			Ptr
			create(Ptr source)
			{
				auto pm = create(source->name());

				pm->data()->copyFrom(source->data());

				return pm;
			}

			inline static
			Ptr
			create(Material::Ptr source)
			{
				auto pm = create(source->name());

				pm->data()->copyFrom(source->data());

				return pm;
			}

			Ptr
			diffuseColor(const math::vec4&);

			Ptr
			diffuseColor(uint);

			math::vec4
			diffuseColor() const;

			Ptr
			uvScale(const math::vec2& v);

			const math::vec2&
			uvScale() const;

			Ptr
			uvOffset(const math::vec2& v);

			const math::vec2&
			uvOffset() const;

			Ptr
            diffuseMap(TexturePtr);

			render::ResourceId
			diffuseMap() const;

			Ptr
			fogColor(const math::vec4&);

			Ptr
			fogColor(uint);

			math::vec4
			fogColor() const;

            Ptr
            fogStart(float);

            float
            fogStart() const;

            Ptr
            fogEnd(float);

            float
            fogEnd() const;

            Ptr
            fogTechnique(material::FogTechnique);

            material::FogTechnique
            fogTechnique() const;

		protected:
			BasicMaterial(const std::string& name);
		};
	}
}
