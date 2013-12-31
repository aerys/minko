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
#include "minko/file/AssetLibrary.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/component/Surface.hpp"
#include "minko/math/Vector4.hpp"

#include "minko/material/BasicMaterial.hpp"
#include "minko/component/Transform.hpp"

using namespace minko;
using namespace component;


Picking::Picking(SceneManagerPtr sceneManager,
				 MousePtr	mouse,
				 NodePtr	camera) :
	_mouse(mouse),
	_camera(camera),
	_pickingId(0),
	_context(sceneManager->assets()->context()),
	_pickingProjection(math::Matrix4x4::create()),
	_sceneManager(sceneManager)
{
	_renderTarget	= render::Texture::create(sceneManager->assets()->context(), _context->viewportWidth(), _context->viewportHeight(), false, true);
	_renderTarget->upload();

	_renderer		= Renderer::create(0x000000FF, _renderTarget, sceneManager->assets()->effect("effect/Picking.effect"));
}

void
Picking::initialize()
{
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
	target->root()->addChild(
		scene::Node::create("backgrond")
			->addComponent(component::Surface::create(
				_sceneManager->assets()->geometry("teapot"),
				material::BasicMaterial::create()->diffuseMap(_renderTarget),
				_sceneManager->assets()->effect("effect/Basic.effect")))
			->addComponent(component::Transform::create(math::Matrix4x4::create()->appendTranslation(0, 10, -30)->prependScale(20))));
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
	_renderingBeginSlot = _renderer->renderingBegin()->connect(std::bind(
		&Picking::renderingBegin,
		shared_from_this(),
		std::placeholders::_1));

	_renderingEndSlot = _renderer->renderingEnd()->connect(std::bind(
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
	auto newPickingId = ++_pickingId;

	_surfaceToPickingId[surface]	= ++_pickingId;
	_pickingIdToSurface[_pickingId] = surface;
	_surfaceToProvider[surface]		= data::ArrayProvider::create("picking");
	
	_surfaceToProvider[surface]->set<math::Vector4::Ptr>("color", math::Vector4::create(1, 1, 1, 1))/*
					((newPickingId >> 16) & 0xff) / 255.f,
					((newPickingId >> 8) & 0xff) / 255.f,
					((newPickingId) & 0xff) / 255.f,
					1
				))*/;

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
	auto mouseX = _mouse->x();
	auto mouseY = _mouse->y();
	/*
	std::vector<float> pickingProjectionData(_camera->data()->get<math::Matrix4x4::Ptr>("camera.projectionMatrix")->data());

//	pickingProjectionData[2] = -mouseX / (_context->viewportWidth() * 2);
//	pickingProjectionData[6] = mouseY / (_context->viewportHeight() * 2);

	_pickingProjection->lock()
		->copyFrom(_camera->data()->get<math::Matrix4x4::Ptr>("camera.viewMatrix"))
		->append(math::Matrix4x4::create()->initialize(pickingProjectionData))
		->unlock();

	if (!(_renderer->targets()[0]->data()->hasProperty("worldToScreenPickingMatrix")))
		_renderer->targets()[0]->data()->addProvider(
		data::ArrayProvider::create("pickingCameraProvider")->set("worldToScreenPickingMatrix", _pickingProjection, true)
		);
	*/
}

void
Picking::renderingEnd(RendererPtr renderer)
{
	unsigned char pixel[4] = {0, 0, 0, 0};

	_context->readPixels(0, 0, 1, 1, &pixel[0]);

	std::cout << uint(pixel[0]) << " " << uint(pixel[1]) << " "  << uint(pixel[2]) << " " << uint(pixel[3]) << std::endl;

	auto pickedSurfaceId = pixel[0] << 16 + pixel[1] << 8 + pixel[2];
	SurfacePtr surface = _pickingIdToSurface[pickedSurfaceId];

	std::cout << (surface ? surface->targets()[0]->name() : "-") << std::endl;
}
