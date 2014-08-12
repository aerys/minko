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
#include "minko/data/Container.hpp"
#include "minko/math/Frustum.hpp"

using namespace minko;

math::OctTree::OctTree(float				worldSize,
					   uint					maxDepth,
					   const math::vec3&	center,
					   uint					depth) :
	_worldSize(worldSize),
	_depth(depth),
	_center(center),
	_splitted(false),
	_maxDepth(maxDepth)
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
		math::mat4 t;

		math::scale(t, math::vec3(1.f));
		math::translate(t, _center);

        auto material = material::BasicMaterial::create();

        material->diffuseColor(0x00FF0030)
            .blendingMode(render::Blending::Mode::ALPHA)
            .triangleCulling(render::TriangleCulling::NONE);

		node
			->addComponent(component::Transform::create(t))
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

	return rootNode;
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
	if (_nodeToOctant.find(node) != _nodeToOctant.end())
		return shared_from_this();

	if (!node->hasComponent<component::BoundingBox>())
		node->addComponent(component::BoundingBox::create());

	auto nodeBoundingBox 	= node->component<component::BoundingBox>();
	//auto modelToWorld 		= node->component<component::Transform>()->modelToWorldMatrix(true);

	// insert
	uint optimalDepth	= std::min(computeDepth(node), _maxDepth);
	uint currentDepth	= 0;
	auto octant			= shared_from_this();

	math::vec3		newNodeCenter(
		(nodeBoundingBox->box()->bottomLeft().x + nodeBoundingBox->box()->topRight().x) / 2.f,
		(nodeBoundingBox->box()->bottomLeft().y + nodeBoundingBox->box()->topRight().y) / 2.f,
		(nodeBoundingBox->box()->bottomLeft().z + nodeBoundingBox->box()->topRight().z) / 2.f
	);

	for (currentDepth = 0; currentDepth != optimalDepth; ++currentDepth)
	{
		if (!octant->_splitted)
			octant->split();
		
		uint xIndex = 0;
		uint yIndex = 0;
		uint zIndex = 0;

		if (newNodeCenter.x > octant->_center.x)
			xIndex = 1;
		
		if (newNodeCenter.y > octant->_center.y)
			yIndex = 1;

		if (newNodeCenter.z > octant->_center.z)
			zIndex = 1;

		octant = octant->_children[xIndex + (yIndex << 1) + (zIndex << 2)];
	}

	octant->_content.push_back(node);
	octant->addChildContent(node);
	_nodeToOctant[node] = octant;
	//_matrixToNode[node->data()->get<math::mat4>("transform.modelToWorldMatrix")] = node;

	if (_nodeToTransformChangedSlot.find(node) == _nodeToTransformChangedSlot.end())
	{
		_nodeToTransformChangedSlot[node] = node->data().propertyChanged("transform.modelToWorldMatrix")->connect(std::bind(
			&math::OctTree::nodeModelToWorldChanged,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2
        ));
	}

	return shared_from_this();
}

bool
math::OctTree::nodeChangedOctant(std::shared_ptr<scene::Node> node)
{
	auto octant = _nodeToOctant[node];
	auto nodeBoundingBox = node->component<component::BoundingBox>();

	math::vec3 nodeCenter(
		(nodeBoundingBox->box()->bottomLeft().x + nodeBoundingBox->box()->topRight().x) / 2.f,
		(nodeBoundingBox->box()->bottomLeft().y + nodeBoundingBox->box()->topRight().y) / 2.f,
		(nodeBoundingBox->box()->bottomLeft().z + nodeBoundingBox->box()->topRight().z) / 2.f
	);

	if (::sqrt(pow(nodeCenter.x - octant->_center.x, 2.f) + pow(nodeCenter.y - octant->_center.y, 2.f) + pow(nodeCenter.z - octant->_center.z, 2.f)) < octant->edgeLength())
		return false;
	return true;
}

void
math::OctTree::nodeModelToWorldChanged(data::Container&     data,
								       const std::string&	propertyName)
{
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
	auto result = frustum->testBoundingBox(_octantBox);
	
	if (result == ShapePosition::AROUND || result == ShapePosition::INSIDE)
	{
		if (_splitted)
		{
			for (auto octantChild : _children)
				octantChild->testFrustum(frustum, insideFrustumCallback, outsideFustumCallback);
		}

		for (auto node : _content)
			insideFrustumCallback(node);
	}
	else
	{
		for (auto node : _childrenContent)
			outsideFustumCallback(node);
		for (auto node : _content)
			insideFrustumCallback(node);
	}
}

math::OctTree::Ptr
math::OctTree::remove(std::shared_ptr<scene::Node> node)
{
	// not reference by the octTree
	if (_nodeToOctant.find(node) == _nodeToOctant.end())
		return shared_from_this();

	auto octant = _nodeToOctant[node];

	octant->_content.remove(node);

	_nodeToOctant.erase(node);
	//_matrixToNode.erase(node->data()->get<math::mat4::Ptr>("transform.modelToWorldMatrix"));
	

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
