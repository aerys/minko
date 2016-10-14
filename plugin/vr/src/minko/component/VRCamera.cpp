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
#include "minko/component/SceneManager.hpp"
#include "minko/file/AssetLibrary.hpp"

#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
# include "minko/oculus/Cardboard.hpp"
#else
# if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
#  include "minko/oculus/WebVR.hpp"
#  include "minko/oculus/Cardboard.hpp"
# else
#  include "minko/oculus/NativeOculus.hpp"
# endif
#endif

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;
using namespace minko::oculus;
using namespace minko::render;

VRCamera::VRCamera() :
    _sceneManager(nullptr),
    _addedSlot(nullptr),
    _removedSlot(nullptr),
    _VRImpl(nullptr),
    _viewportWidth(0.f),
    _viewportHeight(0.f),
    _rendererClearColor(0)
{
}

VRCamera::~VRCamera()
{
    _VRImpl = nullptr;
}

void
VRCamera::updateViewport(int viewportWidth, int viewportHeight)
{
    _viewportWidth = viewportWidth;
    _viewportHeight = viewportHeight;

    _leftRenderer->viewport(math::ivec4(0, 0, viewportWidth / 2, viewportHeight));
    _rightRenderer->viewport(math::ivec4(viewportWidth / 2, 0, viewportWidth / 2, viewportHeight));

    if (_VRImpl)
        _VRImpl->updateViewport(viewportWidth, viewportHeight);
}

bool
VRCamera::detected()
{
#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    return WebVR::detected() || (system::Platform::isMobile() && sensors::Attitude::getInstance()->isSupported());
#elif MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    return true;
#else
    return NativeOculus::detected();
#endif
}

void
VRCamera::initialize(int viewportWidth,
                     int viewportHeight,
                     float zNear,
                     float zFar,
                     minko::uint rendererClearColor,
                     void* window,
                     Renderer::Ptr leftRenderer,
                     Renderer::Ptr rightRenderer)
{
    if (!detected())
        return;

#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    _VRImpl = Cardboard::create(viewportWidth, viewportHeight, zNear, zFar);
#elif MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    if (WebVR::detected())
        _VRImpl = WebVR::create(viewportWidth, viewportHeight, zNear, zFar);
    else if (sensors::Attitude::getInstance()->isSupported())
        _VRImpl = Cardboard::create(viewportWidth, viewportHeight, zNear, zFar);
#elif MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
    _VRImpl = NativeOculus::create(viewportWidth, viewportHeight, zNear, zFar);
#endif

    // Initialize both eyes' renderers
    if (leftRenderer != nullptr)
        _leftRenderer = leftRenderer;
    else
        _leftRenderer = Renderer::create(rendererClearColor);

    if (rightRenderer != nullptr)
        _rightRenderer = rightRenderer;
    else
        _rightRenderer = Renderer::create(rendererClearColor);

    _rightRenderer->clearBeforeRender(false);

    if (_VRImpl)
        _VRImpl->initializeVRDevice(_leftRenderer, _rightRenderer, window);

    updateViewport(viewportWidth, viewportHeight);
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

    findSceneManager();

    // Initialize both eyes' cameras
    auto aspectRatio = (_viewportWidth / 2.f) / _viewportHeight;
    auto zNear = 0.1f;
    auto zFar = 10000.f;

    if (_VRImpl)
    {
        zNear = _VRImpl->zNear();
        zFar = _VRImpl->zFar();
    }

    auto leftCamera = Camera::create(math::perspective(atan(45.f), aspectRatio, zNear, zFar));
    auto rightCamera = Camera::create(math::perspective(atan(45.f), aspectRatio, zNear, zFar));

    _leftCameraNode = scene::Node::create("VRCameraLeftEye")
        ->addComponent(Transform::create(math::inverse(math::lookAt(math::vec3(-0.03f, 0, 0), math::vec3(-0.03f, 0, -1), math::vec3(0, 1, 0)))))
        ->addComponent(leftCamera)
        ->addComponent(_leftRenderer);

    _rightCameraNode = scene::Node::create("VRCameraRightEye")
        ->addComponent(Transform::create(math::inverse(math::lookAt(math::vec3(0.03f, 0, 0), math::vec3(0.03f, 0, -1), math::vec3(0, 1, 0)))))
        ->addComponent(rightCamera)
        ->addComponent(_rightRenderer);

    target->addChild(_leftCameraNode);
    target->addChild(_rightCameraNode);

    if (_VRImpl)
        _VRImpl->targetAdded();
}

void
VRCamera::targetRemoved(NodePtr target)
{
    _leftRenderer->enabled(false);
    _rightRenderer->enabled(false);

    _leftCameraNode->removeComponent(_leftRenderer);
    _rightCameraNode->removeComponent(_rightRenderer);

    _leftCameraNode->removeComponent(_leftCameraNode->component<Camera>());
    _rightCameraNode->removeComponent(_rightCameraNode->component<Camera>());

    target->removeChild(_leftCameraNode);
    target->removeChild(_rightCameraNode);

    if (_VRImpl)
        _VRImpl->targetRemoved();

    _frameBeginSlot = nullptr;

    findSceneManager();
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

    if (roots->nodes().size() == 1)
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

    _frameBeginSlot = sceneManager->frameBegin()->connect([&](SceneMgrPtr sm, float dt, float t)
    {
        updateCamera(_leftCameraNode, _rightCameraNode);
    });

    if (_VRImpl)
        _VRImpl->initialize(sceneManager);
}

void
VRCamera::updateCamera(std::shared_ptr<scene::Node> leftCamera, std::shared_ptr<scene::Node> rightCamera)
{
    if (_VRImpl)
        _VRImpl->updateCamera(target(), leftCamera, rightCamera);

    target()->component<Transform>()->updateModelToWorldMatrix();
}