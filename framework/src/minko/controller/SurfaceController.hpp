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
	using namespace minko;
	using namespace minko::render;
	using namespace minko::geometry;
	using namespace minko::data;
}

namespace minko
{
	namespace controller
	{
		class SurfaceController :
			public AbstractController,
			public std::enable_shared_from_this<SurfaceController>
		{
		public:
			typedef std::shared_ptr<SurfaceController>	Ptr;

		private:
			typedef std::shared_ptr<AbstractController>	AbsCtrlPtr;
			typedef std::shared_ptr<Node>				NodePtr;
			typedef std::shared_ptr<DrawCall>			DrawCallPtr;

		private:
			std::shared_ptr<Geometry>		_geometry;
			std::shared_ptr<DataProvider>	_material;
			std::shared_ptr<Effect>			_effect;

			std::list<DrawCallPtr>			_drawCalls;

			Signal<AbsCtrlPtr, NodePtr>::Slot	_targetAddedSlot;
			Signal<AbsCtrlPtr, NodePtr>::Slot	_targetRemovedSlot;

		public:
			static
			Ptr
			create(std::shared_ptr<Geometry> 		geometry,
					std::shared_ptr<DataProvider> 	material,
					std::shared_ptr<Effect>			effect)
			{
				Ptr surface(new SurfaceController(geometry, material, effect));

				surface->initialize();

				return surface;
			}

			inline
			std::shared_ptr<Geometry>
			geometry()
			{
				return _geometry;
			}

			inline
			std::shared_ptr<DataProvider>
			material()
			{
				return _material;
			}

			inline
			const std::list<DrawCallPtr>&
			drawCalls()
			{
				return _drawCalls;
			}

		private:
			SurfaceController(std::shared_ptr<Geometry> 	geometry,
								std::shared_ptr<DataProvider> material,
								std::shared_ptr<Effect>		effect);

			void
			initialize();

			void
			targetAddedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target);

			void
			targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target);
		};
	}
}
