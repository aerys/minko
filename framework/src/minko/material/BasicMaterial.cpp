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

#include "minko/material/BasicMaterial.hpp"

#include "minko/math/Vector4.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Priority.hpp"

using namespace minko;
using namespace minko::material;
using namespace minko::render;
using namespace minko::math;

			
BasicMaterial::Ptr
BasicMaterial::diffuseColor(math::Vector4::Ptr color)
{
	set("diffuseColor", color);

	return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
}

BasicMaterial::Ptr
BasicMaterial::diffuseColor(uint rgba)
{
	return diffuseColor(
		math::Vector4::create(
			((rgba >> 24) & 0xff) / 255.f,
			((rgba >> 16) & 0xff) / 255.f,
			((rgba >> 8) & 0xff) / 255.f,
			(rgba & 0xff) / 255.f
	));
}

BasicMaterial::Ptr
BasicMaterial::diffuseMap(render::AbstractTexture::Ptr texture)
{
	assert(texture->type() == TextureType::Texture2D);

	set("diffuseMap", texture);

	return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
}

BasicMaterial::Ptr
BasicMaterial::diffuseCubeMap(std::shared_ptr<render::AbstractTexture> texture)
{
	assert(texture->type() == TextureType::CubeTexture);

	set("diffuseCubeMap", texture);

	return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
}

BasicMaterial::Ptr
BasicMaterial::isTransparent(bool value)
{
	set("priority",		value ? priority::TRANSPARENT : priority::OPAQUE);
	set("blendMode",	value ? Blending::Mode::ALPHA : Blending::Mode::DEFAULT);

	return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
}