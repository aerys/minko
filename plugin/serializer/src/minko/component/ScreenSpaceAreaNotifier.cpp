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

#include "minko/component/BoundingBox.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/ScreenSpaceAreaNotifier.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Transform.hpp"
#include "minko/data/Provider.hpp"
#include "minko/data/Store.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::scene;

ScreenSpaceAreaNotifier::ScreenSpaceAreaNotifier() :
    AbstractComponent(),
    _sceneManagerFunction(),
    _rendererFunction(),
    _nodeAddedSlot(),
    _nodeRemovedSlot(),
    _componentAddedSlot(),
    _componentRemovedSlot(),
    _frameBeginSlot(),
    _eyePosition(),
    _fov(0.f),
    _aspectRatio(0.f),
    _viewport(),
    _worldToScreenMatrix(),
    _viewMatrix(),
    _propertyName(),
    _updateRate(0.f),
    _updateTime(0.f),
    _previousTime(0.f)
{
}

AbstractComponent::Ptr
ScreenSpaceAreaNotifier::defaultSceneManagerFunction(Node::Ptr node)
{
    return node->root()->component<SceneManager>();
}

AbstractComponent::Ptr
ScreenSpaceAreaNotifier::defaultRendererFunction(Node::Ptr node)
{
    auto rendererNodes = NodeSet::create(node->root())
        ->descendants(true)
        ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<Renderer>(); });

    return rendererNodes->nodes().empty() ? nullptr : rendererNodes->nodes().front()->component<Renderer>();        
}

void
ScreenSpaceAreaNotifier::targetAdded(Node::Ptr target)
{
    _nodeAddedSlot = target->added().connect(std::bind(
        &ScreenSpaceAreaNotifier::nodeAddedHandler,
        std::static_pointer_cast<ScreenSpaceAreaNotifier>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));

    _nodeRemovedSlot = target->removed().connect(std::bind(
        &ScreenSpaceAreaNotifier::nodeRemovedHandler,
        std::static_pointer_cast<ScreenSpaceAreaNotifier>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));

    _componentAddedSlot = target->componentAdded().connect(std::bind(
        &ScreenSpaceAreaNotifier::componentAddedHandler,
        std::static_pointer_cast<ScreenSpaceAreaNotifier>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_3
    ));

    _componentRemovedSlot = target->componentRemoved().connect(std::bind(
        &ScreenSpaceAreaNotifier::componentRemovedHandler,
        std::static_pointer_cast<ScreenSpaceAreaNotifier>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_3
    ));

    _sceneManagerFunction = std::bind(
        &ScreenSpaceAreaNotifier::defaultSceneManagerFunction,
        std::placeholders::_1
    );

    _rendererFunction = std::bind(
        &ScreenSpaceAreaNotifier::defaultRendererFunction,
        std::placeholders::_1
    );    

    nodeAddedHandler(target, target);
}

void
ScreenSpaceAreaNotifier::targetRemoved(Node::Ptr target)
{
    _nodeAddedSlot = nullptr;
    _nodeRemovedSlot = nullptr;
}

void
ScreenSpaceAreaNotifier::candidateNodeAdded(Node::Ptr target, Node::Ptr node)
{
    auto meshNodes = NodeSet::create(node)
        ->descendants(true)
        ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<Surface>(); });

    for (auto meshNode : meshNodes->nodes())
    for (auto surface : meshNode->components<Surface>())
        surfaceAdded(surface);
}

void
ScreenSpaceAreaNotifier::candidateNodeRemoved(Node::Ptr target, Node::Ptr node)
{
    for (auto surface : node->components<Surface>())
        surfaceRemoved(surface);
}

void
ScreenSpaceAreaNotifier::surfaceAdded(Surface::Ptr surface)
{
    auto surfaceIt = _surfaces.emplace(surface, SurfaceInfo());

    if (!surfaceIt.second)
        return;

    auto surfaceTarget = surface->target();

    if (!surfaceTarget->hasComponent<BoundingBox>())
        surfaceTarget->addComponent(BoundingBox::create());

    auto& surfaceInfo = surfaceIt.first->second;

    surfaceInfo.surface = surface;

	surfaceInfo.modelToWorldMatrixChangedSlot = surfaceTarget->data().propertyChanged("modelToWorldMatrix").connect(
		[this, surface](Store&          	                store,
            	        Provider::Ptr                       provider,
				        const data::Provider::PropertyName& propertyName)
        {
        	modelToWorldMatrixChanged(
                _surfaces.at(surface),
                provider->get<math::mat4>(propertyName)
            );
        }
    );

    modelToWorldMatrixChanged(surfaceInfo, surfaceTarget->data().get<math::mat4>("modelToWorldMatrix"));
}

