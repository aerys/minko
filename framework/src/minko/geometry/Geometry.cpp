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

#include <minko/math/Vector3.hpp>
#include <minko/render/IndexBuffer.hpp>
#include <minko/render/VertexBuffer.hpp>

using namespace minko::math;
using namespace minko::geometry;
using namespace minko::render;

void
Geometry::addVertexBuffer(std::shared_ptr<render::VertexBuffer> vertexBuffer)
{
	unsigned int bufferVertexSize	= 0;

	for (auto attribute : vertexBuffer->attributes())
	{
		_data->set("geometry.vertex.attribute." + std::get<0>(*attribute), vertexBuffer);
		bufferVertexSize += std::get<1>(*attribute);
	}
	_vertexSize	+= bufferVertexSize;
	_data->set("geometry.vertex.size", _vertexSize);

	const unsigned int bufferNumVertices	= vertexBuffer->data().size() / bufferVertexSize;
	if (_numVertices == -1)
		_numVertices	= bufferNumVertices;
	else if (_numVertices != bufferNumVertices)
		throw std::logic_error("inconsistent number of vertices between the geometry's vertex streams.");

}

void
Geometry::computeNormals()
{
	const unsigned int numVertices = this->numVertices();
	if (numVertices == 0)
		return;

	//if (_data->hasProperty("geometry.vertex.attribute.normal"))
	//	throw std::logic_error("The geometry already stores precomputed normals.");
		
	if (!_data->hasProperty("geometry.vertex.attribute.position"))
		throw std::logic_error("Computation of normals requires positions.");

	const std::vector<unsigned short>& indices (this->indices()->data());
	const unsigned int numFaces = indices.size() / 3;

	unsigned short vertexIds[3] = { 0, 0, 0 };
	std::vector<Vector3::Ptr> xyz(3);

	VertexBuffer::Ptr xyzBuffer		= _data->get<VertexBuffer::Ptr>("geometry.vertex.attribute.position");
	const unsigned int xyzSize		= std::get<1>(*xyzBuffer->attribute("position"));
	const unsigned int xyzOffset	= std::get<2>(*xyzBuffer->attribute("position"));
	const std::vector<float>& xyzData (xyzBuffer->data());

	std::vector<float> normalsData(3*numVertices, 0.0f);

	for (unsigned int i = 0, offset = 0; i < numFaces; ++i)
	{
		for (unsigned int k = 0; k < 3; ++k)
		{
			vertexIds[k] = indices[offset++];
			const unsigned int index = xyzOffset + vertexIds[k] * xyzSize;
			xyz[k] = Vector3::create(xyzData[index], xyzData[index+1], xyzData[index+2]);
		}

		Vector3::Ptr faceNormal = Vector3::create()
			->copyFrom(xyz[0] - xyz[2])
			->cross(xyz[0] - xyz[1]);

 		for (unsigned int k = 0; k < 3; ++k)
		{
			const unsigned int index = 3*vertexIds[k];

			normalsData[index]		+= faceNormal->x();
			normalsData[index+1]	+= faceNormal->y();
			normalsData[index+2]	+= faceNormal->z();
		}
	}

	for (unsigned int i = 0, index = 0; i < numVertices; ++i, index += 3)
	{
		const float x = normalsData[index];
		const float y = normalsData[index+1];
		const float z = normalsData[index+2];
		const float lengthSquared = x*x + y*y + z*z;
		const float invLength = lengthSquared > 1e-6f ? 1.0f / sqrtf(lengthSquared) : 0.0f;

		normalsData[index]		*= invLength;
		normalsData[index+1]	*= invLength;
		normalsData[index+2]	*= invLength;
	}

	VertexBuffer::Ptr normalsBuffer = VertexBuffer::create(xyzBuffer->context(), normalsData);
	normalsBuffer->addAttribute("normal", 3, 0);
	addVertexBuffer(normalsBuffer);
}

void
Geometry::computeTangentSpace(bool doNormals)
{
	const unsigned int numVertices = this->numVertices();
	if (numVertices == 0)
		return;

	if (!_data->hasProperty("geometry.vertex.attribute.position") 
		|| !_data->hasProperty("geometry.vertex.attribute.uv"))
		throw std::logic_error("Computation of tangent space requires positions and uv.");

	if (doNormals)
		computeNormals();

	const std::vector<unsigned short>& indices (this->indices()->data());
	const unsigned int numFaces = indices.size() / 3;

	unsigned short vertexIds[3] = { 0, 0, 0 };
	std::vector<Vector3::Ptr> xyz(3);
	std::vector<Vector2::Ptr> uv(3);

	VertexBuffer::Ptr xyzBuffer		= _data->get<VertexBuffer::Ptr>("geometry.vertex.attribute.position");
	const unsigned int xyzSize		= std::get<1>(*xyzBuffer->attribute("position"));
	const unsigned int xyzOffset	= std::get<2>(*xyzBuffer->attribute("position"));
	const std::vector<float>& xyzData (xyzBuffer->data());

	VertexBuffer::Ptr uvBuffer		= _data->get<VertexBuffer::Ptr>("geometry.vertex.attribute.uv");
	const unsigned int uvSize		= std::get<1>(*uvBuffer->attribute("uv"));
	const unsigned int uvOffset		= std::get<2>(*uvBuffer->attribute("uv"));
	const std::vector<float>& uvData (uvBuffer->data());

	std::vector<float> tangentsData(3 * numVertices, 0.0f);

	for (unsigned int i = 0, offset = 0; i < numFaces; ++i)
	{
		for (unsigned int k = 0; k < 3; ++k)
		{
			vertexIds[k] = indices[offset++];
			unsigned int index = xyzOffset + vertexIds[k] * xyzSize;
			xyz[k] = Vector3::create(xyzData[index], xyzData[index+1], xyzData[index+2]);
			index = uvOffset + vertexIds[k] * uvSize;
			uv[k] = Vector3::create(uvData[index], uvData[index+1], uvData[index+2]);
		}

		Vector2::Ptr uv02 = uv[0] - uv[2];
		Vector2::Ptr uv12 = uv[1] - uv[2];
		const float denom = uv02->x() * uv12->y() - uv12->x() * uv02->y();
		const float invDenom = fabsf(denom) > 1e-6f ? 1.0f/denom : 1.0f;

		Vector3::Ptr faceTangent = ((xyz[0]-xyz[2]) * uv12->y() - (xyz[1]-xyz[2]) * uv02->y()) * invDenom;

		for (unsigned int k=0; k<3; ++k)
		{
			const unsigned int index = 3 * vertexIds[k];

			tangentsData[index]		+= faceTangent->x();
			tangentsData[index+1]	+= faceTangent->y();
			tangentsData[index+2]	+= faceTangent->z();
		}
	}

	for (unsigned int i = 0, index = 0; i < numVertices; ++i, index += 3)
	{
		const float x = tangentsData[index];
		const float y = tangentsData[index + 1];
		const float z = tangentsData[index + 2];
		const float lengthSquared = x * x + y * y + z * z;
		const float invLength = lengthSquared > 1e-6f ? 1.0f / sqrtf(lengthSquared) : 0.0f;

		tangentsData[index]		*= invLength;
		tangentsData[index+1]	*= invLength;
		tangentsData[index+2]	*= invLength;
	}

	VertexBuffer::Ptr tangentsBuffer = VertexBuffer::create(xyzBuffer->context(), tangentsData);
	tangentsBuffer->addAttribute("tangent", 3, 0);
	addVertexBuffer(tangentsBuffer);
}