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

#include "minko/component/OculusVRCamera.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/data/StructureProvider.hpp"
#include "minko/render/Texture.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/render/Effect.hpp"
#include "minko/material/Material.hpp"
#include "minko/file/Loader.hpp"

#include "minko/oculus/NativeOculus.hpp"
#include "minko/oculus/WebVROculus.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;
using namespace minko::oculus;
using namespace minko::render;

OculusVRCamera::OculusVRCamera(int viewportWidth, int viewportHeight, float zNear, float zFar) :
    _sceneManager(nullptr),
    _targetAddedSlot(nullptr),
    _targetRemovedSlot(nullptr),
    _addedSlot(nullptr),
    _removedSlot(nullptr),
    _oculusImpl(nullptr)
{
}

OculusVRCamera::~OculusVRCamera()
{
    _oculusImpl->destroy();
}

void
OculusVRCamera::updateViewport(int viewportWidth, int viewportHeight)
{
    _oculusImpl->updateViewport(viewportWidth, viewportHeight);
}

bool
OculusVRCamera::detected()
{
#ifdef EMSCRIPTEN
    return WebVROculus::detected();
#else
    return NativeOculus::detected();
#endif
}

void
OculusVRCamera::initialize(int viewportWidth, int viewportHeight, float zNear, float zFar, void* window)
{
#ifdef EMSCRIPTEN
    _oculusImpl = WebVROculus::create(viewportWidth, viewportHeight, zNear, zFar);
#else
    _oculusImpl = NativeOculus::create(viewportWidth, viewportHeight, zNear, zFar);
#endif

    if (!detected())
        return;

    updateViewport(viewportWidth, viewportHeight);

    _targetAddedSlot = targetAdded()->connect(std::bind(
        &OculusVRCamera::targetAddedHandler,
        std::static_pointer_cast<OculusVRCamera>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));

    _targetRemovedSlot = targetRemoved()->connect(std::bind(
        &OculusVRCamera::targetRemovedHandler,
        std::static_pointer_cast<OculusVRCamera>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));

    initializeOVRDevice(window);
}

void
OculusVRCamera::targetAddedHandler(AbsCmpPtr component, NodePtr target)
{
    if (targets().size() > 1)
        throw std::logic_error("The OculusVRCamera component cannot have more than 1 target.");

    _addedSlot = target->added()->connect(std::bind(
        &OculusVRCamera::findSceneManager,
        std::static_pointer_cast<OculusVRCamera>(shared_from_this())
    ));

    _removedSlot = target->removed()->connect(std::bind(
        &OculusVRCamera::findSceneManager,
        std::static_pointer_cast<OculusVRCamera>(shared_from_this())
    ));

    _oculusImpl->initializeCameras(target);

    findSceneManager();
}

void
OculusVRCamera::targetRemovedHandler(AbsCmpPtr component, NodePtr target)
{
    findSceneManager();
}

void
OculusVRCamera::initializeOVRDevice(void* window)
{
    _oculusImpl->initializeOVRDevice(window);
}

void
OculusVRCamera::findSceneManager()
{
    NodeSet::Ptr roots = NodeSet::create(targets())
        ->roots()
        ->where([](NodePtr node)
        {
            return node->hasComponent<SceneManager>();
        });

    if (roots->nodes().size() > 1)
        throw std::logic_error("OculusVRCamera cannot be in two separate scenes.");
    else if (roots->nodes().size() == 1)
        setSceneManager(roots->nodes()[0]->component<SceneManager>());
    else
        setSceneManager(nullptr);
}

void
OculusVRCamera::setSceneManager(SceneManager::Ptr sceneManager)
{
    if (_sceneManager == sceneManager)
        return;

    _sceneManager = sceneManager;

    auto context = sceneManager->assets()->context();

    _renderBeginSlot = sceneManager->renderingBegin()->connect(std::bind(
        &OculusVRCamera::updateCameraOrientation,
        std::static_pointer_cast<OculusVRCamera>(shared_from_this())
    ), 1000.f);

    _oculusImpl->initialize(_sceneManager);
}

void
OculusVRCamera::updateCameraOrientation()
{
    _oculusImpl->updateCameraOrientation(getTarget(0));
}