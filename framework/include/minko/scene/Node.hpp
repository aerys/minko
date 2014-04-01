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
#include "minko/scene/Layout.hpp"

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
			typedef std::shared_ptr<component::AbstractComponent>	AbsCtrlPtr;

		protected:
			std::string 											_name;
			std::vector<Ptr>										_children;

		private:
			static unsigned int										_id;

			Ptr 													_root;
			Ptr														_parent;
			std::shared_ptr<data::Container>						_container;
			std::shared_ptr<data::Provider>							_data;
			std::list<AbsCtrlPtr>									_components;

			uint													_depth;

			std::shared_ptr<Signal<Ptr, Ptr, Ptr>>					_added;
			std::shared_ptr<Signal<Ptr, Ptr, Ptr>>					_removed;
			std::shared_ptr<Signal<Ptr, Ptr>>						_layoutsChanged;
			std::shared_ptr<Signal<Ptr, Ptr, AbsCtrlPtr>>			_componentAdded;
			std::shared_ptr<Signal<Ptr, Ptr, AbsCtrlPtr>>			_componentRemoved;

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
			name() const
			{
				return _name;
			}

			inline
			int
			id() const
			{
				return _id;
			}

			inline
			void
			name(const std::string& name)
			{
				_name = name;
			}

			LayoutMask
			layouts() const;
			
			Ptr
			layouts(LayoutMask);

			inline
			Ptr
			parent() const
			{
				return _parent;
			}

			inline
			Ptr
			root() const
			{
				return _root;
			}

			inline
			const std::vector<Ptr>&
			children() const
			{
				return _children;
			}

			inline
			std::shared_ptr<data::Container>
			data() const
			{
				return _container;
			}

			inline
			uint
			depth() const
			{
				return _depth;
			}

			inline
			Signal<Ptr, Ptr, Ptr>::Ptr
			added() const
			{
				return _added;
			}

			inline
			Signal<Ptr, Ptr, Ptr>::Ptr
			removed() const
			{
				return _removed;
			}

			inline
			Signal<Ptr, Ptr>::Ptr
			layoutsChanged() const
			{
				return _layoutsChanged;
			}

			inline
			std::shared_ptr<Signal<Ptr, Ptr, AbsCtrlPtr>>
			componentAdded() const
			{
				return _componentAdded;
			}

			inline
			std::shared_ptr<Signal<Ptr, Ptr, AbsCtrlPtr>>
			componentRemoved() const
			{
				return _componentRemoved;
			}

			Ptr
			addChild(Ptr Node);

			Ptr
			removeChild(Ptr Node);

			Ptr
			removeChildren();

			bool
			contains(Ptr Node);

			Ptr
			addComponent(AbsCtrlPtr component);

			Ptr
			removeComponent(AbsCtrlPtr component);

			bool
			hasComponent(AbsCtrlPtr component);

			template <typename T>
			inline
			bool
			hasComponent()
			{
				return component<T>() != nullptr;
			}

			template <typename T>
			std::vector<std::shared_ptr<T>>
			components()
			{
				std::vector<std::shared_ptr<T>> result;

				for (auto component : _components)
				{
					std::shared_ptr<T> typedComponent = std::dynamic_pointer_cast<T>(component);

					if (typedComponent != nullptr)
						result.push_back(typedComponent);
				}

				return result;
			}

			template <typename T>
			std::shared_ptr<T>
			component(const unsigned int position = 0)
			{
				unsigned int counter = 0;

				for (auto component : _components)
				{
					std::shared_ptr<T> typedComponent = std::dynamic_pointer_cast<T>(component);

					if (typedComponent != nullptr)
					{
						if (counter == position)
							return typedComponent;
						else
							++counter;
					}
				}

				return nullptr;
			}

			virtual
			~Node()
			{
			}

			std::string
			toString() const
			{
				std::stringstream stream;

				stream << "Node (" << _name << ")";

				return stream.str();
			}

		protected:
			Node();

			void
			updateRoot();

		private:
			void
			initialize();
		};
	}
}
