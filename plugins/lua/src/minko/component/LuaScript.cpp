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

#include "minko/component/LuaScript.hpp"

#include "LuaGlue/LuaGlue.h"

#include "minko/scene/Node.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/data/Container.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/AbstractCanvas.hpp"

using namespace minko;
using namespace minko::component;

LuaGlue LuaScript::_state;
bool LuaScript::_initialized = false;

LuaScript::LuaScript(const std::string& name) :
    _scriptName(name),
    _hasStartMethod(false),
    _hasUpdateMethod(false),
    _hasStopMethod(false)
{

}

void
LuaScript::initialize()
{
    AbstractScript::initialize();

    if (!_initialized)
        initializeLuaBindings();

    auto name = _scriptName.c_str();

    _state.Class<LuaStub>(name).end().glue();
    _class = _state.lookupClass(name);
}

void
LuaScript::loadScript(const std::string& script)
{
    if(!_state.doString(script))
        printf("err: %s\n", _state.lastError().c_str());

    auto c = dynamic_cast<LuaGlueClass<LuaScript::LuaStub>*>(_class);
    _hasStartMethod = c->hasMethod("start");
    _hasUpdateMethod = c->hasMethod("update");
    _hasStopMethod = c->hasMethod("stop");
}

void
LuaScript::start(scene::Node::Ptr node)
{
    if (_hasStartMethod)
        dynamic_cast<LuaGlueClass<LuaScript::LuaStub>*>(_class)->invokeVoidMethod("start", &_stub, node);
}

void
LuaScript::update(scene::Node::Ptr node)
{
    if (_hasUpdateMethod)
        dynamic_cast<LuaGlueClass<LuaScript::LuaStub>*>(_class)->invokeVoidMethod("update", &_stub, node);
}

void
LuaScript::stop(scene::Node::Ptr node)
{
    if (_hasStopMethod)
        dynamic_cast<LuaGlueClass<LuaScript::LuaStub>*>(_class)->invokeVoidMethod("stop", &_stub, node);
}


void
LuaScript::initializeLuaBindings()
{
    _initialized = true;

    _state
        .Class<math::Vector2>("Vector2")
            .method("getX", static_cast<float (math::Vector2::*)(void)>(&math::Vector2::x))
            .method("setX", static_cast<void (math::Vector2::*)(float)>(&math::Vector2::x))
            .method("getY", static_cast<float (math::Vector2::*)(void)>(&math::Vector2::y))
            .method("setY", static_cast<void (math::Vector2::*)(float)>(&math::Vector2::y))
            .method("toString", &math::Vector2::toString)
        .end()
        .Class<math::Vector3>("Vector3")
            .method("getX", static_cast<float (math::Vector3::*)(void)>(&math::Vector3::x))
            .method("setX", static_cast<void (math::Vector3::*)(float)>(&math::Vector3::x))
            .method("getY", static_cast<float (math::Vector3::*)(void)>(&math::Vector3::y))
            .method("setY", static_cast<void (math::Vector3::*)(float)>(&math::Vector3::y))
            .method("getZ", static_cast<float (math::Vector3::*)(void)>(&math::Vector3::z))
            .method("setZ", static_cast<void (math::Vector3::*)(float)>(&math::Vector3::z))
            .method("toString", &math::Vector3::toString)
        .end()
        .Class<math::Vector4>("Vector4")
            .method("getX", static_cast<float (math::Vector4::*)(void)>(&math::Vector4::x))
            .method("setX", static_cast<void (math::Vector4::*)(float)>(&math::Vector4::x))
            .method("getY", static_cast<float (math::Vector4::*)(void)>(&math::Vector4::y))
            .method("setY", static_cast<void (math::Vector4::*)(float)>(&math::Vector4::y))
            .method("getZ", static_cast<float (math::Vector4::*)(void)>(&math::Vector4::z))
            .method("setZ", static_cast<void (math::Vector4::*)(float)>(&math::Vector4::z))
            .method("getW", static_cast<float (math::Vector4::*)(void)>(&math::Vector4::w))
            .method("setW", static_cast<void (math::Vector4::*)(float)>(&math::Vector4::w))
            .method("toString", &math::Vector4::toString)
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
            //.method("create",       &data::Container::create)
            //.method("hasProperty",  &data::Container::hasProperty)
            .method("getFloat",     &data::Container::get<float>)
            .method("getInt",       &data::Container::get<int>)
            .method("getUint",      &data::Container::get<unsigned int>)
            .method("getMatrix4x4", &data::Container::get<math::Matrix4x4::Ptr>)
        .end()
        .Class<scene::Node>("Node")
            //.method("getName",          static_cast<const std::string& (scene::Node::*)(void)>(&scene::Node::name))
            .method("setName",          static_cast<void (scene::Node::*)(const std::string&)>(&scene::Node::name))
            .method("getData",          &scene::Node::data)
            .method("addChild",         &scene::Node::addChild)
            .method("removeChild",      &scene::Node::removeChild)
            .method("contains",         &scene::Node::contains)
            .method("addComponent",     &scene::Node::addComponent)
            .method("removeComponent",  &scene::Node::removeComponent)
        .end()
        .Class<AbstractCanvas>("AbstractCanvas")
            .method("getMouse", &AbstractCanvas::mouse)
        .end()
        .Class<input::Mouse>("Mouse")
            .method("getX", &input::Mouse::x)
            .method("getY", &input::Mouse::y)
        .end()
        .open()
        .glue();
}
