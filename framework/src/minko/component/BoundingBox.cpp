/*
Copyright (c) 2014 Aerys

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

#include "minko/component/BoundingBox.hpp"

#include "minko/math/Box.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/data/Store.hpp"
#include "minko/CloneOption.hpp"

using namespace minko;
using namespace minko::component;

BoundingBox::BoundingBox(const math::vec3& topRight, const math::vec3& bottomLeft) :
	_fixed(true),
	_box(math::Box::create(topRight, bottomLeft)),
	_worldSpaceBox(math::Box::create(topRight, bottomLeft)),
	_invalidBox(true),
	_invalidWorldSpaceBox(true)
{

}

BoundingBox::BoundingBox() :
	_fixed(false),
	_box(math::Box::create()),
	_worldSpaceBox(math::Box::create()),
	_invalidBox(true),
	_invalidWorldSpaceBox(true)
{

}

BoundingBox::BoundingBox(const BoundingBox& bbox, const CloneOption& option) :
	_fixed(bbox._fixed),
	_box(option == CloneOption::SHALLOW ? bbox._box : math::Box::create(bbox._box->topRight(), bbox._box->bottomLeft())),
	_worldSpaceBox(option == CloneOption::SHALLOW ? bbox._worldSpaceBox : math::Box::create(bbox._worldSpaceBox->topRight(), bbox._worldSpaceBox->bottomLeft())),
	_invalidBox(bbox._invalidBox),
	_invalidWorldSpaceBox(bbox._invalidWorldSpaceBox)
{

}

AbstractComponent::Ptr
BoundingBox::clone(const CloneOption& option)
{
	Ptr bbox(new BoundingBox(*this, option));

	return bbox;
}

void
BoundingBox::targetAdded(scene::Node::Ptr target)
{
	_modelToWorldChangedSlot = target->data().propertyChanged("modelToWorldMatrix").connect(
		[&](data::Store& data, data::Provider::Ptr provider, const data::Provider::PropertyName& propertyName)
		{
			_invalidWorldSpaceBox = true;
		}
	);

        auto componentAddedOrRemovedCallback = [=](scene::Node::Ptr node, scene::Node::Ptr target, AbstractComponent::Ptr cmp)
	{
		if (std::dynamic_pointer_cast<Surface>(cmp))
		{
			_invalidBox = true;
			_invalidWorldSpaceBox = true;
		}
	};

	_componentAddedSlot = target->componentAdded().connect(componentAddedOrRemovedCallback);
	_componentRemovedSlot = target->componentAdded().connect(componentAddedOrRemovedCallback);

	_invalidBox = true;
}

void
BoundingBox::targetRemoved(scene::Node::Ptr target)
{
	_componentAddedSlot = nullptr;
	_componentRemovedSlot = nullptr;
}

void
BoundingBox::computeBox(const std::vector<component::Surface::Ptr>& surfaces, math::vec3& min, math::vec3& max)
{
    for (auto& surface : surfaces)
    {
        auto geom = surface->geometry();
        if (geom->hasVertexAttribute("position"))
        {
            auto xyzBuffer = geom->vertexBuffer("position");
            auto offset = xyzBuffer->attribute("position").offset;

            for (uint i = 0; i < xyzBuffer->numVertices(); ++i)
            {
                auto x = xyzBuffer->data()[i * xyzBuffer->vertexSize() + offset];
                auto y = xyzBuffer->data()[i * xyzBuffer->vertexSize() + offset + 1];
                auto z = xyzBuffer->data()[i * xyzBuffer->vertexSize() + offset + 2];

                if (x < min.x)
                	min.x = x;
                if (x > max.x)
                	max.x = x;

                if (y < min.y)
                	min.y = y;
                if (y > max.y)
                	max.y = y;

                if (z < min.z)
                	min.z = z;
                if (z > max.z)
                	max.z = z;
            }
        }
        else
        {
        	min = { 0.f, 0.f, 0.f };
        	max = { 0.f, 0.f, 0.f };
        }
    }
}

void
BoundingBox::update()
{
    _invalidBox = false;

    auto target = this->target();

    if (!_fixed)
    {
        auto surfaces = target->components<Surface>();

		auto min = math::vec3(
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max()
		);
		auto max = math::vec3(
			-std::numeric_limits<float>::max(),
			-std::numeric_limits<float>::max(),
			-std::numeric_limits<float>::max()
		);

        if (!surfaces.empty())
        {
            computeBox(surfaces, min, max);

			_box->bottomLeft(min);
			_box->topRight(max);
		}
		else
		{
			_box->bottomLeft(math::vec3(0.));
			_box->topRight(math::vec3(0.));
		}
	}

	_invalidWorldSpaceBox = true;
}

void
BoundingBox::updateWorldSpaceBox()
{
	if (_invalidBox)
		update();

	_invalidWorldSpaceBox = false;

	if (!target()->data().hasProperty("modelToWorldMatrix"))
	{
		_worldSpaceBox->topRight(_box->topRight());
		_worldSpaceBox->bottomLeft(_box->bottomLeft());
	}
	else
	{
		auto t = target()->data().get<math::mat4>("modelToWorldMatrix");
		auto vertices = _box->getVertices();
		auto numVertices = vertices.size();

		for (uint i = 0; i < numVertices; ++i)
			vertices[i] = (t * math::vec4(vertices[i], 1.f)).xyz();

		auto max = math::vec3(
			-std::numeric_limits<float>::max(),
			-std::numeric_limits<float>::max(),
			-std::numeric_limits<float>::max()
		);
		auto min = math::vec3(
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max()
		);

		for (auto& vertex : vertices)
		{
			if (vertex.x > max.x)
				max.x = vertex.x;
			if (vertex.x < min.x)
				min.x = vertex.x;

			if (vertex.y > max.y)
				max.y = vertex.y;
			if (vertex.y < min.y)
				min.y = vertex.y;

			if (vertex.z > max.z)
				max.z = vertex.z;
			if (vertex.z < min.z)
				min.z = vertex.z;
		}

		_worldSpaceBox->topRight(max);
		_worldSpaceBox->bottomLeft(min);
	}
}
