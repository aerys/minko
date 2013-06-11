#pragma once

#include "minko/Common.hpp"

namespace
{
	using namespace minko::render::context;
}

namespace minko
{
	namespace render
	{
		namespace stream
		{
			template <typename T>
			class VertexStream
			{
			public:
				typedef std::shared_ptr<VertexStream<T>>	ptr;

			private:
				std::shared_ptr<AbstractContext>	_context;
				std::string							_name;
				std::vector<T>						_data;
				int									_buffer;

			public:
				inline static
				ptr
				create(std::shared_ptr<AbstractContext> context)
				{
					return std::shared_ptr<VertexStream<T>>(new VertexStream<T>(context));
				}

				inline static
				ptr
				create(std::shared_ptr<AbstractContext>	context,
					   const std::string&				name,
					   T*								data,
					   const unsigned int				size,
					   const unsigned int				offset = 0)
				{
					return std::shared_ptr<VertexStream<T>>(new VertexStream(context, name, data, offset, size));
				}

				inline static
				ptr
				create(std::shared_ptr<AbstractContext>		context,
					   const std::string&					name,
					   typename std::vector<T>::iterator	begin,
					   typename std::vector<T>::iterator	end)
				{
					return std::shared_ptr<VertexStream<T>>(new VertexStream(context, name, begin, end));
				}
				
				inline static
				ptr
				create(std::shared_ptr<AbstractContext>	context, const std::string& name, T* begin, T* end)
				{
					return std::shared_ptr<VertexStream<T>>(new VertexStream(
						context, 
						name,
						begin,
						end
					));
				}
				
				inline static
				ptr
				create(std::shared_ptr<AbstractContext>	context, const std::string& name, const std::vector<T>& data)
				{
					return create(context, name, data.begin(), data.end());
				}

				inline
				const std::string&
				name()
				{
					return _name;
				}

				inline
				const std::vector<T>&
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

				void
				upload()
				{
					if (_buffer != -1)
						_context->deleteVertexBuffer(_buffer);

					_buffer = _context->createVertexBuffer(_data.size());
					if (_buffer < 0)
						throw;

					_context->uploadVertexBufferData(_buffer, 0, _data.size(), &_data[0]);
				}

			private:
				VertexStream(std::shared_ptr<AbstractContext> context) :
					_context(context),
					_buffer(-1)
				{
				}

				VertexStream(std::shared_ptr<AbstractContext>	context,
							 const std::string&					name,
							 T*									data,
							 const unsigned int					size,
							 const unsigned int					offset) :
					_context(context),
					_name(name),
					_data(data + offset, data + offset + size),
					_buffer(-1)
				{
				}

				VertexStream(std::shared_ptr<AbstractContext>	context,
							 const std::string&					name,
							 typename std::vector<T>::iterator	begin,
							 typename std::vector<T>::iterator	end) :
					_context(context),
					_name(name),
					_data(begin, end),
					_buffer(-1)
				{
				}

				VertexStream(std::shared_ptr<AbstractContext> context, const std::string& name, T* begin, T* end) :
					_context(context),
					_name(name),
					_data(begin, end),
					_buffer(-1)
				{
				}
			};
		}
	}
}
