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
#include "minko/math/Matrix4x4.hpp"
#include "minko/data/Container.hpp"
#include "minko/math/Frustum.hpp"

using namespace minko;
using namespace minko::math;

OctTree::OctTree(float							worldSize,
			 	 uint							maxDepth,
				 std::shared_ptr<math::Vector3>	center,
				 uint							depth)
{
	_worldSize  = worldSize;
	_depth		= depth;
	_center		= center;
	_splitted	= false;
	_maxDepth	= maxDepth;

	float edgel = edgeLength();

	_octantBox = math::Box::create(
		math::Vector3::create(_center->x() + edgel, _center->y() + edgel, _center->z() + edgel),
		math::Vector3::create(_center->x() - edgel, _center->y() - edgel, _center->z() - edgel));
}

std::shared_ptr<scene::Node>
OctTree::generateVisual(std::shared_ptr<file::AssetLibrary>		assetLibrary,
						std::shared_ptr<scene::Node>			rootNode)
{
	if (!rootNode)
		rootNode = scene::Node::create();

	auto node = scene::Node::create();

	if (_content.size() > 0)
	{
		node->addComponent(component::Transform::create(math::Matrix4x4::create()
						->appendScale(edgeLength() * 2.f - 0.1f)
						->appendTranslation(_center->x(), _center->y(), _center->z())))
					->addComponent(component::Surface::create(
						geometry::CubeGeometry::create(assetLibrary->context()),
						material::BasicMaterial::create()
							->diffuseColor(0x00FF0030)
							->blendingMode(render::Blending::Mode::ALPHA)
							->triangleCulling(render::TriangleCulling::NONE),
						assetLibrary->effect("effect/Basic.effect")
					));
		rootNode->addChild(node);
		debugNode = node;
	}

	if (_splitted)
		for (auto octant : _children)
			octant->generateVisual(assetLibrary, rootNode);

	return rootNode;
}

void
OctTree::split()
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
				
				_children[index] = OctTree::create(
					_worldSize,
					_maxDepth, 
					math::Vector3::create(
						_center->x() + (x == 0 ? -size / 2 : size / 2),
						_center->y() + (y == 0 ? -size / 2 : size / 2),
						_center->z() + (z == 0 ? -size / 2 : size / 2)),
					_depth + 1);
				_children[index]->_parent = shared_from_this();
			}
		}
	}
	_splitted = true;
}

