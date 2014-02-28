#include "minko/Common.hpp"
#include "minko/material/Material.hpp"

namespace minko
{
	namespace material
	{
		class MyCustomMaterial :
			public Material
		{
		public:
			typedef std::shared_ptr<MyCustomMaterial>	Ptr;

		public:
			inline static
				Ptr
				create()
			{
					return std::shared_ptr<MyCustomMaterial>(new MyCustomMaterial());
				}

			inline
				void
				color(std::shared_ptr<math::Vector4> rgba)
			{
					set("color", rgba);
				}
		};
	}
}