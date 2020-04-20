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

#include "minko/component/Picking.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Camera.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/Texture.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Transform.hpp"
#include "minko/data/Provider.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/math/Ray.hpp"
#include "minko/log/Logger.hpp"

#ifdef MINKO_USE_SPARSE_HASH_MAP
# include "sparsehash/sparse_hash_map"
#endif

using namespace minko;
using namespace minko::component;

Picking::Picking(int priority) :
    AbstractPicking(priority),
    _renderTarget(),
    _renderer(),
    _sceneManager(),
    _camera(),
    _pickingProjection(),
    _surfaceToPickingIds(),
    _pickingIdToSurface(),
    _pickingId(0),
    _context(),
    _pickingProvider(data::Provider::create()),
    _layout(scene::BuiltinLayout::PICKING),
    _depthLayout(scene::BuiltinLayout::PICKING_DEPTH),
    _pickingEffect(nullptr),
    _pickingDepthEffect(nullptr),
    _addPickingLayout(true),
    _frameBeginSlot(nullptr),
    _enabled(true),
    _running(false),
    _renderDepth(true),
    _debug(false),
    _multiselecting(false),
    _multiselectionStartPosition()
{
}

Picking::Ptr
Picking::layout(scene::Layout value)
{
    if (_layout == value)
        return std::static_pointer_cast<Picking>(shared_from_this());

    const auto previousValue = _layout;
    _layout = value;

    if (_renderer)
        _renderer->layoutMask(_layout);

    pickingLayoutChanged(previousValue, value);

    return std::static_pointer_cast<Picking>(shared_from_this());
}

Picking::Ptr
Picking::depthLayout(scene::Layout value)
{
    if (_depthLayout == value)
        return std::static_pointer_cast<Picking>(shared_from_this());

    _depthLayout = value;

    if (_depthRenderer)
        _depthRenderer->layoutMask(_depthLayout);

    return std::static_pointer_cast<Picking>(shared_from_this());
}

void
Picking::initialize(NodePtr             camera,
                    bool                addPickingLayout,
                    EffectPtr           pickingEffect,
                    EffectPtr           pickingDepthEffect)
{
    _camera = camera;
    _addPickingLayout = addPickingLayout;
    _pickingEffect = pickingEffect;
    _pickingDepthEffect = pickingDepthEffect;

    _pickingProvider->set("pickingProjection", _pickingProjection);
    _pickingProvider->set("pickingOrigin", math::vec3());
}

void
Picking::unbindSignals()
{
    _frameBeginSlot = nullptr;
    _renderingBeginSlot = nullptr;
    _renderingEndSlot = nullptr;
    _depthRenderingBeginSlot = nullptr;
    _depthRenderingEndSlot = nullptr;
    _componentAddedSlot = nullptr;
    _componentRemovedSlot = nullptr;

    _addedSlot = nullptr;
    _removedSlot = nullptr;
}

static
void
initDebugQuad(scene::Node::Ptr target,
              SceneManager::Ptr sceneManager,
              render::Effect::Ptr pickingEffect,
              Renderer::Ptr pickingRenderer,
              scene::Node::Ptr& debugQuadNode,
              Renderer::Ptr& debugRenderer)
{
    const auto& viewport = target->root()->data().get<math::vec4>("viewport");
    const auto size = math::uvec2(viewport.z, viewport.w);
    const auto renderTarget = minko::render::Texture::create(
        sceneManager->assets()->context(),
        size.x,
        size.y,
        false,
        true,
        true,
        minko::render::TextureFormat::RGBA
    );
    renderTarget->upload();

    const auto backgroundEffect = sceneManager->assets()->effect("effect/Background.effect");
    debugRenderer = minko::component::Renderer::create(
        0xFFFF00FF,
        renderTarget,
        pickingEffect,
        "default",
        -10000.f,
        "pickingDebug"
    );
    debugRenderer->layoutMask(pickingRenderer->layoutMask());
    debugRenderer->enabled(false);
    target->addComponent(debugRenderer);

    const auto debugQuadSurface = minko::component::Surface::create(
        minko::geometry::QuadGeometry::create(sceneManager->assets()->context()),
        minko::material::Material::create(),
        backgroundEffect
    );

    debugQuadSurface->material()->data()->set({
        { "diffuseMap", renderTarget->sampler() },
        { "depthFunction", render::CompareMode::ALWAYS },
        { "priority", render::Priority::LAST - 1.f },
        { "invertV", true },
        { "triangleCulling", render::TriangleCulling::NONE },
        { "cameraRatio", viewport.z / viewport.w }
    });

    debugQuadNode = minko::scene::Node::create("debug")
        ->addComponent(minko::component::Transform::create())
        ->addComponent(debugQuadSurface);
    target->root()->addChild(debugQuadNode);
}