OctTree::Ptr
OctTree::insert(std::shared_ptr<scene::Node> node)
{
	// already reference by the octTree
	if (_nodeToOctant.find(node) != _nodeToOctant.end())
		return shared_from_this();

	if (!node->hasComponent<component::BoundingBox>())
		node->addComponent(component::BoundingBox::create());

	std::shared_ptr<component::BoundingBox> nodeBoundingBox = node->component<component::BoundingBox>();
	std::shared_ptr<math::Matrix4x4>		modelToWorld	= node->component<component::Transform>()->modelToWorldMatrix(true);

	// insert
	uint								optimalDepth	= std::min(computeDepth(node), _maxDepth);
	uint								currentDepth	= 0;
	OctTree::Ptr						octant			= shared_from_this();

	std::shared_ptr<math::Vector3>		newNodeCenter	= math::Vector3::create(
		(nodeBoundingBox->box()->bottomLeft()->x() + nodeBoundingBox->box()->topRight()->x()) / 2,
		(nodeBoundingBox->box()->bottomLeft()->y() + nodeBoundingBox->box()->topRight()->y()) / 2,
		(nodeBoundingBox->box()->bottomLeft()->z() + nodeBoundingBox->box()->topRight()->z()) / 2);

	for (currentDepth = 0; currentDepth != optimalDepth; ++currentDepth)
	{
		if (!octant->_splitted)
			octant->split();
		
		uint xIndex = 0;
		uint yIndex = 0;
		uint zIndex = 0;

		if (newNodeCenter->x() > octant->_center->x())
			xIndex = 1;
		
		if (newNodeCenter->y() > octant->_center->y())
			yIndex = 1;

		if (newNodeCenter->z() > octant->_center->z())
			zIndex = 1;

		octant = octant->_children[xIndex + (yIndex << 1) + (zIndex << 2)];
	}

	octant->_content.push_back(node);
	octant->addChildContent(node);
	_nodeToOctant[node] = octant;
	_matrixToNode[node->data()->get<std::shared_ptr<math::Matrix4x4>>("transform.modelToWorldMatrix")] = node;

	if (_nodeToTransformChangedSlot.find(node) == _nodeToTransformChangedSlot.end())
	{
		_nodeToTransformChangedSlot[node] = node->data()->propertyValueChanged("transform.modelToWorldMatrix")->connect(std::bind(
			&OctTree::nodeModelToWorldChanged,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
	}

	return shared_from_this();
}

bool
OctTree::nodeChangedOctant(std::shared_ptr<scene::Node> node)
{
	Ptr										octant			= _nodeToOctant[node];
	std::shared_ptr<component::BoundingBox> nodeBoundingBox = node->component<component::BoundingBox>();
	std::shared_ptr<math::Vector3>			nodeCenter		= math::Vector3::create(
		(nodeBoundingBox->box()->bottomLeft()->x() + nodeBoundingBox->box()->topRight()->x()) / 2,
		(nodeBoundingBox->box()->bottomLeft()->y() + nodeBoundingBox->box()->topRight()->y()) / 2,
		(nodeBoundingBox->box()->bottomLeft()->z() + nodeBoundingBox->box()->topRight()->z()) / 2);

	if (sqrt(pow(nodeCenter->x() - octant->_center->x(), 2) + pow(nodeCenter->y() - octant->_center->y(), 2) + pow(nodeCenter->z() - octant->_center->z(), 2)) < octant->edgeLength())
		return false;
	return true;
}

void
OctTree::nodeModelToWorldChanged(data::Container::Ptr	data,
								 const std::string&		propertyName)
{
	/*auto node		= _matrixToNode[data->get<std::shared_ptr<math::Matrix4x4>>(propertyName)];
	auto octant		= _nodeToOctant[node];

	if (node && nodeChangedOctant(node)) // node is no more in the octant
	{
		_matrixToNode.erase(data->get<std::shared_ptr<math::Matrix4x4>>(propertyName));
		_nodeToOctant.erase(node);
		octant->_content.remove(node);
		insert(node);
	}*/
}

void
OctTree::testFrustum(std::shared_ptr<math::AbstractShape>				frustum, 
					 std::function<void(std::shared_ptr<scene::Node>)>	insideFrustumCallback,
					 std::function<void(std::shared_ptr<scene::Node>)>	outsideFustumCallback)
{
	ShapePosition result					= frustum->testBoundingBox(_octantBox);
	
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

OctTree::Ptr
OctTree::remove(std::shared_ptr<scene::Node> node)
{
	// not reference by the octTree
	if (_nodeToOctant.find(node) == _nodeToOctant.end())
		return shared_from_this();

	Ptr octant = _nodeToOctant[node];

	octant->_content.remove(node);

	_nodeToOctant.erase(node);
	_matrixToNode.erase(node->data()->get<math::Matrix4x4::Ptr>("transform.modelToWorldMatrix"));
	

	return shared_from_this();
}

uint
OctTree::computeDepth(std::shared_ptr<scene::Node> node)
{
	component::Surface::Ptr surface = node->component<component::Surface>();
	float					radius	= computeRadius(node->component<component::BoundingBox>());

	return (uint)floor(log(_worldSize / radius) / log(2));
}

float
OctTree::computeRadius(std::shared_ptr<component::BoundingBox> boundingBox)
{
	return std::max(boundingBox->box()->width() / 2.f,  std::max(boundingBox->box()->height() / 2.f, boundingBox->box()->depth() / 2.f)); 
}

float
OctTree::edgeLength()
{
	return (float)_k * (_worldSize / powf(2.f, (float)_depth));
}


void
OctTree::addChildContent(NodePtr node)
{
	if (_parent)
	{
		_parent->_childrenContent.push_back(node);
		_parent->addChildContent(node);
	}
}

void
OctTree::removeChildContent(NodePtr node)
{
	if (_parent)
	{
		_parent->_childrenContent.remove(node);
		_parent->removeChildContent(node);
	}
}