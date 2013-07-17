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

#include "Geometry.hpp"
#include <minko/render/IndexBuffer.hpp>

using namespace minko;
using namespace minko::geometry;
using namespace minko::render;

void
Geometry::computeNormals()
{
	const unsigned int numVertices = this->numVertices();
	if (numVertices == 0)
		return;

	if (_data->hasProperty("geometry.vertex.attribute.normal"))
		throw std::logic_error("The geometry already stores precomputed normals.");

	if (!_data->hasProperty("geometry.vertex.attribute.position"))
		throw std::logic_error("Computation of normals requires positions.");

	const std::vector<unsigned short>& indices (this->indices()->data());
	const unsigned int numFaces = indices.size() / 3;

	unsigned short vertexIds[3] = { 0, 0, 0 };

	VertexBuffer::Ptr xyzBuffer = _data->get<VertexBuffer::Ptr>("geometry.vertex.attribute.position");
	VertexAttribute::Ptr xyzAttr = 
	const std::vector<float>& xyzData (xyzBuffer);
	const unsigned int	xyzSize = 

	for (unsigned int i=0, offset=0; i<numFaces; ++i)
	{
		for (unsigned int k=0; k<3; ++k)
		{
			vertexIds[k] = indices[offset++];

		}
	}


}

void
Geometry::computeTangentSpace(bool doNormals)
{
	const unsigned int numVertices = this->numVertices();
	if (numVertices == 0)
		return;

	if (!_data->hasProperty("position") || !_data->hasProperty("uv"))
		throw std::logic_error("Computation of tangent space requires positions and uv.");

	if 
}