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

#include "minko/file/AssetLibrary.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/Texture.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/component/AbstractScript.hpp"
#include "minko/scene/Node.hpp"

#include "LuaGlue/LuaGlue.h"

namespace minko
{
	namespace file
	{
		class LuaAssetLibrary
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
				auto& assetLibrary = state.Class<file::AssetLibrary>("AssetLibrary")
			        .method("queue",        static_cast<file::AssetLibrary::Ptr (file::AssetLibrary::*)(const std::string&)>(&file::AssetLibrary::queue))
			        .method("load",         static_cast<file::AssetLibrary::Ptr (file::AssetLibrary::*)(bool)>(&file::AssetLibrary::load))
			        .method("geometry",     static_cast<geometry::Geometry::Ptr (file::AssetLibrary::*)(const std::string&)>(&file::AssetLibrary::geometry))
			        .method("effect",       static_cast<render::Effect::Ptr (file::AssetLibrary::*)(const std::string&)>(&file::AssetLibrary::effect))
			        .method("texture",      static_cast<render::Texture::Ptr (file::AssetLibrary::*)(const std::string&)>(&file::AssetLibrary::texture))
			        .method("script",       static_cast<component::AbstractScript::Ptr (file::AssetLibrary::*)(const std::string&)>(&file::AssetLibrary::script))
					.method("symbol",		static_cast<scene::Node::Ptr (file::AssetLibrary::*)(const std::string&)>(&file::AssetLibrary::symbol))
					.property("context",    &file::AssetLibrary::context);

			    MINKO_LUAGLUE_BIND_SIGNAL(state, file::AssetLibrary::Ptr);

			    assetLibrary.property("complete", &file::AssetLibrary::complete);
			}
		};
	}
}
