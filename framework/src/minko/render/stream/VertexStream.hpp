#pragma once

#include "minko/Common.hpp"
#include "minko/render/context/AbstractContext.hpp"

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
			class VertexStream
			{
			public:
				typedef std::shared_ptr<VertexStream>	ptr;

			private:
				typedef std::shared_ptr<VertexAttribute>	VxAttrPtr;

			private:
				std::shared_ptr<AbstractContext>	_context;
				std::vector<float>					_data;
				int									_buffer;
				std::list<VxAttrPtr>				_attributes;

			public:
				inline static
				ptr
				create(std::shared_ptr<AbstractContext> context)
				{
					return std::shared_ptr<VertexStream>(new VertexStream(context));
				}

				inline static
				ptr
				create(std::shared_ptr<AbstractContext>	context,
					   float*							data,
					   const unsigned int				size,
					   const unsigned int				offset = 0)
				{
					return std::shared_ptr<VertexStream>(new VertexStream(context, data, offset, size));
				}

				inline static
				ptr
				create(std::shared_ptr<AbstractContext>		context,
					   std::vector<float>::const_iterator	begin,
					   std::vector<float>::const_iterator	end)
				{
					return std::shared_ptr<VertexStream>(new VertexStream(context, begin, end));
				}
				
				inline static
				ptr
				create(std::shared_ptr<AbstractContext>	context, float* begin, float* end)
				{
					return std::shared_ptr<VertexStream>(new VertexStream(
						context, 
						begin,
						end
					));
				}
				
				inline static
				ptr
				create(std::shared_ptr<AbstractContext>	context, const std::vector<float>& data)
				{
					return create(context, data.begin(), data.end());
				}

				inline
				const std::vector<float>&
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

				inline
				const std::list<VxAttrPtr>&
				attributes()
				{
					return _attributes;
				}

				void
				upload();

				void
				addAttribute(VxAttrPtr attribute);

				bool
				hasAttribute(VxAttrPtr attribute);

				VxAttrPtr
				attribute(const std::string& attributeName);

			private:
				VertexStream(std::shared_ptr<AbstractContext> context);

				VertexStream(std::shared_ptr<AbstractContext>	context,
							 float*								data,
							 const unsigned int					size,
							 const unsigned int					offset);

				VertexStream(std::shared_ptr<AbstractContext>	context,
							 std::vector<float>::const_iterator	begin,
							 std::vector<float>::const_iterator	end);

				VertexStream(std::shared_ptr<AbstractContext> context, float* begin, float* end);
			};
		}
	}
}
