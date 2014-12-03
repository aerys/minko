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
#include "minko/render/Texture.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/input/Touch.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/component/Surface.hpp"
#include "minko/math/Vector4.hpp"

#include "minko/material/BasicMaterial.hpp"
#include "minko/component/Transform.hpp"

#include "minko/data/StructureProvider.hpp"
#include "minko/AbstractCanvas.hpp"

using namespace minko;
using namespace component;


Picking::Picking() :
    _sceneManager(nullptr),
    _context(nullptr),
    _mouse(nullptr),
    _touch(nullptr),
    _camera(nullptr),
    _pickingId(0),
    _pickingProjection(math::Matrix4x4::create()),
    _pickingProvider(data::StructureProvider::create("picking")),
    _mouseMove(Signal<NodePtr>::create()),
    _mouseLeftClick(Signal<NodePtr>::create()),
    _mouseRightClick(Signal<NodePtr>::create()),
    _mouseLeftDown(Signal<NodePtr>::create()),
    _mouseRightDown(Signal<NodePtr>::create()),
    _mouseLeftUp(Signal<NodePtr>::create()),
    _mouseRightUp(Signal<NodePtr>::create()),
    _mouseOut(Signal<NodePtr>::create()),
    _mouseOver(Signal<NodePtr>::create()),
    _touchDown(Signal<NodePtr>::create()),
    _touchMove(Signal<NodePtr>::create()),
    _touchUp(Signal<NodePtr>::create()),
    _tap(Signal<NodePtr>::create()),
    _doubleTap(Signal<NodePtr>::create()),
    _longHold(Signal<NodePtr>::create()),
    _addPickingLayout(true),
    _emulateMouseWithTouch(true)
{
}

void
Picking::initialize(NodePtr             camera,
                    bool                addPickingLayout, 
                    bool                emulateMouseWithTouch)
{
    _camera = camera;
    _emulateMouseWithTouch = emulateMouseWithTouch;
    _addPickingLayout = addPickingLayout;

    _pickingProvider->set("projection", _pickingProjection);

    _targetAddedSlot = targetAdded()->connect(std::bind(
        &Picking::targetAddedHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));

    _targetRemovedSlot = targetRemoved()->connect(std::bind(
        &Picking::targetRemovedHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));
}

