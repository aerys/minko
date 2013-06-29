#pragma once

#include "minko/Common.hpp"

namespace minko
{
	namespace scene
	{
		using namespace minko::scene;

		class NodeSet
		{
		public:
			typedef std::shared_ptr<NodeSet> Ptr;

		private:
			std::vector<std::shared_ptr<Node>> _nodes;

		public:
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

			Ptr
			descendants(bool andSelf = false, bool depthFirst = true, Ptr result = nullptr);

			Ptr
			ancestors(bool andSelf = false, Ptr result = nullptr);

			Ptr
			children(bool andSelf = false, Ptr result = nullptr);

			Ptr
			where(std::function<bool(std::shared_ptr<Node>)> filter, Ptr result = nullptr);

		private:
			NodeSet() :
				_nodes()
			{
			}
		};		
	}
}
