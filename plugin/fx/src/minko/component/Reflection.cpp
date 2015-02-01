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

#include "minko/component/Reflection.hpp"

#include "minko/scene/Node.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/Effect.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/file/AssetLibrary.hpp"

using namespace minko;
using namespace math;
using namespace minko::component;

Reflection::Reflection(std::shared_ptr<file::AssetLibrary> assets,
                       uint renderTargetWidth,
                       uint renderTargetHeight,
                       uint clearColor = 0xffffffff) :
    _assets(assets),
    _width(renderTargetWidth),
    _height(renderTargetWidth),
    _clearColor(clearColor),
    _rootAdded(Signal<AbsCmpPtr, std::shared_ptr<scene::Node>>::create()),
    _clipPlane(),
    _activeCamera(nullptr),
	_enabled(true),
	_reflectedViewMatrix()
{
    _renderTarget = render::Texture::create(_assets->context(), clp2(_width), clp2(_height), false, true);
}

Reflection::Reflection(const Reflection& reflection, const CloneOption& option) :
	_assets(reflection._assets),
	_width(reflection._width),
	_height(reflection._height),
	_clearColor(reflection._clearColor),
	_rootAdded(Signal<AbsCmpPtr, std::shared_ptr<scene::Node>>::create()),
	_clipPlane(),
	_activeCamera(reflection._activeCamera),
	_enabled(reflection._enabled),
	_reflectedViewMatrix()
{
	_renderTarget = render::Texture::create(_assets->context(), clp2(_width), clp2(_height), false, true);
}

AbstractComponent::Ptr
Reflection::clone(const CloneOption& option)
{
	auto reflection = std::shared_ptr<Reflection>(new Reflection(*this, option));

	return reflection;
}

void
Reflection::start(NodePtr target)
{
    // Load reflection effect
    // _reflectionEffect = _assets->effect("effect/Reflection/PlanarReflection.effect");
    _addedToSceneSlot = nullptr;

	auto renderTarget = render::Texture::create(_assets->context(), _width, _height, false, true);

	// Create a new render target
	_renderTargets.push_back(renderTarget);

	auto originalCamera = target->components<PerspectiveCamera>()[0];

	// Create a virtual camera
	auto virtualPerspectiveCameraComponent = PerspectiveCamera::create(
		originalCamera->aspectRatio(), originalCamera->fieldOfView(), originalCamera->zNear(), originalCamera->zFar());

	// auto cameraTarget = Vector3::create();
	// auto reflectedPosition = Vector3::create();
    //
	// auto renderer = Renderer::create(_clearColor, _renderTarget, _reflectionEffect, 1000000.f, "Reflection");
    //
	// renderer->layoutMask(scene::Layout::Group::REFLECTION);

	// _virtualCamera = scene::Node::create("virtualCamera")
	// 	->addComponent(renderer)
	// 	->addComponent(virtualPerspectiveCameraComponent)
	// 	->addComponent(Transform::create());
    //
	// enabled(_enabled);

	// Add the virtual camera to the scene
	target->root()->addChild(_virtualCamera);

	// Bind this camera with a virtual camera (by index for now)
	// TODO: Use unordered_map instead
	//_cameras.push_back(child);
	//_virtualCameras.push_back(virtualCamera);

    // We first check that the target has a camera component
    if (target->components<component::PerspectiveCamera>().size() < 1)
        throw std::logic_error("Reflection must be added to a camera");

    // We save the target as active camera
    //_activeCamera = target;
}

void
Reflection::update(NodePtr target)
{
    updateReflectionMatrix();
}

void
Reflection::stop(NodePtr target)
{
}

void
Reflection::updateReflectionMatrix()
{
	// if (!_enabled)
	// 	return;
    //
	// auto transformCmp = target()->component<Transform>();
	// auto transform = transformCmp->modelToWorldMatrix();
	// auto camera = target()->component<PerspectiveCamera>();
	// auto virtualCamera = _virtualCamera->component<PerspectiveCamera>();
    //
	// virtualCamera->fieldOfView(camera->fieldOfView());
	// virtualCamera->aspectRatio(camera->aspectRatio());
    //
    // // Compute active camera data
    // auto cameraPosition = transform->translation();
    // auto cameraDirection = transform->deltaTransform(Vector3::create(0.f, 0.f, -1.f));
    // auto targetPosition = Vector3::create(cameraPosition)->add(cameraDirection);
    //
    // // Compute virtual camera data
    // auto reflectedPosition = Vector3::create()->setTo(cameraPosition->x(), -cameraPosition->y(), cameraPosition->z());
    // auto reflectedTargetPosition = Vector3::create()->setTo(targetPosition->x(), -targetPosition->y(), targetPosition->z());
    //
    // // Compute reflected view matrix
    // _reflectedViewMatrix->lookAt(reflectedTargetPosition, reflectedPosition);
    //
	// _reflectedViewMatrix->lock();
	// _reflectedViewMatrix->transform(Vector3::zero(), reflectedPosition);
	// _reflectedViewMatrix->invert();
	// _reflectedViewMatrix->unlock();
    //
	// _reflectionEffect->setUniform("ReflectedViewMatrix", _reflectedViewMatrix);
}
