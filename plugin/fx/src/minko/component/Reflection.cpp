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

#include "minko/math/Vector3.hpp"
#include "minko/scene/Node.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/Effect.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/data/StructureProvider.hpp"
#include "minko/file/AssetLibrary.hpp"

using namespace minko;
using namespace math;
using namespace minko::component;

Reflection::Reflection(
    std::shared_ptr<file::AssetLibrary> assets,
    uint renderTargetWidth,
    uint renderTargetHeight,
    uint clearColor = 0xffffffff) :
    _assets(assets),
    _width(renderTargetWidth),
    _height(renderTargetWidth),
    _clearColor(clearColor),
    _rootAdded(Signal<AbsCmpPtr, std::shared_ptr<scene::Node>>::create()),
    _clipPlane(),
    _activeCamera(nullptr)
{
    _renderTarget = render::Texture::create(_assets->context(), clp2(_width), clp2(_height), false, true);
}

void
Reflection::initialize()
{
    // Load reflection effect
    _reflectionEffect = _assets->effect("effect/Reflection/PlanarReflection.effect");

    if (!_reflectionEffect)
        throw std::logic_error("The reflection effect has not been loaded.");

    _targetAddedSlot = targetAdded()->connect([&](AbstractComponent::Ptr cmp, NodePtr target)
    {
        if (target->components<Reflection>().size() > 1)
            throw std::logic_error("A node can't have many reflection components.");

        // Create reflection renderer
        _reflectionRenderer = Renderer::create(_clearColor, _renderTarget, _reflectionEffect, 10000);

        // Add the renderer to the camera
        target->addComponent(_reflectionRenderer);

        if (_activeCamera == nullptr && target->root()->hasComponent<SceneManager>())
            targetAddedToScene(nullptr, target, nullptr);
        else
            _addedToSceneSlot = target->added()->connect(std::bind(
            &Reflection::targetAddedToScene,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3));
    });

    _targetRemovedSlot = targetRemoved()->connect([&](AbstractComponent::Ptr cmp, scene::Node::Ptr target)
    {
    });

    _rootAddedSlot = rootAdded()->connect([&](AbstractComponent::Ptr cmp, scene::Node::Ptr target)
    {
        // Get the target's transform to compute clipping plane
        auto transform = target->component<Transform>();

        auto root = target->root();

        for (auto child : root->children())
        {
            auto perspectiveCameras = child->components<PerspectiveCamera>();
            auto renderers = child->components<Renderer>();
            if (perspectiveCameras.size() > 0 && renderers.size() > 0)
            {
                auto perspectiveCamera = perspectiveCameras[0];
                for (auto renderer : renderers)
                {
                    // It's a main camera (renders into the back buffer)
                    if (renderer->target() == nullptr)
                    {
                        auto renderTarget = render::Texture::create(_assets->context(), _width, _height, false, true);

                        // Create a new render target
                        _renderTargets.push_back(renderTarget);

                        // Create a virtual camera
                        auto virtualPerspectiveCameraComponent = PerspectiveCamera::create(
                            (float) _width / (float) _height, (float) PI * 0.25f, .1f, 1000.f);

                        auto cameraTarget = Vector3::create();
                        auto reflectedPosition = Vector3::create();

                        auto virtualCamera = scene::Node::create("virtualCamera")
                            ->addComponent(Renderer::create(_clearColor, _renderTarget, _reflectionEffect))
                            ->addComponent(virtualPerspectiveCameraComponent)
                            ->addComponent(Transform::create(Matrix4x4::create()
                            ->lookAt(cameraTarget, reflectedPosition)));

                        // Add the virtual camera to the scene
                        root->addChild(virtualCamera);

                        // Bind this camera with a virtual camera (by index for now)
                        // TODO: Use unordered_map instead
                        _cameras.push_back(child);
                        _virtualCameras.push_back(virtualCamera);

                        // Use slot to detect when update the virtual camera
                        _viewMatrixChangedSlot = perspectiveCamera->data()->propertyValueChanged()->connect(
                            std::bind(
                            &Reflection::cameraPropertyValueChangedHandler,
                            shared_from_this(),
                            std::placeholders::_1,
                            std::placeholders::_2
                            )
                            );
                    }
                }
            }
        }
    });
}

void
Reflection::targetAddedHandler(AbstractComponent::Ptr cmp, NodePtr target)
{
    target->added()->connect([&](NodePtr node, NodePtr target, NodePtr ancestor)
    {
        if (target->components<Reflection>().size() > 1)
            throw std::logic_error("A node can't have many reflection components.");

        if (target->root()->hasComponent<SceneManager>())
            targetAddedToScene(nullptr, target, nullptr);
    });
}

void
Reflection::targetAddedToScene(NodePtr node, NodePtr target, NodePtr ancestor)
{
    if (target->root()->hasComponent<SceneManager>())
    {
        _addedToSceneSlot = nullptr;

        // We first check that the target has a camera component
        if (target->components<component::PerspectiveCamera>().size() < 1)
            throw std::logic_error("Reflection must be added to a camera");

        // We save the target as active camera
        _activeCamera = target;

        // Listen scene manager
        _frameBeginSlot = target->root()->component<SceneManager>()->frameBegin()->connect(
            [&](std::shared_ptr<SceneManager> sceneManager, float time, float deltaTime)
        {
            updateReflectionMatrix();
        }, 999.f);
    }
}

void
Reflection::cameraPropertyValueChangedHandler(std::shared_ptr<data::Provider> provider, const std::string& property)
{
    if (property == "viewMatrix")
    {
        updateReflectionMatrix();

        // Update virtual matrixes according to associated real cameras
        updateReflectionMatrixes();
    }
}

void
Reflection::updateReflectionMatrix()
{
    auto transform = Matrix4x4::create()->copyFrom(_activeCamera->component<Transform>()->modelToWorldMatrix());

    // Compute active camera data
    auto cameraPosition = transform->translation();
    auto cameraDirection = transform->deltaTransform(Vector3::create(0.f, 0.f, -1.f));
    auto targetPosition = Vector3::create(cameraPosition)->add(cameraDirection);

    // Compute virtual camera data
    auto reflectedPosition = Vector3::create()->setTo(cameraPosition->x(), -cameraPosition->y(), cameraPosition->z());
    auto reflectedTargetPosition = Vector3::create()->setTo(targetPosition->x(), -targetPosition->y(), targetPosition->z());

    // Compute reflected view matrix
    auto reflectedViewMatrix = Matrix4x4::create()->lookAt(
        reflectedTargetPosition, reflectedPosition);
    
    reflectedViewMatrix->lock();
    reflectedViewMatrix->transform(Vector3::zero(), reflectedPosition);
    reflectedViewMatrix->invert();
    reflectedViewMatrix->unlock();

    _reflectionEffect->setUniform("ReflectedViewMatrix", reflectedViewMatrix);
}

void
Reflection::updateReflectionMatrixes()
{
}