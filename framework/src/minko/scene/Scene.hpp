#pragma once

#include "minko/Common.hpp"
#include "minko/scene/Node.hpp"

namespace minko
{
	namespace scene
	{
		class Scene :
			public Node
		{
		public:
			typedef std::shared_ptr<Scene> ptr;

		private:
			std::shared_ptr<Signal<ptr>>	_enterFrame;
			std::shared_ptr<Signal<ptr>>	_exitFrame;

		public:
			inline static
			ptr
			create()
			{
				return std::shared_ptr<Scene>(new Scene());
			}

			inline static
			ptr
			create(const std::string& name)
			{
				ptr scene = create();

				scene->_name = name;

				return scene;
			}

			inline static
			ptr
			create(const std::list<std::shared_ptr<Node>>& children)
			{
				ptr scene = create();

				for (auto child : children)
					scene->addChild(child);

				return scene;
			}

			inline static
			ptr
			create(const std::string& name, const std::list<std::shared_ptr<Node>>& children)
			{
				ptr scene = create();

				scene->_name = name;
				
				for (auto child : children)
					scene->addChild(child);

				return scene;
			}

			inline
			std::shared_ptr<Signal<ptr>>
			enterFrame()
			{
				return _enterFrame;
			}

			inline
			std::shared_ptr<Signal<ptr>>
			exitFrame()
			{
				return _exitFrame;
			}

			void
			render();

		private:
			Scene() :
				Node(),
				_enterFrame(std::shared_ptr<Signal<ptr>>(Signal<ptr>::create())),
				_exitFrame(std::shared_ptr<Signal<ptr>>(Signal<ptr>::create()))
			{
			}
		};
	}
}