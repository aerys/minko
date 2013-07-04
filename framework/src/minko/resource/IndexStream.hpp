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

#include "minko/resource/AbstractResource.hpp"

namespace minko
{
	namespace resource
	{
		class IndexStream :
			public AbstractResource
		{
		public:
			typedef std::shared_ptr<IndexStream>	Ptr;

		private:
			std::vector<unsigned short>	_data;

		public:
			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext> context, std::vector<unsigned short>& data)
			{
				return std::shared_ptr<IndexStream>(new IndexStream(context, data));
			}

			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext> context, unsigned short* begin, unsigned short* end)
			{
				return std::shared_ptr<IndexStream>(new IndexStream(context, begin, end));
			}

			inline
			const std::vector<unsigned short>&
			data()
			{
				return _data;
			}

			void
			upload();
			
			void
			dispose();

		private:
			IndexStream(std::shared_ptr<render::AbstractContext>	context,
						std::vector<unsigned short>					data) :
				AbstractResource(context),
				_data(data)
			{
				upload();
			}

			IndexStream(std::shared_ptr<render::AbstractContext>	context,
						unsigned short*								begin,
						unsigned short*								end) :
				AbstractResource(context),
				_data(begin, end)
			{
				upload();
			}
		};
	}
}
