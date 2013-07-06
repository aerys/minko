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

#include "minko/controller/AbstractController.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/data/Container.hpp"

using namespace minko::scene;
using namespace minko::controller;

unsigned int Node::_id = 0;

Node::Node() :
	_name("Node_" + std::to_string(Node::_id++)),
	_tags(1),
	_root(nullptr),
	_parent(nullptr),
	_container(data::Container::create()),
	_added(Signal<Ptr, Ptr, Ptr>::create()),
	_removed(Signal<Ptr, Ptr, Ptr>::create()),
	_controllerAdded(Signal<Ptr, Ptr, Node::AbsCtrlPtr>::create()),
	_controllerRemoved(Signal<Ptr, Ptr, Node::AbsCtrlPtr>::create()),
	_tagsChanged(Signal<Ptr, Ptr>::create())
{
}

void
Node::tags(unsigned int tags)
{
	if (_tags != tags)
	{
		_tags = tags;

		// bubble down
        auto descendants = NodeSet::create(NodeSet::Mode::MANUAL)
            ->select(shared_from_this())
            ->descendants(true);
		for (auto descendant : descendants->nodes())
			descendant->_tagsChanged->execute(descendant, shared_from_this());

		// bubble up
		auto ancestors = NodeSet::create(NodeSet::Mode::MANUAL)
            ->select(shared_from_this())
            ->ancestors();
		for (auto ancestor : ancestors->nodes())
			ancestor->_tagsChanged->execute(ancestor, shared_from_this());
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
	auto descendants = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(child)
        ->descendants(true);
	for (auto descendant : descendants->nodes())
		descendant->_added->execute(descendant, child, shared_from_this());

	// bubble up
	auto ancestors = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(shared_from_this())
        ->ancestors(true);
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
    auto descendants = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(child)
        ->descendants(true);
	for (auto descendant : descendants->nodes())
		descendant->_removed->execute(descendant, child, shared_from_this());

	// bubble up
	auto ancestors = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(shared_from_this())
        ->ancestors(true);
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
Node::addController(std::shared_ptr<AbstractController> controller)
{
	if (hasController(controller))
		throw std::logic_error("The same controller cannot be added twice.");

	_controllers.push_back(controller);
	controller->_targets.push_back(shared_from_this());

	// bubble down
	auto descendants = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(shared_from_this())
        ->descendants(true);
	for (auto descendant : descendants->nodes())
		descendant->_controllerAdded->execute(descendant, shared_from_this(), controller);

	// bubble up
	auto ancestors = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(shared_from_this())
        ->ancestors();
	for (auto ancestor : ancestors->nodes())
		ancestor->_controllerAdded->execute(ancestor, shared_from_this(), controller);

	controller->targetAdded()->execute(controller, shared_from_this());

	return shared_from_this();
}

Node::Ptr
Node::removeController(std::shared_ptr<AbstractController> controller)
{
	std::list<AbstractController::Ptr>::iterator it = std::find(
		_controllers.begin(), _controllers.end(), controller
	);

	if (it == _controllers.end())
		throw std::invalid_argument("controller");

	_controllers.erase(it);
	controller->_targets.erase(
		std::find(controller->_targets.begin(), controller->_targets.end(), shared_from_this())
	);

	// bubble down
	auto descendants = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(shared_from_this())
        ->descendants(true);
	for (auto descendant : descendants->nodes())
		descendant->_controllerRemoved->execute(descendant, shared_from_this(), controller);

	// bubble up
	auto ancestors = NodeSet::create(NodeSet::Mode::MANUAL)
        ->select(shared_from_this())
        ->ancestors();
	for (auto ancestor : ancestors->nodes())
		ancestor->_controllerRemoved->execute(ancestor, shared_from_this(), controller);

	controller->targetRemoved()->execute(controller, shared_from_this());

	return shared_from_this();
}

bool
Node::hasController(std::shared_ptr<AbstractController> controller)
{
	return std::find(_controllers.begin(), _controllers.end(), controller) != _controllers.end();
}

void
Node::updateRoot()
{
	_root = _parent ? (_parent->_root ? _parent->_root : _parent) : shared_from_this();

	for (auto child : _children)
		child->updateRoot();
}
