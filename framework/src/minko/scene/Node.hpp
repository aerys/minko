#pragma once

#include <typeinfo>

#include "minko/Common.hpp"
#include "minko/Signal.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/scene/data/DataBindings.hpp"

namespace
{
	using namespace minko::scene::data;
	using namespace minko::scene::controller;
}

namespace minko
{
	namespace scene
	{
		class Node :
			public std::enable_shared_from_this<Node>
		{
		public:
			typedef std::shared_ptr<Node>	ptr;

		private:
			typedef std::shared_ptr<AbstractController>	AbsCtrlPtr;

		protected:
			std::string 									_name;
			std::list<ptr>									_children;			

		private:
			static unsigned int								_id;

			unsigned int									_tags;
			ptr 											_root;
			ptr												_parent;
			std::shared_ptr<DataBindings>					_bindings;
			std::list<AbsCtrlPtr>							_controllers;

			std::shared_ptr<Signal<ptr, ptr, ptr>>			_added;
			std::shared_ptr<Signal<ptr, ptr, ptr>>			_removed;
			std::shared_ptr<Signal<ptr, ptr>>				_tagsChanged;
			std::shared_ptr<Signal<ptr, ptr, AbsCtrlPtr>>	_controllerAdded;
			std::shared_ptr<Signal<ptr, ptr, AbsCtrlPtr>>	_controllerRemoved;

		public:

			static
			ptr
			create()
			{
				ptr node = std::shared_ptr<Node>(new Node());

				node->_root = node;

				return node;
			}

			static
			ptr
			create(const std::list<ptr>& children)
			{
				ptr node = create();

				for (auto child : children)
					node->addChild(child);

				return node;				
			}

			static
			ptr
			create(const std::string& name)
			{
				ptr node = create();

				node->name(name);

				return node;
			}

			static
			ptr
			create(const std::string& name, const std::list<ptr>& children)
			{
				ptr node = create(name);

				for (auto child : children)
					node->addChild(child);

				return node;
			}

			inline
			const std::string&
			name()
			{
				return _name;
			}

			inline
			void
			name(const std::string& name)
			{
				_name = name;
			}

			inline
			unsigned int
			tags()
			{
				return _tags;
			}
			inline
			void
			tags(unsigned int tags)
			{
				if (_tags != tags)
				{
					_tags = tags;

					// bubble down
					auto descendants = NodeSet::create(shared_from_this())->descendants(true);
					for (auto descendant : descendants->nodes())
						descendant->_tagsChanged->execute(descendant, shared_from_this());

					// bubble up
					auto ancestors = NodeSet::create(shared_from_this())->ancestors();
					for (auto ancestor : ancestors->nodes())
						ancestor->_tagsChanged->execute(ancestor, shared_from_this());
				}
			}

			inline
			ptr
			parent()
			{
				return _parent;
			}

			inline
			ptr
			root()
			{
				return _root;
			}

			inline
			const std::list<ptr>&
			children()
			{
				return _children;
			}

			inline
			std::shared_ptr<DataBindings>
			bindings()
			{
				return _bindings;
			}

			inline
			Signal<ptr, ptr, ptr>::ptr
			added()
			{
				return _added;
			}

			inline
			Signal<ptr, ptr, ptr>::ptr
			removed()
			{
				return _removed;
			}

			inline
			Signal<ptr, ptr>::ptr
			tagsChanged()
			{
				return _tagsChanged;
			}

			inline
			std::shared_ptr<Signal<ptr, ptr, AbsCtrlPtr>>
			controllerAdded()
			{
				return _controllerAdded;
			}

			inline
			std::shared_ptr<Signal<ptr, ptr, AbsCtrlPtr>>
			controllerRemoved()
			{
				return _controllerRemoved;
			}

			ptr
			addChild(ptr Node);
			
			ptr
			removeChild(ptr Node);
			
			bool
			contains(ptr Node);

			ptr
			addController(std::shared_ptr<AbstractController> controller);

			ptr
			removeController(std::shared_ptr<AbstractController> controller);

			bool
			hasController(std::shared_ptr<AbstractController> controller);

			template <typename T>
			std::vector<std::shared_ptr<T>>
			controllers()
			{
				std::vector<std::shared_ptr<T>> result;
				unsigned int counter = 0;

				for (auto controller : _controllers)
				{
					std::shared_ptr<T> typedController = std::dynamic_pointer_cast<T>(controller);

					if (typedController != nullptr)
						result.push_back(typedController);
				}

				return result;
			}

			template <typename T>
			std::shared_ptr<T>
			controller(const unsigned int position = 0)
			{
				unsigned int counter = 0;

				for (auto controller : _controllers)
				{
					std::shared_ptr<T> typedController = std::dynamic_pointer_cast<T>(controller);

					if (typedController != nullptr)
						if (counter == position)
							return typedController;
						else
							++counter;
				}

				return nullptr;
			}

			virtual ~Node()
			{
			}

		protected:
			Node();

			void
			updateRoot();
		};
	}
}
