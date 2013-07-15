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

#include "Surface.hpp"

#include "minko/scene/Node.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/Pass.hpp"
#include "minko/data/Container.hpp"

using namespace minko::controller;
using namespace minko::geometry;
using namespace minko::render;

Surface::Surface(Geometry::Ptr 			geometry,
				 data::Provider::Ptr 	material,
				 Effect::Ptr			effect) :
	AbstractController(),
	_geometry(geometry),
	_material(material),
	_effect(effect),
	_drawCalls()
{
}

void
Surface::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&Surface::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&Surface::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
Surface::geometry(std::shared_ptr<geometry::Geometry> newGeometry)
{
	for (unsigned int i = 0; i < numTargets(); ++i)
	{
		std::shared_ptr<scene::Node> target = getTarget(i);

		target->data()->removeProvider(_geometry->data());
		target->data()->addProvider(newGeometry->data());

		_drawCalls.clear();

		for (auto pass : _effect->passes())
			_drawCalls.push_back(pass->createDrawCall(target->data()));
	}

	_geometry = newGeometry;
}

void
Surface::targetAddedHandler(AbstractController::Ptr	ctrl,
							scene::Node::Ptr		target)

{
	auto data = target->data();

	data->addProvider(_material);
	data->addProvider(_geometry->data());

	_drawCalls.clear();

	for (auto pass : _effect->passes())
		_drawCalls.push_back(pass->createDrawCall(data));
}

void
Surface::targetRemovedHandler(AbstractController::Ptr	ctrl,
							  scene::Node::Ptr			target)
{
	auto data = target->data();

	data->removeProvider(_material);
	data->removeProvider(_geometry->data());

	_drawCalls.clear();
}
