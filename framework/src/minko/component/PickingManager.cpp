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

#include "minko/component/PickingManager.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Camera.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/input/Touch.hpp"
#include "minko/component/Surface.hpp"
#include "minko/data/Provider.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/math/Ray.hpp"
#include "minko/log/Logger.hpp"

#ifdef MINKO_USE_SPARSE_HASH_MAP
# include "sparsehash/sparse_hash_map"
#endif

using namespace minko;
using namespace minko::component;

PickingManager::PickingManager() :
    _sceneManager(),
    _mouse(),
    _touch(),
    _camera(),
    _context(),
    _mouseMove(Signal<NodePtr>::create()),
    _mouseLeftClick(Signal<NodePtr>::create()),
    _mouseRightClick(Signal<NodePtr>::create()),
    _mouseLeftDown(Signal<NodePtr>::create()),
    _mouseRightDown(Signal<NodePtr>::create()),
    _mouseLeftUp(Signal<NodePtr>::create()),
    _mouseRightUp(Signal<NodePtr>::create()),
    _mouseOut(Signal<NodePtr>::create()),
    _mouseOver(Signal<NodePtr>::create()),
    _mouseWheel(Signal<NodePtr>::create()),
    _touchDown(Signal<NodePtr>::create()),
    _touchMove(Signal<NodePtr>::create()),
    _touchUp(Signal<NodePtr>::create()),
    _tap(Signal<NodePtr>::create()),
    _doubleTap(Signal<NodePtr>::create()),
    _longHold(Signal<NodePtr>::create()),
    _lastDepthValue(0.f),
    _lastMergingMask(0),
    _addPickingLayout(true),
    _emulateMouseWithTouch(true),
    _frameEndSlot(nullptr),
    _running(false),
    _debug(false)
{
}

void
PickingManager::initialize(bool emulateMouseWithTouch)
{
    _emulateMouseWithTouch = emulateMouseWithTouch;
}

