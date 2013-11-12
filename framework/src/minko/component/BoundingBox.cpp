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

#include "BoundingBox.hpp"

#include "minko/math/Box.hpp"
#include "minko/scene/Node.hpp"
#include "minko/component/Transform.hpp"

using namespace minko;
using namespace minko::component;

BoundingBox::BoundingBox(std::shared_ptr<math::Vector3> topRight, std::shared_ptr<math::Vector3> bottomLeft) :
	_fixed(true),
	_box(math::Box::create(topRight, bottomLeft)),
	_worldSpaceBox(math::Box::create(topRight, bottomLeft))
{

}

BoundingBox::BoundingBox() :
	_fixed(false),
	_box(math::Box::create()),
	_worldSpaceBox(math::Box::create())
{

}

void
BoundingBox::initialize()
{
	_targetAddedSlot = targetAdded()->connect([&](AbstractComponent::Ptr cmp, scene::Node::Ptr target)
	{
		if (targets().size() > 1)
			throw std::logic_error("The same BoundingBox cannot have 2 different targets");

		update();
	});

	_targetRemovedSlot = targetRemoved()->connect([&](AbstractComponent::Ptr cmp, scene::Node::Ptr target)
	{

	});
}

void
BoundingBox::update()
{
	auto target = targets()[0];

	if (!_fixed)
	{
		//for (auto& geometry : target->components<geometry::Geometry>())
		{
			// get geometry bounds and update _box
		}
	}

	// update _worldSpaceBox
	auto t = target->component<Transform>();

	if (t)
	{
		t->transform()->transform(_box->topRight(), _worldSpaceBox->topRight());
		t->transform()->transform(_box->bottomLeft(), _worldSpaceBox->bottomLeft());
	}
	else
	{
		_worldSpaceBox->topRight()->copyFrom(_box->topRight());
		_worldSpaceBox->bottomLeft()->copyFrom(_box->bottomLeft());
	}
}
