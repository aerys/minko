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

#include "minko/data/Provider.hpp"
#include "minko/render/Texture.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
	namespace data
	{
		class LuaProvider
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
				state.Class<Provider>("Provider")
					.method("create", 		static_cast<Provider::Ptr (*)()>(&Provider::create))
					.method("setTexture",   static_cast<Provider::Ptr (Provider::*)(const std::string&, render::Texture::Ptr)>(&Provider::set<render::Texture::Ptr>))
		            .method("setInt",       static_cast<Provider::Ptr (Provider::*)(const std::string&, int)>(&Provider::set<int>))
		            .method("setUint",      static_cast<Provider::Ptr (Provider::*)(const std::string&, unsigned int)>(&Provider::set<unsigned int>))
		            .method("setFloat",     static_cast<Provider::Ptr (Provider::*)(const std::string&, float)>(&Provider::set<float>))
		            .method("setVector2",   static_cast<Provider::Ptr (Provider::*)(const std::string&, math::Vector2::Ptr)>(&Provider::set<math::Vector2::Ptr>))
		            .method("setVector3",   static_cast<Provider::Ptr (Provider::*)(const std::string&, math::Vector3::Ptr)>(&Provider::set<math::Vector3::Ptr>))
		            .method("setVector4",   static_cast<Provider::Ptr (Provider::*)(const std::string&, math::Vector4::Ptr)>(&Provider::set<math::Vector4::Ptr>))
		            .method("setMatrix4x4", static_cast<Provider::Ptr (Provider::*)(const std::string&, math::Matrix4x4::Ptr)>(&Provider::set<math::Matrix4x4::Ptr>))
		            .method("getTexture",   static_cast<render::Texture::Ptr (Provider::*)(const std::string&)>(&Provider::get<render::Texture::Ptr>))
		            .method("getInt",       static_cast<int (Provider::*)(const std::string&)>(&Provider::get<int>))
		            .method("getUint",      static_cast<unsigned int (Provider::*)(const std::string&)>(&Provider::get<unsigned int>))
		            .method("getFloat",     static_cast<float (Provider::*)(const std::string&)>(&Provider::get<float>))
		            .method("getVector2",   static_cast<math::Vector2::Ptr (Provider::*)(const std::string&)>(&Provider::get<math::Vector2::Ptr>))
		            .method("getVector3",   static_cast<math::Vector3::Ptr (Provider::*)(const std::string&)>(&Provider::get<math::Vector3::Ptr>))
		            .method("getVector4",   static_cast<math::Vector4::Ptr (Provider::*)(const std::string&)>(&Provider::get<math::Vector4::Ptr>))
		            .method("getMatrix4x4", static_cast<math::Matrix4x4::Ptr (Provider::*)(const std::string&)>(&Provider::get<math::Matrix4x4::Ptr>));
			}
		};
	}
}
