#include "Node.hpp"
#include "minko/scene/controller/AbstractController.hpp"
#include "minko/scene/Scene.hpp"

using namespace minko::scene;

unsigned int Node::_id = 0;

Node::Node() :
	enable_shared_from_this(),
	_name("Node_" + std::to_string(Node::_id++)),
	_tags(1),
	_scene(nullptr),
	_root(nullptr),
	_parent(nullptr),
	_bindings(DataBindings::create()),
	_added(Signal<ptr, ptr>::create()),
	_removed(Signal<ptr, ptr>::create()),
	_descendantAdded(Signal<ptr, ptr>::create()),
	_descendantRemoved(Signal<ptr, ptr>::create()),
	_tagsChanged(Signal<ptr>::create())
{
}

Node::ptr
Node::addChild(Node::ptr child)
{
	if (child->_parent)
		child->_parent->removeChild(child);

	_children.push_back(child);

	// bubble up
	_childToDescendantAddedCd[child] = child->_descendantAdded->add(std::bind(
		&Signal<Node::ptr, Node::ptr>::execute, _descendantAdded, std::placeholders::_1, std::placeholders::_2
	));
	_childToDescendantRemovedCd[child] = child->_descendantRemoved->add(std::bind(
		&Signal<Node::ptr, Node::ptr>::execute, _descendantRemoved, std::placeholders::_1, std::placeholders::_2
	));

	// bubble down
	_childToAddedCd[child] = _added->add(std::bind(
		&Signal<Node::ptr, Node::ptr>::execute, child->_added, std::placeholders::_1, std::placeholders::_2
	));
	_childToRemovedCd[child] = _removed->add(std::bind(
		&Signal<Node::ptr, Node::ptr>::execute, child->_removed, std::placeholders::_1, std::placeholders::_2
	));

	child->_parent = shared_from_this();
	child->updateRoot();

	child->_added->execute(child, shared_from_this());
	_descendantAdded->execute(shared_from_this(), child);

	return shared_from_this();
}

Node::ptr
Node::removeChild(Node::ptr child)
{
	std::list<Node::ptr>::iterator it = std::find(_children.begin(), _children.end(), child);

	if (it == _children.end())
		throw;

	_children.erase(it);

	// bubble up
	(*child->_descendantAdded) -= _childToDescendantAddedCd[child];
	(*child->_descendantRemoved) -= _childToDescendantRemovedCd[child];
	_childToDescendantAddedCd.erase(child);
	_childToDescendantRemovedCd.erase(child);

	// bubble down
	*_added -= _childToAddedCd[child];
	*_removed -= _childToRemovedCd[child];
	_childToAddedCd.erase(child);
	_childToRemovedCd.erase(child);

	child->_parent = 0;
	child->updateRoot();

	child->_removed->execute(child, shared_from_this());
	_descendantRemoved->execute(shared_from_this(), child);

	return shared_from_this();
}

bool
Node::contains(Node::ptr node)
{
	return std::find(_children.begin(), _children.end(), node) != _children.end();
}

Node::ptr
Node::addController(std::shared_ptr<AbstractController> controller)
{
	if (hasController(controller))
		throw;

	_controllers.push_back(controller);
	controller->targetAdded()->execute(controller, shared_from_this());

	return shared_from_this();
}

Node::ptr
Node::removeController(std::shared_ptr<AbstractController> controller)
{
	std::list<AbstractController::ptr>::iterator it = std::find(
		_controllers.begin(), _controllers.end(), controller
	);

	if (it == _controllers.end())
		throw;

	_controllers.erase(it);
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
	_scene = std::dynamic_pointer_cast<Scene>(_root);

	for (auto child : _children)
		child->updateRoot();
}
