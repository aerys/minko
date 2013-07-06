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

namespace minko
{
	namespace scene
	{
		class Node :
			public std::enable_shared_from_this<Node>
		{
		public:
			typedef std::shared_ptr<Node>	Ptr;

		private:
			typedef std::shared_ptr<controller::AbstractController>	AbsCtrlPtr;

		protected:
			std::string 									_name;
			std::list<Ptr>									_children;

		private:
			static unsigned int								_id;

			unsigned int									_tags;
			Ptr 											_root;
			Ptr												_parent;
			std::shared_ptr<data::Container>				_container;
			std::list<AbsCtrlPtr>							_controllers;

			std::shared_ptr<Signal<Ptr, Ptr, Ptr>>			_added;
			std::shared_ptr<Signal<Ptr, Ptr, Ptr>>			_removed;
			std::shared_ptr<Signal<Ptr, Ptr>>				_tagsChanged;
			std::shared_ptr<Signal<Ptr, Ptr, AbsCtrlPtr>>	_controllerAdded;
			std::shared_ptr<Signal<Ptr, Ptr, AbsCtrlPtr>>	_controllerRemoved;

		public:

			static
			Ptr
			create()
			{
				Ptr node = std::shared_ptr<Node>(new Node());

				node->_root = node;

				return node;
			}

			static
			Ptr
			create(const std::list<Ptr>& children)
			{
				Ptr node = create();

				for (auto child : children)
					node->addChild(child);

				return node;
			}

			static
			Ptr
			create(const std::string& name)
			{
				Ptr node = create();

				node->name(name);

				return node;
			}

			static
			Ptr
			create(const std::string& name, const std::list<Ptr>& children)
			{
				Ptr node = create(name);

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
			
			void
			tags(unsigned int tags);

			inline
			Ptr
			parent()
			{
				return _parent;
			}

			inline
			Ptr
			root()
			{
				return _root;
			}

			inline
			const std::list<Ptr>&
			children()
			{
				return _children;
			}

			inline
			std::shared_ptr<data::Container>
			data()
			{
				return _container;
			}

			inline
			Signal<Ptr, Ptr, Ptr>::Ptr
			added()
			{
				return _added;
			}

			inline
			Signal<Ptr, Ptr, Ptr>::Ptr
			removed()
			{
				return _removed;
			}

			inline
			Signal<Ptr, Ptr>::Ptr
			tagsChanged()
			{
				return _tagsChanged;
			}

			inline
			std::shared_ptr<Signal<Ptr, Ptr, AbsCtrlPtr>>
			controllerAdded()
			{
				return _controllerAdded;
			}

			inline
			std::shared_ptr<Signal<Ptr, Ptr, AbsCtrlPtr>>
			controllerRemoved()
			{
				return _controllerRemoved;
			}

			Ptr
			addChild(Ptr Node);

			Ptr
			removeChild(Ptr Node);

			bool
			contains(Ptr Node);

			Ptr
			addController(AbsCtrlPtr controller);

			Ptr
			removeController(AbsCtrlPtr controller);

			bool
			hasController(AbsCtrlPtr controller);

			template <typename T>
			inline
			bool
			hasController()
			{
				return controller<T>() != nullptr;
			}

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
					{
						if (counter == position)
							return typedController;
						else
							++counter;
					}
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
