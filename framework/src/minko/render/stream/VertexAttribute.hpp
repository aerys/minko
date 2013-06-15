#pragma once

#include "minko/Common.hpp"

namespace minko
{
	namespace render
	{
		namespace stream
		{
			class VertexAttribute
			{
			public:
				typedef std::shared_ptr<VertexAttribute> ptr;

			private:
				const std::string	_name;
				const unsigned int	_size;
				const unsigned int	_offset;

			public:
				inline static
				ptr
				create(const std::string& name, unsigned int size, unsigned int offset)
				{
					return std::shared_ptr<VertexAttribute>(new VertexAttribute(name, size, offset));
				}

				inline
				const std::string&
				name()
				{
					return _name;
				}

				inline
				const unsigned int
				size()
				{
					return _size;
				}

				inline
				const unsigned int
				offset()
				{
					return _offset;
				}

			private:
				VertexAttribute(const std::string& name, const unsigned int size, const unsigned int offset) :
					_name(name),
					_size(size),
					_offset(offset)
				{
				}
			};
		}
	}
}
