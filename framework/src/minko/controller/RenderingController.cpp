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

RenderingController::RenderingController(AbsContextPtr context) :
	_context(context),
	_surfaces(scene::NodeSet::create(scene::NodeSet::Mode::AUTO)),
    _backgroundColor(0),
	_enterFrame(Signal<Ptr>::create()),
	_exitFrame(Signal<Ptr>::create())
{
}

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

	_surfaces->root()
		->descendants(true)
		->hasController<SurfaceController>();
	_surfaceAddedSlot = _surfaces->nodeAdded()->connect(std::bind(
		&RenderingController::surfaceAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
	_surfaceRemovedSlot = _surfaces->nodeRemoved()->connect(std::bind(
		&RenderingController::surfaceRemovedHandler,
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

	_surfaces->select(targets().begin(), targets().end())->update();
}

void
RenderingController::targetRemovedHandler(std::shared_ptr<AbstractController> 	ctrl,
										  std::shared_ptr<Node> 				target)
{
	_surfaces->select(targets().begin(), targets().end())->update();
}

void
RenderingController::surfaceAddedHandler(NodeSet::Ptr	surfaces,
										 Node::Ptr		surfaceNode)
{
	for (auto surface : surfaceNode->controllers<SurfaceController>())
		_drawCalls.insert(_drawCalls.end(), surface->drawCalls().begin(), surface->drawCalls().end());
}

void
RenderingController::surfaceRemovedHandler(NodeSet::Ptr	surfaces,
										   Node::Ptr	surfaceNode)
{
	for (auto surface : surfaceNode->controllers<SurfaceController>())
		for (auto drawCall : surface->drawCalls())
			_drawCalls.remove(drawCall);
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

	_exitFrame->execute(shared_from_this());
}
