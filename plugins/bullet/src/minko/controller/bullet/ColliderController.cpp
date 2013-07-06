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

#include "ColliderController.hpp"
#include "minko/scene/Node.hpp"
#include "minko/controller/bullet/Collider.hpp"
#include "minko/controller/bullet/PhysicsWorld.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::controller;

bullet::ColliderController::ColliderController(ColliderPtr collider):
	AbstractController(),
	_collider(collider),
	_target(nullptr),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_physicsWorld(nullptr)
{

}

void
	bullet::ColliderController::initialize()
{
	_targetAddedSlot	= targetAdded()->connect(std::bind(
		&bullet::ColliderController::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_targetRemovedSlot	= targetRemoved()->connect(std::bind(
		&bullet::ColliderController::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_transformChangedSlot	= _collider->transformChanged()->connect(std::bind(
		&bullet::ColliderController::transformChangedHandler,
		shared_from_this(),
		std::placeholders::_1
		));
}

void
	bullet::ColliderController::targetAddedHandler(
	std::shared_ptr<AbstractController> controller, 
	std::shared_ptr<Node> target)
{
	if (_target)
		throw std::logic_error("A collider controller can currently target only one node.");
	_target		= target;

	_addedSlot	= _target->added()->connect(std::bind(
		&bullet::ColliderController::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
		));

	_removedSlot	= _target->removed()->connect(std::bind(
		&bullet::ColliderController::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
		));
}

void
	bullet::ColliderController::targetRemovedHandler(
	std::shared_ptr<AbstractController> controller, 
	std::shared_ptr<Node> target)
{
	_addedSlot				= nullptr;
	_removedSlot			= nullptr;
	_transformChangedSlot	= nullptr;
	_target					= nullptr;
}

void 
	bullet::ColliderController::addedHandler(
	std::shared_ptr<Node> node, 
	std::shared_ptr<Node> target, 
	std::shared_ptr<Node> parent)
{
#ifdef DEBUG
	std::cout << "collider controller's target has been added\tto be added to physics world." << std::endl;
#endif // DEBUG

	_physicsWorld	= getRootPhysicsWorld(target->root());  
	_physicsWorld->addChild(_collider);
}

void
	bullet::ColliderController::removedHandler(
	std::shared_ptr<Node> node, 
	std::shared_ptr<Node> target, 
	std::shared_ptr<Node> parent)
{
	_physicsWorld->removeChild(_collider);
	_physicsWorld	= nullptr;
}

void 
	bullet::ColliderController::transformChangedHandler(ColliderPtr collider)
{
	auto transform	= collider->transform();
	std::cout << "update transform of collider's target in ColliderController::transformChangedHandler" << std::endl;
}

/*static*/
std::shared_ptr<bullet::PhysicsWorld>
	bullet::ColliderController::getRootPhysicsWorld(NodePtr target)
{
	if (target == nullptr)
		throw std::invalid_argument("target");

	// get and count all PhysicsWorld controllers in the descendants of the target's root
	auto nodeSet	= NodeSet::create(target->root())
		->descendants(true)
		->where([](NodePtr node)
	{
		return node->hasController<bullet::PhysicsWorld>();
	});

	uint numControllers	= 0;
	for (auto node: nodeSet->nodes())
		numControllers	+= node->controllers<bullet::PhysicsWorld>().size();

#ifdef DEBUG
	std::cout << numControllers << " rendering controllers" << std::endl;
#endif // DEBUG

	if (numControllers != 1)
		throw std::logic_error("ColliderController requires exactly one PhysicsWorld controller among the descendants of its target node.");

	return nodeSet->nodes()[0]->controllers<bullet::PhysicsWorld>()[0];
}