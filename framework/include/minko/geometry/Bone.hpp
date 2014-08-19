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

namespace minko
{
    namespace geometry
    {
        class Bone:
            public std::enable_shared_from_this<Bone>
        {
        public:
            typedef std::shared_ptr<Bone>               Ptr;

        private:
            typedef std::shared_ptr<scene::Node>        NodePtr;
            typedef std::shared_ptr<math::Matrix4x4>    Matrix4x4Ptr;

        private:
			
			Matrix4x4Ptr								_offsetMatrix;
            const std::vector<unsigned short>           _vertexIds;
            const std::vector<float>                    _vertexWeights;

        public:
            static
            inline
            Ptr
			create(Matrix4x4Ptr							offsetMatrix, 
                   const std::vector<unsigned short>&   vertexIds,
                   const std::vector<float>&            vertexWeights)
            {
				return std::shared_ptr<Bone>(new Bone(offsetMatrix, vertexIds, vertexWeights));
            }

            inline
            Matrix4x4Ptr
            offsetMatrix() const
            {
                return _offsetMatrix;
            }

            inline
            const std::vector<unsigned short>&
            vertexIds() const
            {
                return _vertexIds;
            }

            inline
            const std::vector<float>&
            vertexWeights() const
            {
                return _vertexWeights;
            }

        private:
			Bone(Matrix4x4Ptr, const std::vector<unsigned short>&, const std::vector<float>&);
        };
    }
}
