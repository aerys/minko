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
#include "minko/file/FileLoader.hpp"
#include "minko/render/AbstractContext.hpp"
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

#include "minko/LuaContext.hpp"
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
#include "minko/render/LuaEffect.hpp"
#include "minko/file/LuaAssetLibrary.hpp"
#include "minko/input/LuaKeyboard.hpp"
#include "minko/input/LuaMouse.hpp"
#include "minko/input/LuaJoystick.hpp"
#include "minko/LuaAbstractCanvas.hpp"
#include "minko/component/LuaPerspectiveCamera.hpp"
#include "minko/component/LuaOrthographicCamera.hpp"
#include "minko/component/LuaTransform.hpp"
#include "minko/component/LuaAnimation.hpp"

using namespace minko;
using namespace minko::component;

void
LuaScriptManager::initialize()
{
    AbstractScript::initialize();

	initializeBindings();

	_state
		.func("getCanvas", &LuaContext::getCanvas)
		.func("getSceneManager", &LuaContext::getSceneManager)
		.func("getOption", &LuaContext::getOption);

	_state.open().glue();
}

void
LuaScriptManager::initialize(std::vector<std::function<void(LuaGlue&)>> bindingsFunctions)
{
	for (auto f : bindingsFunctions)
		f(_state);

	initialize();
}

void
LuaScriptManager::targetAddedHandler(AbstractComponent::Ptr cmp, scene::Node::Ptr target)
{
    AbstractScript::targetAddedHandler(cmp, target);

    if (targets().size() > 1)
        throw;

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
		auto loader = createLoader(filename, assets);

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
	_state.Class<render::Texture>("Texture");
	_state.Class<Surface>("Surface")
		.method("create", static_cast<Surface::Ptr(*)(geometry::Geometry::Ptr, data::Provider::Ptr, render::Effect::Ptr)>(&Surface::create));
	_state.Class<render::AbstractContext>("AbstractContext");
	_state.Class<BoundingBox>("BoundingBox")
		.property("box", &BoundingBox::box);
	_state.Class<Renderer>("Renderer")
		.method("create", static_cast<Renderer::Ptr(*)(void)>(&Renderer::create))
		.property("backgroundColor", &Renderer::backgroundColor, &Renderer::backgroundColor);
	_state.Class<AmbientLight>("AmbientLight")
		.method("create", &AmbientLight::create)
		.property("color", static_cast<math::Vector3::Ptr(AmbientLight::*)()>(&AbstractLight::color));
	_state.Class<DirectionalLight>("DirectionalLight")
		.method("create", &DirectionalLight::create)
		.property("color", static_cast<math::Vector3::Ptr(DirectionalLight::*)()>(&AbstractLight::color));
	_state.Class<SpotLight>("SpotLight")
		.method("create", &SpotLight::create)
		.property("color", static_cast<math::Vector3::Ptr(SpotLight::*)()>(&AbstractLight::color));
	_state.Class<PointLight>("PointLight")
		.method("create", &PointLight::create)
		.property("color", static_cast<math::Vector3::Ptr(PointLight::*)()>(&AbstractLight::color));

    math::LuaMatrix4x4::bind(_state);
    math::LuaVector2::bind(_state);
    math::LuaVector3::bind(_state);
    math::LuaVector4::bind(_state);
    math::LuaBox::bind(_state);
    data::LuaProvider::bind(_state);
    data::LuaContainer::bind(_state);
    geometry::LuaGeometry::bind(_state);
    material::LuaMaterial::bind(_state);
    render::LuaEffect::bind(_state);
    file::LuaAssetLibrary::bind(_state);
    input::LuaMouse::bind(_state);
    input::LuaKeyboard::bind(_state);
	input::LuaJoystick::bind(_state);
    LuaAbstractCanvas::bind(_state);
	component::LuaPerspectiveCamera::bind(_state);
	component::LuaOrthographicCamera::bind(_state);
	component::LuaTransform::bind(_state);
    scene::LuaNode::bind(_state);
    scene::LuaNodeSet::bind(_state);
	component::LuaAnimation::bind(_state);

    auto& sceneManager = _state.Class<SceneManager>("SceneManager")
        .property("assets",     &SceneManager::assets);
    MINKO_LUAGLUE_BIND_SIGNAL(_state, SceneManager::Ptr, float, float);
    sceneManager.property("nextFrame",  &SceneManager::frameBegin);
}
