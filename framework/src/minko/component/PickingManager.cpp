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

Picking::Picking() :
    _sceneManager(),
    _mouse(),
    _touch(),
    _camera(),
    _context(),
    _pickingColorSet(Signal<Ptr, SurfacePtr, uint, const minko::math::vec4&>::create()),
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
    _frameBeginSlot(nullptr),
    _enabled(false),
    _renderDepth(true),
    _debug(false),
    _multiselecting(false),
    _multiselectionStartPosition()
{
}

void
Picking::initialize(NodePtr             camera,
                    bool                addPickingLayout,
                    bool                emulateMouseWithTouch,
                    EffectPtr           pickingEffect,
                    EffectPtr           pickingDepthEffect)
{
    _camera = camera;
    _addPickingLayout = addPickingLayout;
    _emulateMouseWithTouch = emulateMouseWithTouch;
    _pickingEffect = pickingEffect;
    _pickingDepthEffect = pickingDepthEffect;

    _pickingProvider->set("pickingProjection", _pickingProjection);
    _pickingProvider->set("pickingOrigin", math::vec3());
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
        std::placeholders::_1
    ));

    _mouseRightDownSlot = _mouse->rightButtonDown()->connect(std::bind(
        &Picking::mouseRightDownHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1
    ));

    _mouseLeftClickSlot = _mouse->leftButtonClick()->connect(std::bind(
        &Picking::mouseLeftClickHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1
    ));

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

    _mouseWheelSlot = _mouse->wheel()->connect(std::bind(
        &Picking::mouseWheelHandler,
        std::static_pointer_cast<Picking>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3));

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
Picking::unbindSignals()
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

void
Picking::targetAdded(NodePtr target)
{
    _sceneManager = target->root()->component<SceneManager>();
    auto canvas = _sceneManager->canvas();

    _mouse = canvas->mouse();
    _touch = canvas->touch();
    _context = canvas->context();

    bindSignals();

    auto priority = _debug ? -1000.0f : 1000.0f;

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

    addSurfacesForNode(target);
}

void
Picking::targetRemoved(NodePtr target)
{
    unbindSignals();

    _sceneManager = nullptr;
    _enabled = false;

    removedHandler(target->root(), target, target->parent());
}

void
Picking::enabled(bool enabled)
{
    if (enabled && !_frameBeginSlot)
    {
        _enabled = true;

        _frameBeginSlot = _sceneManager->frameBegin()->connect(std::bind(
            &Picking::frameBeginHandler,
            std::static_pointer_cast<Picking>(shared_from_this()),
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
        ), 1000.0f);
    }
    else if (!enabled && _frameBeginSlot != nullptr)
    {
        _frameBeginSlot = nullptr;
    }

    _enabled = enabled;
}

void
Picking::frameBeginHandler(SceneManagerPtr, float, float)
{
    // renderPickingFrame();
}

void
Picking::renderingEnd(RendererPtr renderer)
{
    if (!_enabled)
        return;

    // Dispatch events.
    /*
    {
        dispatchEvents(pickedSurface, _lastDepthValue);
    }
    else
    {
        dispatchEvents(nullptr, _lastDepthValue);
    }
    */
}

