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
#include "minko/data/ArrayProvider.hpp"
#include "minko/render/VertexBuffer.hpp"

namespace minko
{
	namespace geometry
	{
		class Geometry :
			public std::enable_shared_from_this<Geometry>
		{
		public:
			typedef std::shared_ptr<Geometry> Ptr;

		private:
			typedef std::shared_ptr<render::VertexBuffer> VBPtr;
			typedef std::shared_ptr<data::ArrayProvider>  ProviderPtr;

		private:
			ProviderPtr								_data;
			unsigned int							_vertexSize;
			unsigned int							_numVertices;
			std::list<VBPtr>						_vertexBuffers;
			std::shared_ptr<render::IndexBuffer>	_indexBuffer;

			std::unordered_map<VBPtr, Signal<VBPtr, int>::Slot>	_vbToVertexSizeChangedSlot;

		public:
			virtual
			~Geometry()
			{
				
			}

			static
			Ptr
			create()
			{
				return std::shared_ptr<Geometry>(new Geometry());
			}

			inline
			ProviderPtr
			data() const
			{
				return _data;
			}

			inline
			const std::list<VBPtr>&
			vertexBuffers() const
			{
				return _vertexBuffers;
			}

			inline
			VBPtr
			vertexBuffer(const std::string& vertexAttributeName)
			{
				auto vertexBufferIt = std::find_if(
					_vertexBuffers.begin(),
					_vertexBuffers.end(),
					[&](render::VertexBuffer::Ptr vb) { return vb->hasAttribute(vertexAttributeName); }
				);

				if (vertexBufferIt == _vertexBuffers.end())
					throw std::invalid_argument("vertexAttributeName = " + vertexAttributeName);

				return *vertexBufferIt;
			}

			inline
			bool
			hasVertexBuffer(VBPtr vertexBuffer) const
			{
				return std::find(_vertexBuffers.begin(), _vertexBuffers.end(), vertexBuffer) != _vertexBuffers.end();
			}

			inline
			bool
			hasVertexAttribute(const std::string& vertexAttributeName) const
			{
				return _data->hasProperty(vertexAttributeName);
			}

			inline
			void
			indices(std::shared_ptr<render::IndexBuffer> indices)
			{
				_indexBuffer = indices;
				_data->set("indices", indices);
			}

			inline
			std::shared_ptr<render::IndexBuffer>
			indices() const
			{
				return _indexBuffer;
			}

			void
			addVertexBuffer(std::shared_ptr<render::VertexBuffer>);

			void
			removeVertexBuffer(VBPtr vertexBuffer);

			void
			removeVertexBuffer(const std::string& vertexAttributeName);

			inline 
			unsigned int
			numVertices() const
			{
				return _numVertices;
			}
			
			inline
			unsigned int
			vertexSize() const
			{
				return _vertexSize;
			};

			Ptr
			computeNormals();

			Ptr
			computeTangentSpace(bool computeNormals);

			void
			removeDuplicatedVertices();

			static
			void
			removeDuplicatedVertices(std::vector<unsigned short>&		indices,
									 std::vector<std::vector<float>>&	vertices,
									 uint								numVertices);

			bool
			cast(std::shared_ptr<math::Ray>		ray,
				 float&							distance,
				 uint&							triangle,
				 std::shared_ptr<math::Vector3>	hitXyz		= nullptr,
				 std::shared_ptr<math::Vector2>	hitUv 		= nullptr,
				 std::shared_ptr<math::Vector3>	hitNormal 	= nullptr);

			void
			upload();

			inline
			bool
			equals(Ptr geom)
			{
				bool vertexEquality		= _vertexBuffers.size() == geom->_vertexBuffers.size();
				bool indexEquality		= _indexBuffer == geom->_indexBuffer;
				auto vertexBuffer1Start = _vertexBuffers.begin();
				auto vertexBuffer2Start = geom->_vertexBuffers.begin();

				if (vertexEquality)
				{
					for (uint i = 0; i < _vertexBuffers.size() && vertexEquality; ++i)
					{
						vertexEquality = vertexEquality && (*vertexBuffer1Start == *vertexBuffer2Start);
						std::next(vertexBuffer1Start);
						std::next(vertexBuffer2Start);
					}
				}

				return vertexEquality && indexEquality;
			}

		protected:
			Geometry();

			inline
			void
			vertexSize(unsigned int value)
			{
				_vertexSize = value;
			}

			void
			vertexSizeChanged(VBPtr vertexBuffer, int offset);

		private:
			void
			removeVertexBuffer(std::list<VBPtr>::iterator vertexBufferIt);

			void
			getHitUv(uint triangle, std::shared_ptr<math::Vector2> lambda, std::shared_ptr<math::Vector2> hitUv);

			void
			getHitNormal(uint triangle, std::shared_ptr<math::Vector3> hitNormal);
		};
	}
}
