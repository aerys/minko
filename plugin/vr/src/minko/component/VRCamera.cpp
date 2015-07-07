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
    _viewportWidth = viewportWidth;
    _viewportHeight = viewportHeight;

    auto aspectRatio = (float)(viewportWidth / 2) / (float)viewportHeight;  

    if (_leftCameraNode)
    {
        _leftCameraNode->component<PerspectiveCamera>()->aspectRatio(aspectRatio);
    }
    if (_rightCameraNode)
    {
        _rightCameraNode->component<PerspectiveCamera>()->aspectRatio(aspectRatio);
    }

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

    // Initialize both eyes' renderers
    _leftRenderer = Renderer::create();
    _rightRenderer = Renderer::create();
    _rightRenderer->clearBeforeRender(false);

    _leftRenderer->viewport(
        math::ivec4(
            0,
            0,
			math::clp2(viewportWidth / 2),
			math::clp2(viewportHeight)
        )
    );

    _rightRenderer->viewport(
        math::ivec4(
			math::clp2(viewportWidth / 2),
            0,
			math::clp2(viewportWidth / 2),
			math::clp2(viewportHeight)
        )
    );

    updateViewport(viewportWidth, viewportHeight);
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

    auto aspectRatio = (float)(_viewportWidth / 2) / _viewportHeight;

    auto leftCamera = PerspectiveCamera::create(
        aspectRatio,
        atan(45),
        _VRImpl->zNear(),
        _VRImpl->zFar()
    );

    _leftCameraNode = scene::Node::create("cameraLeftEye")
        ->addComponent(Transform::create(math::inverse(math::lookAt(math::vec3(0), math::vec3(0, 10, -10), math::vec3(0, 1, 0)))))
        ->addComponent(leftCamera)
        ->addComponent(_leftRenderer);

    target->addChild(_leftCameraNode);

    auto rightCamera = PerspectiveCamera::create(
        aspectRatio,
        atan(45),
        _VRImpl->zNear(),
        _VRImpl->zFar()
    );

    _rightCameraNode = scene::Node::create("cameraRightEye")
        ->addComponent(Transform::create(math::inverse(math::lookAt(math::vec3(1000.f, 0, 0), math::vec3(1000.f, 10, -10), math::vec3(0, 1, 0)))))
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
        std::static_pointer_cast<VRCamera>(shared_from_this()),
        _leftCameraNode,
        _rightCameraNode
    ), 1000.f);

    _VRImpl->initialize(_sceneManager);
}

void
VRCamera::updateCameraOrientation(std::shared_ptr<scene::Node> leftCamera, std::shared_ptr<scene::Node> rightCamera)
{
    _VRImpl->updateCameraOrientation(target(), leftCamera, rightCamera);
}

Signal<>::Ptr
VRCamera::actionButtonPressed()
{
    return _VRImpl->actionButtonPressed();
}

Signal<>::Ptr
VRCamera::actionButtonReleased()
{
    return _VRImpl->actionButtonReleased();
}