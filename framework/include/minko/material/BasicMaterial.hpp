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
#include "minko/render/TriangleCulling.hpp"
#include "minko/render/Blending.hpp"

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

			Ptr
			diffuseColor(std::shared_ptr<math::Vector4>);

			Ptr
			diffuseColor(uint);

			Ptr
			diffuseMap(std::shared_ptr<render::AbstractTexture>);

			Ptr
			diffuseCubeMap(std::shared_ptr<render::AbstractTexture>);

			Ptr
			isTransparent(bool, bool zsort = false);

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
