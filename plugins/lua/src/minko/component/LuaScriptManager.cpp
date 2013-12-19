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

#include "minko/component/LuaScriptManager.hpp"

#include "minko/scene/Node.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/data/Container.hpp"
#include "minko/data/Provider.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/input/Keyboard.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/Texture.hpp"
#include "minko/material/Material.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/geometry/CubeGeometry.hpp"
#include "minko/geometry/SphereGeometry.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/MouseManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/LuaScript.hpp"

#define BIND_SIGNAL(...) \
    {                                                                                               \
        _state->Class<Signal<__VA_ARGS__>>("Signal<" #__VA_ARGS__ ">")                              \
            .method("connect", &LuaScriptManager::wrapSignalConnect<__VA_ARGS__>);                  \
        _state->Class<Signal<__VA_ARGS__>::Slot::element_type>("SignalSlot<" #__VA_ARGS__ ">")      \
            .method("disconnect", &Signal<__VA_ARGS__>::Slot::element_type::disconnect);            \
    }

using namespace minko;
using namespace minko::component;

std::shared_ptr<scene::Node> LuaScriptManager::LuaGlobalStub::_root = nullptr;

SceneManager::Ptr
LuaScriptManager::LuaGlobalStub::getSceneManager()
{
	return _root->component<SceneManager>();
}

input::Mouse::Ptr
LuaScriptManager::LuaGlobalStub::getMouse()
{
	return _root->component<MouseManager>()->mouse();
}

input::Keyboard::Ptr
LuaScriptManager::LuaGlobalStub::getKeyboard()
{
	return nullptr;
}

math::Matrix4x4::Ptr
LuaScriptManager::LuaGlobalStub::getModelToWorldMatrix(std::shared_ptr<scene::Node> n)
{
    return n->data()->get<math::Matrix4x4::Ptr>("transform.modelToWorldMatrix");
}

void
LuaScriptManager::initialize()
{
    _targetAddedSlot = targetAdded()->connect(std::bind(
        &LuaScriptManager::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
    ));

	initializeBindings();
}

void
LuaScriptManager::targetAddedHandler(AbstractComponent::Ptr cmp, scene::Node::Ptr target)
{
    if (targets().size() > 1)
        throw;

    LuaGlobalStub::_root = target->root();
}

void
LuaScriptManager::initializeBindings()
{
    _state = new LuaGlue();
    _state->open();

	_state
        ->Class<render::Texture>("Texture")
            //.property("width",  &render::Texture::width)
            //.property("height", &render::Texture::height)
        .end()
        .Class<math::Vector2>("Vector2")
            .method("create",   static_cast<math::Vector2::Ptr (*)(float, float)>(&math::Vector2::create))
            .method("toString", &math::Vector2::toString)
            .property("x",      static_cast<float (math::Vector2::*)(void)>(&math::Vector2::x), static_cast<void (math::Vector2::*)(float)>(&math::Vector2::x))
            .property("y",      static_cast<float (math::Vector2::*)(void)>(&math::Vector2::y), static_cast<void (math::Vector2::*)(float)>(&math::Vector2::y))
        .end()
        .Class<math::Vector3>("Vector3")
            .method("create",   static_cast<math::Vector3::Ptr (*)(float, float, float)>(&math::Vector3::create))
            .method("toString", &math::Vector3::toString)
            .method("zero",     &math::Vector3::zero)
            .method("one",      &math::Vector3::one)
            .method("up",       &math::Vector3::up)
            .method("forward",  &math::Vector3::forward)
            .method("xAxis",    &math::Vector3::xAxis)
            .method("yAxis",    &math::Vector3::yAxis)
            .method("zAxis",    &math::Vector3::zAxis)
            .property("x",      static_cast<float (math::Vector3::*)(void)>(&math::Vector3::x), static_cast<void (math::Vector3::*)(float)>(&math::Vector3::x))
            .property("y",      static_cast<float (math::Vector3::*)(void)>(&math::Vector3::y), static_cast<void (math::Vector3::*)(float)>(&math::Vector3::y))
            .property("z",      static_cast<float (math::Vector3::*)(void)>(&math::Vector3::z), static_cast<void (math::Vector3::*)(float)>(&math::Vector3::z))
        .end()
        .Class<math::Vector4>("Vector4")
            .method("create",   static_cast<math::Vector4::Ptr (*)(float, float, float, float)>(&math::Vector4::create))
            .method("toString", &math::Vector4::toString)
            .property("x",      static_cast<float (math::Vector4::*)(void)>(&math::Vector4::x), static_cast<void (math::Vector4::*)(float)>(&math::Vector4::x))
            .property("y",      static_cast<float (math::Vector4::*)(void)>(&math::Vector4::y), static_cast<void (math::Vector4::*)(float)>(&math::Vector4::y))
            .property("z",      static_cast<float (math::Vector4::*)(void)>(&math::Vector4::z), static_cast<void (math::Vector4::*)(float)>(&math::Vector4::z))
            .property("w",      static_cast<float (math::Vector4::*)(void)>(&math::Vector4::w), static_cast<void (math::Vector4::*)(float)>(&math::Vector4::w))
        .end()
        .Class<math::Matrix4x4>("Matrix4x4")
            .method("create",               static_cast<math::Matrix4x4::Ptr (*)()>(&math::Matrix4x4::create))
            .method("lookAt",               &math::Matrix4x4::lookAt)
            .method("identity",             &math::Matrix4x4::identity)
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
            //.method("setName",          static_cast<void (scene::Node::*)(const std::string&)>(&scene::Node::name))
            //.prop("name", &scene::Node::name, &scene::Node::name)
            .method("create",           static_cast<scene::Node::Ptr (*)(void)>(&scene::Node::create))
            .method("addChild",         &scene::Node::addChild)
            .method("removeChild",      &scene::Node::removeChild)
            .method("contains",         &scene::Node::contains)
            .method("addComponent",     &scene::Node::addComponent)
            .method("removeComponent",  &scene::Node::removeComponent)
            .property("data",           &scene::Node::data)
            .property("root",           &scene::Node::root)
        .end()
        .Class<geometry::Geometry>("Geometry")
        .end()
        .Class<geometry::CubeGeometry>("CubeGeometry")
            .method("create", &geometry::CubeGeometry::create)
        .end()
        .Class<geometry::SphereGeometry>("SphereGeometry")
            .method("create", &geometry::SphereGeometry::create)
        .end()
        .Class<material::Material>("Material")
            .method("create",       static_cast<material::Material::Ptr (*)(void)>(&material::Material::create))
            .method("setTexture",   static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, render::Texture::Ptr)>(&data::Provider::set<render::Texture::Ptr>))
            .method("setInt",       static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, int)>(&data::Provider::set<int>))
            .method("setUint",      static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, unsigned int)>(&data::Provider::set<unsigned int>))
            .method("setFloat",     static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, float)>(&data::Provider::set<float>))
            .method("setVector2",   static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, math::Vector2::Ptr)>(&data::Provider::set<math::Vector2::Ptr>))
            .method("setVector3",   static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, math::Vector3::Ptr)>(&data::Provider::set<math::Vector3::Ptr>))
            .method("setVector4",   static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, math::Vector4::Ptr)>(&data::Provider::set<math::Vector4::Ptr>))
            .method("setMatrix4x4", static_cast<data::Provider::Ptr (material::Material::*)(const std::string&, math::Matrix4x4::Ptr)>(&data::Provider::set<math::Matrix4x4::Ptr>))
            .method("getTexture",   static_cast<render::Texture::Ptr (material::Material::*)(const std::string&)>(&material::Material::get<render::Texture::Ptr>))
            .method("getInt",       static_cast<int (material::Material::*)(const std::string&)>(&data::Provider::get<int>))
            .method("getUint",      static_cast<unsigned int (material::Material::*)(const std::string&)>(&data::Provider::get<unsigned int>))
            .method("getFloat",     static_cast<float (material::Material::*)(const std::string&)>(&data::Provider::get<float>))
            .method("getVector2",   static_cast<math::Vector2::Ptr (material::Material::*)(const std::string&)>(&data::Provider::get<math::Vector2::Ptr>))
            .method("getVector3",   static_cast<math::Vector3::Ptr (material::Material::*)(const std::string&)>(&data::Provider::get<math::Vector3::Ptr>))
            .method("getVector4",   static_cast<math::Vector4::Ptr (material::Material::*)(const std::string&)>(&data::Provider::get<math::Vector4::Ptr>))
            .method("getMatrix4x4", static_cast<math::Matrix4x4::Ptr (material::Material::*)(const std::string&)>(&data::Provider::get<math::Matrix4x4::Ptr>))
        .end()
        .Class<render::Effect>("Effect")
        .end()
        .Class<Surface>("Surface")
            .method("create", static_cast<Surface::Ptr (*)(geometry::Geometry::Ptr, data::Provider::Ptr, render::Effect::Ptr)>(&Surface::create))
        .end()
        .Class<render::AbstractContext>("AbstractContext")
        .end()
        .Class<SceneManager>("SceneManager")
            .property("assets",     &SceneManager::assets)
        .end()
        .Class<Transform>("Transform")
            .method("create",           static_cast<Transform::Ptr (*)(void)>(&Transform::create))
            .method("createFromMatrix", static_cast<Transform::Ptr (*)(math::Matrix4x4::Ptr)>(&Transform::create))
        .end()
        .Class<PerspectiveCamera>("PerspectiveCamera")
            .method("create",           &PerspectiveCamera::create)
            .method("updateProjection", &PerspectiveCamera::updateProjection)
        .end()
        .Class<Renderer>("Renderer")
            .method("create",               static_cast<Renderer::Ptr (*)(void)>(&Renderer::create))
            .property("backgroundColor",    &Renderer::backgroundColor, &Renderer::backgroundColor)
        .end()
        .Class<AmbientLight>("AmbientLight")
            .method("create", &AmbientLight::create)
        .end()
        .Class<DirectionalLight>("DirectionalLight")
            .method("create", &DirectionalLight::create)
        .end()
        .Class<AmbientLight>("SpotLight")
            .method("create", &SpotLight::create)
        .end()
        .Class<AmbientLight>("PointLight")
            .method("create", &PointLight::create)
        .end();

    auto& input_mouse = _state->Class<input::Mouse>("Mouse")
        .property("x",                  &input::Mouse::x)
        .property("y",                  &input::Mouse::y)
        .property("leftButtonIsDown",   &input::Mouse::leftButtonIsDown)
        .property("rightButtonIsDown",  &input::Mouse::rightButtonIsDown);
    BIND_SIGNAL(input::Mouse::Ptr);
    input_mouse.property("leftButtonDown", &input::Mouse::leftButtonDown);

    auto& file_assetlibrary = _state->Class<file::AssetLibrary>("AssetLibrary")
        .method("queue",        static_cast<file::AssetLibrary::Ptr (file::AssetLibrary::*)(const std::string&)>(&file::AssetLibrary::queue))
        .method("load",         static_cast<file::AssetLibrary::Ptr (file::AssetLibrary::*)(void)>(&file::AssetLibrary::load))
        .method("geometry",     static_cast<geometry::Geometry::Ptr (file::AssetLibrary::*)(const std::string&)>(&file::AssetLibrary::geometry))
        .method("effect",       static_cast<render::Effect::Ptr (file::AssetLibrary::*)(const std::string&)>(&file::AssetLibrary::effect))
        .method("texture",      static_cast<render::Texture::Ptr (file::AssetLibrary::*)(const std::string&)>(&file::AssetLibrary::texture))
        .method("script",       static_cast<AbstractScript::Ptr (file::AssetLibrary::*)(const std::string&)>(&file::AssetLibrary::script))
        .property("context",    &file::AssetLibrary::context);
    BIND_SIGNAL(file::AssetLibrary::Ptr);
    file_assetlibrary.property("complete", &file::AssetLibrary::complete);

    _state
        ->func("getMouse",              &LuaGlobalStub::getMouse)
        .func("getKeyboard",            &LuaGlobalStub::getKeyboard)
        .func("getSceneManager",        &LuaGlobalStub::getSceneManager)
        .func("getModelToWorldMatrix",  &LuaGlobalStub::getModelToWorldMatrix);

    _state->glue();
}
