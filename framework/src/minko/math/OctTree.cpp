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
	_center(center),
    _frustumLastPlaneId(0u)
{
	const auto halfEdgeLength = edgeLength() / 2.f;

	_octantBox = math::Box::create(_center + halfEdgeLength, _center - halfEdgeLength);
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

    const auto halfEdgeLength = edgeLength() / 2.f;

	for (uint x = 0; x < 2; ++x)
	{
		for (uint y = 0; y < 2; ++y)
		{
			for (uint z = 0; z < 2; ++z)
			{
				uint index = x + (y << 1) + (z << 2);

                auto child = math::OctTree::create(
					_worldSize,
					_maxDepth,
					math::vec3(
						_center.x + (x == 0 ? -halfEdgeLength / 2.f : halfEdgeLength / 2.f),
						_center.y + (y == 0 ? -halfEdgeLength / 2.f : halfEdgeLength / 2.f),
						_center.z + (z == 0 ? -halfEdgeLength / 2.f : halfEdgeLength / 2.f)
					),
					_depth + 1
				);
                _children[index] = child;

                child->_parent = shared_from_this();
                child->_root = _root;
			}
		}
	}
	_splitted = true;
}

math::OctTree::Ptr
math::OctTree::insert(std::shared_ptr<scene::Node> node)
{
	if (_nodeToOctant.find(node) != _nodeToOctant.end())
		return shared_from_this();

	if (!node->hasComponent<component::BoundingBox>())
        return shared_from_this();

    node->component<component::Transform>()->updateModelToWorldMatrix();

	uint optimalDepth = std::min(computeDepth(node), _maxDepth);
	uint currentDepth = 0u;

	return _root.lock()->doInsert(node, 0, optimalDepth);
}

math::OctTree::Ptr
math::OctTree::doInsert(NodePtr node, unsigned int currentDepth, unsigned int optimalDepth)
{
    if (!_splitted)
        split();

    auto octants = std::vector<Ptr>();

    if (!childOctantsIntersection(node, octants))
    {
        addToContent(node);

        return shared_from_this();
    }

    const auto childOctantsConflict = octants.size() > 1u;

    auto octant = childOctantsConflict
        ? shared_from_this()
        : octants.front();

    if (childOctantsConflict || currentDepth == optimalDepth)
    {
        octant->addToContent(node);

        return shared_from_this();
    }

    return octant->doInsert(node, currentDepth + 1u, optimalDepth);
}

bool
math::OctTree::intersects(scene::Node::Ptr node)
{
    auto nodeBox = node->component<component::BoundingBox>()->box();

    const auto nodeMinBound = nodeBox->bottomLeft();
    const auto nodeMaxBound = nodeBox->topRight();

    const auto minBound = _octantBox->bottomLeft();
    const auto maxBound = _octantBox->topRight();

    if (nodeMinBound.x >= maxBound.x ||
        nodeMaxBound.x < minBound.x)
        return false;

    if (nodeMinBound.y >= maxBound.y ||
        nodeMaxBound.y < minBound.y)
        return false;

    if (nodeMinBound.z >= maxBound.z ||
        nodeMaxBound.z < minBound.z)
        return false;

    return true;
}

bool
math::OctTree::childOctantsIntersection(scene::Node::Ptr    node,
                                        std::vector<Ptr>&   octants)
{

    auto nodeBox = node->component<component::BoundingBox>()->box();

    const auto nodeMinBound = nodeBox->bottomLeft();
    const auto nodeMaxBound = nodeBox->topRight();

    for (auto childOctant : _children)
    {
        if (!childOctant->intersects(node))
            continue;

        octants.push_back(childOctant);
    }

    return !octants.empty();
}

void
math::OctTree::nodeModelToWorldChanged(scene::Node::Ptr node)
{
    invalidateNode(node);
}

void
math::OctTree::invalidateNode(scene::Node::Ptr node)
{
    _root.lock()->_invalidNodes.insert(node);
}

