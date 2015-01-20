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

#include "minko/geometry/Geometry.hpp"

namespace minko
{
	namespace geometry
	{
		class LineGeometry:	public Geometry
		{
		public:
			typedef std::shared_ptr<LineGeometry> Ptr;

		private:
			static const uint						MAX_NUM_LINES;
			static const std::string				ATTRNAME_START_POS;
			static const std::string				ATTRNAME_STOP_POS;
			static const std::string				ATTRNAME_WEIGHTS;

			float									_currentX;
			float									_currentY;
			float									_currentZ;
			uint									_numLines;

			std::shared_ptr<render::VertexBuffer>	_vertexBuffer;
			std::shared_ptr<render::IndexBuffer>	_indexBuffer;

		public:
			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext> context)
			{
				Ptr ptr = std::shared_ptr<LineGeometry>(new LineGeometry());

				ptr->initialize(context);

				return ptr;
			}

			math::vec3
			currentXYZ() const;

			inline
			uint
			numLines() const
			{
				return _numLines;
			}

			inline
			Ptr
			moveTo(float x, float y, float z)
			{
				_currentX	= x;
				_currentY	= y;
				_currentZ	= z;

				return std::static_pointer_cast<LineGeometry>(shared_from_this());
			}

			Ptr
			moveTo(const math::vec3&);

			Ptr
			lineTo(float x, float y, float z, unsigned int numSegments = 1);

			Ptr
			lineTo(const math::vec3&, unsigned int numSegments = 1);

			void
			upload();

		protected:
            LineGeometry();

			void
			initialize(std::shared_ptr<render::AbstractContext> context);
		};
	}
}
