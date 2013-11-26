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

#include "LuaScriptParser.hpp"

#include "minko/component/LuaScript.hpp"
#include "minko/file/AssetLibrary.hpp"

#include "minko/scene/Node.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/data/Container.hpp"

#include "LuaGlue/LuaGlue.h"

using namespace minko;
using namespace minko::file;

void
LuaScriptParser::parse(const std::string&				    filename,
                       const std::string&                   resolvedFilename,
                       std::shared_ptr<Options>             options,
                       const std::vector<unsigned char>&	data,
                       std::shared_ptr<AssetLibrary>	    assetLibrary)
{
    if (!_initialized)
        initializeLuaBindings();

    if (luaL_loadstring(_state.state(), std::string((char*)&data[0], data.size()).c_str()))
    {
        auto error = lua_tostring(_state.state(), -1);
        
        std::cerr << error << std::endl;
        throw std::runtime_error(error);
    }
    if (lua_pcall(_state.state(), 0, 0, 0))
    {
        auto error = lua_tostring(_state.state(), -1);

        std::cerr << error << std::endl;
        throw std::runtime_error(error);
    }

    auto sepPos = resolvedFilename.find_last_of("/\\");
    auto dotPos = resolvedFilename.find_last_of('.');
    auto start = sepPos == std::string::npos ? 0 : sepPos + 1;
    auto length = dotPos - start;
    auto scriptName = resolvedFilename.substr(start, length);

    auto script = component::LuaScript::create(_state, scriptName);

    assetLibrary->script(filename, script);

    _complete->execute(shared_from_this());
}


void
LuaScriptParser::initializeLuaBindings()
{
    _initialized = true;

    _state
        .Class<math::Vector2>("Vector2")
            .method("setX", static_cast<float (math::Vector2::*)(void)>(&math::Vector2::x))
            .method("setX", static_cast<void (math::Vector2::*)(float)>(&math::Vector2::x))
            .method("setY", static_cast<float (math::Vector2::*)(void)>(&math::Vector2::y))
            .method("setY", static_cast<void (math::Vector2::*)(float)>(&math::Vector2::y))
        .end()
        .Class<math::Vector3>("Vector3")
            .method("setX", static_cast<float (math::Vector3::*)(void)>(&math::Vector3::x))
            .method("setX", static_cast<void (math::Vector3::*)(float)>(&math::Vector3::x))
            .method("setY", static_cast<float (math::Vector3::*)(void)>(&math::Vector3::y))
            .method("setY", static_cast<void (math::Vector3::*)(float)>(&math::Vector3::y))
            .method("setZ", static_cast<float (math::Vector3::*)(void)>(&math::Vector3::z))
            .method("setZ", static_cast<void (math::Vector3::*)(float)>(&math::Vector3::z))
        .end()
        .Class<math::Vector4>("Vector4")
            .method("setX", static_cast<float (math::Vector4::*)(void)>(&math::Vector4::x))
            .method("setX", static_cast<void (math::Vector4::*)(float)>(&math::Vector4::x))
            .method("setY", static_cast<float (math::Vector4::*)(void)>(&math::Vector4::y))
            .method("setY", static_cast<void (math::Vector4::*)(float)>(&math::Vector4::y))
            .method("setZ", static_cast<float (math::Vector4::*)(void)>(&math::Vector4::z))
            .method("setZ", static_cast<void (math::Vector4::*)(float)>(&math::Vector4::z))
            .method("setW", static_cast<float (math::Vector4::*)(void)>(&math::Vector4::w))
            .method("setW", static_cast<void (math::Vector4::*)(float)>(&math::Vector4::w))
        .end()
        .Class<math::Matrix4x4>("Matrix4x4")
            .method("appendRotationX",      &math::Matrix4x4::appendRotationX)
            .method("appendRotationY",      &math::Matrix4x4::appendRotationY)
            .method("appendRotationZ",      &math::Matrix4x4::appendRotationZ)
            .method("appendRotation",       &math::Matrix4x4::appendRotation)
            .method("appendTranslation",    static_cast<math::Matrix4x4::Ptr (math::Matrix4x4::*)(float, float, float)>(&math::Matrix4x4::appendTranslation))
            .method("appendTranslation",    static_cast<math::Matrix4x4::Ptr(math::Matrix4x4::*)(math::Vector3::Ptr)>(&math::Matrix4x4::appendTranslation))
            .method("prependRotationX",     &math::Matrix4x4::prependRotationX)
            .method("prependRotationY",     &math::Matrix4x4::prependRotationY)
            .method("prependRotationZ",     &math::Matrix4x4::prependRotationZ)
            .method("prependRotation",      &math::Matrix4x4::prependRotation)
            .method("prependTranslation",   static_cast<math::Matrix4x4::Ptr(math::Matrix4x4::*)(float, float, float)>(&math::Matrix4x4::prependTranslation))
            .method("prependTranslation",   static_cast<math::Matrix4x4::Ptr(math::Matrix4x4::*)(math::Vector3::Ptr)>(&math::Matrix4x4::prependTranslation))
        .end()
        .Class<data::Container>("Container")
            .method("getFloat",     &data::Container::get<float>)
            .method("getInt",       &data::Container::get<int>)
            .method("getUint",      &data::Container::get<unsigned int>)
            .method("getMatrix4x4", &data::Container::get<math::Matrix4x4::Ptr>)
        .end()
        .Class<scene::Node>("Node")
            .method("getName",          static_cast<const std::string& (scene::Node::*)(void)>(&scene::Node::name))
            .method("setName",          static_cast<void (scene::Node::*)(const std::string&)>(&scene::Node::name))
            .method("getData",          &scene::Node::data)
            .method("addChild",         &scene::Node::addChild)
            .method("removeChild",      &scene::Node::removeChild)
            .method("contains",         &scene::Node::contains)
            .method("addComponent",     &scene::Node::addComponent)
            .method("removeComponent",  &scene::Node::removeComponent)
        .end()
        .open()
        .glue();
}
