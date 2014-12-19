/*
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

#include "minko/scene/NodeSet.hpp"

#include "minko/scene/Node.hpp"

using namespace minko;

scene::NodeSet::Ptr
scene::NodeSet::descendants(bool andSelf, bool depthFirst, scene::NodeSet::Ptr result)
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

scene::NodeSet::Ptr
scene::NodeSet::ancestors(bool andSelf, scene::NodeSet::Ptr result)
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

scene::NodeSet::Ptr
scene::NodeSet::children(bool andSelf, scene::NodeSet::Ptr result)
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

scene::NodeSet::Ptr
scene::NodeSet::where(std::function<bool(std::shared_ptr<Node>)> filter, scene::NodeSet::Ptr result)
{
    if (result == nullptr)
        result = create();

    for (auto node : _nodes)
        if (filter(node))
            result->_nodes.push_back(node);

    return result;
}

scene::NodeSet::Ptr
    scene::NodeSet::roots(scene::NodeSet::Ptr result)
{
    if (result == nullptr)
        result = create();

    for (auto node : _nodes)
        if (std::find(result->_nodes.begin(), result->_nodes.end(), node->root()) == result->_nodes.end())
            result->_nodes.push_back(node->root());

    return result;
}
