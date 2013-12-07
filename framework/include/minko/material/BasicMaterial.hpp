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

#pragma once

#include "minko/Common.hpp"

#include "minko/material/Material.hpp"

namespace minko
{
	namespace material
	{
		class BasicMaterial :
			public Material
		{
		public:
			typedef std::shared_ptr<BasicMaterial>	Ptr;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<BasicMaterial>(new BasicMaterial());
			}

			inline
			Ptr
			diffuseColor(std::shared_ptr<math::Vector4> color)
			{
				set("diffuseColor", color);

				return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
			}

			inline
			Ptr
			diffuseColor(const uint rgba)
			{
				return diffuseColor(math::Vector4::create(
					((rgba >> 24) & 0xff) / 255.f,
					((rgba >> 16) & 0xff) / 255.f,
					((rgba >> 8) & 0xff) / 255.f,
					(rgba & 0xff) / 255.f
				));
			}

			inline
			Ptr
			diffuseMap(std::shared_ptr<render::Texture> diffuseMap)
			{
				set("diffuseMap", diffuseMap);

				return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
			}

			inline
			Ptr
			blendMode(render::Blending::Mode blendMode)
			{
				set("blendMode", blendMode);

				return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
			}

			inline
			Ptr
			triangleCulling(render::TriangleCulling culling)
			{
				set("triangleCulling", culling);

				return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
			}
		};
	}
}
