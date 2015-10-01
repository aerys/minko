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
#  include "minko/oculus/WebVROculus.hpp"
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
    
    auto aspectRatio = (viewportWidth / 2.f) / viewportHeight;

    if (_leftCameraNode)
        _leftCameraNode->component<PerspectiveCamera>()->aspectRatio(aspectRatio);

    if (_rightCameraNode)
        _rightCameraNode->component<PerspectiveCamera>()->aspectRatio(aspectRatio);

    _leftRenderer->viewport(math::ivec4(0, 0, viewportWidth / 2, viewportHeight));
    _rightRenderer->viewport(math::ivec4(viewportWidth / 2, 0, viewportWidth / 2, viewportHeight));

    if (_VRImpl)
        _VRImpl->updateViewport(viewportWidth, viewportHeight);
}

void
VRCamera::forceRatio(float aspectRatio)
{
    if (_leftCameraNode)
        _leftCameraNode->component<PerspectiveCamera>()->aspectRatio(aspectRatio);

    if (_rightCameraNode)
        _rightCameraNode->component<PerspectiveCamera>()->aspectRatio(aspectRatio);
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
VRCamera::initialize(int viewportWidth, int viewportHeight, float zNear, float zFar, minko::uint rendererClearColor, void* window)
{
#ifdef EMSCRIPTEN
    if (detected())
	    _VRImpl = WebVROculus::create(viewportWidth, viewportHeight, zNear, zFar);
    else
        _VRImpl = Cardboard::create(viewportWidth, viewportHeight, zNear, zFar);
#else
	#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
		_VRImpl = Cardboard::create(viewportWidth, viewportHeight, zNear, zFar);
	#else
		_VRImpl = NativeOculus::create(viewportWidth, viewportHeight, zNear, zFar);
	#endif
#endif

    // Initialize both eyes' renderers
    _leftRenderer = Renderer::create(rendererClearColor);
    _rightRenderer = Renderer::create(rendererClearColor);
    _rightRenderer->clearBeforeRender(false);

    updateViewport(viewportWidth, viewportHeight);

    if (_VRImpl)
        _VRImpl->initializeVRDevice(_leftRenderer, _rightRenderer, window);
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

    auto leftCamera = PerspectiveCamera::create(
        aspectRatio,
        atan(45),
        zNear,
        zFar
    );

    _leftCameraNode = scene::Node::create("cameraLeftEye")
        ->addComponent(Transform::create(math::inverse(math::lookAt(math::vec3(-0.03f, 0, 0), math::vec3(-0.03f, 0, -1), math::vec3(0, 1, 0)))))
        ->addComponent(leftCamera)
        ->addComponent(_leftRenderer);

    target->addChild(_leftCameraNode);

    auto rightCamera = PerspectiveCamera::create(
        aspectRatio,
        atan(45),
        zNear,
        zFar
    );

    _rightCameraNode = scene::Node::create("cameraRightEye")
        ->addComponent(Transform::create(math::inverse(math::lookAt(math::vec3(0.03f, 0, 0), math::vec3(0.03f, 0, -1), math::vec3(0, 1, 0)))))
        ->addComponent(rightCamera)
        ->addComponent(_rightRenderer);

    target->addChild(_rightCameraNode);
}

void
VRCamera::targetRemoved(NodePtr target)
{
    _leftCameraNode->removeComponent(_leftRenderer);
    _rightCameraNode->removeComponent(_rightRenderer);

    _leftCameraNode->removeComponent(_leftCameraNode->component<PerspectiveCamera>());
    _rightCameraNode->removeComponent(_rightCameraNode->component<PerspectiveCamera>());

    target->removeChild(_leftCameraNode);
    target->removeChild(_rightCameraNode);

    if (_VRImpl)
        _VRImpl->targetRemoved();

    _renderBeginSlot->disconnect();
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

    _renderBeginSlot = sceneManager->renderingBegin()->connect(std::bind(
        &VRCamera::updateCameraOrientation,
        std::static_pointer_cast<VRCamera>(shared_from_this()),
        _leftCameraNode,
        _rightCameraNode
    ), 1000.f);

    if (_VRImpl)
        _VRImpl->initialize(_sceneManager);
}

void
VRCamera::updateCameraOrientation(std::shared_ptr<scene::Node> leftCamera, std::shared_ptr<scene::Node> rightCamera)
{
    if (_VRImpl)
        _VRImpl->updateCameraOrientation(target(), leftCamera, rightCamera);
}
