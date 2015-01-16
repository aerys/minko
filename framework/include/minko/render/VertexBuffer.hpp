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

#include "minko/Common.hpp"

#include "minko/render/AbstractResource.hpp"
#include "minko/render/VertexAttribute.hpp"

namespace minko
{
	namespace render
	{
		class VertexBuffer :
			public AbstractResource,
			public std::enable_shared_from_this<VertexBuffer>
		{
		public:
			typedef std::shared_ptr<VertexBuffer>		Ptr;

		private:
			std::vector<float>					_data;
            std::list<VertexAttribute>		    _attributes;
			uint								_vertexSize;

			std::shared_ptr<Signal<Ptr, int>>	_vertexSizeChanged;

		public:
			~VertexBuffer()
			{
				dispose();
			}

			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext> context)
			{
				return std::shared_ptr<VertexBuffer>(new VertexBuffer(context));
			}

			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext>	context,
				   const float*								data,
				   const unsigned int						size,
				   const unsigned int						offset = 0)
			{
				return std::shared_ptr<VertexBuffer>(new VertexBuffer(
                    context, data, size, offset
				));
			}

			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext>	context,
					std::vector<float>::const_iterator		begin,
					std::vector<float>::const_iterator		end)
			{
				return std::shared_ptr<VertexBuffer>(new VertexBuffer(context, begin, end));
			}

			inline static
			Ptr
            create(std::shared_ptr<render::AbstractContext>   context,
                   float*                                     begin,
                   float*                                     end)
			{
				return std::shared_ptr<VertexBuffer>(new VertexBuffer(
					context,
					begin,
					end
				));
			}

			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext>	context,
				   const std::vector<float>&				data)
			{
				return create(context, data.begin(), data.end());
			}

			inline
			std::vector<float>&
			data()
			{
				return _data;
			}

			inline
            const std::list<VertexAttribute>&
			attributes()
			{
				return _attributes;
			}

			inline
			const uint
			vertexSize()
			{
				return _vertexSize;
			}

			inline
			std::shared_ptr<Signal<Ptr, int>>
			vertexSizeChanged()
			{
				return _vertexSizeChanged;
			}

			inline
			uint
			numVertices() const
			{
				return _vertexSize > 0 ? _data.size() / _vertexSize : 0;
			}

			inline
			void
			upload()
			{
				upload(0, 0);
			}

			void
			upload(uint offset, uint numVertices = 0);

			void
			upload(uint offset, uint numVertices, const std::vector<float>& data);

			void
			dispose();

            void
            disposeData();

			void
			addAttribute(const std::string& name, const unsigned int size, const unsigned int offset = 0);

			void
			removeAttribute(const std::string& name);

			bool
			hasAttribute(const std::string& attributeName) const;

            const VertexAttribute&
			attribute(const std::string& attributeName) const;

			bool
			equals(std::shared_ptr<VertexBuffer> vertexBuffer)
			{
				return _data == vertexBuffer->_data;
			}

		protected:
			VertexBuffer(std::shared_ptr<render::AbstractContext> context);

			VertexBuffer(std::shared_ptr<render::AbstractContext>	context,
						 const float*								data,
						 const unsigned int							size,
						 const unsigned int							offset);

			VertexBuffer(std::shared_ptr<render::AbstractContext>	context,
							std::vector<float>::const_iterator		begin,
							std::vector<float>::const_iterator		end);

			VertexBuffer(std::shared_ptr<render::AbstractContext>	context,
						 float*										begin,
						 float*										end);

			void
			vertexSize(unsigned int value);
		};
	}
}