void
ScreenSpaceAreaNotifier::modelToWorldMatrixChanged(SurfaceInfo&      surfaceInfo,
                                                   const math::mat4& modelToWorldMatrix)
{
    auto surface = surfaceInfo.surface;

    auto box = surface->target()->component<BoundingBox>()->box();

    const auto bottomLeft = box->bottomLeft();
    const auto topRight = box->topRight();

    const auto width = box->width();
    const auto height = box->height();
    const auto depth = box->depth();

	surfaceInfo.boxVertices = std::array<math::vec3, 8>
	{{
		math::vec3(bottomLeft),
		math::vec3(bottomLeft.x + width, bottomLeft.y, bottomLeft.z),
		math::vec3(bottomLeft.x + width, bottomLeft.y + height, bottomLeft.z),
		math::vec3(bottomLeft.x, bottomLeft.y + height, bottomLeft.z),

		math::vec3(bottomLeft.x, bottomLeft.y, bottomLeft.z + depth),
		math::vec3(bottomLeft.x + width, bottomLeft.y, bottomLeft.z + depth),
		math::vec3(bottomLeft.x + width, bottomLeft.y + height, bottomLeft.z + depth),
		math::vec3(bottomLeft.x, bottomLeft.y + height, bottomLeft.z + depth),
	}};
}

void
ScreenSpaceAreaNotifier::surfaceRemoved(Surface::Ptr surface)
{
    _surfaces.erase(surface);
}

void
ScreenSpaceAreaNotifier::viewPropertyChanged(const math::mat4&   worldToScreenMatrix,
                                             const math::mat4&   viewMatrix,
                                             const math::vec3&   eyePosition,
                                             float               fov,
                                             float               aspectRatio,
                                             float               zNear,
                                             float               zFar)
{
    for (auto& surfaceToSurfaceInfo : _surfaces)
        surfaceToSurfaceInfo.second.updateNeeded = true;

    _eyePosition = eyePosition;
    _fov = fov;
    _aspectRatio = aspectRatio;
    _worldToScreenMatrix = worldToScreenMatrix;
    _viewMatrix = viewMatrix;
}

void
ScreenSpaceAreaNotifier::viewportChanged(const math::vec4& viewport)
{
    for (auto& surfaceToSurfaceInfo : _surfaces)
        surfaceToSurfaceInfo.second.updateNeeded = true;

    _viewport = viewport;
}

void
ScreenSpaceAreaNotifier::update(float time)
{
    for (auto& surfaceToSurfaceInfoPair : _surfaces)
    {
        auto& surfaceInfo = surfaceToSurfaceInfoPair.second;

        if (!surfaceInfo.updateNeeded)
            continue;

        surfaceInfo.updateNeeded = false;

        if (updateRate() <= 0.f)
        {
            surfaceInfo.previousValue = surfaceInfo.targetValue;
        
            surfaceInfo.targetValue = computeScreenSpaceArea(
                surfaceInfo,
                _eyePosition,
                _viewport,
                _worldToScreenMatrix,
                _viewMatrix,
                time
            );

            surfaceInfo.surface->data()->set(
                propertyName(),
                surfaceInfo.targetValue
            );
        }
        else
        {
            auto elapsedTime = time - _previousTime;

            if (elapsedTime >= _updateTime)
            {
                elapsedTime = elapsedTime - _updateTime;

                _previousTime = time;

                surfaceInfo.previousValue = surfaceInfo.targetValue;
        
                surfaceInfo.targetValue = computeScreenSpaceArea(
                    surfaceInfo,
                    _eyePosition,
                    _viewport,
                    _worldToScreenMatrix,
                    _viewMatrix,
                    time
                );
            }

            surfaceInfo.surface->data()->set(
                propertyName(),
                math::mix(surfaceInfo.previousValue, surfaceInfo.targetValue, elapsedTime / _updateTime)
            );
        }
    }
}