void
Picking::dispatchEvents(SurfacePtr pickedSurface, float depth)
{
    if (_lastPickedSurface != pickedSurface)
    {
        if (_lastPickedSurface && _mouseOut->numCallbacks() > 0)
            _mouseOut->execute(_lastPickedSurface->target());

        _lastPickedSurface = pickedSurface;

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

    if (!(_mouseOver->numCallbacks() > 0 || _mouseOut->numCallbacks() > 0))
        enabled(false);

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
    if (_mouseOver->numCallbacks() > 0 || _mouseOut->numCallbacks() > 0 || _mouseMove->numCallbacks() > 0)
    {
        _executeMoveHandler = true;
        enabled(true);
    }
}

void
Picking::mouseRightUpHandler(MousePtr mouse)
{
    if (_mouseRightUp->numCallbacks() > 0)
    {
        _executeRightUpHandler = true;
        enabled(true);
    }
}

void
Picking::mouseLeftUpHandler(MousePtr mouse)
{
    if (_mouseLeftUp->numCallbacks() > 0)
    {
        _executeLeftUpHandler = true;
        enabled(true);
    }
}

void
Picking::mouseRightClickHandler(MousePtr mouse)
{
    if (_mouseRightClick->numCallbacks() > 0)
    {
        _executeRightClickHandler = true;
        enabled(true);
    }
}

void
Picking::mouseLeftClickHandler(MousePtr mouse)
{
    if (_mouseLeftClick->numCallbacks() > 0)
    {
        _executeLeftClickHandler = true;
        enabled(true);
    }
}

void
Picking::mouseRightDownHandler(MousePtr mouse)
{
    if (_mouseRightDown->numCallbacks() > 0)
    {
        _executeRightDownHandler = true;
        enabled(true);
    }
}

void
Picking::mouseLeftDownHandler(MousePtr mouse)
{
    if (_mouseLeftDown->numCallbacks() > 0)
    {
        _executeLeftDownHandler = true;
        enabled(true);
    }
}

void
Picking::mouseWheelHandler(MousePtr mouse, int x, int y)
{
    if (_mouseWheel->numCallbacks() > 0)
    {
        _executeMouseWheel = true;
        enabled(true);
    }
}

void
Picking::touchDownHandler(TouchPtr touch, int identifier, float x, float y)
{
    if (_touchDown->numCallbacks() > 0)
    {
        _executeTouchDownHandler = true;
        enabled(true);
    }
    if (_emulateMouseWithTouch && _touch->numTouches() == 1 && _mouseLeftDown->numCallbacks() > 0)
    {
        _executeLeftDownHandler = true;
        enabled(true);
    }
}

void
Picking::touchUpHandler(TouchPtr touch, int identifier, float x, float y)
{
    if (_touchUp->numCallbacks() > 0)
    {
        _executeTouchUpHandler = true;
        enabled(true);
    }
    if (_emulateMouseWithTouch && _touch->numTouches() == 1 && _mouseLeftUp->numCallbacks() > 0)
    {
        _executeLeftUpHandler = true;
        enabled(true);
    }
}

void
Picking::touchMoveHandler(TouchPtr touch, int identifier, float x, float y)
{
    if (_touchMove->numCallbacks() > 0)
    {
        _executeTouchMoveHandler = true;
        enabled(true);
    }
    if (_emulateMouseWithTouch && _touch->numTouches() == 1 && _mouseMove->numCallbacks() > 0)
    {
        _executeMoveHandler = true;
        enabled(true);
    }
}

void
Picking::touchTapHandler(TouchPtr touch, float x, float y)
{
    if (_tap->numCallbacks() > 0)
    {
        _executeTapHandler = true;
        enabled(true);
    }
    if (_emulateMouseWithTouch && _mouseLeftClick->numCallbacks() > 0)
    {
        _executeLeftClickHandler = true;
        enabled(true);
    }
}

void
Picking::touchDoubleTapHandler(TouchPtr touch, float x, float y)
{
    if (_doubleTap->numCallbacks() > 0)
    {
        _executeDoubleTapHandler = true;
        enabled(true);
    }
}

void
Picking::touchLongHoldHandler(TouchPtr touch, float x, float y)
{
    if (_doubleTap->numCallbacks() > 0)
    {
        _executeDoubleTapHandler = true;
        enabled(true);
    }
    if (_emulateMouseWithTouch && _mouseRightClick->numCallbacks() > 0)
    {
        _executeRightClickHandler = true;
        enabled(true);
    }
}

Picking::map<scene::Node::Ptr, std::set<unsigned char>>
Picking::pickArea(const minko::math::vec2& bottomLeft, const minko::math::vec2& topRight, bool fullyInside)
{
    // FIXME
}