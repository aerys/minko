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

#include "MeshRenderer.hpp"

#include "minko/scene/Node.hpp"
#include "minko/controller/Surface.hpp"

using namespace minko::controller;

MeshRenderer::MeshRenderer()
{
}

void
MeshRenderer::initialize()
{
	targetAdded()->connect(std::bind(
		&MeshRenderer::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));
	targetRemoved()->connect(std::bind(
		&MeshRenderer::targetRemovedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));
}

void
MeshRenderer::targetAddedHandler(AbstractController::Ptr ctrl, NodePtr node)
{
	_controllerSlots[node].push_back(node->controllerAdded()->connect(std::bind(
		&MeshRenderer::controllerAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_controllerSlots[node].push_back(node->controllerRemoved()->connect(std::bind(
		&MeshRenderer::controllerRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));

	for (auto surface : node->controllers<Surface>())
		addSurface(surface);
}

void
MeshRenderer::targetRemovedHandler(AbstractController::Ptr ctrl, NodePtr node)
{
	_controllerSlots.erase(node);
}

void
MeshRenderer::controllerAddedHandler(NodePtr node, NodePtr target, AbstractController::Ptr ctrl)
{
	auto surface = std::dynamic_pointer_cast<Surface>(ctrl);

	if (surface)
		addSurface(surface);
}

void
MeshRenderer::controllerRemovedHandler(NodePtr node, NodePtr target, AbstractController::Ptr ctrl)
{
	auto surface = std::dynamic_pointer_cast<Surface>(ctrl);

	if (surface)
		removeSurface(surface);
}

void
MeshRenderer::addSurface(std::shared_ptr<Surface> surface)
{
	// FIXME
}

void
MeshRenderer::removeSurface(std::shared_ptr<Surface> surface)
{
	// FIXME
}
