/*
Copyright (c) 2013 Aerys

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
#include "minko/component/Surface.hpp"

#include "minko/material/BasicMaterial.hpp"
#include "minko/component/Transform.hpp"

#include "minko/data/StructureProvider.hpp"
#include "minko/AbstractCanvas.hpp"

using namespace minko;
using namespace minko::component;

Picking::Picking(SceneManagerPtr	sceneManager,
				 AbstractCanvasPtr	canvas,
				 NodePtr			camera) :
	_mouse(canvas->mouse()),
	_camera(camera),
	_pickingId(0),
	_context(sceneManager->assets()->context()),
	_pickingProjection(1.f),
	_sceneManager(sceneManager),
	_pickingProvider(data::StructureProvider::create("picking")),
	_mouseMove(Signal<NodePtr>::create()),
	_mouseLeftClick(Signal<NodePtr>::create()),
	_mouseRightClick(Signal<NodePtr>::create()),
	_mouseOut(Signal<NodePtr>::create()),
	_mouseOver(Signal<NodePtr>::create())
{
	_renderer = Renderer::create(0xFFFF00FF, nullptr, sceneManager->assets()->effect("effect/Picking.effect"), 1000.f);
}

void
Picking::initialize()
{
	_pickingProvider->set("projection", _pickingProjection);

	_mouseMoveSlot = _mouse->move()->connect(std::bind(
		&Picking::mouseMoveHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_mouseLeftClickSlot = _mouse->leftButtonUp()->connect(std::bind(
		&Picking::mouseLeftClickHandler,
		shared_from_this(),
		std::placeholders::_1
	));

	_mouseRightClickSlot = _mouse->rightButtonUp()->connect(std::bind(
		&Picking::mouseRightClickHandler,
		shared_from_this(),
		std::placeholders::_1
	));

	_targetAddedSlot = targetAdded()->connect(std::bind(
		&Picking::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));	

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&Picking::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
Picking::targetAddedHandler(AbsCtrlPtr ctrl, NodePtr target)
{
	if (target->components<Picking>().size() > 1)
		throw std::logic_error("There cannot be two Picking on the same node.");

	_addedSlot = target->added()->connect(std::bind(
		&Picking::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedSlot = target->removed()->connect(std::bind(
		&Picking::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	addedHandler(target->root(), target, target->parent());
	target->addComponent(_renderer);

	auto perspectiveCamera = _camera->component<component::PerspectiveCamera>();

	target->data()->addProvider(_pickingProvider);
	target->data()->addProvider(perspectiveCamera->data());
	
	auto surfaces = scene::NodeSet::create(target->root())
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
Picking::targetRemovedHandler(AbsCtrlPtr ctrl, NodePtr target)
{
	_addedSlot = nullptr;
	_removedSlot = nullptr;
	
	removedHandler(target->root(), target, target->parent());
}

void
Picking::addedHandler(NodePtr node, NodePtr target, NodePtr parent)
{
	if (node == target)
	{
		_renderingBeginSlot = _renderer->renderingBegin()->connect(std::bind(
			&Picking::renderingBegin,
			shared_from_this(),
			std::placeholders::_1));

		_renderingEndSlot = _renderer->beforePresent()->connect(std::bind(
			&Picking::renderingEnd,
			shared_from_this(),
			std::placeholders::_1));
	
		_componentAddedSlot = target->root()->componentAdded()->connect(std::bind(
			&Picking::componentAddedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
		));

		_componentRemovedSlot = target->root()->componentRemoved()->connect(std::bind(
			&Picking::componentRemovedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
		));
	}
	else
	{
		for (auto surface : target->components<Surface>())
			addSurface(surface);
	}

}

void
Picking::componentAddedHandler(NodePtr								node,
							   NodePtr								target,
							   std::shared_ptr<AbstractComponent>	ctrl)
{
	auto surfaceCtrl = std::dynamic_pointer_cast<Surface>(ctrl);
	
	if (surfaceCtrl)
		addSurface(surfaceCtrl);

	auto sceneManagerCtrl = std::dynamic_pointer_cast<SceneManager>(ctrl);

	if (sceneManagerCtrl)
	{
		auto surfaces = scene::NodeSet::create()
			->descendants(true)
			->where([](scene::Node::Ptr node)
		{
			return node->hasComponent<Surface>();
		});

		for (auto surfaceNode : surfaces->nodes())
			for (auto surface : surfaceNode->components<Surface>())
				addSurface(surface);
	}
}

void
Picking::componentRemovedHandler(NodePtr							node,
								 NodePtr							target,
								 std::shared_ptr<AbstractComponent>	ctrl)
{
	auto surfaceCtrl = std::dynamic_pointer_cast<Surface>(ctrl);
	auto sceneManager = std::dynamic_pointer_cast<SceneManager>(ctrl);

	if (surfaceCtrl)
		removeSurface(surfaceCtrl);
		auto sceneManagerCtrl = std::dynamic_pointer_cast<SceneManager>(ctrl);

	if (sceneManagerCtrl)
	{
		auto surfaces = scene::NodeSet::create()
			->descendants(true)
			->where([](scene::Node::Ptr node)
		{
			return node->hasComponent<Surface>();
		});

		for (auto surfaceNode : surfaces->nodes())
			for (auto surface : surfaceNode->components<Surface>())
				removeSurface(surface);
	}
}

void
Picking::addSurface(SurfacePtr surface)
{
	_pickingId += 2;

	_surfaceToPickingId[surface] = _pickingId;
	_pickingIdToSurface[_pickingId] = surface;
	_surfaceToProvider[surface]		= data::StructureProvider::create("picking");
	
	_surfaceToProvider[surface]->set<math::vec4>("color", math::vec4(
		((_pickingId >> 16) & 0xff) / 255.f,
		((_pickingId >> 8) & 0xff) / 255.f,
		((_pickingId)& 0xff) / 255.f,
		1
	));

	surface->targets()[0]->data()->addProvider(_surfaceToProvider[surface]);
}

void
Picking::removeSurface(SurfacePtr surface)
{
	auto surfacePickingId = _surfaceToPickingId[surface];

	surface->targets()[0]->data()->removeProvider(_surfaceToProvider[surface]);
	_surfaceToPickingId.erase(surface);
	_surfaceToProvider.erase(surface);
	_pickingIdToSurface.erase(surfacePickingId);
}

void
Picking::removedHandler(NodePtr node, NodePtr target, NodePtr parent)
{
	_renderingBeginSlot = nullptr;
	_renderingEndSlot	= nullptr;
}

void
Picking::renderingBegin(RendererPtr renderer)
{
	auto perspectiveCamera	= _camera->component<component::PerspectiveCamera>();
	auto projection			= perspectiveCamera->projectionMatrix();

	projection[2][0] = (float)_mouse->x() / _context->viewportWidth() * 2.f;
	projection[1][3] = (float)_mouse->y() / _context->viewportHeight() * 2.f;

	_pickingProvider->set("projection", _pickingProjection);
}

void
Picking::renderingEnd(RendererPtr renderer)
{
	_context->readPixels(0, 0, 1, 1, &_lastColor[0]);

	uint pickedSurfaceId = (_lastColor[0] << 16) + (_lastColor[1] << 8) + _lastColor[2];

	if (_lastPickedSurface != _pickingIdToSurface[pickedSurfaceId])
	{
		if (_lastPickedSurface)
			_mouseOut->execute(_lastPickedSurface->targets()[0]);

		_lastPickedSurface = _pickingIdToSurface[pickedSurfaceId];

		if (_lastPickedSurface)
			_mouseOver->execute(_lastPickedSurface->targets()[0]);
	}
}

void
Picking::mouseMoveHandler(MousePtr mouse, int dx, int dy)
{
	if (_lastPickedSurface)
		_mouseMove->execute(_lastPickedSurface->targets()[0]);
}

void
Picking::mouseRightClickHandler(MousePtr mouse)
{
	if (_lastPickedSurface)
		_mouseRightClick->execute(_lastPickedSurface->targets()[0]);
}

void
Picking::mouseLeftClickHandler(MousePtr mouse)
{
	if (_lastPickedSurface)
		_mouseLeftClick->execute(_lastPickedSurface->targets()[0]);
}
