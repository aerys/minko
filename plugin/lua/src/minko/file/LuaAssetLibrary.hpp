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

#include "minko/file/AssetLibrary.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/component/AbstractScript.hpp"
#include "minko/scene/Node.hpp"

#include "LuaGlue/LuaGlue.h"

namespace minko
{
    namespace file
    {
        class LuaAssetLibrary : public LuaWrapper
        {
        public:
            static
            void
            bind(LuaGlue& state)
            {
                auto& assetLibrary = state.Class<AssetLibrary>("AssetLibrary")
                    .method("geometry",     static_cast<geometry::Geometry::Ptr (AssetLibrary::*)(const std::string&)>(&AssetLibrary::geometry))
                    .method("effect",       static_cast<render::Effect::Ptr (AssetLibrary::*)(const std::string&)>(&AssetLibrary::effect))
                    .method("texture",        static_cast<render::Texture::Ptr (AssetLibrary::*)(const std::string&) const>(&AssetLibrary::texture))
                    .method("cubeTexture",    static_cast<render::CubeTexture::Ptr (AssetLibrary::*)(const std::string&) const>(&AssetLibrary::cubeTexture))
                    .method("script",       static_cast<component::AbstractScript::Ptr (AssetLibrary::*)(const std::string&)>(&AssetLibrary::script))
                    .method("symbol",        static_cast<scene::Node::Ptr (AssetLibrary::*)(const std::string&)>(&AssetLibrary::symbol))
                    .property("context",    &AssetLibrary::context)
                    .property("loader",     &AssetLibrary::loader);

                //MINKO_LUAGLUE_BIND_SIGNAL(state, AssetLibrary::Ptr);

                //assetLibrary.property("complete", &AssetLibrary::complete);
            }
        };
    }
}
