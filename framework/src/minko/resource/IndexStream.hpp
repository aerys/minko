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

namespace
{
	using namespace minko::render;
}

namespace minko
{
	namespace resource
	{
		class IndexStream
		{
		public:
			typedef std::shared_ptr<IndexStream>	Ptr;

		private:
			std::vector<unsigned short>			_data;
			std::shared_ptr<AbstractContext>	_context;
			int									_buffer;

		public:
			inline static
			Ptr
			create(std::shared_ptr<AbstractContext> context, std::vector<unsigned short>& data)
			{
				return std::shared_ptr<IndexStream>(new IndexStream(context, data));
			}

			inline static
			Ptr
			create(std::shared_ptr<AbstractContext> context, unsigned short* begin, unsigned short* end)
			{
				return std::shared_ptr<IndexStream>(new IndexStream(context, begin, end));
			}

			inline
			const std::vector<unsigned short>
			data()
			{
				return _data;
			}

			inline
			const int
			buffer()
			{
				return _buffer;
			}

		private:
			IndexStream(std::shared_ptr<AbstractContext>	context,
						std::vector<unsigned short>			data) :
				_data(data),
				_context(context),
				_buffer(-1)
			{
				upload();
			}

			IndexStream(std::shared_ptr<AbstractContext>	context,
						unsigned short*						begin,
						unsigned short*						end) :
				_data(begin, end),
				_context(context),
				_buffer(-1)
			{
				upload();
			}

			void
			upload();
		};
	}
}
