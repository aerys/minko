#pragma once

#include "minko/Common.hpp"

namespace minko
{
	namespace scene
	{
		using namespace minko::scene;

		class NodeSet :
			public std::enable_shared_from_this<NodeSet>
		{
		public:
			typedef std::shared_ptr<NodeSet> ptr;

		private:
			std::set<std::shared_ptr<Node>> _nodes;

		public:
			inline static
			ptr
			create(const std::list<std::shared_ptr<Node>>& nodes)
			{
				ptr set = create();

				set->_nodes.insert(nodes.begin(), nodes.end());

				return set;
			}

			inline static
			ptr
			create()
			{
				return std::shared_ptr<NodeSet>(new NodeSet());
			}

			inline
			const std::set<std::shared_ptr<Node>>
			nodes()
			{
				return _nodes;
			}

			ptr
			descendants(bool andSelf = false, ptr result = nullptr);

			ptr
			children(bool andSelf = false, ptr result = nullptr);

			ptr
			where(std::function<bool(std::shared_ptr<Node>)> filter, ptr result = nullptr);

		private:
			NodeSet(const std::set<std::shared_ptr<Node>>& nodes) :
				std::enable_shared_from_this<NodeSet>(),
				_nodes(nodes)
			{
			}

			NodeSet() :
				std::enable_shared_from_this<NodeSet>(),
				_nodes()
			{
			}
		};		
	}
}
