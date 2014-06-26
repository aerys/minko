/*,
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

#include "minko/component/bullet/ColliderDebug.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/component/bullet/Collider.hpp"
#include "minko/component/bullet/ColliderData.hpp"
#include "minko/component/bullet/AbstractPhysicsShape.hpp"
#include "minko/component/Transform.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/component/Surface.hpp"
#include "minko/geometry/LineGeometry.hpp"
#include "minko/data/ArrayProvider.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/render/Priority.hpp"
#include "minko/material/Material.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::math;

bullet::ColliderDebug::ColliderDebug(file::AssetLibrary::Ptr assets) :
AbstractComponent(),
_assets(assets),
_surface(nullptr),
_physicsTransformChangedSlot(nullptr),
_targetAddedSlot(nullptr),
_targetRemovedSlot(nullptr),
_addedSlot(nullptr),
_removedSlot(nullptr)
{
	if (_assets == nullptr ||
		_assets->context() == nullptr ||
		_assets->effect("line") == nullptr)
		throw std::invalid_argument("assets");
}

AbstractComponent::Ptr
bullet::ColliderDebug::clone(const CloneOption& option)
{
	ColliderDebug::Ptr origin = std::static_pointer_cast<ColliderDebug>(shared_from_this());
	for (auto component : targets().front()->components<Surface>())
	{	
		if (component->name() == "ColliderDebugSurface") 
		{
			targets().front()->removeComponent(component);
		}
	}
	return ColliderDebug::create(origin->_assets);
}

void
bullet::ColliderDebug::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&ColliderDebug::targetAddedHandler,
		std::static_pointer_cast<ColliderDebug>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&ColliderDebug::targetRemovedHandler,
		std::static_pointer_cast<ColliderDebug>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2
		));
}

void
bullet::ColliderDebug::targetAddedHandler(AbstractComponent::Ptr, Node::Ptr target)
{
	if (targets().size() > 1)
		throw std::logic_error("Collider debugging component cannot be added twice.");

	_addedSlot = target->added()->connect(std::bind(
		&ColliderDebug::addedHandler,
		std::static_pointer_cast<ColliderDebug>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
		));

	_removedSlot = target->removed()->connect(std::bind(
		&ColliderDebug::removedHandler,
		std::static_pointer_cast<ColliderDebug>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
		));

	initializeDisplay();
}

void
bullet::ColliderDebug::targetRemovedHandler(AbstractComponent::Ptr, Node::Ptr target)
{
	_surface = nullptr;
	_physicsTransformChangedSlot = nullptr;

	_addedSlot = nullptr;
	_removedSlot = nullptr;
}

void
bullet::ColliderDebug::initializeDisplay()
{
	if (_surface)
		return; // Collider is already being tracked

	if (targets().empty() || !targets().front()->hasComponent<Collider>())
		return;

	auto collider = targets().front()->component<Collider>();
	assert(collider);

	/*_node = Node::create("collider_debug_" + targets().front()->name())
	->addComponent(Surface::create(
	collider->colliderData()->shape()->getGeometry(_assets->context()),
	data::ArrayProvider::create("material")
	->set("diffuseColor",	math::Vector4::create(0.0f, 1.0f, 1.0f, 1.0f))
	->set("lineThickness",	1.0f)
	->set("depthFunc",		render::CompareMode::ALWAYS)
	->set("priority",		render::Priority::LAST),
	_assets->effect("line")
	))
	->addComponent(Transform::create(
	collider->getPhysicsTransform()
	));*/
	
	auto geom_collider = collider->colliderData()->shape()->getGeometry(_assets->context());
	std::cout << "target front name" << targets().front()->name() << std::endl;
	auto geom_target = targets().front()->root()->component<Surface>()->geometry();

	auto target = targets().front()->root();

	_surface = Surface::create(
		"ColliderDebugSurface",
		geom_collider,
		material::Material::create("material")
		->set("diffuseColor", math::Vector4::create(0.0f, 1.0f, 1.0f, 1.0f))
		->set("lineThickness", 1.0f)
		->set("depthFunc", render::CompareMode::ALWAYS)
		->set("priority", render::Priority::LAST),
		_assets->effect("line"),
		"default"
		);

	/*_physicsTransformChangedSlot = collider->physicsTransformChanged()->connect(std::bind(
	&bullet::ColliderDebug::physicsTransformChangedHandler,
	std::static_pointer_cast<ColliderDebug>(shared_from_this()),
	std::placeholders::_1,
	std::placeholders::_2
	));*/

	targets().front()->root()->addComponent(_surface);
}

void
bullet::ColliderDebug::addedHandler(Node::Ptr node, Node::Ptr target, Node::Ptr)
{
	initializeDisplay();

	std::cout << "target matrix" << target->component<Transform>()->matrix()->toString() << std::endl;
}

void
bullet::ColliderDebug::removedHandler(Node::Ptr, Node::Ptr, Node::Ptr)
{

}

//void
//bullet::ColliderDebug::physicsTransformChangedHandler(Collider::Ptr, 
//													  Matrix4x4::Ptr physicsTransform)
//{
//	if (_node)
//		_node->component<Transform>()->matrix()->copyFrom(physicsTransform);
//}