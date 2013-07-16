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

#include "Rendering.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/Surface.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/Pass.hpp"
#include "minko/render/AbstractContext.hpp"

using namespace minko::component;
using namespace minko::scene;

void
Rendering::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&Rendering::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));	

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&Rendering::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
Rendering::targetAddedHandler(std::shared_ptr<AbstractComponent> ctrl,
										std::shared_ptr<Node> 				target)
{
	if (target->components<Rendering>().size() > 1)
		throw std::logic_error("There cannot be two Rendering on the same node.");

	_addedSlot = target->added()->connect(std::bind(
		&Rendering::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedSlot = target->removed()->connect(std::bind(
		&Rendering::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	addedHandler(target->root(), target, target->parent());
}

void
Rendering::targetRemovedHandler(std::shared_ptr<AbstractComponent> 	ctrl,
										  std::shared_ptr<Node> 				target)
{
	_addedSlot = nullptr;
	_removedSlot = nullptr;

	removedHandler(target->root(), target, target->parent());
}

void
Rendering::addedHandler(std::shared_ptr<Node> node,
								  std::shared_ptr<Node> target,
								  std::shared_ptr<Node> parent)
{
	_rootDescendantAddedSlot = target->root()->added()->connect(std::bind(
		&Rendering::rootDescendantAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_rootDescendantRemovedSlot = target->root()->removed()->connect(std::bind(
		&Rendering::rootDescendantRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_componentAddedSlot = target->root()->componentAdded()->connect(std::bind(
		&Rendering::componentAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_componentRemovedSlot = target->root()->componentRemoved()->connect(std::bind(
		&Rendering::componentRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	rootDescendantAddedHandler(target->root(), target, target->parent());
}

void
Rendering::removedHandler(std::shared_ptr<Node> node,
									std::shared_ptr<Node> target,
									std::shared_ptr<Node> parent)
{
	_rootDescendantAddedSlot = nullptr;
	_rootDescendantRemovedSlot = nullptr;
	_componentAddedSlot = nullptr;
	_componentRemovedSlot = nullptr;

	rootDescendantRemovedHandler(target->root(), target, target->parent());
}

void
Rendering::rootDescendantAddedHandler(std::shared_ptr<Node> node,
									  std::shared_ptr<Node> target,
									  std::shared_ptr<Node> parent)
{
    auto surfaceNodes = NodeSet::create(node)
		->descendants(true)
        ->where([](scene::Node::Ptr node)
        {
            return node->hasComponent<Surface>();
        });

	for (auto surfaceNode : surfaceNodes->nodes())
		for (auto surface : surfaceNode->components<Surface>())
			addSurfaceComponent(surface);
}

void
Rendering::rootDescendantRemovedHandler(std::shared_ptr<Node> node,
									    std::shared_ptr<Node> target,
									    std::shared_ptr<Node> parent)
{
	auto surfaceNodes = NodeSet::create(target)
		->descendants(true)
        ->where([](scene::Node::Ptr node)
        {
            return node->hasComponent<Surface>();
        });

	for (auto surfaceNode : surfaceNodes->nodes())
		for (auto surface : surfaceNode->components<Surface>())
			removeSurfaceComponent(surface);
}

void
Rendering::componentAddedHandler(std::shared_ptr<Node>				node,
											std::shared_ptr<Node>				target,
											std::shared_ptr<AbstractComponent>	ctrl)
{
	auto surfaceCtrl = std::dynamic_pointer_cast<Surface>(ctrl);
	
	if (surfaceCtrl)
		addSurfaceComponent(surfaceCtrl);
}

void
Rendering::componentRemovedHandler(std::shared_ptr<Node>				node,
								   std::shared_ptr<Node>				target,
								   std::shared_ptr<AbstractComponent>	ctrl)
{
	auto surfaceCtrl = std::dynamic_pointer_cast<Surface>(ctrl);

	if (surfaceCtrl)
		removeSurfaceComponent(surfaceCtrl);
}

void
Rendering::addSurfaceComponent(std::shared_ptr<Surface> ctrl)
{
	_drawCalls.insert(_drawCalls.end(), ctrl->drawCalls().begin(), ctrl->drawCalls().end());
}

void
Rendering::removeSurfaceComponent(std::shared_ptr<Surface> ctrl)
{
#ifdef __GNUC__
  // Temporary non-fix for GCC missing feature N2350: http://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html
#else
	_drawCalls.erase(ctrl->drawCalls().begin(), ctrl->drawCalls().end());
#endif
}

void
Rendering::render()
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
