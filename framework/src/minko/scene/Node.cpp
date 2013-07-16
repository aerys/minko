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

#include "Node.hpp"

#include "minko/component/AbstractComponent.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/data/Container.hpp"

using namespace minko::scene;
using namespace minko::component;

unsigned int Node::_id = 0;

Node::Node() :
	_name("Node_" + std::to_string(Node::_id++)),
	_layers(1),
	_root(nullptr),
	_parent(nullptr),
	_container(data::Container::create()),
	_added(Signal<Ptr, Ptr, Ptr>::create()),
	_removed(Signal<Ptr, Ptr, Ptr>::create()),
	_componentAdded(Signal<Ptr, Ptr, Node::AbsCtrlPtr>::create()),
	_componentRemoved(Signal<Ptr, Ptr, Node::AbsCtrlPtr>::create()),
	_layersChanged(Signal<Ptr, Ptr>::create())
{
}

void
Node::layers(unsigned int layers)
{
	if (_layers != layers)
	{
		_layers = layers;

		// bubble down
        auto descendants = NodeSet::create(shared_from_this())->descendants(true);
		for (auto descendant : descendants->nodes())
			descendant->_layersChanged->execute(descendant, shared_from_this());

		// bubble up
		auto ancestors = NodeSet::create(shared_from_this())->ancestors();
		for (auto ancestor : ancestors->nodes())
			ancestor->_layersChanged->execute(ancestor, shared_from_this());
	}
}

Node::Ptr
Node::addChild(Node::Ptr child)
{
	if (child->_parent)
		child->_parent->removeChild(child);

	_children.push_back(child);

	child->_parent = shared_from_this();
	child->updateRoot();

	// bubble down
	auto descendants = NodeSet::create(child)->descendants(true);
	for (auto descendant : descendants->nodes())
		descendant->_added->execute(descendant, child, shared_from_this());

	// bubble up
	auto ancestors = NodeSet::create(shared_from_this())->ancestors(true);
	for (auto ancestor : ancestors->nodes())
		ancestor->_added->execute(ancestor, child, shared_from_this());

	return shared_from_this();
}

Node::Ptr
Node::removeChild(Node::Ptr child)
{
	std::list<Node::Ptr>::iterator it = std::find(_children.begin(), _children.end(), child);

	if (it == _children.end())
		throw std::invalid_argument("child");

	_children.erase(it);

	child->_parent = nullptr;
	child->updateRoot();

	// bubble down
    auto descendants = NodeSet::create(child)->descendants(true);
	for (auto descendant : descendants->nodes())
		descendant->_removed->execute(descendant, child, shared_from_this());

	// bubble up
	auto ancestors = NodeSet::create(shared_from_this())->ancestors(true);
	for (auto ancestor : ancestors->nodes())
		ancestor->_removed->execute(ancestor, child, shared_from_this());

	return shared_from_this();
}

bool
Node::contains(Node::Ptr node)
{
	return std::find(_children.begin(), _children.end(), node) != _children.end();
}

Node::Ptr
Node::addComponent(std::shared_ptr<AbstractComponent> component)
{
	if (hasComponent(component))
		throw std::logic_error("The same component cannot be added twice.");

	_components.push_back(component);
	component->_targets.push_back(shared_from_this());

    component->targetAdded()->execute(component, shared_from_this());

	// bubble down
	auto descendants = NodeSet::create(shared_from_this())->descendants(true);
	for (auto descendant : descendants->nodes())
		descendant->_componentAdded->execute(descendant, shared_from_this(), component);

	// bubble up
	auto ancestors = NodeSet::create(shared_from_this())->ancestors();
	for (auto ancestor : ancestors->nodes())
		ancestor->_componentAdded->execute(ancestor, shared_from_this(), component);

	return shared_from_this();
}

Node::Ptr
Node::removeComponent(std::shared_ptr<AbstractComponent> component)
{
	std::list<AbstractComponent::Ptr>::iterator it = std::find(
		_components.begin(), _components.end(), component
	);

	if (it == _components.end())
		throw std::invalid_argument("component");

	_components.erase(it);
	component->_targets.erase(
		std::find(component->_targets.begin(), component->_targets.end(), shared_from_this())
	);

   	component->targetRemoved()->execute(component, shared_from_this());

	// bubble down
	auto descendants = NodeSet::create(shared_from_this())->descendants(true);
	for (auto descendant : descendants->nodes())
		descendant->_componentRemoved->execute(descendant, shared_from_this(), component);

	// bubble up
	auto ancestors = NodeSet::create(shared_from_this())->ancestors();
	for (auto ancestor : ancestors->nodes())
		ancestor->_componentRemoved->execute(ancestor, shared_from_this(), component);

	return shared_from_this();
}

bool
Node::hasComponent(std::shared_ptr<AbstractComponent> component)
{
	return std::find(_components.begin(), _components.end(), component) != _components.end();
}

void
Node::updateRoot()
{
	_root = _parent ? (_parent->_root ? _parent->_root : _parent) : shared_from_this();

	for (auto child : _children)
		child->updateRoot();
}