void
Picking::bindSignals()
{
    _mouseMoveSlot = _mouse->move()->connect(std::bind(
        &Picking::mouseMoveHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _mouseLeftDownSlot = _mouse->leftButtonDown()->connect(std::bind(
        &Picking::mouseLeftDownHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1));

    _mouseRightDownSlot = _mouse->rightButtonDown()->connect(std::bind(
        &Picking::mouseRightDownHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1));

    _mouseLeftClickSlot = _mouse->leftButtonClick()->connect(std::bind(
        &Picking::mouseLeftClickHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1));

    _mouseRightClickSlot = _mouse->rightButtonClick()->connect(std::bind(
        &Picking::mouseRightClickHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1));

    _mouseLeftUpSlot = _mouse->leftButtonUp()->connect(std::bind(
        &Picking::mouseLeftUpHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1));

    _mouseRightUpSlot = _mouse->rightButtonUp()->connect(std::bind(
        &Picking::mouseRightUpHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1));
    
    _touchDownSlot = _touch->touchDown()->connect(std::bind(
        &Picking::touchDownHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));
    
    _touchUpSlot = _touch->touchUp()->connect(std::bind(
        &Picking::touchUpHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));
    
    _touchMoveSlot = _touch->touchMove()->connect(std::bind(
        &Picking::touchMoveHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));
    
    _touchTapSlot = _touch->tap()->connect(std::bind(
        &Picking::touchTapHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3));
    
    _touchDoubleTapSlot = _touch->doubleTap()->connect(std::bind(
        &Picking::touchDoubleTapHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3));
    
    _touchLongHoldSlot = _touch->longHold()->connect(std::bind(
        &Picking::touchLongHoldHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3));

        _executeMoveHandler = false;
        _executeRightClickHandler = false;
        _executeLeftClickHandler = false;
        _executeRightDownHandler = false;
        _executeLeftDownHandler = false;
        _executeRightUpHandler = false;
        _executeLeftUpHandler = false;
        _executeTouchDownHandler = false;
        _executeTouchUpHandler = false;
        _executeTouchMoveHandler = false;
        _executeTapHandler = false;
        _executeDoubleTapHandler = false;
        _executeLongHoldHandler = false;
}

void
Picking::targetAddedHandler(AbsCtrlPtr ctrl, NodePtr target)
{
    if (numTargets() > 1)
        throw std::logic_error("The Picking component cannot be added to two targets");

    _sceneManager = target->root()->component<SceneManager>();
    auto canvas = _sceneManager->canvas();
    
    _mouse = canvas->mouse();
    _touch = canvas->touch();
    _context = canvas->context();

    bindSignals();
    
    _renderer = Renderer::create(0xFFFF00FF, nullptr, _sceneManager->assets()->effect("effect/Picking.effect"), 1000.f, "Picking Renderer");
    _renderer->scissor(0, 0, 1, 1);
    _renderer->layoutMask(scene::Layout::Group::PICKING);
    
    updateDescendants(target);

    _addedSlot = target->added()->connect(std::bind(
        &Picking::addedHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _removedSlot = target->removed()->connect(std::bind(
        &Picking::removedHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    if (target->parent() != nullptr || target->hasComponent<SceneManager>())
        addedHandler(target, target, target->parent());

    target->addComponent(_renderer);

    auto perspectiveCamera = _camera->component<component::PerspectiveCamera>();

    target->data()->addProvider(_pickingProvider);
    target->data()->addProvider(perspectiveCamera->data());

    addSurfacesForNode(target);
}

void
Picking::targetRemovedHandler(AbsCtrlPtr ctrl, NodePtr target)
{
    _renderer = nullptr;
    _sceneManager = nullptr;

    _addedSlot = nullptr;
    _removedSlot = nullptr;

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
            std::placeholders::_1));

        _renderingEndSlot = _renderer->beforePresent()->connect(std::bind(
            &Picking::renderingEnd,
            std::static_pointer_cast<Picking>(shared_from_this()),
            std::placeholders::_1));

        _componentAddedSlot = child->componentAdded()->connect(std::bind(
            &Picking::componentAddedHandler,
            std::static_pointer_cast<Picking>(shared_from_this()),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        ));

        _componentRemovedSlot = child->componentRemoved()->connect(std::bind(
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
Picking::componentAddedHandler(NodePtr                                target,
                               NodePtr                                node,
                               std::shared_ptr<AbstractComponent>    ctrl)
{
    if (std::find(_descendants.begin(), _descendants.end(), node) == _descendants.end())
        return;

    auto surfaceCtrl = std::dynamic_pointer_cast<Surface>(ctrl);

    if (surfaceCtrl)
        addSurface(surfaceCtrl);
}

void
Picking::componentRemovedHandler(NodePtr                            target,
                                 NodePtr                            node,
                                 std::shared_ptr<AbstractComponent>    ctrl)
{
    if (std::find(_descendants.begin(), _descendants.end(), node) == _descendants.end())
        return;

    auto surfaceCtrl = std::dynamic_pointer_cast<Surface>(ctrl);
    auto sceneManager = std::dynamic_pointer_cast<SceneManager>(ctrl);

    if (surfaceCtrl)
        removeSurface(surfaceCtrl, node);

    if (!node->hasComponent<Surface>() && _addPickingLayout)
        node->layouts(node->layouts() & ~scene::Layout::Group::PICKING);
}

void
Picking::addSurface(SurfacePtr surface)
{
    if (_surfaceToPickingId.find(surface) == _surfaceToPickingId.end())
    {
        _pickingId += 2;

        _surfaceToPickingId[surface] = _pickingId;
        _pickingIdToSurface[_pickingId] = surface;

        _surfaceToProvider[surface] = data::StructureProvider::create("picking");

        _surfaceToProvider[surface]->set<math::Vector4::Ptr>("color", math::Vector4::create(
            ((_pickingId >> 16) & 0xff) / 255.f,
            ((_pickingId >> 8) & 0xff) / 255.f,
            ((_pickingId)& 0xff) / 255.f,
            1
            ));


        auto target = surface->targets()[0];

        if (_targetToProvider.find(target) == _targetToProvider.end())
        {
            _targetToProvider[target] = _surfaceToProvider[surface];

            target->data()->addProvider(_surfaceToProvider[surface]);
        }

        if (_addPickingLayout)
            target->layouts(target->layouts() | scene::Layout::Group::PICKING);
    }
}

void
Picking::removeSurface(SurfacePtr surface, NodePtr node)
{
    if (_surfaceToPickingId.find(surface) == _surfaceToPickingId.end())
        return;

    auto surfacePickingId = _surfaceToPickingId[surface];

    if (_surfaceToProvider.find(surface) == _surfaceToProvider.end())
    {
        node->data()->removeProvider(_surfaceToProvider[surface]);

        if (_targetToProvider[node] == _surfaceToProvider[surface])
            _targetToProvider.erase(node);

        _surfaceToProvider.erase(surface);
    }

    _surfaceToPickingId.erase(surface);
    _pickingIdToSurface.erase(surfacePickingId);
}

void
Picking::removedHandler(NodePtr target, NodePtr child, NodePtr parent)
{
    if (std::find(_descendants.begin(), _descendants.end(), child) == _descendants.end())
        return;

    if (target == child)
    {
        _renderingBeginSlot = nullptr;
        _renderingEndSlot = nullptr;
    }

    removeSurfacesForNode(child);

    updateDescendants(target);
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
        surfaceNode->layouts(surfaceNode->layouts() & ~scene::Layout::Group::PICKING);

        for (auto surface : surfaceNode->components<Surface>())
            removeSurface(surface, surfaceNode);
    }
}

void
Picking::renderingBegin(RendererPtr renderer)
{
    float mouseX = (float)_mouse->x();
    float mouseY = (float)_mouse->y();

    auto perspectiveCamera    = _camera->component<component::PerspectiveCamera>();
    auto projection            = math::Matrix4x4::create()->perspective(perspectiveCamera->fieldOfView(), perspectiveCamera->aspectRatio(), perspectiveCamera->zNear(), perspectiveCamera->zFar());

    std::vector<float> pickingProjectionData(projection->data());

    pickingProjectionData[2] = mouseX / _context->viewportWidth() * 2.f;
    pickingProjectionData[6] = (_context->viewportHeight() - mouseY) / _context->viewportHeight() * 2.f;

    _pickingProjection->initialize(pickingProjectionData);
}

void
Picking::renderingEnd(RendererPtr renderer)
{
    _context->readPixels(0, 0, 1, 1, &_lastColor[0]);

    uint pickedSurfaceId = (_lastColor[0] << 16) + (_lastColor[1] << 8) + _lastColor[2];

    if (_lastPickedSurface != _pickingIdToSurface[pickedSurfaceId])
    {
        if (_lastPickedSurface && _mouseOut->numCallbacks() > 0)
            _mouseOut->execute(_lastPickedSurface->targets()[0]);

        _lastPickedSurface = _pickingIdToSurface[pickedSurfaceId];

        if (_lastPickedSurface && _mouseOver->numCallbacks() > 0)
            _mouseOver->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeMoveHandler && _lastPickedSurface)
    {
        _mouseMove->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeRightDownHandler && _lastPickedSurface)
    {
        _mouseRightDown->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeLeftDownHandler && _lastPickedSurface)
    {
        _mouseLeftDown->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeRightClickHandler && _lastPickedSurface)
    {
        _mouseRightClick->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeLeftClickHandler && _lastPickedSurface)
    {
        _mouseLeftClick->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeRightUpHandler && _lastPickedSurface)
    {
        _mouseRightUp->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeLeftUpHandler && _lastPickedSurface)
    {
        _mouseLeftUp->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeTouchDownHandler && _lastPickedSurface)
    {
        _touchDown->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeTouchUpHandler && _lastPickedSurface)
    {
        _touchUp->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeTouchMoveHandler && _lastPickedSurface)
    {
        _touchMove->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeTapHandler && _lastPickedSurface)
    {
        _tap->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeDoubleTapHandler && _lastPickedSurface)
    {
        _doubleTap->execute(_lastPickedSurface->targets()[0]);
    }

    if (_executeLongHoldHandler && _lastPickedSurface)
    {
        _longHold->execute(_lastPickedSurface->targets()[0]);
    }

    if (!(_mouseOver->numCallbacks() > 0 || _mouseOut->numCallbacks() > 0))
        _renderer->enabled(false);

    _executeMoveHandler = false;
    _executeRightDownHandler = false;
    _executeLeftDownHandler = false;
    _executeRightClickHandler = false;
    _executeLeftClickHandler = false;
    _executeRightUpHandler = false;
    _executeLeftUpHandler = false;
}

void
Picking::mouseMoveHandler(MousePtr mouse, int dx, int dy)
{
    if (_mouseOver->numCallbacks() > 0 || _mouseOut->numCallbacks() > 0)
    {
        _executeMoveHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::mouseRightUpHandler(MousePtr mouse)
{
    if (_mouseRightUp->numCallbacks() > 0)
    {
        _executeRightUpHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::mouseLeftUpHandler(MousePtr mouse)
{
    if (_mouseLeftUp->numCallbacks() > 0)
    {
        _executeLeftUpHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::mouseRightClickHandler(MousePtr mouse)
{
    if (_mouseRightClick->numCallbacks() > 0)
    {
        _executeRightClickHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::mouseLeftClickHandler(MousePtr mouse)
{
    if (_mouseLeftClick->numCallbacks() > 0)
    {
        _executeLeftClickHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::mouseRightDownHandler(MousePtr mouse)
{

    if (_mouseRightDown->numCallbacks() > 0)
    {
        _executeRightDownHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::mouseLeftDownHandler(MousePtr mouse)
{
    if (_mouseLeftDown->numCallbacks() > 0)
    {
        _executeLeftDownHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::touchDownHandler(TouchPtr touch, int identifier, float x, float y)
{
    if (_touchDown->numCallbacks() > 0)
    {
        _executeTouchDownHandler = true;
        _renderer->enabled(true);
    }
    if (_emulateMouseWithTouch && _touch->numTouches() == 1 && _mouseLeftDown->numCallbacks() > 0)
    {
        _executeLeftDownHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::touchUpHandler(TouchPtr touch, int identifier, float x, float y)
{
    if (_touchUp->numCallbacks() > 0)
    {
        _executeTouchUpHandler = true;
        _renderer->enabled(true);
    }
    if (_emulateMouseWithTouch && _touch->numTouches() == 1 && _mouseLeftUp->numCallbacks() > 0)
    {
        _executeLeftUpHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::touchMoveHandler(TouchPtr touch, int identifier, float x, float y)
{
    if (_touchMove->numCallbacks() > 0)
    {
        _executeTouchMoveHandler = true;
        _renderer->enabled(true);
    }
    if (_emulateMouseWithTouch && _touch->numTouches() == 1 && _mouseMove->numCallbacks() > 0)
    {
        _executeMoveHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::touchTapHandler(TouchPtr touch, float x, float y)
{
    if (_tap->numCallbacks() > 0)
    {
        _executeTapHandler = true;
        _renderer->enabled(true);
    }
    if (_emulateMouseWithTouch && _mouseLeftClick->numCallbacks() > 0)
    {
        _executeLeftClickHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::touchDoubleTapHandler(TouchPtr touch, float x, float y)
{
    if (_doubleTap->numCallbacks() > 0)
    {
        _executeDoubleTapHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::touchLongHoldHandler(TouchPtr touch, float x, float y)
{
    if (_doubleTap->numCallbacks() > 0)
    {
        _executeDoubleTapHandler = true;
        _renderer->enabled(true);
    }
    if (_emulateMouseWithTouch && _mouseRightClick->numCallbacks() > 0)
    {
        _executeRightClickHandler = true;
        _renderer->enabled(true);
    }
}

void
Picking::updateDescendants(NodePtr target)
{
    auto nodeSet = scene::NodeSet::create(target)->descendants(true);

    _descendants = nodeSet->nodes();
}