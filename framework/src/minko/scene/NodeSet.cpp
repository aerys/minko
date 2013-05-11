#include "NodeSet.hpp"

#include "minko/scene/Node.hpp"

using namespace minko::scene;

NodeSet::ptr
NodeSet::descendants(bool andSelf, NodeSet::ptr result)
{
	if (result == nullptr)
		result = create();

	std::list<std::shared_ptr<Node>> nodesStack(_nodes.begin(), _nodes.end());

	while (nodesStack.size() != 0)
	{
		auto node = nodesStack.front();

		nodesStack.pop_front();

		if (andSelf)
			result->_nodes.insert(node);

		nodesStack.insert(nodesStack.end(), node->children().begin(), node->children().end());
		result->_nodes.insert(node->children().begin(), node->children().end());
	}

	return result;
}

NodeSet::ptr
NodeSet::ancestors(bool andSelf, NodeSet::ptr result)
{
	if (result == nullptr)
		result = create();

	for (auto node : _nodes)
	{
		while (node != nullptr)
		{
			if (andSelf)
				result->_nodes.insert(node);

			if (node->parent() != nullptr)
				result->_nodes.insert(node->parent());
			node = node->parent();
		}
	}
}

NodeSet::ptr
NodeSet::children(bool andSelf, NodeSet::ptr result)
{
	if (result == nullptr)
		result = create();

	for (auto node : _nodes)
	{
		if (andSelf)
			result->_nodes.insert(node);

		result->_nodes.insert(node->children().begin(), node->children().end());
	}

	return result;
}

NodeSet::ptr
NodeSet::where(std::function<bool(std::shared_ptr<Node>)> filter, ptr result)
{
	if (result == nullptr)
		result = create();

	for (auto node : _nodes)
		if (filter(node))
			result->_nodes.insert(node);

	return result;
}
