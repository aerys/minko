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

#include "minko/render/AbstractResource.hpp"

namespace minko
{
	namespace render
	{
		class IndexBuffer :
			public AbstractResource,
			public Convertible<IndexBuffer>
		{
		public:
			typedef std::shared_ptr<IndexBuffer>	Ptr;

		private:
			std::vector<unsigned short>	_data;

		public:
			~IndexBuffer()
			{
				dispose();
			}

			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext> context)
			{
				return std::shared_ptr<IndexBuffer>(new IndexBuffer(context));
			}

			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext> context,
				   const std::vector<unsigned short>&		data)
			{
				return std::shared_ptr<IndexBuffer>(new IndexBuffer(context, data));
			}

			template <typename T>
			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext> context, T* begin, T* end)
			{
				return std::shared_ptr<IndexBuffer>(new IndexBuffer(context, begin, end));
			}

			inline
			std::vector<unsigned short>&
			data()
			{
				return _data;
			}

			inline
			void
			upload()
			{
				upload(0, 0);
			}

			void
			upload(uint offset, uint count = 0);

			void
			dispose();

			bool
			equals(std::shared_ptr<IndexBuffer> indexBuffer)
			{
				return _data == indexBuffer->_data;
			}
		protected:
			IndexBuffer(std::shared_ptr<render::AbstractContext> context) :
				AbstractResource(context)
			{

			}

			IndexBuffer(std::shared_ptr<render::AbstractContext>	context,
						const std::vector<unsigned short>&			data) :
				AbstractResource(context),
				_data(data)
			{
				upload();
			}

			template <typename T>
			IndexBuffer(std::shared_ptr<render::AbstractContext>	context,
						T*											begin,
						T*											end) :
				AbstractResource(context),
				_data(begin, end)
			{
				upload();
			}
		};
	}
}
