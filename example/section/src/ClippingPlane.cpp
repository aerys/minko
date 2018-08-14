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

#include "minko/Canvas.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Transform.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/geometry/LineGeometry.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/material/BasicMaterial.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/render/Priority.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/scene/Layout.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/Camera.hpp"

#include "ClippingPlane.hpp"
#include "ClippingPlaneLayout.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::geometry;
using namespace minko::material;
using namespace minko::render;
using namespace minko::scene;

using namespace player;
using namespace player::component;

int ClippingPlane::_nextPlaneId = 0;
Renderer::Ptr ClippingPlane::_depthRenderer = nullptr;
Renderer::Ptr ClippingPlane::_stencilRenderer = nullptr;

ClippingPlane::ClippingPlane() :
    AbstractComponent(),
    _planeId(_nextPlaneId++)
{
}

void
ClippingPlane::targetAdded(Node::Ptr target)
{
    if (target->root()->hasComponent<SceneManager>())
        addedHandler(nullptr, target, nullptr);
    else
        _addedSlot = target->added().connect(std::bind(
            &ClippingPlane::addedHandler,
            std::static_pointer_cast<ClippingPlane>(shared_from_this()),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        ));
}

void
ClippingPlane::targetRemoved(Node::Ptr target)
{
    _addedSlot = nullptr;
}

void
ClippingPlane::addedHandler(Node::Ptr node, Node::Ptr target, Node::Ptr added)
{
    if (_planeNode == nullptr && target->root()->hasComponent<SceneManager>())
    {
        _sceneManager = target->root()->component<SceneManager>();

        initialize();
    }
}

void
ClippingPlane::initialize()
{
    auto target = this->target();
    auto root = target->root();

    _originNode = Node::create("origin")->addComponent(Transform::create());

    _planeNode = Node::create(stringifiedPlaneId());

    _planeNode->layout(ClippingPlaneLayout::CLIPPING);

    if (_planeGeometry == nullptr)
        _planeGeometry = createDefaultPlaneGeometry();

    if (_planeMaterial == nullptr)
        _planeMaterial = createDefaultPlaneMaterial();

    if (_planeEffect == nullptr)
        _planeEffect = createDefaultPlaneEffect();

    auto planeSurface = Surface::create(
        _planeGeometry,
        _planeMaterial,
        _planeEffect
    );

    _planeNode
        ->addComponent(Transform::create(_basePlaneTransformMatrix))
        ->addComponent(planeSurface);
    planeSurface->data()->set("clippingPlaneId", (float) _planeId);

    _planeNodeModelToWorldChangedSlot = _planeNode->data().propertyChanged("modelToWorldMatrix").connect(
        [this](Store&                           store,
               Provider::Ptr                    provider,
               const Provider::PropertyName&    propertyName)
        {
            transformChanged(provider->get<math::mat4>(propertyName));
        }
    );

    _provider = Provider::create();

    transformChanged(_planeNode->component<Transform>()->modelToWorldMatrix(true));

    auto cameraNode = NodePtr();

    if (_cameraNodeFunction)
    {
        cameraNode = _cameraNodeFunction(target);
    }
    else
    {
        auto cameraNodes = NodeSet::create(root)
            ->descendants(true)
            ->where([](Node::Ptr descendant) -> bool
            {
                return descendant->hasComponent<Camera>();
            });

        if (!cameraNodes->nodes().empty())
            cameraNode = cameraNodes->nodes().front();
    }

    if (!_depthRenderer)
        createRenderers(cameraNode);

    cameraNode->data().addProvider(_provider);

    _originNode->addChild(_planeNode);
    target->root()->addChild(_originNode);
}

void
ClippingPlane::enable(bool enabled)
{
    _planeNode->layout(enabled ? ClippingPlaneLayout::CLIPPING : 0u);

    _depthRenderer->enabled(enabled);
    _stencilRenderer->enabled(enabled);
}

void
ClippingPlane::transformChanged(const math::mat4& matrix)
{
    const auto planeBaseNormal = math::vec3(0.f, 0.f, 1.f);

    auto planeSurface = _planeNode->component<Surface>();

    const auto planeNormal = math::normalize(math::mat3(matrix) * planeBaseNormal);

    updatePlane(math::vec3(matrix[3]), planeNormal);
}

void
ClippingPlane::updatePlane(const math::vec3& position, const math::vec3& normal)
{
	auto plane = math::vec4();

	plane.x = normal.x;
	plane.y = normal.y;
	plane.z = normal.z;
	plane.w = (-(math::dot(normal, position)));

    setPlaneData(plane);
}

void
ClippingPlane::setPlaneData(const minko::math::vec4& plane)
{
    _provider->set(stringifiedPlaneId(), plane);
}

Geometry::Ptr
ClippingPlane::createDefaultPlaneGeometry()
{
    auto planeGeometry = QuadGeometry::create(_sceneManager->canvas()->context());

    return planeGeometry;
}

Material::Ptr
ClippingPlane::createDefaultPlaneMaterial()
{
    auto planeMaterial = BasicMaterial::create();

    static const auto colors = std::vector<math::vec4>{
        math::vec4(1.f, 0.f, 0.f, 1.f),
        math::vec4(0.f, 1.f, 0.f, 1.f),
        math::vec4(0.f, 0.f, 1.f, 1.f)
    };

	planeMaterial
        ->diffuseColor(colors[_planeId])
        ->triangleCulling(TriangleCulling::NONE)
        ->stencilFunction(CompareMode::EQUAL)
        ->stencilReference(1);

    return planeMaterial;
}

Effect::Ptr
ClippingPlane::createDefaultPlaneEffect()
{
    auto assetLibrary = _sceneManager->assets();

    auto planeEffect = assetLibrary->effect("effect/Stripes.effect");

    return planeEffect;
}

void
ClippingPlane::createRenderers(NodePtr cameraNode)
{
    auto assetLibrary = _sceneManager->assets();
    auto mainRenderer = cameraNode->component<Renderer>();

    _depthRenderer = Renderer::create(
        0x3e3e3eff,
        nullptr,
        assetLibrary->effect("effect/CrossSectionDepth.effect"),
        "default",
        10042.f,
        "depth-renderer"
    );

    _depthRenderer->layoutMask(ClippingPlaneLayout::CLIPPING);
    _depthRenderer->clearFlags(ClearFlags::COLOR | ClearFlags::DEPTH | ClearFlags::STENCIL);

    _stencilRenderer = Renderer::create(
        0x0,
        nullptr,
        assetLibrary->effect("effect/CrossSectionStencil.effect"),
        "default",
        10000.f,
        "stencil-renderer"
    );

    _stencilRenderer->layoutMask(ClippingPlaneLayout::CLIPPED);
    _stencilRenderer->clearFlags(ClearFlags::COLOR | ClearFlags::STENCIL);

    cameraNode
        ->addComponent(_depthRenderer)
        ->addComponent(_stencilRenderer);
}
