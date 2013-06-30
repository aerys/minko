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

#include "FrameController.hpp"

#include "minko/Signal.hpp"

using namespace minko::controller;

FrameController::FrameController() :
	_app(Signal<FrameController::Ptr>::create()),
	_cull(Signal<FrameController::Ptr>::create()),
	_draw(Signal<FrameController::Ptr>::create())
{
}

void
FrameController::nextFrame()
{
	// TODO: run each signal in a separate thread?

	_app->execute(shared_from_this());

	_cull->execute(shared_from_this());

	_draw->execute(shared_from_this());
}

void
FrameController::targetAddedHandler(std::shared_ptr<AbstractController>	controller,
									std::shared_ptr<Node>				target)
{
	// FIXME
}

void
FrameController::addedHandler(std::shared_ptr<Node>	node,
							  std::shared_ptr<Node> target,
							  std::shared_ptr<Node> parent)
{
	// FIXME
}

void
FrameController::controllerAddedHandler(std::shared_ptr<Node>				node,
					 				    std::shared_ptr<Node>				target,
									    std::shared_ptr<AbstractController>	controller)
{
	if (std::dynamic_pointer_cast<FrameController>(controller))
		throw;
}
