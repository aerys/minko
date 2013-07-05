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
	namespace file
	{
		class Options
		{
		public:
			typedef std::shared_ptr<Options> Ptr;

		private:
			std::shared_ptr<render::AbstractContext>	_context;
			std::string									_includePath;
			std::shared_ptr<AssetsLibrary>				_assets;

		public:
			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext> context)
			{
				return std::shared_ptr<Options>(new Options(context));
			}

			inline
			std::shared_ptr<render::AbstractContext>
			context()
			{
				return _context;
			}
			
			inline
			std::shared_ptr<AssetsLibrary>
			assets()
			{
				return _assets;
			}

			inline
			void
			assets(std::shared_ptr<AssetsLibrary> value)
			{
				_assets = value;
			}

			inline
			const std::string&
			includePath()
			{
				return _includePath;
			}

			inline
			void
			includePath(const std::string& includePath)
			{
				_includePath = includePath;
			}

		private:
			Options(std::shared_ptr<render::AbstractContext>	context) :
				_context(context)
			{
			}
		};
	}
}