void
ScreenSpaceAreaNotifier::sceneManagerSet(SceneManager::Ptr sceneManager)
{
    if (sceneManager == nullptr)
    {
        _frameBeginSlot = nullptr;
        _rootNodePropertyChangedSlot = nullptr;
    }
    else
    {
        _frameBeginSlot = sceneManager->frameBegin()->connect(std::bind(
            &ScreenSpaceAreaNotifier::frameBeginHandler,
            std::static_pointer_cast<ScreenSpaceAreaNotifier>(shared_from_this()),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        ));

        auto& rootData = sceneManager->target()->data();

        if (rootData.hasProperty("viewport"))
        {
            viewportChanged(rootData.get<math::vec4>("viewport"));
        }

        _rootNodePropertyChangedSlot = sceneManager->target()->data().propertyChanged().connect(
            std::bind(
                &ScreenSpaceAreaNotifier::rootNodePropertyChangedHandler,
                std::static_pointer_cast<ScreenSpaceAreaNotifier>(shared_from_this()),
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
            )
        );
    }
}

void
ScreenSpaceAreaNotifier::rendererSet(Renderer::Ptr renderer)
{
    if (renderer == nullptr)
    {
        _rendererNodePropertyChangedSlot = nullptr;
    }
    else
    {
        auto& rendererData = renderer->target()->data();

        if (rendererData.hasProperty("worldToScreenMatrix"))
        {
            rendererNodePropertyChangedHandler(rendererData, nullptr, "worldToScreenMatrix");
        }

        _rendererNodePropertyChangedSlot = renderer->target()->data().propertyChanged().connect(
            std::bind(
                &ScreenSpaceAreaNotifier::rendererNodePropertyChangedHandler,
                std::static_pointer_cast<ScreenSpaceAreaNotifier>(shared_from_this()),
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
            )
        );
    }
}

void
ScreenSpaceAreaNotifier::nodeAddedHandler(Node::Ptr target, Node::Ptr node)
{
    auto sceneManager = sceneManagerFunction()(node);
    sceneManagerSet(sceneManager == nullptr ? nullptr : std::static_pointer_cast<SceneManager>(sceneManager));

    auto renderer = rendererFunction()(node);
    rendererSet(renderer == nullptr ? nullptr : std::static_pointer_cast<Renderer>(renderer));

    candidateNodeAdded(target, node);
}

void
ScreenSpaceAreaNotifier::nodeRemovedHandler(Node::Ptr target, Node::Ptr node)
{
    auto sceneManager = sceneManagerFunction()(node);
    sceneManagerSet(sceneManager == nullptr ? nullptr : std::static_pointer_cast<SceneManager>(sceneManager));

    auto renderer = rendererFunction()(node);
    rendererSet(renderer == nullptr ? nullptr : std::static_pointer_cast<Renderer>(renderer));

    candidateNodeRemoved(target, node);
}

void
ScreenSpaceAreaNotifier::componentAddedHandler(Node::Ptr target, AbstractComponent::Ptr component)
{
    auto node = component->target();

    auto sceneManager = std::dynamic_pointer_cast<SceneManager>(component);

    if (sceneManager != nullptr)
        sceneManagerSet(std::static_pointer_cast<SceneManager>(sceneManagerFunction()(node)));

    auto renderer = std::dynamic_pointer_cast<Renderer>(component);

    if (renderer != nullptr)
        rendererSet(std::static_pointer_cast<Renderer>(rendererFunction()(node)));

    auto surface = std::dynamic_pointer_cast<Surface>(component);

    if (surface != nullptr)
    {
        surfaceAdded(surface);
    }
}

