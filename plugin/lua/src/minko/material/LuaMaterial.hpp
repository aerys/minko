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
#include "minko/material/BasicMaterial.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/TriangleCulling.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
	namespace material
	{
		class LuaMaterial
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
				state.Class<material::Material>("Material")
					.method("create", static_cast<material::Material::Ptr(*)(void)>(&material::Material::create))
		            .method("setTexture",   static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, render::Texture::Ptr)>(&data::Provider::set<render::Texture::Ptr>))
		            .method("setInt",       static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, int)>(&data::Provider::set<int>))
		            .method("setUint",      static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, unsigned int)>(&data::Provider::set<unsigned int>))
		            .method("setFloat",     static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, float)>(&data::Provider::set<float>))
		            .method("setVector2",   static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, math::Vector2::Ptr)>(&data::Provider::set<math::Vector2::Ptr>))
		            .method("setVector3",   static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, math::Vector3::Ptr)>(&data::Provider::set<math::Vector3::Ptr>))
		            .method("setVector4",   static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, math::Vector4::Ptr)>(&data::Provider::set<math::Vector4::Ptr>))
		            .method("setMatrix4x4", static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, math::Matrix4x4::Ptr)>(&data::Provider::set<math::Matrix4x4::Ptr>))
                    .method("getTexture",   static_cast<render::Texture::Ptr(material::Material::*)(const std::string&) const>(&material::Material::get<render::Texture::Ptr>))
                    .method("getInt",       static_cast<int (material::Material::*)(const std::string&) const>(&data::Provider::get<int>))
                    .method("getUint",      static_cast<unsigned int (material::Material::*)(const std::string&) const>(&data::Provider::get<unsigned int>))
                    .method("getFloat",     static_cast<float (material::Material::*)(const std::string&) const>(&data::Provider::get<float>))
                    .method("getVector2",   static_cast<math::Vector2::Ptr(material::Material::*)(const std::string&) const>(&data::Provider::get<math::Vector2::Ptr>))
                    .method("getVector3",   static_cast<math::Vector3::Ptr(material::Material::*)(const std::string&) const>(&data::Provider::get<math::Vector3::Ptr>))
                    .method("getVector4",   static_cast<math::Vector4::Ptr(material::Material::*)(const std::string&) const>(&data::Provider::get<math::Vector4::Ptr>))
                    .method("getMatrix4x4", static_cast<math::Matrix4x4::Ptr(material::Material::*)(const std::string&) const>(&data::Provider::get<math::Matrix4x4::Ptr>));


				auto& triangleCulling = state.Enum<render::TriangleCulling>("TriangleCulling");

				triangleCulling
					.constant("BACK",	static_cast<int>(render::TriangleCulling::BACK))
					.constant("FRONT",	static_cast<int>(render::TriangleCulling::FRONT))
					.constant("NONE",	static_cast<int>(render::TriangleCulling::NONE))
					.constant("BOTH",	static_cast<int>(render::TriangleCulling::BOTH));
			}
		};
	}
}
