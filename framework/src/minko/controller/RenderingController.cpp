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

#include "RenderingController.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/controller/SurfaceController.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/AbstractContext.hpp"

using namespace minko::controller;
using namespace minko::scene;

void
RenderingController::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&RenderingController::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));	

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&RenderingController::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
RenderingController::targetAddedHandler(std::shared_ptr<AbstractController> ctrl,
										std::shared_ptr<Node> 				target)
{
	if (target->controllers<RenderingController>().size() > 1)
		throw std::logic_error("There cannot be two RenderingController on the same node.");

	_addedSlot = target->added()->connect(std::bind(
		&RenderingController::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedSlot = target->removed()->connect(std::bind(
		&RenderingController::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	addedHandler(target->root(), target, target->parent());
}

void
RenderingController::targetRemovedHandler(std::shared_ptr<AbstractController> 	ctrl,
										  std::shared_ptr<Node> 				target)
{
	_addedSlot = nullptr;
	_removedSlot = nullptr;

	removedHandler(target->root(), target, target->parent());
}

void
RenderingController::addedHandler(std::shared_ptr<Node> node,
								  std::shared_ptr<Node> target,
								  std::shared_ptr<Node> parent)
{
	_rootDescendantAddedSlot = target->root()->added()->connect(std::bind(
		&RenderingController::rootDescendantAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_rootDescendantRemovedSlot = target->root()->removed()->connect(std::bind(
		&RenderingController::rootDescendantRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_controllerAddedSlot = target->root()->controllerAdded()->connect(std::bind(
		&RenderingController::controllerAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_controllerRemovedSlot = target->root()->controllerRemoved()->connect(std::bind(
		&RenderingController::controllerRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	rootDescendantAddedHandler(target->root(), target, target->parent());
}

void
RenderingController::removedHandler(std::shared_ptr<Node> node,
									std::shared_ptr<Node> target,
									std::shared_ptr<Node> parent)
{
	_rootDescendantAddedSlot = nullptr;
	_rootDescendantRemovedSlot = nullptr;
	_controllerAddedSlot = nullptr;
	_controllerRemovedSlot = nullptr;

	rootDescendantRemovedHandler(target->root(), target, target->parent());
}

void
RenderingController::rootDescendantAddedHandler(std::shared_ptr<Node> node,
												std::shared_ptr<Node> target,
												std::shared_ptr<Node> parent)
{
    auto surfaceNodes = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(target)
		->descendants(true)
		->hasController<SurfaceController>();

	for (auto surfaceNode : surfaceNodes->nodes())
		for (auto surface : surfaceNode->controllers<SurfaceController>())
			addSurfaceController(surface);
}

void
RenderingController::rootDescendantRemovedHandler(std::shared_ptr<Node> node,
												  std::shared_ptr<Node> target,
												  std::shared_ptr<Node> parent)
{
	auto surfaceNodes = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(target)
		->descendants(true)
        ->hasController<SurfaceController>();

	for (auto surfaceNode : surfaceNodes->nodes())
		for (auto surface : surfaceNode->controllers<SurfaceController>())
			removeSurfaceController(surface);
}

void
RenderingController::controllerAddedHandler(std::shared_ptr<Node>				node,
											std::shared_ptr<Node>				target,
											std::shared_ptr<AbstractController>	ctrl)
{
	auto surfaceCtrl = std::dynamic_pointer_cast<SurfaceController>(ctrl);
	
	if (surfaceCtrl)
		addSurfaceController(surfaceCtrl);
}

void
RenderingController::controllerRemovedHandler(std::shared_ptr<Node>					node,
											  std::shared_ptr<Node>					target,
											  std::shared_ptr<AbstractController>	ctrl)
{
	auto surfaceCtrl = std::dynamic_pointer_cast<SurfaceController>(ctrl);

	if (surfaceCtrl)
		removeSurfaceController(surfaceCtrl);
}

void
RenderingController::addSurfaceController(std::shared_ptr<SurfaceController> ctrl)
{
	_drawCalls.insert(_drawCalls.end(), ctrl->drawCalls().begin(), ctrl->drawCalls().end());
}

void
RenderingController::removeSurfaceController(std::shared_ptr<SurfaceController> ctrl)
{
#ifdef __GNUC__
  // Temporary non-fix for GCC missing feature N2350: http://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html
#else
        _drawCalls.erase(ctrl->drawCalls().begin(), ctrl->drawCalls().end());
#endif
}

void
RenderingController::render()
{
	_enterFrame->execute(shared_from_this());

	_context->clear(
		((_backgroundColor >> 24) & 0xff) / 255.f,
		((_backgroundColor >> 16) & 0xff) / 255.f,
		((_backgroundColor >> 8) & 0xff) / 255.f,
		(_backgroundColor & 0xff) / 255.f
	);

	for (auto drawCall : _drawCalls)
		drawCall->render(_context);

	_context->present();
}
