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

namespace minko
{
	class AssetsLibrary :
		public std::enable_shared_from_this<AssetsLibrary>
	{
	public:
		typedef std::shared_ptr<AssetsLibrary>						Ptr;

	private:
		typedef std::shared_ptr<render::geometry::Geometry>			GeometryPtr;
		typedef std::shared_ptr<render::context::AbstractContext>	AbsContextPtr;

	private:
		std::map<std::string, GeometryPtr>	_geometries;

	public:
		inline static
		Ptr
		create(AbsContextPtr context)
		{
			return std::shared_ptr<AssetsLibrary>(new AssetsLibrary(context));
		}

	private:
		AssetsLibrary(AbsContextPtr context);
	};
}