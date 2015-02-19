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

#pragma once

#include "minko/Common.hpp"

namespace minko
{
	namespace scene
	{
		class NodeSet
		{
		public:
			typedef std::shared_ptr<NodeSet> Ptr;

		private:
			std::vector<std::shared_ptr<Node>> _nodes;

		public:
            ~NodeSet()
            {
                _nodes.clear();
            }

			inline static
			Ptr
			create(const std::list<std::shared_ptr<Node>>& nodes)
			{
				Ptr set = create();

				set->_nodes.insert(set->_nodes.end(), nodes.begin(), nodes.end());

				return set;
			}

			inline static
			Ptr
			create(const std::vector<std::shared_ptr<Node>>& nodes)
			{
				Ptr set = create();

				set->_nodes.insert(set->_nodes.end(), nodes.begin(), nodes.end());

				return set;
			}

			inline static
			Ptr
			create(std::shared_ptr<Node> node)
			{
				Ptr set = create();

				set->_nodes.push_back(node);

				return set;
			}

			inline static
			Ptr
			create()
			{
				return std::shared_ptr<NodeSet>(new NodeSet());
			}

			inline
			const std::vector<std::shared_ptr<Node>>&
			nodes()
			{
				return _nodes;
			}

            inline
            int
            size() const
            {
                return _nodes.size();
            }

			Ptr
			descendants(bool andSelf = false, bool depthFirst = true, Ptr result = nullptr);

			Ptr
			ancestors(bool andSelf = false, Ptr result = nullptr);

			Ptr
			children(bool andSelf = false, Ptr result = nullptr);

			Ptr
			where(std::function<bool(std::shared_ptr<Node>)> filter, Ptr result = nullptr);

			Ptr
			roots(Ptr result = nullptr);

		private:
			NodeSet() :
				_nodes()
			{
			}
		};		
	}
}
