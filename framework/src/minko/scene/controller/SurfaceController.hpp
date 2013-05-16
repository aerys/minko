#pragma once

#include "minko/Common.hpp"
#include "minko/scene/controller/AbstractController.hpp"

namespace
{
	using namespace minko;
	using namespace minko::render;
	using namespace minko::render::geometry;
	using namespace minko::scene::data;
}

namespace minko
{
	namespace scene
	{
		namespace controller
		{
			class SurfaceController :
				public AbstractController,
				public std::enable_shared_from_this<SurfaceController>
			{
			public:
				typedef std::shared_ptr<SurfaceController>	ptr;

			public:
				static
				ptr
				create(std::shared_ptr<Geometry> 		geometry,
					   std::shared_ptr<DataProvider> 	material,
					   std::shared_ptr<Effect>			effect)
				{
					ptr surface(new SurfaceController(geometry, material, effect));

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

			private:
				std::shared_ptr<Geometry>		_geometry;
				std::shared_ptr<DataProvider>	_material;
				std::shared_ptr<Effect>			_effect;

				std::shared_ptr<DrawCall>		_drawCall;

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
}
