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

#include "minko/math/OctTree.hpp"

#include "minko/component/Surface.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/scene/Node.hpp"
#include "minko/math/Box.hpp"
#include "minko/component/Transform.hpp"
#include "minko/geometry/CubeGeometry.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/material/BasicMaterial.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/data/Store.hpp"
#include "minko/math/Frustum.hpp"

using namespace minko;

math::OctTree::OctTree(float				worldSize,
					   uint					maxDepth,
					   const math::vec3&	center,
					   uint					depth) :
	_maxDepth(maxDepth),
	_depth(depth),
	_splitted(false),
	_worldSize(worldSize),
	_center(center)
{
	float edgel = edgeLength();

	_octantBox = math::Box::create(_center + edgel, _center - edgel);
}

std::shared_ptr<scene::Node>
math::OctTree::generateVisual(std::shared_ptr<file::AssetLibrary>	assetLibrary,
							  std::shared_ptr<scene::Node>			rootNode)
{
	if (!rootNode)
		rootNode = scene::Node::create();

	auto node = scene::Node::create();

	if (_content.size() > 0)
	{
        auto matrix =
            math::translate(_center) *
            math::scale(math::vec3(edgeLength() * 2.f - 0.1f));

        auto material = material::BasicMaterial::create();

        material->diffuseColor(0x00FF0020);
		material->blendingMode(render::Blending::Mode::ALPHA);
        material->triangleCulling(render::TriangleCulling::NONE);
        material->priority(render::Priority::TRANSPARENT);

		node
			->addComponent(component::Transform::create(matrix))
			->addComponent(component::Surface::create(
				geometry::CubeGeometry::create(assetLibrary->context()),
				material,
				assetLibrary->effect("effect/Basic.effect")
			));
		rootNode->addChild(node);
		_debugNode = node;
	}

	if (_splitted)
		for (auto octant : _children)
			octant->generateVisual(assetLibrary, rootNode);

	return node;
}

void
math::OctTree::split()
{
	this->_children.resize(8);

	float size = (float)_k * _worldSize / (powf(2.f, (float)_depth));

	for (uint x = 0; x < 2; ++x)
	{
		for (uint y = 0; y < 2; ++y)
		{
			for (uint z = 0; z < 2; ++z)
			{
				uint index = x + (y << 1) + (z << 2);

				_children[index] = math::OctTree::create(
					_worldSize,
					_maxDepth,
					math::vec3(
						_center.x + (x == 0 ? -size / 2 : size / 2),
						_center.y + (y == 0 ? -size / 2 : size / 2),
						_center.z + (z == 0 ? -size / 2 : size / 2)
					),
					_depth + 1
				);
				_children[index]->_parent = shared_from_this();
			}
		}
	}
	_splitted = true;
}

math::OctTree::Ptr
math::OctTree::insert(std::shared_ptr<scene::Node> node)
{
	// already reference by the octTree
	if (_nodeToOctants.find(node) != _nodeToOctants.end())
		return shared_from_this();

	if (!node->hasComponent<component::BoundingBox>())
		node->addComponent(component::BoundingBox::create());

	uint optimalDepth = std::min(computeDepth(node), _maxDepth);
	uint currentDepth = 0u;
	auto octant = shared_from_this();

    doInsert(octant, node, 0, optimalDepth);

	return shared_from_this();
}

void
math::OctTree::doInsert(Ptr octant, NodePtr node, unsigned int currentDepth, unsigned int optimalDepth)
{
    if (currentDepth == optimalDepth)
    {
        octant->_content.push_back(node);

        octant->addChildContent(node);

        return;
    }

    if (!octant->_splitted)
        octant->split();

    node->component<component::Transform>()->updateModelToWorldMatrix();
    auto nodeBox = node->component<component::BoundingBox>()->box();

    const auto nodeMinBound = nodeBox->bottomLeft();
    const auto nodeMaxBound = nodeBox->topRight();

    for (auto childOctant : octant->_children)
    {
        const auto childMinBound = childOctant->_octantBox->bottomLeft();
        const auto childMaxBound = childOctant->_octantBox->topRight();

        if (nodeMinBound.x >= childMaxBound.x ||
            nodeMaxBound.x < childMinBound.x)
            continue;

        if (nodeMinBound.y >= childMaxBound.y ||
            nodeMaxBound.y < childMinBound.y)
            continue;

        if (nodeMinBound.z >= childMaxBound.z ||
            nodeMaxBound.z < childMinBound.z)
            continue;

        doInsert(childOctant, node, currentDepth + 1u, optimalDepth);
    }
}

