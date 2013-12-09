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
	namespace render
	{
		enum class Layout
		{
			FIRST		= 0,
			OPAQUE		= 1000,
			TRANSPARENT	= 2000,
			LAST		= 3000
		};

		inline
		std::string
		layoutName(Layout value)
		{
			return value == Layout::FIRST		? "first" : 
				value == Layout::OPAQUE			? "opaque" :
				value == Layout::TRANSPARENT	? "transparent" : 
				"last";
		}

		inline
		Layout
		layout(const std::string& str)
		{
			return str == layoutName(Layout::FIRST)		? Layout::FIRST :
				str == layoutName(Layout::OPAQUE)		? Layout::OPAQUE :
				str == layoutName(Layout::TRANSPARENT)	? Layout::TRANSPARENT : 
				Layout::LAST;
		}

		inline
		float
		layoutValue(const std::string& name)
		{
			return (float)layout(name);
		}
	}
}
