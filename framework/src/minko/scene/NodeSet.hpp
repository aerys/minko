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

#pragma once

#include "minko/Common.hpp"

#include "minko/Signal.hpp"
#include "minko/scene/Node.hpp"
#include "minko/Any.hpp"

namespace minko
{
    namespace scene
    {
	    class NodeSet :
            public std::enable_shared_from_this<NodeSet>
	    {
	    public:
		    typedef std::shared_ptr<NodeSet> Ptr;

            enum Mode
            {
                AUTO,
                MANUAL
            };

        private:
            typedef std::shared_ptr<Node>                           NodePtr;
            typedef std::shared_ptr<controller::AbstractController> AbsCtrlPtr;
            typedef std::vector<NodePtr>                            Nodes;
            typedef std::function<void(Nodes&)>                     Operator;

        private:
            const Mode                              _mode;
            Nodes                                   _nodes;
            Nodes                                   _input;
            Nodes                                   _output;
            Nodes                                   _result;
            std::list<Operator>                     _operators;

            std::vector<Any>                        _slots;
            bool                                    _watchingNodes;
            bool                                    _watchingControllers;

            std::shared_ptr<Signal<Ptr, NodePtr>>   _nodeAdded;
            std::shared_ptr<Signal<Ptr, NodePtr>>   _nodeRemoved;

	    public:
		    inline static
		    Ptr
		    create(Mode mode)
		    {
			    return std::shared_ptr<NodeSet>(new NodeSet(mode));
		    }

            inline
            Mode
            mode()
            {
                return _mode;
            }

            inline
            Nodes&
            nodes()
            {
                return _result;
            }

            inline
            std::shared_ptr<Signal<Ptr, NodePtr>>
            nodeAdded()
            {
                return _nodeAdded;
            }

            inline
            std::shared_ptr<Signal<Ptr, NodePtr>>
            nodeRemoved()
            {
                return _nodeRemoved;
            }

            void
            update();

            template <typename T>
            Ptr
            select(T begin, T end)
            {
                auto watchingNodes          = _watchingNodes;
                auto watchingControllers    = _watchingControllers;

                unwatchNodesAndControllers();

                _nodes.clear();
                _nodes.insert(_nodes.begin(), begin, end);
                _input = _nodes;

                if (watchingNodes)
                    watchNodes();
                if (watchingControllers)
                    watchControllers();

                return shared_from_this();
            }

            Ptr
            select(NodePtr node);

            Ptr
            root();

            Ptr
            parent();

            Ptr
            ancestors(bool andSelf = false);

            Ptr
            descendants(bool andSelf = false, bool depthFirst = true);

            Ptr
            hasController(AbsCtrlPtr ctrl, bool expectedResult = true);

            template <typename T>
            Ptr
            hasController(bool expectedResult = true)
            {
                watchControllers();

                _output.clear();
                operatorHasController<T>(_input, expectedResult);
                std::swap(_input, _output);

                _operators.push_back(std::bind(
                    &NodeSet::operatorHasController<T>,
                    shared_from_this(),
                    std::placeholders::_1,
                    expectedResult
                ));

                executeSignals();

                return shared_from_this();
            }

         private:
            NodeSet(Mode mode);

            void
            operatorRoot(Nodes& input);

            void
            operatorParent(Nodes& input);

            void
            operatorAncestors(Nodes& input, bool andSelf);

            void
            operatorDescendants(Nodes&    input,
                                bool      andSelf,
                                bool      depthFirst);

            void
            operatorLayer(Nodes& input, const unsigned int layer);

            void
            operatorHasController(Nodes& input, AbsCtrlPtr ctrl, bool expectedResult);

            template <typename T>
            void
            operatorHasController(Nodes& input, bool expectedResult)
            {
                for (auto in : input)
                    if (in->hasController<T>() == expectedResult)
                        output(in);
            }

            void
            watchNodes();

            void
            watchControllers();

            void
            unwatchNodesAndControllers();

            void
            controllerAddedOrRemovedHandler(NodePtr node, NodePtr target, AbsCtrlPtr ctrl);

            void
            addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr ancestor);

            void
            output(NodePtr node);

            void
            executeSignals();
	    };
    }
}
