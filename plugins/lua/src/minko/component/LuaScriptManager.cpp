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
#include "minko/component/LuaScript.hpp"

#include "minko/file/Options.hpp"
#include "minko/file/Loader.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/Texture.hpp"

#include "minko/component/SceneManager.hpp"
#include "minko/component/MouseManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/BoundingBox.hpp"

#include "minko/LuaWrapper.hpp"

#include "minko/math/LuaVector2.hpp"
#include "minko/math/LuaVector3.hpp"
#include "minko/math/LuaVector4.hpp"
#include "minko/math/LuaBox.hpp"
#include "minko/math/LuaMatrix4x4.hpp"
#include "minko/data/LuaProvider.hpp"
#include "minko/data/LuaContainer.hpp"
#include "minko/scene/LuaNode.hpp"
#include "minko/scene/LuaNodeSet.hpp"
#include "minko/geometry/LuaGeometry.hpp"
#include "minko/material/LuaMaterial.hpp"
#include "minko/file/LuaAssetLibrary.hpp"
#include "minko/input/LuaKeyboard.hpp"
#include "minko/input/LuaMouse.hpp"
#include "minko/LuaAbstractCanvas.hpp"
#include "minko/component/LuaPerspectiveCamera.hpp"
#include "minko/component/LuaTransform.hpp"

using namespace minko;
using namespace minko::component;

std::shared_ptr<scene::Node> LuaScriptManager::LuaGlobalStub::_root = nullptr;
std::shared_ptr<AbstractCanvas> LuaScriptManager::LuaGlobalStub::_canvas = nullptr;

AbstractCanvas::Ptr
LuaScriptManager::LuaGlobalStub::getCanvas()
{
    return _canvas;
}

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
	return _canvas->keyboard();
}

void
LuaScriptManager::initialize()
{
    AbstractScript::initialize();

	initializeBindings();
}

void
LuaScriptManager::targetAddedHandler(AbstractComponent::Ptr cmp, scene::Node::Ptr target)
{
    AbstractScript::targetAddedHandler(cmp, target);

    if (targets().size() > 1)
        throw;

    LuaGlobalStub::_root = target->root();
    LuaGlobalStub::_canvas = _canvas;

    loadStandardLibrary();
}

void
LuaScriptManager::loadStandardLibrary()
{
    auto assets = targets()[0]->root()->component<SceneManager>()->assets();
    auto options = assets->defaultOptions();
    auto createLoader = assets->defaultOptions()->loaderFunction();
    auto filesToLoad = {
        "script/minko.coroutine.lua",
        "script/minko.time.lua"
    };

    _numDependencies = filesToLoad.size();

    for (auto& filename : filesToLoad)
    {
        auto loader = createLoader(filename);

        _dependencySlots.push_back(loader->complete()->connect(std::bind(
            &LuaScriptManager::dependencyLoadedHandler,
            std::dynamic_pointer_cast<LuaScriptManager>(shared_from_this()),
            std::placeholders::_1
        )));
        loader->load(filename, options);
    }
}

void
LuaScriptManager::dependencyLoadedHandler(AbsLoaderPtr loader)
{
    auto& data = loader->data();

    _state.doString(std::string((char*)&data[0], data.size()));

    ++_numLoadedDependencies;
}

void
LuaScriptManager::update(scene::Node::Ptr target)
{
    time_point t = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> deltaT = t - _previousTime;

    _state.invokeVoidFunction("wakeUpWaitingThreads", deltaT.count() * 1000.f);
    _previousTime = t;
}

void
LuaScriptManager::initializeBindings()
{
	_state
        .Class<render::Texture>("Texture")
            //.property("width",  &render::Texture::width)
            //.property("height", &render::Texture::height)
        .end()
        .Class<render::Effect>("Effect")
        .end()
        .Class<Surface>("Surface")
            .method("create", static_cast<Surface::Ptr (*)(geometry::Geometry::Ptr, data::Provider::Ptr, render::Effect::Ptr)>(&Surface::create))
        .end()
        .Class<render::AbstractContext>("AbstractContext")
        .end()
        .Class<BoundingBox>("BoundingBox")
            .property("box",    &BoundingBox::box)
        .end()
        .Class<Renderer>("Renderer")
            .method("create",               static_cast<Renderer::Ptr (*)(void)>(&Renderer::create))
            .property("backgroundColor",    &Renderer::backgroundColor, &Renderer::backgroundColor)
        .end()
        .Class<AmbientLight>("AmbientLight")
            .method("create",   &AmbientLight::create)
            .property("color",  static_cast<math::Vector3::Ptr (AmbientLight::*)()>(&AbstractLight::color))
        .end()
        .Class<DirectionalLight>("DirectionalLight")
            .method("create",   &DirectionalLight::create)
            .property("color",  static_cast<math::Vector3::Ptr (DirectionalLight::*)()>(&AbstractLight::color))
        .end()
        .Class<SpotLight>("SpotLight")
            .method("create",   &SpotLight::create)
            .property("color",  static_cast<math::Vector3::Ptr (SpotLight::*)()>(&AbstractLight::color))
        .end()
        .Class<PointLight>("PointLight")
            .method("create",   &PointLight::create)
            .property("color",  static_cast<math::Vector3::Ptr (PointLight::*)()>(&AbstractLight::color))
        .end();

    math::LuaMatrix4x4::bind(_state);
    math::LuaVector2::bind(_state);
    math::LuaVector3::bind(_state);
    math::LuaVector4::bind(_state);
    math::LuaBox::bind(_state);
    data::LuaProvider::bind(_state);
    data::LuaContainer::bind(_state);
    geometry::LuaGeometry::bind(_state);
    material::LuaMaterial::bind(_state);
    file::LuaAssetLibrary::bind(_state);
    input::LuaMouse::bind(_state);
    input::LuaKeyboard::bind(_state);
    LuaAbstractCanvas::bind(_state);
    component::LuaPerspectiveCamera::bind(_state);
    component::LuaTransform::bind(_state);
    scene::LuaNode::bind(_state);
    scene::LuaNodeSet::bind(_state);

    auto& sceneManager = _state.Class<SceneManager>("SceneManager")
        .property("assets",     &SceneManager::assets);
    MINKO_LUAGLUE_BIND_SIGNAL(_state, SceneManager::Ptr);
    sceneManager.property("nextFrame",  &SceneManager::frameBegin);

    _state
        .func("getCanvas",              &LuaGlobalStub::getCanvas)
        .func("getMouse",               &LuaGlobalStub::getMouse)
        .func("getKeyboard",            &LuaGlobalStub::getKeyboard)
        .func("getSceneManager",        &LuaGlobalStub::getSceneManager);

    _state.open().glue();
}