void
Picking::targetAdded(NodePtr target)
{
    _sceneManager = target->root()->component<SceneManager>();
    auto canvas = _sceneManager->canvas();

    _context = canvas->context();

    if (_pickingEffect == nullptr)
        _pickingEffect = _sceneManager->assets()->effect("effect/Picking.effect");

    auto priority = 1000.0f;
    auto pickingRendererColor = 0x000000FF;

    _renderer = Renderer::create(
        pickingRendererColor,
        nullptr,
        _pickingEffect,
        "default",
        priority,
        "Picking Renderer"
    );

    _renderer->scissorBox(0, 0, 1, 1);
    _renderer->enabled(false);

    _renderer->layoutMask(_layout);

    if (_pickingDepthEffect == nullptr)
        _pickingDepthEffect = _sceneManager->assets()->effect("effect/PickingDepth.effect");

    _depthRenderer = Renderer::create(
        pickingRendererColor,
        nullptr,
        _pickingDepthEffect,
        "default",
        999.f,
        "Depth Picking Renderer"
    );
    _depthRenderer->scissorBox(0, 0, 1, 1);
    _depthRenderer->layoutMask(_depthLayout);
    _depthRenderer->enabled(false);

    if (_debug)
        initDebugQuad(target, _sceneManager, _pickingEffect, _renderer, _debugQuad, _debugRenderer);

    updateDescendants(target);

    _addedSlot = target->added().connect(std::bind(
        &Picking::addedHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _removedSlot = target->removed().connect(std::bind(
        &Picking::removedHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    if (target->parent() != nullptr || target->hasComponent<SceneManager>())
        addedHandler(target, target, target->parent());

    target->addComponent(_renderer);
    target->addComponent(_depthRenderer);

    auto perspectiveCamera = _camera->component<component::Camera>();

    target->data().addProvider(_pickingProvider);
    target->data().addProvider(perspectiveCamera->data());

    addSurfacesForNode(target);
}

void
Picking::targetRemoved(NodePtr target)
{
    unbindSignals();

    if (target->hasComponent(_renderer))
        target->removeComponent(_renderer);
    if (target->hasComponent(_depthRenderer))
        target->removeComponent(_depthRenderer);

    _renderer = nullptr;
    _depthRenderer = nullptr;
    _sceneManager = nullptr;
    _running = false;

    removedHandler(target->root(), target, target->parent());
}

void
Picking::addedHandler(NodePtr target, NodePtr child, NodePtr parent)
{
    updateDescendants(target);

    if (std::find(_descendants.begin(), _descendants.end(), child) == _descendants.end())
        return;

    if (child == target && _renderingBeginSlot == nullptr)
    {
        _renderingBeginSlot = _renderer->renderingBegin()->connect(std::bind(
            &Picking::renderingBegin,
            std::static_pointer_cast<Picking>(shared_from_this()),
            std::placeholders::_1
        ));

        _renderingEndSlot = _renderer->beforePresent()->connect(std::bind(
            &Picking::renderingEnd,
            std::static_pointer_cast<Picking>(shared_from_this()),
            std::placeholders::_1
        ));

        _depthRenderingBeginSlot = _depthRenderer->renderingBegin()->connect(std::bind(
            &Picking::depthRenderingBegin,
            std::static_pointer_cast<Picking>(shared_from_this()),
            std::placeholders::_1
        ));

        _depthRenderingEndSlot = _depthRenderer->beforePresent()->connect(std::bind(
            &Picking::depthRenderingEnd,
            std::static_pointer_cast<Picking>(shared_from_this()),
            std::placeholders::_1
        ));

        _componentAddedSlot = child->componentAdded().connect(std::bind(
            &Picking::componentAddedHandler,
            std::static_pointer_cast<Picking>(shared_from_this()),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        ));

        _componentRemovedSlot = child->componentRemoved().connect(std::bind(
            &Picking::componentRemovedHandler,
            std::static_pointer_cast<Picking>(shared_from_this()),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        ));
    }

    if (std::find(_descendants.begin(), _descendants.end(), child) != _descendants.end())
        addSurfacesForNode(child);
}

void
Picking::componentAddedHandler(NodePtr								target,
                               NodePtr								node,
                               std::shared_ptr<AbstractComponent>	ctrl)
{
    if (std::find(_descendants.begin(), _descendants.end(), node) == _descendants.end())
        return;

    auto surfaceCtrl = std::dynamic_pointer_cast<Surface>(ctrl);

    if (surfaceCtrl)
        addSurface(surfaceCtrl);
}

void
Picking::componentRemovedHandler(NodePtr							target,
                                 NodePtr							node,
                                 std::shared_ptr<AbstractComponent>	ctrl)
{
    if (std::find(_descendants.begin(), _descendants.end(), node) == _descendants.end())
        return;

    auto surfaceCtrl = std::dynamic_pointer_cast<Surface>(ctrl);
    auto sceneManager = std::dynamic_pointer_cast<SceneManager>(ctrl);

    if (surfaceCtrl)
        removeSurface(surfaceCtrl, node);

    if (!node->hasComponent<Surface>() && _addPickingLayout)
        node->layout(node->layout() & ~_layout);
}

void
Picking::addSurface(SurfacePtr surface)
{
    if (_surfaceToPickingIds.find(surface) == _surfaceToPickingIds.end())
    {
        auto pickingId = createPickingId(surface);

        surface->data()->set("pickingColor", pickingId.second);

        if (_addPickingLayout)
            surface->target()->layout(target()->layout() | _layout);

        surface->layoutMask(surface->layoutMask() & ~_depthLayout);
    }
}

void
Picking::removeSurface(SurfacePtr surface, NodePtr node)
{
    if (_surfaceToPickingIds.find(surface) == _surfaceToPickingIds.end())
        return;

    surface->data()->unset("pickingColor");

    const auto& surfacePickingIds = _surfaceToPickingIds[surface];

    for (const auto& pickingId : surfacePickingIds)
        _pickingIdToSurface.erase(pickingId);

    _surfaceToPickingIds.erase(surface);
}

void
Picking::removedHandler(NodePtr target, NodePtr child, NodePtr parent)
{
    if (std::find(_descendants.begin(), _descendants.end(), child) == _descendants.end())
        return;

    removeSurfacesForNode(child);

    updateDescendants(target);
}

void
Picking::enabled(bool enabled)
{
    if (_running && !enabled)
        running(enabled);
    _enabled = enabled;
}

void
Picking::addSurfacesForNode(NodePtr node)
{
    auto surfaces = scene::NodeSet::create(node)
        ->descendants(true)
        ->where([](scene::Node::Ptr node)
    {
        return node->hasComponent<Surface>();
    });

    for (auto surfaceNode : surfaces->nodes())
        for (auto surface : surfaceNode->components<Surface>())
            addSurface(surface);
}

void
Picking::removeSurfacesForNode(NodePtr node)
{
    auto surfaces = scene::NodeSet::create(node)
        ->descendants(true)
        ->where([](scene::Node::Ptr node)
    {
        return node->hasComponent<Surface>();
    });

    for (auto surfaceNode : surfaces->nodes())
    {
        if (_addPickingLayout)
            surfaceNode->layout(surfaceNode->layout() & ~_layout);

        for (auto surface : surfaceNode->components<Surface>())
            removeSurface(surface, surfaceNode);
    }
}

void
Picking::updateDescendants(NodePtr target)
{
    auto nodeSet = scene::NodeSet::create(target)->descendants(true);

    _descendants = nodeSet->nodes();
}

void
Picking::running(bool running)
{
    if (running && !_frameBeginSlot)
    {
        _running = true;
        _frameBeginSlot = _sceneManager->frameBegin()->connect(std::bind(
            &Picking::frameBeginHandler,
            std::static_pointer_cast<Picking>(shared_from_this()),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        ), 1000.0f);
    }
    else if (!running && _frameBeginSlot != nullptr)
    {
        _frameBeginSlot = nullptr;
    }

    _running = running;
}

void
Picking::frameBeginHandler(SceneManagerPtr, float, float)
{
    renderPickingFrame();
}

void
Picking::renderingBegin(RendererPtr renderer)
{
    if (!_running)
        return;

    updatePickingProjection();
}

void
Picking::renderPickingFrame()
{
    _renderer->enabled(true);
    _renderer->render(_sceneManager->canvas()->context());
    _renderer->enabled(false);

    if (_debug)
    {
        _debugRenderer->enabled(true);
        _debugRenderer->render(_sceneManager->canvas()->context());
        _debugRenderer->enabled(false);
    }

    _sceneManager->forceRenderNextFrame();
}

void
Picking::renderingEnd(RendererPtr renderer)
{
    if (!_running)
        return;

    _context->readPixels(0, 0, 1, 1, &_lastColor[0]);

    uint pickedSurfaceId = (_lastColor[0] << 16) + (_lastColor[1] << 8) + _lastColor[2];

    auto surfaceIt = _pickingIdToSurface.find(pickedSurfaceId);

    if (surfaceIt != _pickingIdToSurface.end())
    {
        _lastPickedSurfaceId = pickedSurfaceId;
        auto pickedSurface = surfaceIt->second;

        if (_renderDepth)
            renderDepth(_depthRenderer, pickedSurface);
        else
            dispatchEvents(pickedSurface);
    }
    else
    {
        dispatchEvents(nullptr);
    }
}

void
Picking::renderDepth(RendererPtr renderer, SurfacePtr pickedSurface)
{
    if (!_running)
        return;

    auto pickedSurfaceTarget = pickedSurface->target();

    pickedSurfaceTarget->layout(pickedSurfaceTarget->layout() | _depthLayout);
    pickedSurface->layoutMask(pickedSurface->layoutMask() | _depthLayout);

    renderer->enabled(true);
    renderer->render(_sceneManager->canvas()->context());
    renderer->enabled(false);

    pickedSurfaceTarget->layout(pickedSurfaceTarget->layout() & ~_depthLayout);
    pickedSurface->layoutMask(pickedSurface->layoutMask() & ~_depthLayout);
}

static
float
unpack(const math::vec4& depth)
{
    return math::dot(depth, math::vec4(1.0, 1.0 / 255.0, 1.0 / 65025.0, 1.0 / 16581375.0));
}

static
float
unpack(const math::vec3& depth)
{
    return math::dot(depth, math::vec3(1.0, 1.0 / 255.0, 1.0 / 65025.0));
}

void
Picking::depthRenderingBegin(RendererPtr renderer)
{
    if (!_running)
        return;

    updatePickingOrigin();
}

void
Picking::depthRenderingEnd(RendererPtr renderer)
{
    if (!_running)
        return;

    uint pickedSurfaceId = (_lastColor[0] << 16) + (_lastColor[1] << 8) + _lastColor[2];

    auto surfaceIt = _pickingIdToSurface.find(pickedSurfaceId);

    if (surfaceIt != _pickingIdToSurface.end())
    {
        auto pickedSurface = surfaceIt->second;

        _context->readPixels(0, 0, 1, 1, &_lastDepth[0]);

        const auto zNear = _camera->data().get<float>("zNear");
        const auto zFar = _camera->data().get<float>("zFar");

        const auto normalizedDepth = unpack(math::vec3(_lastDepth[0], _lastDepth[1], _lastDepth[2]) / 255.f) * (zFar - zNear);

        _lastDepthValue = normalizedDepth;
        _lastMergingMask = _lastDepth[3];

        dispatchEvents(pickedSurface);
    }
}

void
Picking::updatePickingProjection()
{
    auto mouseX = _singleSelectionPosition.x;
    auto mouseY = _singleSelectionPosition.y;

    if (_multiselecting && _multiselectionStartPosition != math::vec2(0))
    {
        mouseX = _multiselectionStartPosition.x;
        mouseY = _multiselectionStartPosition.y;
    }

    auto perspectiveCamera	= _camera->component<component::Camera>();
    auto projection	= perspectiveCamera->projectionMatrix();

    math::vec2 normalizedMouse(mouseX / _context->viewportWidth() * 2.f, (_context->viewportHeight() - mouseY) / _context->viewportHeight() * 2.f);
    projection[2][0] = -normalizedMouse.x * projection[2][3];
    projection[2][1] = -normalizedMouse.y * projection[2][3];

    projection[3][0] = -normalizedMouse.x * projection[3][3];
    projection[3][1] = -normalizedMouse.y * projection[3][3];

    _pickingProvider->set("pickingProjection", projection);
}

void
Picking::updatePickingOrigin()
{
    auto perspectiveCamera	= _camera->component<component::Camera>();

    const auto normalizedMouseX = _singleSelectionNormalizedPosition.x;
    const auto normalizedMouseY = _singleSelectionNormalizedPosition.y;

    auto pickingRay = perspectiveCamera->unproject(normalizedMouseX, normalizedMouseY);

    _pickingProvider->set("pickingOrigin", pickingRay->origin());
}

void
Picking::dispatchEvents(SurfacePtr pickedSurface)
{
    _pickingComplete->execute(std::dynamic_pointer_cast<AbstractPicking>(shared_from_this()), pickedSurface);
    running(false);
}

void
Picking::pick(const minko::math::vec2& point, const minko::math::vec2& normalizedPoint)
{
    _singleSelectionPosition = point;
    _singleSelectionNormalizedPosition = normalizedPoint;
    running(true);
}

AbstractPicking::map<scene::Node::Ptr, std::set<unsigned char>>
Picking::pickArea(const minko::math::vec2& bottomLeft, const minko::math::vec2& topRight, bool fullyInside)
{
    auto pickedNodes = Picking::map<scene::Node::Ptr, std::set<unsigned char>>();

    auto width = static_cast<int>(topRight.x - bottomLeft.x);
    auto height = static_cast<int>(bottomLeft.y - topRight.y);
    auto singleClick = false;

    // Handle single click
    if (width == 0 && height == 0)
        singleClick = true;

    // Make sure the area is not null
    if (width == 0)
        width = 1;

    if (height == 0)
        height = 1;

    _multiselecting = true;
    _multiselectionStartPosition = bottomLeft;

    // Change the scissor box size and make sure to update the projection
    _renderer->scissorBox(0, 0, width, height);
    updatePickingProjection();

    // Force picking renderer to render a frame
    renderPickingFrame();

    // Make sure to reset the scissor box size
    _renderer->scissorBox(0, 0, 1, 1);

    // Read and store all pixels in the selection area
    auto pixelSize = 4;
    std::vector<unsigned char> selectAreaPixelBuffer(pixelSize * width * height);
    _context->readPixels(0, 0, width, height, &selectAreaPixelBuffer[0]);

    // Retrieve all surface ids in the selection area
    auto maxSurfaceId = 0;

    if (!_pickingIdToSurface.empty())
        maxSurfaceId = _pickingIdToSurface.rbegin()->first;

    uint lastPickedSurfaceId = 0;
    unsigned char lastAlphaValue = 0;
    auto elementsToRemove = map<scene::Node::Ptr, std::set<unsigned char>>();

    for (int i = 0; i < static_cast<int>(selectAreaPixelBuffer.size()); i += pixelSize)
    {
        auto currentPixel = &selectAreaPixelBuffer[i];
        uint pickedSurfaceId = (currentPixel[0] << 16) + (currentPixel[1] << 8) + currentPixel[2];
        auto alpha = currentPixel[3];

        if ((lastPickedSurfaceId != pickedSurfaceId || lastAlphaValue != alpha || (fullyInside && !singleClick)) && pickedSurfaceId <= maxSurfaceId)
        {
            lastPickedSurfaceId = pickedSurfaceId;
            lastAlphaValue = alpha;

            auto surfaceIt = _pickingIdToSurface.find(pickedSurfaceId);

            if (surfaceIt != _pickingIdToSurface.end())
            {
                auto pickedSurface = surfaceIt->second;

                if (fullyInside && !singleClick)
                {
                    auto pixelBufferWidth = pixelSize * width;

                    // Check that the read pixel is not on the edge of the picking rendering
                    if ((
                        i <= pixelBufferWidth ||                                // Bottom border
                        i >= (pixelSize * width * height) - pixelBufferWidth || // Top border
                        i % pixelBufferWidth == 0 ||                            // Right border
                        i % pixelBufferWidth == pixelBufferWidth - pixelSize    // Left border
                        ))
                    {
                        // Store the combinaison node / alpha value to remove afterward
                        auto surfaceAlphaValuesToRemove = elementsToRemove.find(pickedSurface->target());
                        if (surfaceAlphaValuesToRemove == elementsToRemove.end())
                            elementsToRemove.insert(std::make_pair(pickedSurface->target(), std::set<unsigned char> { alpha }));
                        else
                            surfaceAlphaValuesToRemove->second.insert(alpha);

                        continue;
                    }
                }

                // Store the combinaison node / alpha value to return as picked
                auto surfaceAlphaValues = pickedNodes.find(pickedSurface->target());
                if (surfaceAlphaValues == pickedNodes.end())
                    pickedNodes.insert(std::make_pair(pickedSurface->target(), std::set<unsigned char> { alpha }));
                else
                    surfaceAlphaValues->second.insert(alpha);
            }
        }
    }

    if (fullyInside && !singleClick)
    {
        for (const auto& element : elementsToRemove)
        {
            auto& subSurfaces = pickedNodes[element.first];

            for (const auto& a : element.second)
                subSurfaces.erase(a);

            if (subSurfaces.empty())
                pickedNodes.erase(element.first);
        }
    }

    _multiselecting = false;
    return pickedNodes;
}

void
Picking::pickingLayoutChanged(scene::Layout previousValue, scene::Layout value)
{
    if (!_addPickingLayout)
        return;

    // Update automatically assigned layout

    for (auto node : _descendants)
    {
        auto nodeLayout = node->layout();

        nodeLayout = nodeLayout & ~previousValue;
        nodeLayout = nodeLayout | value;

        node->layout(nodeLayout);
    }
}

std::pair<uint, math::vec4>
Picking::createPickingId(SurfacePtr surface)
{
    _pickingId = _pickingId + (_debug ? 5000 : 1);

    _surfaceToPickingIds[surface].push_back(_pickingId);
    _pickingIdToSurface[_pickingId] = surface;

    math::vec4 color(
        ((_pickingId >> 16) & 0xff) / 255.f,
        ((_pickingId >> 8) & 0xff) / 255.f,
        ((_pickingId)& 0xff) / 255.f,
        1
    );

    return std::make_pair(_pickingId, color);
}