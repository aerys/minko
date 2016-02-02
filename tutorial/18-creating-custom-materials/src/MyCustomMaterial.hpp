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
			create(const std::string& name = "MyCustomMaterial")
			{
				return Ptr(new MyCustomMaterial(name));
			}

			inline
			void
			color(math::vec4 rgba)
			{
				set({ {"color", rgba} });
			}
		
		private:
			MyCustomMaterial(const std::string& name):
				Material(name)
			{
			}
		};

	}
}