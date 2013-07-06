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

#include "NodeSet.hpp"

#include "minko/Any.hpp"

using namespace minko::scene;

NodeSet::NodeSet(Mode mode) :
    _mode(mode),
    _watchingNodes(false),
    _watchingControllers(false),
    _nodeAdded(Signal<Ptr, NodePtr>::create()),
    _nodeRemoved(Signal<Ptr, NodePtr>::create())
{
}

void
NodeSet::update()
{
    _input = _nodes;
    for (auto& op : _operators)
    {
        _output.clear();
        op(_input);
        std::swap(_input, _output);

        if (_input.size() == 0)
            break ;
    }

    executeSignals();   
}

NodeSet::Ptr
NodeSet::select(NodePtr node)
{
    auto watchingNodes          = _watchingNodes;
    auto watchingControllers    = _watchingControllers;

    unwatchNodesAndControllers();

    _nodes.clear();
    _nodes.push_back(node);
    _input = _nodes;

    if (watchingNodes)
        watchNodes();
    if (watchingControllers)
        watchControllers();

    executeSignals();

    return shared_from_this();
}

NodeSet::Ptr
NodeSet::root()
{
    watchNodes();

    _output.clear();
    operatorRoot(_input);
    std::swap(_input, _output);

    _operators.push_back(std::bind(
        &NodeSet::operatorRoot,
        shared_from_this(),
        std::placeholders::_1
    ));

    executeSignals();

    return shared_from_this();
}

NodeSet::Ptr
NodeSet::parent()
{
    watchNodes();

    _output.clear();
    operatorParent(_input);
    std::swap(_input, _output);

    _operators.push_back(std::bind(
        &NodeSet::operatorParent,
        shared_from_this(),
        std::placeholders::_1
    ));

    executeSignals();
            
    return shared_from_this();
}

NodeSet::Ptr
NodeSet::ancestors(bool andSelf)
{
    watchNodes();

    _output.clear();
    operatorAncestors(_input, andSelf);
    std::swap(_input, _output);

    _operators.push_back(std::bind(
        &NodeSet::operatorAncestors,
        shared_from_this(),
        std::placeholders::_1,
        andSelf
    ));

    executeSignals();
            
    return shared_from_this();
}

NodeSet::Ptr
NodeSet::descendants(bool andSelf, bool depthFirst)
{
    watchNodes();

    _output.clear();
    operatorDescendants(_input, andSelf, depthFirst);
    std::swap(_input, _output);

    _operators.push_back(std::bind(
        &NodeSet::operatorDescendants,
        shared_from_this(),
        std::placeholders::_1,
        andSelf,
        depthFirst
    ));

    executeSignals();
            
    return shared_from_this();
}

NodeSet::Ptr
NodeSet::hasController(AbsCtrlPtr ctrl, bool expectedResult)
{
    watchControllers();

     _output.clear();
    operatorHasController(_input, ctrl, expectedResult);
    std::swap(_input, _output);

    _operators.push_back(std::bind(
        &NodeSet::operatorHasController,
        shared_from_this(),
        std::placeholders::_1,
        ctrl,
        expectedResult
    ));

    executeSignals();

    return shared_from_this();
}

void
NodeSet::operatorRoot(Nodes& input)
{
    for (auto in : input)
        output(in->root());
}

void
NodeSet::operatorParent(Nodes& input)
{
    for (auto in : input)
        output(in->parent());
}

void
NodeSet::operatorAncestors(Nodes& input, bool andSelf)
{
    for (auto in : input)
    {
        if (andSelf)
			output(in);
		
		while (in != nullptr)
		{
			if (in->parent() != nullptr)
				output(in->parent());
			in = in->parent();
		}
    }
}

void
NodeSet::operatorDescendants(Nodes& input, bool andSelf, bool depthFirst)
{
    for (auto in : input)
    {
        std::list<NodePtr> stack;

        stack.push_front(in);
        while (!stack.empty())
        {
            auto node = stack.front();

            stack.pop_front();

            if (node != in || andSelf)
				output(node);

			stack.insert(
				depthFirst ? stack.begin() : stack.end(),
				node->children().begin(),
				node->children().end()
			);
        }
    }
}

void
NodeSet::operatorLayer(Nodes& input, const unsigned int layer)
{
    for (auto in : input)
        if (in->tags() & layer)
            output(in);
}

void
NodeSet::operatorHasController(Nodes& input, AbsCtrlPtr ctrl, bool expectedResult)
{
    for (auto in : input)
        if (in->hasController(ctrl) == expectedResult)
            output(in);
}

void
NodeSet::watchNodes()
{
    if (_watchingNodes || _mode == Mode::MANUAL)
        return;

    _watchingNodes = true;

    auto addedOrRemoved = std::bind(
        &NodeSet::addedOrRemovedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    );
                
    for (auto node : _nodes)
    {
        auto root = node->root();

        _slots.push_back(Any(root->added()->connect(addedOrRemoved)));
        _slots.push_back(Any(root->removed()->connect(addedOrRemoved)));
    }
}

void
NodeSet::watchControllers()
{
    if (_watchingControllers || _mode == Mode::MANUAL)
        return;

    _watchingControllers = true;

    auto controllerAddedOrRemoved = std::bind(
        &NodeSet::controllerAddedOrRemovedHandler,
        shared_from_this(),
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    );

    for (auto node : _nodes)
    {
        auto root = node->root();

        _slots.push_back(Any(root->controllerAdded()->connect(controllerAddedOrRemoved)));
        _slots.push_back(Any(root->controllerRemoved()->connect(controllerAddedOrRemoved)));
    }
}

void
NodeSet::unwatchNodesAndControllers()
{
    if (_mode == Mode::MANUAL)
        return;

    _slots.clear();
    _watchingNodes = false;
    _watchingControllers = false;
}

void
NodeSet::controllerAddedOrRemovedHandler(NodePtr node, NodePtr target, AbsCtrlPtr ctrl)
{
    update();
}

void
NodeSet::addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
    update();
}

void
NodeSet::output(NodePtr node)
{
    if (std::find(_output.begin(), _output.end(), node) == _output.end())
        _output.push_back(node);
}

void
NodeSet::executeSignals()
{
    for (auto in : _input)
        if (std::find(_result.begin(), _result.end(), in) == _result.end())
            _nodeAdded->execute(shared_from_this(), in);
    for (auto res : _result)
        if (std::find(_input.begin(), _input.end(), res) == _input.end())
            _nodeRemoved->execute(shared_from_this(), res);

    _result = _input;
}