bool
math::OctTree::nodeChangedOctant(std::shared_ptr<scene::Node> node)
{
    // FIXME

/*
	auto octant = _nodeToOctants[node].front();
	auto nodeBoundingBox = node->component<component::BoundingBox>();

	math::vec3 nodeCenter(
		(nodeBoundingBox->box()->bottomLeft().x + nodeBoundingBox->box()->topRight().x) / 2.f,
		(nodeBoundingBox->box()->bottomLeft().y + nodeBoundingBox->box()->topRight().y) / 2.f,
		(nodeBoundingBox->box()->bottomLeft().z + nodeBoundingBox->box()->topRight().z) / 2.f
	);

	if (::sqrt(pow(nodeCenter.x - octant->_center.x, 2.f) + pow(nodeCenter.y - octant->_center.y, 2.f) + pow(nodeCenter.z - octant->_center.z, 2.f)) < octant->edgeLength())
		return false;
*/

	return false;
}

void
math::OctTree::nodeModelToWorldChanged()
{
    // FIXME
	/*auto node		= _matrixToNode[data->get<std::shared_ptr<math::mat4>>(propertyName)];
	auto octant		= _nodeToOctant[node];

	if (node && nodeChangedOctant(node)) // node is no more in the octant
	{
		_matrixToNode.erase(data->get<std::shared_ptr<math::mat4>>(propertyName));
		_nodeToOctant.erase(node);
		octant->_content.remove(node);
		insert(node);
	}*/
}

void
math::OctTree::testFrustum(std::shared_ptr<math::AbstractShape>				frustum,
					 std::function<void(std::shared_ptr<scene::Node>)>	insideFrustumCallback,
					 std::function<void(std::shared_ptr<scene::Node>)>	outsideFustumCallback)
{
    for (auto node : _childrenContent)
        _nodeToInsideFrustumRefCountMap[node] = 0u;

    doTestFrustum(frustum, _nodeToInsideFrustumRefCountMap);

    for (auto nodeToInsideFrustumRefCountPair : _nodeToInsideFrustumRefCountMap)
    {
        auto node = nodeToInsideFrustumRefCountPair.first;

        if (nodeToInsideFrustumRefCountPair.second == 0u)
        {
            outsideFustumCallback(node);
        }
        else
        {
            insideFrustumCallback(node);
        }
    }
}

void
math::OctTree::doTestFrustum(math::AbstractShape::Ptr                   frustum,
                             std::unordered_map<NodePtr, unsigned int>& refCountMap)
{
    const auto result = frustum->testBoundingBox(_octantBox);

    if (result == ShapePosition::AROUND || result == ShapePosition::INSIDE)
    {
        if (_splitted)
        {
            for (auto octantChild : _children)
                octantChild->doTestFrustum(frustum, refCountMap);
        }

        for (auto node : _content)
            ++refCountMap[node];
    }
}

math::OctTree::Ptr
math::OctTree::remove(std::shared_ptr<scene::Node> node)
{
	// not reference by the octTree
	if (_nodeToOctants.find(node) == _nodeToOctants.end())
		return shared_from_this();

    // FIXME

/*
	auto octant = _nodeToOctant[node];

	octant->_content.remove(node);

	_nodeToOctant.erase(node);
	_matrixToNode.erase(node->data()->get<math::mat4::Ptr>("transform.modelToWorldMatrix"));
*/

	return shared_from_this();
}

uint
math::OctTree::computeDepth(std::shared_ptr<scene::Node> node)
{
	auto surface = node->component<component::Surface>();
	auto radius	= computeRadius(node->component<component::BoundingBox>());

	return (uint)std::floor(std::log(_worldSize / radius) / std::log(2));
}

float
math::OctTree::computeRadius(std::shared_ptr<component::BoundingBox> boundingBox)
{
	return std::max(
		boundingBox->box()->width() / 2.f,
		std::max(
			boundingBox->box()->height() / 2.f,
			boundingBox->box()->depth() / 2.f
		)
	);
}

float
math::OctTree::edgeLength()
{
	return (float)_k * (_worldSize / powf(2.f, (float)_depth));
}


void
math::OctTree::addChildContent(NodePtr node)
{
	if (_parent)
	{
		_parent->_childrenContent.push_back(node);
		_parent->addChildContent(node);
	}
}

void
math::OctTree::removeChildContent(NodePtr node)
{
	if (_parent)
	{
		_parent->_childrenContent.remove(node);
		_parent->removeChildContent(node);
	}
}
