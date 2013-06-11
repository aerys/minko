#pragma once

#include "minko/Common.hpp"

namespace minko
{
	class AssetsLibrary :
		public std::enable_shared_from_this<AssetsLibrary>
	{
	public:
		typedef std::shared_ptr<AssetsLibrary>						ptr;

	private:
		typedef std::shared_ptr<render::geometry::Geometry>			GeometryPtr;
		typedef std::shared_ptr<render::context::AbstractContext>	AbsContextPtr;

	private:
		std::map<std::string, GeometryPtr>	_geometries;

	public:
		inline static
		ptr
		create(AbsContextPtr context)
		{
			return std::shared_ptr<AssetsLibrary>(new AssetsLibrary(context));
		}

	private:
		AssetsLibrary(AbsContextPtr context);
	};
}