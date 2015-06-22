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

#include "minko/oculus/Cardboard.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/material/Material.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/scene/Node.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/Texture.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/SceneManager.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::oculus;
using namespace minko::render;
using namespace minko::file;

#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
using namespace minko::sensors;
#endif

Cardboard::Cardboard(int viewportWidth, int viewportHeight, float zNear, float zFar) :
    _zNear(zNear),
    _zFar(zFar),
    _aspectRatio((float)viewportWidth / (float)viewportHeight),
    _leftRenderer(nullptr),
    _rightRenderer(nullptr),
    _renderEndSlot(nullptr),
    _leftCameraNode(nullptr),
    _rightCameraNode(nullptr)
{
    _uvScaleOffset[0].first = math::vec2();
    _uvScaleOffset[0].second = math::vec2();
    _uvScaleOffset[1].first = math::vec2();
    _uvScaleOffset[1].second = math::vec2();

    _viewportWidth = viewportWidth;
    _viewportHeight = viewportHeight;
}

void
Cardboard::initialize(std::shared_ptr<component::SceneManager> sceneManager)
{
    _sceneManager = sceneManager;
}

void
Cardboard::initializeVRDevice(void* window)
{
#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    _attitude = Attitude::getInstance();
    _attitude->initialize();
    _attitude->startTracking();
#endif

    _leftRenderer = Renderer::create();
    _rightRenderer = Renderer::create();

    // Create renderer for each eye
    _rightRenderer->clearBeforeRender(false);

    // Compute each viewport pos and size
    _leftRenderer->viewport(
        math::ivec4(
            0,
            0,
            _viewportWidth / 2,
            _viewportHeight
        )
    );

    _rightRenderer->viewport(
        math::ivec4(
           _viewportWidth / 2,
            0,
            _viewportWidth / 2,
            _viewportHeight
        )
    );
}

void
Cardboard::initializeCameras(scene::Node::Ptr target)
{
    _target = target;
    auto aspectRatio = (float)(_viewportWidth / 2) / (float)_viewportHeight;

    auto leftCamera = PerspectiveCamera::create(
        aspectRatio,
        atan(45),
        _zNear,
        _zFar
    );

    _leftCameraNode = scene::Node::create("cardboardLeftEye")
        ->addComponent(Transform::create())
        ->addComponent(leftCamera)
        ->addComponent(_leftRenderer);

    target->addChild(_leftCameraNode);
    
    auto rightCamera = PerspectiveCamera::create(
        aspectRatio,
        atan(45),
        _zNear,
        _zFar
    );

    _rightCameraNode = scene::Node::create("cardboardRightEye")
        ->addComponent(Transform::create(math::translate(math::vec3(.5f, 0, 0))))
        ->addComponent(rightCamera)
        ->addComponent(_rightRenderer);


    target->addChild(_rightCameraNode);
}

Cardboard::~Cardboard()
{
}

void
Cardboard::targetRemoved()
{
#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    _attitude->stopTracking();
#endif

    auto sceneManager = _target->root()->component<SceneManager>();
    auto canvas = sceneManager->canvas();

    _leftCameraNode->removeComponent(_leftRenderer);
    _rightCameraNode->removeComponent(_rightRenderer);

    _leftCameraNode->removeComponent(_leftCameraNode->component<PerspectiveCamera>());
    _rightCameraNode->removeComponent(_rightCameraNode->component<PerspectiveCamera>());

    _target->removeChild(_leftCameraNode);
    _target->removeChild(_rightCameraNode);
}

void
Cardboard::updateViewport(int viewportWidth, int viewportHeight)
{
    _viewportWidth = viewportWidth;
    _viewportHeight = viewportHeight;
    _aspectRatio = (float)(viewportWidth / 2) / (float)viewportHeight;  
	
    if (_leftCameraNode)
    {
        _leftCameraNode->component<PerspectiveCamera>()->aspectRatio(_aspectRatio);
    }
    if (_rightCameraNode)
    {
        _rightCameraNode->component<PerspectiveCamera>()->aspectRatio(_aspectRatio);
    }
}

void
Cardboard::updateCameraOrientation(std::shared_ptr<scene::Node> target)
{
#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    auto rotationMatrix = math::transpose(_attitude->rotationMatrix());

    target->component<Transform>()->matrix(rotationMatrix);
#endif
}