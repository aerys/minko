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

#include "SurfaceController.hpp"
#include "minko/scene/Node.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/DrawCall.hpp"

using namespace minko::controller;
using namespace minko::geometry;
using namespace minko::render;
using namespace minko::data;
using namespace minko::scene;

SurfaceController::SurfaceController(std::shared_ptr<Geometry> 	geometry,
									 std::shared_ptr<Provider> 	material,
									 std::shared_ptr<Effect>	effect) :
	AbstractController(),
	_geometry(geometry),
	_material(material),
	_effect(effect),
	_drawCalls()
{
}

void
SurfaceController::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&SurfaceController::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&SurfaceController::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
SurfaceController::targetAddedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target)
{
	target->data()->addProvider(_material);
	target->data()->addProvider(_geometry->data());
	target->data()->addProvider(_effect->data());

	_drawCalls.clear();
	for (auto shader : _effect->shaders())
	{
		auto drawCall = DrawCall::create(target->data(), _effect->inputNameToBindingName());

		_drawCalls.push_back(drawCall);
	}
}

void
SurfaceController::targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target)
{
	target->data()->removeProvider(_material);
	target->data()->removeProvider(_geometry->data());
	target->data()->removeProvider(_effect->data());

	_drawCalls.clear();
}
