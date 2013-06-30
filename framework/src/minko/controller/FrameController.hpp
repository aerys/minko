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
#include "minko/controller/AbstractController.hpp"

namespace
{
}

namespace minko
{
	namespace controller
	{
		class FrameController :
			public AbstractController,
			public std::enable_shared_from_this<FrameController>
		{
		public:
			typedef std::shared_ptr<FrameController> Ptr;

		private:
			std::shared_ptr<Signal<Ptr>>	_app;
			std::shared_ptr<Signal<Ptr>>	_cull;
			std::shared_ptr<Signal<Ptr>>	_draw;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<FrameController>(new FrameController());
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			app()
			{
				return _app;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			cull()
			{
				return _cull;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			draw()
			{
				return _draw;
			}

			void
			nextFrame();

		private:
			FrameController();

			void
			targetAddedHandler(std::shared_ptr<AbstractController>	controller,
							   std::shared_ptr<Node>				target);

			void
			addedHandler(std::shared_ptr<Node>	node,
						 std::shared_ptr<Node>	target,
						 std::shared_ptr<Node>	parent);

			void
			controllerAddedHandler(std::shared_ptr<Node>				node,
								   std::shared_ptr<Node>				target,
								   std::shared_ptr<AbstractController>	controller);
		};
	}
}
