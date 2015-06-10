/*
Copyright (c) 2014 Aerys

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

#include "minko/Any.hpp"
#include "minko/Common.hpp"
#include "minko/Signal.hpp"

#include "minko/render/AbstractResource.hpp"

namespace minko
{
	namespace render
	{
		class IndexBuffer :
			public AbstractResource,
			public std::enable_shared_from_this<IndexBuffer>
		{
		public:
			typedef std::shared_ptr<IndexBuffer>		Ptr;

		private:
			typedef std::shared_ptr<AbstractContext>	AbsContextPtr;

		private:
			Any					            _data;
			unsigned int				    _numIndices;

			std::shared_ptr<Signal<Ptr>>	_changed;

		public:
			inline static
			Ptr
			create(AbsContextPtr context)
			{
				return std::shared_ptr<IndexBuffer>(new IndexBuffer(context));
			}

			inline static
			Ptr
			create(AbsContextPtr						context,
				   const std::vector<unsigned short>&	data)
			{
				Ptr ptr = std::shared_ptr<IndexBuffer>(new IndexBuffer(context, data));

				ptr->upload();

				return ptr;
			}

			inline static
			Ptr
			create(AbsContextPtr						context,
				   const std::vector<unsigned int>&	    data)
			{
				Ptr ptr = std::shared_ptr<IndexBuffer>(new IndexBuffer(context, data));

				return ptr;
			}

			template <typename T>
			inline static
			Ptr
			create(AbsContextPtr	context,
				   T*				begin,
				   T*				end)
			{
				Ptr ptr = std::shared_ptr<IndexBuffer>(new IndexBuffer(context, begin, end));

				ptr->upload();

				return ptr;
			}

			~IndexBuffer()
			{
				dispose();
			}

			inline
			std::vector<unsigned short>&
			data()
			{
				return *Any::unsafe_cast<std::vector<unsigned short>>(&_data);
			}

            template <typename T>
            inline
            std::vector<T>*
            dataPointer()
			{
			    return Any::cast<std::vector<T>>(&_data);
			}

			inline
			unsigned int
			numIndices() const
			{
				return _numIndices;
			}

			inline
			void
			upload()
			{
				upload(0);
			}

			void
			upload(uint offset, int count = -1);

			void
			upload(uint offset, int count, const std::vector<unsigned short>& data);

			void
			dispose();

            void
            disposeData();

			bool
			equals(Ptr indexBuffer)
			{
                return
                    dataPointer<unsigned short>() == indexBuffer->dataPointer<unsigned short>() ||
                    dataPointer<unsigned int>() == indexBuffer->dataPointer<unsigned int>();
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			changed() const
			{
				return _changed;
			}

		protected:
			inline
			IndexBuffer(AbsContextPtr context) :
				AbstractResource(context),
				_data(std::vector<unsigned short>()),
				_numIndices(0),
				_changed(Signal<IndexBuffer::Ptr>::create())
			{
			}

			inline
			IndexBuffer(AbsContextPtr						context,
						const std::vector<unsigned short>&	data) :
				AbstractResource(context),
				_data(data),
				_numIndices(data.size()),
				_changed(Signal<IndexBuffer::Ptr>::create())
			{
			}

			inline
			IndexBuffer(AbsContextPtr						context,
						const std::vector<unsigned int>&	data) :
				AbstractResource(context),
				_data(data),
				_numIndices(data.size()),
				_changed(Signal<IndexBuffer::Ptr>::create())
			{
			}

			template <typename T>
			IndexBuffer(AbsContextPtr	context,
						T*				begin,
						T*				end) :
				AbstractResource(context),
				_data(std::vector<T>(begin, end)),
				_numIndices(dataPointer<T>()->size()),
				_changed(Signal<Ptr>::create())
			{
			}
		};
	}
}
