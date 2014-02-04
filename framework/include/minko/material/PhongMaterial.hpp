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

#include "minko/material/Material.hpp"

namespace minko
{
	namespace material
	{
		class PhongMaterial:
			public Material
		{
		public:
			typedef std::shared_ptr<PhongMaterial>				Ptr;

		private:
			typedef std::shared_ptr<math::Vector4>				Vector4Ptr;
			typedef std::shared_ptr<render::AbstractTexture>	AbsTexturePtr;
			typedef std::shared_ptr<render::Texture>			TexturePtr;
			typedef std::shared_ptr<render::CubeTexture>		CubeTexturePtr;

		public:
			inline static
			Ptr
			create()
			{
				Ptr ptr = std::shared_ptr<PhongMaterial>(new PhongMaterial());

				ptr->initialize();

				return ptr;
			}

			Ptr
			diffuseColor(Vector4Ptr);

			Ptr
			diffuseColor(uint);

			Vector4Ptr
			diffuseColor(Vector4Ptr out = nullptr) const;

			Ptr
			specularColor(Vector4Ptr);

			Ptr
			specularColor(uint);

			Vector4Ptr
			specularColor(Vector4Ptr out = nullptr) const;

			Ptr
			shininess(float);

			float
			shininess() const;

			Ptr
			normalMap(AbsTexturePtr);

			TexturePtr
			normalMap() const;

			Ptr
			specularMap(AbsTexturePtr);

			TexturePtr
			specularMap() const;

			Ptr
			environmentMap(AbsTexturePtr, render::EnvironmentMap2dType type = render::EnvironmentMap2dType::Unset);

			CubeTexturePtr
			environmentCubemap() const;

			TexturePtr
			environmentMap2d() const;

			render::EnvironmentMap2dType
			environmentMap2dType() const;

			Ptr
			reflectivity(float);

			float
			reflectivity() const;

		private:
			PhongMaterial();

			void
			initialize();
		};
	}
}