void
math::OctTree::testFrustum(std::shared_ptr<math::AbstractShape>				    frustum,
					       std::function<void(std::shared_ptr<scene::Node>)>	insideFrustumCallback,
					       std::function<void(std::shared_ptr<scene::Node>)>	outsideFustumCallback)
{
    if (!_invalidNodes.empty())
    {
        for (auto nodeIt = _invalidNodes.begin(); nodeIt != _invalidNodes.end();)
        {
            auto node = *nodeIt;

            nodeIt = _invalidNodes.erase(nodeIt);

            remove(node);
            insert(node);
        }
    }

    auto frustumPtr = std::dynamic_pointer_cast<Frustum>(frustum);

    if (frustumPtr)
    {
        const auto result = frustumPtr->testBoundingBox(_octantBox, _frustumLastPlaneId);

        const auto shapePosition = result.first;
        _frustumLastPlaneId = result.second;

        if (shapePosition == ShapePosition::AROUND || shapePosition == ShapePosition::INSIDE)
        {
            if (_splitted)
            {
                for (auto octantChild : _children)
                    octantChild->testFrustum(frustum, insideFrustumCallback, outsideFustumCallback);
            }

            for (const auto& nodeEntry : _content)
            {
                auto node = nodeEntry.node;
                auto nodeBox = nodeEntry.box;

                const auto nodeResult = frustumPtr->testBoundingBox(nodeBox, _frustumLastPlaneId);

                if (nodeResult.first == ShapePosition::AROUND || nodeResult.first == ShapePosition::INSIDE)
                    insideFrustumCallback(node);
                else
                    outsideFustumCallback(node);
            }
        }
        else
        {
            for (auto node : _childrenContent)
                outsideFustumCallback(node);
        }
    }
}

math::OctTree::Ptr
math::OctTree::remove(scene::Node::Ptr node)
{
    auto root = _root.lock();

    root->_invalidNodes.erase(node);
    
    return root->doRemove(node);
}

math::OctTree::Ptr
math::OctTree::doRemove(scene::Node::Ptr node)
{
    auto octantIt = _nodeToOctant.find(node);

    if (octantIt == _nodeToOctant.end())
        return shared_from_this();

    auto octant = octantIt->second;

    _childrenContent.erase(
        std::remove(
            _childrenContent.begin(),
            _childrenContent.end(),
            node),
        _childrenContent.end()
    );

    _nodeToOctant.erase(node);

    if (removeFromContent(node) || octant == shared_from_this())
        return shared_from_this();

    return octant->doRemove(node);
}

uint
math::OctTree::computeDepth(std::shared_ptr<scene::Node> node)
{
	auto surface = node->component<component::Surface>();
	auto size = computeSize(node->component<component::BoundingBox>());

	return static_cast<uint>(std::floor(std::log(_worldSize / size) / std::log(2)));
}

float
math::OctTree::computeSize(std::shared_ptr<component::BoundingBox> boundingBox)
{
	return std::max(
		boundingBox->box()->width(),
		std::max(
			boundingBox->box()->height(),
			boundingBox->box()->depth()
		)
	);
}

float
math::OctTree::edgeLength()
{
	return (_worldSize / powf(2.f, float(_depth)));
}

void
math::OctTree::addToChildContent(NodePtr node)
{
    _childrenContent.push_back(node);

    if (_parent.expired())
        return;

    auto parent = _parent.lock();

    parent->_nodeToOctant[node] = shared_from_this();
    parent->addToChildContent(node);
}

void
math::OctTree::addToContent(scene::Node::Ptr node)
{
    _nodeToOctant[node] = shared_from_this();

    addToChildContent(node);

    _content.emplace_back(node, node->component<component::BoundingBox>()->box());

    _nodeToTransformChangedSlot.emplace(node, node->data().propertyChanged("modelToWorldMatrix").connect(
        [this, node](data::Store&                         store,
            data::Provider::Ptr                  provider,
            const data::Provider::PropertyName&  propertyName)
        {
            nodeModelToWorldChanged(node);
        }
    ));
}

bool
math::OctTree::removeFromContent(scene::Node::Ptr node)
{
    auto contentNodeIt = std::find_if(
        _content.begin(),
        _content.end(),
        [node](const NodeEntry& nodeEntry) -> bool { return nodeEntry.node == node; });

    if (contentNodeIt == _content.end())
        return false;

    _content.erase(contentNodeIt);
    _nodeToTransformChangedSlot.erase(node);

    return true;
}
