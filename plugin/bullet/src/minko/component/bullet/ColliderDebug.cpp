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
#include "minko/component/bullet/Collider.hpp"
#include "minko/component/bullet/ColliderData.hpp"
#include "minko/component/bullet/AbstractPhysicsShape.hpp"
#include "minko/component/Transform.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/component/Surface.hpp"
#include "minko/geometry/LineGeometry.hpp"
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
    _node(nullptr),
	_physicsTransformChangedSlot(nullptr),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr)
{
	if (_assets == nullptr ||
		_assets->context() == nullptr ||
		_assets->effect("line") == nullptr)
	{
		throw std::invalid_argument("assets");
	}
}

AbstractComponent::Ptr
bullet::ColliderDebug::clone(const CloneOption& option)
{
	ColliderDebug::Ptr origin = std::static_pointer_cast<ColliderDebug>(shared_from_this());

    if (target() != nullptr)
    {
        for (auto component : target()->components<Surface>())
        {
            if (component->name() == "ColliderDebugSurface")
            {
                target()->removeComponent(component);
            }
        }
    }

	return ColliderDebug::create(origin->_assets);
}

void
bullet::ColliderDebug::targetAdded(scene::Node::Ptr	target)
{
	_addedSlot = target->added().connect(std::bind(
		&ColliderDebug::addedHandler,
		std::static_pointer_cast<ColliderDebug>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedSlot = target->removed().connect(std::bind(
		&ColliderDebug::removedHandler,
		std::static_pointer_cast<ColliderDebug>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));
}

void
bullet::ColliderDebug::targetRemoved(Node::Ptr target)
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

	if (!target()->hasComponent<Collider>())
		return;

	auto collider = target()->component<Collider>();
	assert(collider);
	
	auto geomCollider = collider->colliderData()->shape()->getGeometry(_assets->context());

    auto material = material::Material::create("material");
    material->data()->set("diffuseColor", math::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    material->data()->set("lineThickness", 1.0f);
    material->data()->set("depthFunc", render::CompareMode::ALWAYS);
    material->data()->set("priority", render::Priority::LAST);;

	_surface = Surface::create(
		"ColliderDebugSurface",
		geomCollider,
		material,
		_assets->effect("line"),
		"default"
	);

    _node = Node::create()
        ->addComponent(_surface)
        ->addComponent(Transform::create());

    target()->root()->addChild(_node);

    _physicsTransformChangedSlot = collider->physicsTransformChanged()->connect(std::bind(
        &bullet::ColliderDebug::physicsTransformChangedHandler,
        std::static_pointer_cast<ColliderDebug>(shared_from_this()),
        std::placeholders::_1,
        std::placeholders::_2
    ));

    if (_node)
        _node->component<Transform>()->matrix(collider->getPhysicsTransform());
}

void
bullet::ColliderDebug::addedHandler(Node::Ptr node, Node::Ptr target, Node::Ptr)
{
	initializeDisplay();
}

void
bullet::ColliderDebug::removedHandler(Node::Ptr, Node::Ptr, Node::Ptr)
{
}

void
bullet::ColliderDebug::physicsTransformChangedHandler(Collider::Ptr, const math::mat4& physicsTransform)
{
	if (_node)
		_node->component<Transform>()->matrix(physicsTransform);
}
