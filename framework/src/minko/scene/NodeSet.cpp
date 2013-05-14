#include "NodeSet.hpp"

#include "minko/scene/Node.hpp"

using namespace minko::scene;

NodeSet::ptr
NodeSet::descendants(bool andSelf, bool depthFirst, NodeSet::ptr result)
{
	if (result == nullptr)
		result = create();

	std::list<std::shared_ptr<Node>> nodesStack;

	for (auto node : _nodes)
	{
		nodesStack.push_front(node);

		while (nodesStack.size() != 0)
		{
			auto descendant = nodesStack.front();

			nodesStack.pop_front();

			if (descendant != node || andSelf)
				result->_nodes.push_back(descendant);

			nodesStack.insert(
				depthFirst ? nodesStack.begin() : nodesStack.end(),
				descendant->children().begin(),
				descendant->children().end()
			);
		}
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
		if (andSelf)
			result->_nodes.push_back(node);
		
		while (node != nullptr)
		{
			if (node->parent() != nullptr)
				result->_nodes.push_back(node->parent());
			node = node->parent();
		}
	}

	return result;
}

NodeSet::ptr
NodeSet::children(bool andSelf, NodeSet::ptr result)
{
	if (result == nullptr)
		result = create();

	for (auto node : _nodes)
	{
		if (andSelf)
			result->_nodes.push_back(node);

		result->_nodes.insert(result->_nodes.end(), node->children().begin(), node->children().end());
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
			result->_nodes.push_back(node);

	return result;
}
