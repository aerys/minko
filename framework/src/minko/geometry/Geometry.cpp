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

#include "minko/geometry/Geometry.hpp"

#include "minko/math/Ray.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/VertexBuffer.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::geometry;
using namespace minko::render;

Geometry::Geometry() :
	_data(data::ArrayProvider::create("geometry")),
	_vertexSize(0),
	_numVertices(0),
	_indexBuffer(nullptr)
{
}

void
Geometry::addVertexBuffer(render::VertexBuffer::Ptr vertexBuffer)
{
	if (hasVertexBuffer(vertexBuffer))
		throw std::invalid_argument("vertexBuffer");

	const unsigned bufVertexSize	= vertexBuffer->vertexSize();
	const unsigned bufNumVertices	= vertexBuffer->numVertices();

	for (auto attribute : vertexBuffer->attributes())
		_data->set(std::get<0>(*attribute), vertexBuffer);
	_vertexSize	+= bufVertexSize;
	_data->set("vertex.size", _vertexSize);

	if (_vertexBuffers.size() > 0 && _numVertices != bufNumVertices)
		throw std::logic_error("inconsistent number of vertices between the geometry's vertex streams.");
	else if (_vertexBuffers.size() == 0)
		_numVertices = bufNumVertices;

	_vertexBuffers.push_back(vertexBuffer);

	_vbToVertexSizeChangedSlot[vertexBuffer] = vertexBuffer->vertexSizeChanged()->connect(std::bind(
		&Geometry::vertexSizeChanged,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
Geometry::removeVertexBuffer(std::list<render::VertexBuffer::Ptr>::iterator vertexBufferIt)
{
	VertexBuffer::Ptr vertexBuffer	= *vertexBufferIt;
	vertexBuffer->dispose();

	for (auto attribute : vertexBuffer->attributes())
		_data->unset(std::get<0>(*attribute));

	_vertexSize	-= vertexBuffer->vertexSize();
	_data->set("vertex.size", _vertexSize);

	_vertexBuffers.erase(vertexBufferIt);

	if (_vertexBuffers.size() == 0)
		_numVertices = 0;

	_vbToVertexSizeChangedSlot.erase(vertexBuffer);	
}

void
Geometry::removeVertexBuffer(render::VertexBuffer::Ptr vertexBuffer)
{
	if (!hasVertexBuffer(vertexBuffer))
		throw std::invalid_argument("vertexBuffer");

	removeVertexBuffer(std::find(_vertexBuffers.begin(), _vertexBuffers.end(), vertexBuffer));
}

void
Geometry::removeVertexBuffer(const std::string& attributeName)
{
	auto vertexBufferIt = std::find_if(
		_vertexBuffers.begin(),
		_vertexBuffers.end(),
		[&](render::VertexBuffer::Ptr vb) { return vb->hasAttribute(attributeName); }
	);

	if (vertexBufferIt == _vertexBuffers.end())
		throw std::invalid_argument("attributeName = " + attributeName);

	removeVertexBuffer(vertexBufferIt);
}

Geometry::Ptr
Geometry::computeNormals()
{
	const unsigned int numVertices = this->numVertices();

	if (numVertices == 0)
		return shared_from_this();

	if (_data->hasProperty("normal"))
		throw std::logic_error("The geometry already stores precomputed normals.");
		
	if (!_data->hasProperty("position"))
		throw std::logic_error("Computation of normals requires positions.");

	const std::vector<unsigned short>& indices	= this->indices()->data();
	const unsigned int numFaces					= indices.size() / 3;

	unsigned short vertexIds[3] = { 0, 0, 0 };
	std::vector<Vector3> xyz(3);

	VertexBuffer::Ptr xyzBuffer			= _data->get<VertexBuffer::Ptr>("position");
	const unsigned int xyzSize			= xyzBuffer->vertexSize();
	const unsigned int xyzOffset		= std::get<2>(*xyzBuffer->attribute("position"));
	const std::vector<float>& xyzData	= xyzBuffer->data();

	std::vector<float> normalsData(3 * numVertices, 0.0f);

	for (unsigned int i = 0, offset = 0; i < numFaces; ++i)
	{
		for (unsigned int k = 0; k < 3; ++k)
		{
			vertexIds[k] = indices[offset++];
			const unsigned int index = xyzOffset + vertexIds[k] * xyzSize;
			xyz[k] = Vector3(xyzData[index], xyzData[index + 1], xyzData[index + 2]);
		}

		Vector3 faceNormal = math::cross(xyz[0] - xyz[1], xyz[0] - xyz[2]);
 		for (unsigned int k = 0; k < 3; ++k)
		{
			const unsigned int index = 3 * vertexIds[k];

			normalsData[index]		+= faceNormal.x;
			normalsData[index + 1]	+= faceNormal.y;
			normalsData[index + 2]	+= faceNormal.z;
		}
	}

	for (unsigned int i = 0, index = 0; i < numVertices; ++i, index += 3)
	{
		const float x				= normalsData[index];
		const float y				= normalsData[index + 1];
		const float z				= normalsData[index + 2];
		const float lengthSquared	= x * x + y * y + z * z;
		const float invLength		= lengthSquared > 1e-6f ? 1.0f / sqrtf(lengthSquared) : 1.0f;

		normalsData[index]		*= invLength;
		normalsData[index + 1]	*= invLength;
		normalsData[index + 2]	*= invLength;
	}

	VertexBuffer::Ptr normalsBuffer = VertexBuffer::create(xyzBuffer->context(), normalsData);
	normalsBuffer->addAttribute("normal", 3, 0);
	addVertexBuffer(normalsBuffer);

	return shared_from_this();
}

Geometry::Ptr
Geometry::computeTangentSpace(bool doNormals)
{
	const unsigned int numVertices = this->numVertices();

	if (numVertices == 0)
		return shared_from_this();

	if (!_data->hasProperty("position") 
		|| !_data->hasProperty("uv"))
		throw std::logic_error("Computation of tangent space requires positions and uv.");

	if (doNormals)
		computeNormals();

	const std::vector<unsigned short>& indices (this->indices()->data());
	const unsigned int numFaces = indices.size() / 3;

	unsigned short vertexIds[3] = { 0, 0, 0 };
	std::vector<Vector3> xyz(3);
	std::vector<Vector2> uv(3);

	VertexBuffer::Ptr xyzBuffer			= _data->get<VertexBuffer::Ptr>("position");
	const unsigned int xyzSize			= xyzBuffer->vertexSize();
	const unsigned int xyzOffset		= std::get<2>(*xyzBuffer->attribute("position"));
	const std::vector<float>& xyzData	= xyzBuffer->data();

	VertexBuffer::Ptr uvBuffer			= _data->get<VertexBuffer::Ptr>("uv");
	const unsigned int uvSize			= uvBuffer->vertexSize();
	const unsigned int uvOffset			= std::get<2>(*uvBuffer->attribute("uv"));
	const std::vector<float>& uvData	= uvBuffer->data();

	std::vector<float> tangentsData(3 * numVertices, 0.0f);

	for (unsigned int i = 0, offset = 0; i < numFaces; ++i)
	{
		for (unsigned int k = 0; k < 3; ++k)
		{
			vertexIds[k] = indices[offset++];
			unsigned int index = xyzOffset + vertexIds[k] * xyzSize;
			xyz[k] = Vector3(xyzData[index], xyzData[index + 1], xyzData[index + 2]);
			index = uvOffset + vertexIds[k] * uvSize;
			uv[k] = Vector2(uvData[index], uvData[index + 1]);
		}

		Vector2 uv02			= uv[0] - uv[2];
		Vector2 uv12			= uv[1] - uv[2];
		const float denom		= uv02.x * uv12.y - uv12.x * uv02.y;
		const float invDenom	= fabsf(denom) > 1e-6f ? 1.0f/denom : 1.0f;

		Vector3 faceTangent = (xyz[0] - xyz[2]) * uv12.y - (xyz[1] - xyz[2]) * uv02.y * invDenom;

		for (unsigned int k = 0; k < 3; ++k)
		{
			const unsigned int index = 3 * vertexIds[k];

			tangentsData[index]		+= faceTangent.x;
			tangentsData[index + 1]	+= faceTangent.y;
			tangentsData[index + 2]	+= faceTangent.z;
		}
	}

	for (unsigned int i = 0, index = 0; i < numVertices; ++i, index += 3)
	{
		const float x				= tangentsData[index];
		const float y				= tangentsData[index + 1];
		const float z				= tangentsData[index + 2];
		const float lengthSquared	= x * x + y * y + z * z;
		const float invLength		= lengthSquared > 1e-6f ? 1.0f / sqrtf(lengthSquared) : 1.0f;

		tangentsData[index]		*= invLength;
		tangentsData[index + 1]	*= invLength;
		tangentsData[index + 2]	*= invLength;
	}

	VertexBuffer::Ptr tangentsBuffer = VertexBuffer::create(xyzBuffer->context(), tangentsData);
	tangentsBuffer->addAttribute("tangent", 3, 0);
	addVertexBuffer(tangentsBuffer);

	return shared_from_this();
}

void
Geometry::vertexSizeChanged(VertexBuffer::Ptr vertexBuffer, int offset)
{
	_vertexSize += offset;
}

void
Geometry::removeDuplicatedVertices()
{
	std::vector<std::vector<float>> vertices;

	for (auto vb : _vertexBuffers)
		vertices.push_back(vb->data());

	removeDuplicatedVertices(_indexBuffer->data(),	vertices, numVertices());
}

void
Geometry::removeDuplicatedVertices(std::vector<unsigned short>&		indices,
								   std::vector<std::vector<float>>&	vertices,
								   uint								numVertices)
{
	auto newVertexCount = 0;
	auto newLimit = 0;

	std::unordered_map<std::string, uint> hashToNewVertexId;
	std::unordered_map<uint, uint> oldVertexIdToNewVertexId;

	for (uint oldVertexId = 0; oldVertexId < numVertices; ++oldVertexId)
	{
		std::string hash;

		for (auto& vb : vertices)
		{
			auto vertexSize = vb.size() / numVertices;

			for (uint i = 0; i < vertexSize; ++i)
				hash += std::to_string(vb[oldVertexId * vertexSize + i]) + " ";
		}

		auto newVertexId = 0;

		if (!hashToNewVertexId.count(hash))
		{
			newVertexId = newVertexCount++;
			hashToNewVertexId[hash] = newVertexId;
			newLimit = 1 + newVertexId;

			if (newVertexId != oldVertexId)
			{
				for (auto& vb : vertices)
				{
					auto vertexSize = vb.size() / numVertices;

					std::copy(
						vb.begin() + oldVertexId * vertexSize,
						vb.begin() + (oldVertexId + 1) * vertexSize,
						vb.begin() + newVertexId * vertexSize
					);
				}
			}
		}
		else
			newVertexId = hashToNewVertexId[hash];

		oldVertexIdToNewVertexId[oldVertexId] = newVertexId;
	}

	for (auto& vb : vertices)
		vb.resize(newLimit * vb.size() / numVertices);

	for (auto& index : indices)
		index = oldVertexIdToNewVertexId[index];
}

bool
Geometry::cast(std::shared_ptr<math::Ray>	ray,
			   float&						distance,
			   uint&						triangle,
			   Vector3*						hitXyz,
			   Vector2*						hitUv,
			   Vector3*						hitNormal)
{
	static const auto EPSILON = 0.00001f;

	auto hit = false;
	auto& indicesData = _indexBuffer->data();
	auto numIndices = indicesData.size();

	auto xyzBuffer = vertexBuffer("position");
	auto& xyzData = xyzBuffer->data();
	auto xyzPtr = &xyzData[0];
	auto xyzVertexSize = xyzBuffer->vertexSize();
	auto xyzOffset = std::get<2>(*xyzBuffer->attribute("position"));

	auto minDistance = std::numeric_limits<float>::lowest();
	auto triangleIndice = -3;

	Vector2 lambda;

	for (uint i = 0; i < numIndices; i += 3)
	{
		auto v0 = math::make_vec3(xyzPtr + indicesData[i] * xyzVertexSize);
		auto v1 = math::make_vec3(xyzPtr + indicesData[i + 1] * xyzVertexSize);
		auto v2 = math::make_vec3(xyzPtr + indicesData[i + 2] * xyzVertexSize);
		auto edge1 = v1 - v0;
		auto edge2 = v2 - v0;
		auto pvec = math::cross(ray->direction(), edge2);
		auto dot = math::dot(edge1, pvec);

		if (dot > -EPSILON && dot < EPSILON)
			continue;

		auto invDot = 1.f / dot;
		auto tvec = ray->origin() - v0;

		auto u = math::dot(tvec, pvec) * invDot;
		if (u < 0.f || u > 1.f)
			continue;

		auto qvec = math::cross(tvec, edge1);
		auto v = math::dot(ray->origin(), qvec) * invDot;
		if (v < 0.f || u + v > 1.f)
			continue;

		auto t = math::dot(edge2, qvec) * invDot;
		if (t < minDistance)
		{
			minDistance = t;
			distance = t;
			triangle = i;
			hit = true;

			if (hitUv)
			{
				lambda.x = u;
				lambda.y = v;
			}
		}

		if (hitXyz)
		{
			hitXyz->x = ray->origin().x + minDistance * ray->direction().x;
			hitXyz->y = ray->origin().y + minDistance * ray->direction().y;
			hitXyz->z = ray->origin().z + minDistance * ray->direction().z;
		}

		if (hitUv)
			getHitUv(triangle, lambda, *hitUv);

		if (hitNormal)
			getHitNormal(triangle, *hitNormal);
	}

	return hit;
}

void
Geometry::getHitUv(uint triangle, Vector2 lambda, Vector2& hitUv)
{
	auto uvBuffer = vertexBuffer("uv");
	auto& uvData = uvBuffer->data();
	auto uvPtr = &uvData[0];
	auto uvVertexSize = uvBuffer->vertexSize();
	auto uvOffset = std::get<2>(*uvBuffer->attribute("uv"));
	auto& indicesData = _indexBuffer->data();

	auto u0 = uvData[indicesData[triangle] * uvVertexSize + uvOffset];
	auto v0 = uvData[indicesData[triangle] * uvVertexSize + uvOffset + 1];

	auto u1 = uvData[indicesData[triangle + 1] * uvVertexSize + uvOffset];
	auto v1 = uvData[indicesData[triangle + 1] * uvVertexSize + uvOffset + 1];

	auto u2 = uvData[indicesData[triangle + 2] * uvVertexSize + uvOffset];
	auto v2 = uvData[indicesData[triangle + 2] * uvVertexSize + uvOffset + 1];

	auto z = 1.f - lambda.x - lambda.y;

	hitUv = Vector2(
		z * u0 + lambda.x * u1 + lambda.y * u2,
		z * v0 + lambda.x * v1 + lambda.y * v2
	);
}

void
Geometry::getHitNormal(uint triangle, Vector3& hitNormal)
{
	auto normalBuffer = vertexBuffer("normal");
	auto& normalData = normalBuffer->data();
	auto normalPtr = &normalData[0];
	auto normalVertexSize = normalBuffer->vertexSize();
	auto normalOffset = std::get<2>(*normalBuffer->attribute("normal"));
	auto& indicesData = _indexBuffer->data();

	auto v0 = math::make_vec3(normalPtr + indicesData[triangle] * normalVertexSize + normalOffset);
	auto v1 = math::make_vec3(normalPtr + indicesData[triangle] * normalVertexSize + normalOffset);
	auto v2 = math::make_vec3(normalPtr + indicesData[triangle] * normalVertexSize + normalOffset);

	auto edge1 = math::normalize(v1 - v0);
	auto edge2 = math::normalize(v2 - v0);

	hitNormal = math::cross(edge2, edge1);
}

void
Geometry::upload()
{
	for (const auto& vb : _vertexBuffers)
		vb->upload();

	_indexBuffer->upload();
}
