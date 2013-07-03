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

namespace minko
{
	class NodeSet2 :
        public std::enable_shared_from_this<NodeSet2>
	{
	public:
		typedef std::shared_ptr<NodeSet2> Ptr;

    private:
        typedef std::shared_ptr<scene::Node>                    NodePtr;
        typedef std::set<NodePtr>                               NodeSet;
        typedef std::function<void(NodeSet, NodeSet)>           Operator;
        typedef std::shared_ptr<controller::AbstractController> AbsCtrl;
        typedef std::list<Any>                                  SlotsList;

        enum OperatorType
        {
            Root,
            Parent,
            Descendants,
            Where
        };

    private:
        std::list<NodePtr>                      _nodes;
        NodeSet                                 _result;
        std::list<std::pair<Operator, NodeSet>> _operations;

	public:
		inline static
		Ptr
		create()
		{
			return std::shared_ptr<NodeSet2>(new NodeSet2());
		}

        Ptr
        select(const std::list<NodePtr>& nodes)
        {
            _nodes.clear();
            _nodes.assign(nodes.begin(), nodes.end());

            return shared_from_this();
        }

        Ptr
        root()
        {
            auto op = std::bind(
                &NodeSet2::operatorRoot,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2
            );

            _operations.push_back(std::pair<Operator, NodeSet>(op, _result));
            
            NodeSet newResult;

            operatorRoot(_result, newResult);
            _result = newResult;

            return shared_from_this();
        }

        Ptr
        parent()
        {
            
            return shared_from_this();
        }

        Ptr
        descendants()
        {
            
            return shared_from_this();
        }

        Ptr
        filter(std::function<bool(NodePtr)> filterFunction)
        {
            
            return shared_from_this();
        }

     private:
        void
        operatorRoot(NodeSet& input, NodeSet& output)
        {
            for (auto node : input)
                output.insert(node->root());
        }

        void
        operatorParent(NodeSet& input, NodeSet& output)
        {
            for (auto node : input)
                output.insert(node->parent());
        }

        void
        operatorDescendants(NodeSet& input, NodeSet& output)
        {
            // FIXME
            throw;
        }

        void
        operatorFilter(NodeSet& input, NodeSet& output, std::function<bool(NodePtr)> filterFunction)
        {
            for (auto node : input)
                if (filterFunction(node))
                    output.insert(node);
        }

        void
        operatorLayer(NodeSet& input, NodeSet& output, const unsigned int layer)
        {
            for (auto node : input)
                if (node->tags() & layer)
                    output.insert(node);
        }

        void
        operatorHasController(NodeSet& input, NodeSet& output, AbsCtrl ctrl)
        {
            for (auto node : input)
                if (node->hasController(ctrl))
                    output.insert(node);
        }

        template <typename T>
        void
        operatorHasController(NodeSet& input, NodeSet& output)
        {
            for (auto node : input)
                if (node->hasController<T>())
                    output.insert(node);
        }

        void
        update()
        {

        }

        void
        controllerAddedOrRemovedHandler(NodePtr node, NodePtr target, AbsCtrl ctrl)
        {
            // FIXME
        }

        void
        addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
        {
            // FIXME
        }
	};
}