void
ScreenSpaceAreaNotifier::componentRemovedHandler(Node::Ptr target, AbstractComponent::Ptr component)
{
    auto node = component->target();

    auto sceneManager = std::dynamic_pointer_cast<SceneManager>(component);

    if (sceneManager != nullptr)
        sceneManagerSet(std::static_pointer_cast<SceneManager>(sceneManagerFunction()(node)));

    auto renderer = std::dynamic_pointer_cast<Renderer>(component);

    if (renderer != nullptr)
        rendererSet(std::static_pointer_cast<Renderer>(rendererFunction()(node)));

    auto surface = std::dynamic_pointer_cast<Surface>(component);

    if (surface != nullptr)
    {
        surfaceRemoved(surface);
    }
}

void
ScreenSpaceAreaNotifier::frameBeginHandler(SceneManager::Ptr sceneManager, float time, float deltaTime)
{
    update(time);
}

void
ScreenSpaceAreaNotifier::rootNodePropertyChangedHandler(Store&                                 store,
                                                        Provider::Ptr                          provider,
                                                        const data::Provider::PropertyName&    propertyName)
{
    if (*propertyName == "viewport")
    {
        viewportChanged(provider->get<math::vec4>(propertyName));
    }
}

void
ScreenSpaceAreaNotifier::rendererNodePropertyChangedHandler(Store&                                 store,
                                                            Provider::Ptr                          provider,
                                                            const data::Provider::PropertyName&    propertyName)
{
    if (*propertyName == "worldToScreenMatrix")
    {
        viewPropertyChanged(
            store.get<math::mat4>("worldToScreenMatrix"),
            store.get<math::mat4>("viewMatrix"),
            store.get<math::vec3>("eyePosition"),
            store.get<float>("fov"),
            store.get<float>("aspectRatio"),
            store.get<float>("zNear"),
            store.get<float>("zFar")
        );
    }
}

float
ScreenSpaceAreaNotifier::computeScreenSpaceArea(SurfaceInfo&        surfaceInfo,
                                                const math::vec3&   eyePosition,
                                                const math::vec4&   viewport,
                                                const math::mat4&   worldToScreenMatrix,
                                                const math::mat4&   viewMatrix,
                                                float               time)
{
    const auto& modelToWorldMatrix = surfaceInfo.surface->target()->component<Transform>()->modelToWorldMatrix(true);

    const auto screenArea = aabbApproxScreenSpaceArea(
        surfaceInfo.boxVertices,
        modelToWorldMatrix,
        eyePosition,
        viewport,
        worldToScreenMatrix,
        viewMatrix
    );

    return screenArea;
}

