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

#include "minko/component/VRCamera.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/Effect.hpp"

#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
#include "minko/oculus/Cardboard.hpp"
#else
	#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
	#include "minko/oculus/WebVROculus.hpp"
	#else
	#include "minko/oculus/NativeOculus.hpp"
	#endif
#endif

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;
using namespace minko::oculus;
using namespace minko::render;

VRCamera::VRCamera(int viewportWidth, int viewportHeight, float zNear, float zFar) :
    _sceneManager(nullptr),
    _addedSlot(nullptr),
    _removedSlot(nullptr),
    _VRImpl(nullptr)
{
}

VRCamera::~VRCamera()
{
    _VRImpl = nullptr;
}

void
VRCamera::updateViewport(int viewportWidth, int viewportHeight)
{
    _VRImpl->updateViewport(viewportWidth, viewportHeight);
}

bool
VRCamera::detected()
{
#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    return WebVROculus::detected();
#else
	#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
		return true;
	#else
		return NativeOculus::detected();
	#endif
#endif
}

void
VRCamera::initialize(int viewportWidth, int viewportHeight, float zNear, float zFar, void* window)
{
#ifdef EMSCRIPTEN
	_VRImpl = WebVROculus::create(viewportWidth, viewportHeight, zNear, zFar);
#else
	#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
		_VRImpl = Cardboard::create(viewportWidth, viewportHeight, zNear, zFar);
	#else
		_VRImpl = NativeOculus::create(viewportWidth, viewportHeight, zNear, zFar);
	#endif
#endif

    if (!detected())
        return;

    updateViewport(viewportWidth, viewportHeight);
    initializeVRDevice(window);
}

void
VRCamera::targetAdded(NodePtr target)
{
    _addedSlot = target->added().connect([=](scene::Node::Ptr n, scene::Node::Ptr t, scene::Node::Ptr p)
    {
        findSceneManager();
    });


    _removedSlot = target->removed().connect([=](scene::Node::Ptr n, scene::Node::Ptr t, scene::Node::Ptr p)
    {
        findSceneManager();
    });

    _VRImpl->initializeCameras(target);

    findSceneManager();
}

void
VRCamera::targetRemoved(NodePtr target)
{
    _VRImpl->targetRemoved();
    _renderBeginSlot->disconnect();
    findSceneManager();
}

void
VRCamera::initializeVRDevice(void* window)
{
    _VRImpl->initializeVRDevice(window);
}

void
VRCamera::findSceneManager()
{
    NodeSet::Ptr roots = NodeSet::create(target())
        ->roots()
        ->where([](NodePtr node)
        {
            return node->hasComponent<SceneManager>();
        });

    if (roots->nodes().size() > 1)
        throw std::logic_error("VRCamera cannot be in two separate scenes.");
    else if (roots->nodes().size() == 1)
        setSceneManager(roots->nodes()[0]->component<SceneManager>());
    else
        setSceneManager(nullptr);
}

void
VRCamera::setSceneManager(SceneManager::Ptr sceneManager)
{
    if (_sceneManager == sceneManager)
        return;

    _sceneManager = sceneManager;

    auto context = sceneManager->assets()->context();

    _renderBeginSlot = sceneManager->renderingBegin()->connect(std::bind(
        &VRCamera::updateCameraOrientation,
        std::static_pointer_cast<VRCamera>(shared_from_this())
    ), 1000.f);

    _VRImpl->initialize(_sceneManager);
}

void
VRCamera::updateCameraOrientation()
{
    _VRImpl->updateCameraOrientation(target());
}