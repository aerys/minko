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
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/input/Touch.hpp"
#include "minko/component/Surface.hpp"
#include "minko/data/Provider.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/math/Ray.hpp"

using namespace minko;
using namespace component;

Picking::Picking() :
    _sceneManager(nullptr),
    _context(nullptr),
    _mouse(nullptr),
    _touch(nullptr),
    _camera(nullptr),
    _pickingId(0),
    _pickingProjection(1.f),
    _pickingProvider(data::Provider::create()),
    _pickingEffect(nullptr),
    _pickingDepthEffect(nullptr),
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
    _debug(false)
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
    
    if (_pickingEffect == nullptr)
        _pickingEffect = _sceneManager->assets()->effect("effect/Picking.effect");
        
    auto priority = _debug ? -1000.0f : 1000.0f;

    _renderer = Renderer::create(
        0xFFFF00FF,
        nullptr,
        _pickingEffect,
        "default",
        priority,
        "Picking Renderer"
    );
    if (!_debug)
        _renderer->scissorBox(0, 0, 1, 1);
    _renderer->layoutMask(scene::BuiltinLayout::PICKING);
    if (!_debug)
        _renderer->enabled(false);

    if (_pickingDepthEffect == nullptr)
        _pickingDepthEffect = _sceneManager->assets()->effect("effect/PickingDepth.effect");

    _depthRenderer = Renderer::create(
        0xFFFF00FF,
        nullptr,
        _pickingDepthEffect,
        "default",
        999.f,
        "Depth Picking Renderer"
    );
    _depthRenderer->scissorBox(0, 0, 1, 1);
    _depthRenderer->layoutMask(scene::BuiltinLayout::PICKING_DEPTH);
    _depthRenderer->enabled(false);

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

	auto perspectiveCamera = _camera->component<component::PerspectiveCamera>();

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
    _enabled = false;

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
		node->layout(node->layout() & ~scene::BuiltinLayout::PICKING);
}

void
Picking::addSurface(SurfacePtr surface)
{
	if (_surfaceToPickingId.find(surface) == _surfaceToPickingId.end())
	{
		_pickingId += 2;

		_surfaceToPickingId[surface] = _pickingId;
		_pickingIdToSurface[_pickingId] = surface;

		surface->data()->set("pickingColor", math::vec4(
			((_pickingId >> 16) & 0xff) / 255.f,
			((_pickingId >> 8) & 0xff) / 255.f,
			((_pickingId)& 0xff) / 255.f,
			1
		));

        if (_addPickingLayout)
            surface->target()->layout(target()->layout() | scene::BuiltinLayout::PICKING);

        surface->layoutMask(surface->layoutMask() & ~scene::BuiltinLayout::PICKING_DEPTH);
	}
}

void
Picking::removeSurface(SurfacePtr surface, NodePtr node)
{
	if (_surfaceToPickingId.find(surface) == _surfaceToPickingId.end())
		return;

    surface->data()->unset("pickingColor");

	auto surfacePickingId = _surfaceToPickingId[surface];

	_surfaceToPickingId.erase(surface);
	_pickingIdToSurface.erase(surfacePickingId);
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
		surfaceNode->layout(surfaceNode->layout() & ~scene::BuiltinLayout::PICKING);

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
    if (_debug)
        return;

    _renderer->enabled(true);
    _renderer->render(_sceneManager->canvas()->context());
    _renderer->enabled(false);
}

void
Picking::renderingBegin(RendererPtr renderer)
{
    if (!_enabled)
        return;

    updatePickingProjection();
}

void
Picking::renderingEnd(RendererPtr renderer)
{
    if (!_enabled)
        return;

    _context->readPixels(0, 0, 1, 1, &_lastColor[0]);

    uint pickedSurfaceId = (_lastColor[0] << 16) + (_lastColor[1] << 8) + _lastColor[2];

    auto surfaceIt = _pickingIdToSurface.find(pickedSurfaceId);

    if (surfaceIt != _pickingIdToSurface.end())
    {
        auto pickedSurface = surfaceIt->second;

        if (_renderDepth)
            renderDepth(_depthRenderer, pickedSurface);
        else
            dispatchEvents(pickedSurface, _lastDepthValue);
    }
    else
    {
        dispatchEvents(nullptr, _lastDepthValue);
    }
}

void
Picking::renderDepth(RendererPtr renderer, SurfacePtr pickedSurface)
{
    if (!_enabled)
        return;

    auto pickedSurfaceTarget = pickedSurface->target();

    pickedSurfaceTarget->layout(pickedSurfaceTarget->layout() | scene::BuiltinLayout::PICKING_DEPTH);
    pickedSurface->layoutMask(pickedSurface->layoutMask() | scene::BuiltinLayout::PICKING_DEPTH);

    renderer->enabled(true);
    renderer->render(_sceneManager->canvas()->context());
    renderer->enabled(false);

    pickedSurfaceTarget->layout(pickedSurfaceTarget->layout() & ~scene::BuiltinLayout::PICKING_DEPTH);
    pickedSurface->layoutMask(pickedSurface->layoutMask() & ~scene::BuiltinLayout::PICKING_DEPTH);
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
    if (!_enabled)
        return;
    
    updatePickingOrigin();
}

void
Picking::depthRenderingEnd(RendererPtr renderer)
{
    if (!_enabled)
        return;
    
    uint pickedSurfaceId = (_lastColor[0] << 16) + (_lastColor[1] << 8) + _lastColor[2];

    auto surfaceIt = _pickingIdToSurface.find(pickedSurfaceId);

    if (surfaceIt != _pickingIdToSurface.end())
    {
        auto pickedSurface = surfaceIt->second;

        _context->readPixels(0, 0, 1, 1, &_lastDepth[0]);

        const auto zFar = _camera->data().get<float>("zFar");

        const auto normalizedDepth = unpack(math::vec3(_lastDepth[0], _lastDepth[1], _lastDepth[2]) / 255.f) * zFar;

        _lastDepthValue = normalizedDepth;

        _lastMergingMask = _lastDepth[3];

        dispatchEvents(pickedSurface, _lastDepthValue);
    }
}

void
Picking::updatePickingProjection()
{
	const auto mouseX = static_cast<float>(_mouse->x());
	const auto mouseY = static_cast<float>(_mouse->y());

	auto perspectiveCamera	= _camera->component<component::PerspectiveCamera>();
	auto projection	= math::perspective(
		perspectiveCamera->fieldOfView(),
		perspectiveCamera->aspectRatio(),
		perspectiveCamera->zNear(),
		perspectiveCamera->zFar()
	);

	projection[2][0] = mouseX / _context->viewportWidth() * 2.f;
	projection[2][1] = (_context->viewportHeight() - mouseY) / _context->viewportHeight() * 2.f;

	_pickingProvider->set("pickingProjection", projection);
}

void
Picking::updatePickingOrigin()
{
	auto perspectiveCamera	= _camera->component<component::PerspectiveCamera>();

    const auto normalizedMouseX = _mouse->normalizedX();
    const auto normalizedMouseY = _mouse->normalizedY();

    auto pickingRay = perspectiveCamera->unproject(normalizedMouseX, normalizedMouseY);

    _pickingProvider->set("pickingOrigin", pickingRay->origin());
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
	if (_mouseOver->numCallbacks() > 0 || _mouseOut->numCallbacks() > 0)
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