float
ScreenSpaceAreaNotifier::accurateScreenSpaceArea(const std::array<math::vec3, 8>&    boxVertices,
                                                 const math::mat4&                   modelToWorld,
                                                 const math::vec3&                   eyePosition,
                                                 const math::vec4&                   viewport,
                                                 const math::mat4&                   worldToScreenMatrix,
                                                 const math::mat4&                   viewMatrix) const
{
    // ref: http://www.cg.tuwien.ac.at/research/publications/1999/Fuhr-1999-Conc/TR-186-2-99-05Paper.pdf

    static const int hullVertexData[64][7] =
    {
        0, -1, -1, -1, -1, -1, -1,
        4, 0,  4,  7,  3,  -1, -1,
        4, 1,  2,  6,  5,  -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        4, 0,  1,  5,  4,  -1, -1,
        6, 0,  1,  5,  4,  7,  3,
        6, 0,  1,  2,  6,  5,  4,
        0, -1, -1, -1, -1, -1, -1,
        4, 2,  3,  7,  6,  -1, -1,
        6, 4,  7,  6,  2,  3,  0,
        6, 2,  3,  7,  6,  5,  1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        4,  0, 3,  2,  1,  -1, -1,
        6,  0, 4,  7,  3,  2,  1,
        6,  0, 3,  2,  6,  5,  1,
        0, -1, -1, -1, -1, -1, -1,
        6, 0,  3,  2,  1,  5,  4,
        6, 2,  1,  5,  4,  7,  3,
        6, 0,  3,  2,  6,  5,  4,
        0, -1, -1, -1, -1, -1, -1,
        6, 0,  3,  7,  6,  2,  1,
        6, 0,  4,  7,  6,  2,  1,
        6, 0,  3,  7,  6,  5,  1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        4, 4,  5,  6,  7,  -1, -1,
        6, 4,  5,  6,  7,  3,  0,
        6, 1,  2,  6,  7,  4,  5,
        0, -1, -1, -1, -1, -1, -1,
        6, 0,  1,  5,  6,  7,  4,
        6, 0,  1,  5,  6,  7,  3,
        6, 0,  1,  2,  6,  7,  4,
        0, -1, -1, -1, -1, -1, -1,
        6, 2,  3,  7,  4,  5,  6,
        6, 0,  4,  5,  6,  2,  3,
        6, 1,  2,  3,  7,  4,  5,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1
    };

    const auto viewportWidth = viewport.z;
    const auto viewportHeight = viewport.w;

    auto projectedVertices = std::array<math::vec3, 8>();

    auto area = 0.f;

    const auto position =
        ((eyePosition.x < boxVertices[0].x)) +
        ((eyePosition.x > boxVertices[6].x) << 1) +
        ((eyePosition.y < boxVertices[0].y) << 2) +
        ((eyePosition.y > boxVertices[6].y) << 3) +
        ((eyePosition.z < boxVertices[0].z) << 4) +
        ((eyePosition.z > boxVertices[6].z) << 5);

    if (position == 0)
        return viewportWidth * viewportHeight;

    const auto numVertices = hullVertexData[position][0];

    if (numVertices == 0)
        return 0.f;

    for (auto i = 0u; i < numVertices; ++i)
    {
        const auto projectedVertex = PerspectiveCamera::project(
            boxVertices[hullVertexData[position][i + 1]],
            viewportWidth,
            viewportHeight,
            viewMatrix,
            worldToScreenMatrix
        );

        projectedVertices[i] = math::clamp(
            projectedVertex,
            math::vec3(viewport.x, viewport.y, 0.f),
            math::vec3(viewport.z, viewport.w, 0.f)
        );
    }

    for (auto i = 0u; i < numVertices; ++i)
        area += (projectedVertices[i].x - projectedVertices[(i + 1) % numVertices].x) *
                (projectedVertices[i].y + projectedVertices[(i + 1) % numVertices].y);

    return math::abs(area) * 0.5f;
}

float
ScreenSpaceAreaNotifier::aabbApproxScreenSpaceArea(const std::array<math::vec3, 8>&  boxVertices,
                                                   const math::mat4&                 modelToWorld,
                                                   const math::vec3&                 eyePosition,
                                                   const math::vec4&                 viewport,
                                                   const math::mat4&                 worldToScreenMatrix,
                                                   const math::mat4&                 viewMatrix) const
{
    const auto viewportWidth = viewport.z;
    const auto viewportHeight = viewport.w;

    auto projectedVertices = std::array<math::vec3, 8>();

    auto zSignChanged = -1;
    auto positiveZ = true;

    for (auto i = 0u; i < 8u; ++i)
    {
        const auto projectedVertex = PerspectiveCamera::project(
            boxVertices[i],
            viewportWidth,
            viewportHeight,
            viewMatrix,
            worldToScreenMatrix
        );

        projectedVertices[i] = math::clamp(
            projectedVertex,
            math::vec3(viewport.x, viewport.y, projectedVertex.z),
            math::vec3(viewport.z, viewport.w, projectedVertex.z)
        );

        if (zSignChanged == -1)
        {
            positiveZ = projectedVertices[i].z >= 0.f;

            ++zSignChanged;
        }
        else
        {
            const auto localPositiveZ = projectedVertices[i].z >= 0.f;

            if (positiveZ != localPositiveZ)
                ++zSignChanged;
        }
    }

    if (zSignChanged > 0)
    {
        return viewportWidth * viewportHeight;
    }

    if (!positiveZ)
    {
        return 0.f;
    }

    auto boxScreenBottomLeft = math::vec2(std::numeric_limits<float>::max());
    auto boxScreenTopRight = math::vec2(-std::numeric_limits<float>::max());

    for (const auto& vertex : projectedVertices)
    {
        boxScreenBottomLeft = math::min(boxScreenBottomLeft, vertex.xy());
        boxScreenTopRight = math::max(boxScreenTopRight, vertex.xy());
    }

    const auto boxScreenSize = boxScreenTopRight - boxScreenBottomLeft;

    return boxScreenSize.x * boxScreenSize.y;
}