void
PickingManager::bindSignals()
{
    _mouseMoveSlot = _mouse->move()->connect(std::bind(
        &PickingManager::mouseMoveHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _mouseLeftDownSlot = _mouse->leftButtonDown()->connect(std::bind(
        &PickingManager::mouseLeftDownHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1
    ));

    _mouseRightDownSlot = _mouse->rightButtonDown()->connect(std::bind(
        &PickingManager::mouseRightDownHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1
    ));

    _mouseLeftClickSlot = _mouse->leftButtonClick()->connect(std::bind(
        &PickingManager::mouseLeftClickHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1
    ));

    _mouseRightClickSlot = _mouse->rightButtonClick()->connect(std::bind(
        &PickingManager::mouseRightClickHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1));

    _mouseLeftUpSlot = _mouse->leftButtonUp()->connect(std::bind(
        &PickingManager::mouseLeftUpHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1));

    _mouseRightUpSlot = _mouse->rightButtonUp()->connect(std::bind(
        &PickingManager::mouseRightUpHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1));

    _mouseWheelSlot = _mouse->wheel()->connect(std::bind(
        &PickingManager::mouseWheelHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3));

    _touchDownSlot = _touch->touchDown()->connect(std::bind(
        &PickingManager::touchDownHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    _touchUpSlot = _touch->touchUp()->connect(std::bind(
        &PickingManager::touchUpHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    _touchMoveSlot = _touch->touchMove()->connect(std::bind(
        &PickingManager::touchMoveHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    _touchTapSlot = _touch->tap()->connect(std::bind(
        &PickingManager::touchTapHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3));

    _touchDoubleTapSlot = _touch->doubleTap()->connect(std::bind(
        &PickingManager::touchDoubleTapHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3));

    _touchLongHoldSlot = _touch->longHold()->connect(std::bind(
        &PickingManager::touchLongHoldHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
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
PickingManager::unbindSignals()
{
    _mouseMoveSlot = nullptr;
    _mouseLeftDownSlot = nullptr;
    _mouseRightDownSlot = nullptr;
    _mouseLeftClickSlot = nullptr;
    _mouseRightClickSlot = nullptr;
    _mouseLeftUpSlot = nullptr;
    _mouseRightUpSlot = nullptr;
    _touchDownSlot = nullptr;
    _touchUpSlot = nullptr;
    _touchMoveSlot = nullptr;
    _touchTapSlot = nullptr;
    _touchDoubleTapSlot = nullptr;
    _touchLongHoldSlot = nullptr;

    _frameEndSlot = nullptr;
    _componentAddedSlot = nullptr;
    _componentRemovedSlot = nullptr;
}

void
PickingManager::targetAdded(NodePtr target)
{
    _sceneManager = target->root()->component<SceneManager>();
    auto canvas = _sceneManager->canvas();

    _mouse = canvas->mouse();
    _touch = canvas->touch();
    _context = canvas->context();

    bindSignals();

    auto priority = _debug ? -1000.0f : 1000.0f;


    _frameEndSlot = _sceneManager->frameEnd()->connect(std::bind(
        &PickingManager::frameEndHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));


    _componentAddedSlot = target->componentAdded().connect(std::bind(
        &PickingManager::componentAddedHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    _componentRemovedSlot = target->componentRemoved().connect(std::bind(
        &PickingManager::componentRemovedHandler,
        std::static_pointer_cast<PickingManager>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    ));

    auto nodesWithPicking = scene::NodeSet::create(target)
        ->descendants(true)
        ->where([](scene::Node::Ptr node)
    {
        return node->hasComponent<AbstractPicking>();
    });

    for (auto node : nodesWithPicking->nodes())
        for (auto picking : node->components<AbstractPicking>())
            addPicking(picking);
}

void
PickingManager::targetRemoved(NodePtr target)
{
    unbindSignals();

    _sceneManager = nullptr;
    _running = false;
    _pickings.clear();
}

void
PickingManager::performPicking()
{
    _running = true;
    _lastPickingPriority = 0;

    math::vec2 mousePos(_mouse->x(), _mouse->y());

    for (auto& entry : _pickings)
    {
        entry.answered = false;
        entry.picking->pick(mousePos);
    }
}

void
PickingManager::frameEndHandler(SceneManagerPtr, float, float)
{
    if (_running)
    {
        // Check if all the picking scripts have answered.
        for (const auto& entry : _pickings)
        {
            if (entry.picking->enabled() && !entry.answered)
                return;
        }

        // If they all answered, dispatch event.
        dispatchEvents();

        _running = false;


        if (!(_mouseOver->numCallbacks() > 0 || _mouseOut->numCallbacks() > 0))
            performPicking(); // FIXME: we need to say we want to re-iterate the picking. Do this on the end frame?
    }
}

void
PickingManager::addPicking(std::shared_ptr<AbstractComponent> component)
{
    auto picking = std::dynamic_pointer_cast<AbstractPicking>(component);

    if (picking)
    {
        PickingEntry entry;
        entry.picking = picking;
        entry.answered = true;
        entry.slot = picking->pickingComplete()->connect([this](AbstractPicking::Ptr picking, SurfacePtr surface) {
            pickingCompleteHandler(picking, surface);
        });

        _pickings.push_back(entry);
    }
}

void
PickingManager::componentAddedHandler(NodePtr								target,
                                      NodePtr								node,
                                      std::shared_ptr<AbstractComponent>	component)
{
    addPicking(component);
}

void
PickingManager::componentRemovedHandler(NodePtr							target,
                                        NodePtr							node,
                                        std::shared_ptr<AbstractComponent>	component)
{
    auto picking = std::dynamic_pointer_cast<AbstractPicking>(component);

    if (picking)
    {
        auto toRemove = std::find_if(_pickings.begin(), _pickings.end(), [=](const PickingEntry& entry) { return entry.picking == picking; });
        _pickings.erase(toRemove);
    }
}

void
PickingManager::pickingCompleteHandler(AbstractPicking::Ptr picking, SurfacePtr surface)
{
    if (!_running)
        return;

    auto entry = std::find_if(_pickings.begin(), _pickings.end(), [=](const PickingEntry& entry) { return entry.picking == picking; });
    entry->answered = true;

    if (picking->priority() >= _lastPickingPriority)
    {
		_lastPickedSurface = surface;
        _lastDepthValue = picking->pickedDepth();
        _lastMergingMask = picking->pickedMergingMask();
        _lastPickedSurfaceId = picking->pickedSurfaceId();
        _lastPickingPriority = picking->priority();
    }
}

void
PickingManager::dispatchEvents()
{
    if (_lastPickedSurface != _pickedSurfaceBeforeNewPick)
    {
        if (_lastPickedSurface && _mouseOut->numCallbacks() > 0)
            _mouseOut->execute(_pickedSurfaceBeforeNewPick->target());

        _pickedSurfaceBeforeNewPick = _lastPickedSurface;

        if (_lastPickedSurface && _mouseOver->numCallbacks() > 0)
            _mouseOver->execute(_lastPickedSurface->target());
    }

    if (_executeMoveHandler && _lastPickedSurface)
    {
        _mouseMove->execute(_lastPickedSurface->target());
    }

    if (_executeRightDownHandler && _lastPickedSurface)
    {
        _mouseRightDown->execute(_lastPickedSurface->target());
    }

    if (_executeLeftDownHandler && _lastPickedSurface)
    {
        _mouseLeftDown->execute(_lastPickedSurface->target());
    }

    if (_executeRightClickHandler && _lastPickedSurface)
    {
        _mouseRightClick->execute(_lastPickedSurface->target());
    }

    if (_executeLeftClickHandler && _lastPickedSurface)
    {
        _mouseLeftClick->execute(_lastPickedSurface->target());
    }

    if (_executeRightUpHandler && _lastPickedSurface)
    {
        _mouseRightUp->execute(_lastPickedSurface->target());
    }

    if (_executeLeftUpHandler && _lastPickedSurface)
    {
        _mouseLeftUp->execute(_lastPickedSurface->target());
    }

    if (_executeMouseWheel && _lastPickedSurface)
    {
        _mouseWheel->execute(_lastPickedSurface->target());
    }

    if (_executeTouchDownHandler && _lastPickedSurface)
    {
        _touchDown->execute(_lastPickedSurface->target());
    }

    if (_executeTouchUpHandler && _lastPickedSurface)
    {
        _touchUp->execute(_lastPickedSurface->target());
    }

    if (_executeTouchMoveHandler && _lastPickedSurface)
    {
        _touchMove->execute(_lastPickedSurface->target());
    }

    if (_executeTapHandler && _lastPickedSurface)
    {
        _tap->execute(_lastPickedSurface->target());
    }

    if (_executeDoubleTapHandler && _lastPickedSurface)
    {
        _doubleTap->execute(_lastPickedSurface->target());
    }

    if (_executeLongHoldHandler && _lastPickedSurface)
    {
        _longHold->execute(_lastPickedSurface->target());
    }

    _executeMoveHandler = false;
    _executeRightDownHandler = false;
    _executeLeftDownHandler = false;
    _executeRightClickHandler = false;
    _executeLeftClickHandler = false;
    _executeRightUpHandler = false;
    _executeLeftUpHandler = false;
}

void
PickingManager::mouseMoveHandler(MousePtr mouse, int dx, int dy)
{
    if (_mouseOver->numCallbacks() > 0 || _mouseOut->numCallbacks() > 0 || _mouseMove->numCallbacks() > 0)
    {
        _executeMoveHandler = true;
        performPicking();
    }
}

void
PickingManager::mouseRightUpHandler(MousePtr mouse)
{
    if (_mouseRightUp->numCallbacks() > 0)
    {
        _executeRightUpHandler = true;
        performPicking();
    }
}

void
PickingManager::mouseLeftUpHandler(MousePtr mouse)
{
    if (_mouseLeftUp->numCallbacks() > 0)
    {
        _executeLeftUpHandler = true;
        performPicking();
    }
}

void
PickingManager::mouseRightClickHandler(MousePtr mouse)
{
    if (_mouseRightClick->numCallbacks() > 0)
    {
        _executeRightClickHandler = true;
        performPicking();
    }
}

void
PickingManager::mouseLeftClickHandler(MousePtr mouse)
{
    if (_mouseLeftClick->numCallbacks() > 0)
    {
        _executeLeftClickHandler = true;
        performPicking();
    }
}

void
PickingManager::mouseRightDownHandler(MousePtr mouse)
{
    if (_mouseRightDown->numCallbacks() > 0)
    {
        _executeRightDownHandler = true;
        performPicking();
    }
}

void
PickingManager::mouseLeftDownHandler(MousePtr mouse)
{
    if (_mouseLeftDown->numCallbacks() > 0)
    {
        _executeLeftDownHandler = true;
        performPicking();
    }
}

void
PickingManager::mouseWheelHandler(MousePtr mouse, int x, int y)
{
    if (_mouseWheel->numCallbacks() > 0)
    {
        _executeMouseWheel = true;
        performPicking();
    }
}

void
PickingManager::touchDownHandler(TouchPtr touch, int identifier, float x, float y)
{
    if (_touchDown->numCallbacks() > 0)
    {
        _executeTouchDownHandler = true;
        performPicking();
    }
    if (_emulateMouseWithTouch && _touch->numTouches() == 1 && _mouseLeftDown->numCallbacks() > 0)
    {
        _executeLeftDownHandler = true;
        performPicking();
    }
}

void
PickingManager::touchUpHandler(TouchPtr touch, int identifier, float x, float y)
{
    if (_touchUp->numCallbacks() > 0)
    {
        _executeTouchUpHandler = true;
        performPicking();
    }
    if (_emulateMouseWithTouch && _touch->numTouches() == 1 && _mouseLeftUp->numCallbacks() > 0)
    {
        _executeLeftUpHandler = true;
        performPicking();
    }
}

void
PickingManager::touchMoveHandler(TouchPtr touch, int identifier, float x, float y)
{
    if (_touchMove->numCallbacks() > 0)
    {
        _executeTouchMoveHandler = true;
        performPicking();
    }
    if (_emulateMouseWithTouch && _touch->numTouches() == 1 && _mouseMove->numCallbacks() > 0)
    {
        _executeMoveHandler = true;
        performPicking();
    }
}

void
PickingManager::touchTapHandler(TouchPtr touch, float x, float y)
{
    if (_tap->numCallbacks() > 0)
    {
        _executeTapHandler = true;
        performPicking();
    }
    if (_emulateMouseWithTouch && _mouseLeftClick->numCallbacks() > 0)
    {
        _executeLeftClickHandler = true;
        performPicking();
    }
}

void
PickingManager::touchDoubleTapHandler(TouchPtr touch, float x, float y)
{
    if (_doubleTap->numCallbacks() > 0)
    {
        _executeDoubleTapHandler = true;
        performPicking();
    }
}

void
PickingManager::touchLongHoldHandler(TouchPtr touch, float x, float y)
{
    if (_doubleTap->numCallbacks() > 0)
    {
        _executeDoubleTapHandler = true;
        performPicking();
    }
    if (_emulateMouseWithTouch && _mouseRightClick->numCallbacks() > 0)
    {
        _executeRightClickHandler = true;
        performPicking();
    }
}

AbstractPicking::map<scene::Node::Ptr, std::set<unsigned char>>
PickingManager::pickArea(const minko::math::vec2& bottomLeft, const minko::math::vec2& topRight, bool fullyInside)
{
    AbstractPicking::map<scene::Node::Ptr, std::set<unsigned char>> result;
    int highestPriority = 0;

    for (const auto& entry : _pickings)
    {
        if (entry.picking->enabled() && entry.picking->priority() >= highestPriority)
            result = entry.picking->pickArea(bottomLeft, topRight, fullyInside);
    }

    return result;
}