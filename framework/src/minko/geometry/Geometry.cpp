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

#include "minko/geometry/Geometry.hpp"

#include "minko/math/Ray.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/VertexAttribute.hpp"

using namespace minko;
using namespace minko::geometry;
using namespace minko::render;

Geometry::Geometry(const std::string& name) :
	_data(data::Provider::create()),
	_vertexSize(0),
	_numVertices(0),
	_indexBuffer(nullptr)
{
    _data->set("name", name);
    _data->set("uuid", _data->uuid());
}

Geometry::Geometry(const Geometry& geometry) :
	_data(geometry._data->clone()),
	_vertexSize(geometry._vertexSize),
	_numVertices(geometry._numVertices),
	_vertexBuffers(geometry._vertexBuffers),
	_indexBuffer(geometry._indexBuffer)
{
}

std::shared_ptr<Geometry>
Geometry::clone()
{
	Ptr geometry(new Geometry(*this));	

	return geometry;
}

void
Geometry::addVertexBuffer(render::VertexBuffer::Ptr vertexBuffer)
{
	if (hasVertexBuffer(vertexBuffer))
		throw std::invalid_argument("vertexBuffer");

	const unsigned bufVertexSize	= vertexBuffer->vertexSize();
	const unsigned bufNumVertices	= vertexBuffer->numVertices();

	for (const auto& attribute : vertexBuffer->attributes())
		_data->set(attribute.name, attribute);
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

    computeCenterPosition();
}

void
Geometry::removeVertexBuffer(std::list<render::VertexBuffer::Ptr>::iterator vertexBufferIt)
{
	VertexBuffer::Ptr vertexBuffer	= *vertexBufferIt;
	vertexBuffer->dispose();

	for (auto attribute : vertexBuffer->attributes())
		_data->unset(*attribute.name);

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

    auto normalBuffer = vertexBuffer("normal");
    // if (normalBuffer)
        // throw std::logic_error("The geometry already stores precomputed normals.");

    auto xyzBuffer = vertexBuffer("position");
	if (!xyzBuffer)
		throw std::logic_error("Computation of normals requires positions.");

	const std::vector<unsigned short>& indices	= this->indices()->data();
	const unsigned int numFaces					= indices.size() / 3;

	unsigned short vertexIds[3] = { 0, 0, 0 };
	std::vector<math::vec3> xyz(3);

	const auto& xyzAttribute			= xyzBuffer->attribute("position");
    const unsigned int xyzSize          = *xyzAttribute.vertexSize; // xyzBuffer->vertexSize();
    const unsigned int xyzOffset        = xyzAttribute.offset;
    const std::vector<float>& xyzData   = xyzBuffer->data();

    unsigned int normalSize;
    unsigned int normalOffset;
    std::vector<float>* normalsData;

    if (normalBuffer)
    {
        normalsData = &normalBuffer->data();
	    const auto& normalAttribute = normalBuffer->attribute("normal");
        normalSize = *normalAttribute.vertexSize;
        normalOffset = normalAttribute.offset;
    }
    else
    {
        normalsData = new std::vector<float>(3 * numVertices, 0.0f);
        normalSize = 3;
        normalOffset = 0;
    }

    for (auto i = 0u; i < numVertices; ++i)
    {
        const auto index = normalOffset + i * normalSize;

        (*normalsData)[index] = 0.f;
        (*normalsData)[index + 1] = 0.f;
        (*normalsData)[index + 2] = 0.f;
    }

	for (unsigned int i = 0, offset = 0; i < numFaces; ++i)
	{
		for (unsigned int k = 0; k < 3; ++k)
		{
			vertexIds[k] = indices[offset++];
			const unsigned int index = xyzOffset + vertexIds[k] * xyzSize;
			xyz[k] = { xyzData[index], xyzData[index + 1], xyzData[index + 2] };
		}

		auto faceNormal = math::cross(xyz[0] - xyz[1], xyz[0] - xyz[2]);

 		for (unsigned int k = 0; k < 3; ++k)
		{
			const unsigned int index = normalOffset + normalSize * vertexIds[k];

			(*normalsData)[index] += faceNormal.x;
			(*normalsData)[index + 1] += faceNormal.y;
			(*normalsData)[index + 2] += faceNormal.z;
		}
	}

	for (unsigned int i = 0; i < numVertices; ++i)
	{
        const auto indexOffset = normalOffset + i * normalSize;

		const float x = (*normalsData)[indexOffset];
        const float y = (*normalsData)[indexOffset + 1];
        const float z = (*normalsData)[indexOffset + 2];
		const float lengthSquared = x * x + y * y + z * z;
		const float invLength = lengthSquared > 1e-6f ? 1.0f / sqrtf(lengthSquared) : 1.0f;

        (*normalsData)[indexOffset] *= invLength;
        (*normalsData)[indexOffset + 1] *= invLength;
        (*normalsData)[indexOffset + 2] *= invLength;
	}

    if (!normalBuffer)
    {
        normalBuffer = VertexBuffer::create(xyzBuffer->context(), *normalsData);
        normalBuffer->addAttribute("normal", normalSize, normalOffset);
        addVertexBuffer(normalBuffer);

        delete normalsData;
    }

	return shared_from_this();
}

Geometry::Ptr
Geometry::computeTangentSpace(bool doNormals)
{
	const unsigned int numVertices = this->numVertices();

	if (numVertices == 0)
		return shared_from_this();


    auto xyzBuffer = vertexBuffer("position");
    if (!xyzBuffer)
        throw std::logic_error("Computation of tangent space requires positions.");

    auto uvBuffer = vertexBuffer("uv");
    if (!uvBuffer)
		throw std::logic_error("Computation of tangent space requires uvs.");

	if (doNormals)
		computeNormals();

	const std::vector<unsigned short>& indices (this->indices()->data());
	const unsigned int numFaces = indices.size() / 3;

	unsigned short vertexIds[3] = { 0, 0, 0 };
	std::vector<math::vec3> xyz(3);
	std::vector<math::vec2> uv(3);

	const unsigned int xyzSize			= xyzBuffer->vertexSize();
    const unsigned int xyzOffset        = xyzBuffer->attribute("position").offset;
	const std::vector<float>& xyzData	= xyzBuffer->data();

	const unsigned int uvSize			= uvBuffer->vertexSize();
	const unsigned int uvOffset			= uvBuffer->attribute("uv").offset;
	const std::vector<float>& uvData	= uvBuffer->data();

	std::vector<float> tangentsData(3 * numVertices, 0.0f);

	for (unsigned int i = 0, offset = 0; i < numFaces; ++i)
	{
		for (unsigned int k = 0; k < 3; ++k)
		{
			vertexIds[k] = indices[offset++];
			unsigned int index = xyzOffset + vertexIds[k] * xyzSize;
			xyz[k] = { xyzData[index], xyzData[index + 1], xyzData[index + 2] };
			index = uvOffset + vertexIds[k] * uvSize;
			uv[k] = { uvData[index], uvData[index + 1] };
		}

		auto uv02		= uv[0] - uv[2];
		auto uv12		= uv[1] - uv[2];
		const float denom		= uv02.x * uv12.y - uv12.x * uv02.y;
		const float invDenom	= fabsf(denom) > 1e-6f ? 1.f / denom : 1.f;

		math::vec3 faceTangent = ((xyz[0] - xyz[2]) * uv12.y - (xyz[1] - xyz[2]) * uv02.y) * invDenom;

		for (unsigned int k=0; k<3; ++k)
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

Geometry::Ptr
Geometry::computeCenterPosition()
{
    const unsigned int numVertices = this->numVertices();

    if (numVertices == 0)
        return shared_from_this();

    auto xyzBuffer = vertexBuffer("position");
    if (!xyzBuffer)
        return shared_from_this();

    const auto& xyzAttr = xyzBuffer->attribute("position");
    const unsigned int xyzOffset = xyzAttr.offset;
    const unsigned int xyzSize = std::max(0u, std::min(3u, xyzAttr.size));
    const std::vector<float>& xyzData = xyzBuffer->data();

    float minXYZ[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
    float maxXYZ[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    unsigned int vertexIndex = xyzOffset;
    while (vertexIndex < xyzData.size())
    {
        for (unsigned int k = 0; k < xyzSize; ++k)
        {
            const float vk = xyzData[vertexIndex + k];

            minXYZ[k] = std::min(minXYZ[k], vk);
            maxXYZ[k] = std::max(maxXYZ[k], vk);
        }

        vertexIndex += xyzBuffer->vertexSize();
    }

    auto minPosition = math::vec3(minXYZ[0], minXYZ[1], minXYZ[2]);
    auto maxPosition = math::vec3(maxXYZ[0], maxXYZ[1], maxXYZ[2]);

    auto centerPosition = (minPosition + maxPosition) * .5f;

    _data->set("centerPosition", centerPosition);

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
			   math::vec3*					hitXyz,
			   math::vec2*					hitUv,
			   math::vec3*					hitNormal)
{
	static const auto EPSILON = 0.00001f;

	auto hit = false;
	auto& indicesData = _indexBuffer->data();
	auto numIndices = indicesData.size();

	auto xyzBuffer = vertexBuffer("position");
	auto& xyzData = xyzBuffer->data();
	auto xyzPtr = &xyzData[0];
	auto xyzVertexSize = xyzBuffer->vertexSize();
	auto xyzOffset = xyzBuffer->attribute("position").offset;

	auto minDistance = std::numeric_limits<float>::infinity();
	auto lambda = math::vec2(0.f);
	auto triangleIndice = -3;

	math::vec3 v0;
	math::vec3 v1;
	math::vec3 v2;
	math::vec3 edge1;
	math::vec3 edge2;
	math::vec3 pvec;
	math::vec3 tvec;
	math::vec3 qvec;

	auto dot = 0.f;
	auto invDot = 0.f;
	auto u = 0.f;
	auto v = 0.f;
	auto t = 0.f;

	for (uint i = 0; i < numIndices; i += 3)
	{
		v0 = math::make_vec3(xyzPtr + indicesData[i] * xyzVertexSize);
		v1 = math::make_vec3(xyzPtr + indicesData[i + 1] * xyzVertexSize);
		v2 = math::make_vec3(xyzPtr + indicesData[i + 2] * xyzVertexSize);

		edge1 = v1 - v0;
		edge2 = v2 - v0;

		pvec = math::cross(ray->direction(), edge2);
		dot = math::dot(edge1, pvec);

		if (dot > -EPSILON && dot < EPSILON)
			continue;

		invDot = 1.f / dot;

		tvec = ray->origin() - v0;
		u = math::dot(tvec, pvec) * invDot;
		if (u < 0.f || u > 1.f)
			continue;

		qvec = math::cross(tvec, edge1);
		v = math::dot(ray->direction(), qvec) * invDot;
		if (v < 0.f || u + v > 1.f)
			continue;

		t = math::dot(edge2, qvec) * invDot;
		if (t < minDistance && t > 0)
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

			if (hitXyz)
			{
				*hitXyz = {
					ray->origin().x + minDistance * ray->direction().x,
					ray->origin().y + minDistance * ray->direction().y,
					ray->origin().z + minDistance * ray->direction().z					
				};
			}
		}

		if (hitUv)
			getHitUv(triangle, lambda, hitUv);

		if (hitNormal)
			getHitNormal(triangle, hitNormal);
	}

	return hit;
}

void
Geometry::getHitUv(uint triangle, math::vec2& lambda, math::vec2* hitUv)
{
	auto uvBuffer = vertexBuffer("uv");
	auto& uvData = uvBuffer->data();
	auto uvPtr = &uvData[0];
	auto uvVertexSize = uvBuffer->vertexSize();
	auto uvOffset = uvBuffer->attribute("uv").offset;
	auto& indicesData = _indexBuffer->data();

	auto u0 = uvData[indicesData[triangle] * uvVertexSize + uvOffset];
	auto v0 = uvData[indicesData[triangle] * uvVertexSize + uvOffset + 1];

	auto u1 = uvData[indicesData[triangle + 1] * uvVertexSize + uvOffset];
	auto v1 = uvData[indicesData[triangle + 1] * uvVertexSize + uvOffset + 1];

	auto u2 = uvData[indicesData[triangle + 2] * uvVertexSize + uvOffset];
	auto v2 = uvData[indicesData[triangle + 2] * uvVertexSize + uvOffset + 1];

	auto z = 1.f - lambda.x - lambda.y;

	*hitUv = {
		z * u0 + lambda.x * u1 + lambda.y * u2,
		z * v0 + lambda.x * v1 + lambda.y * v2
	};
}

void
Geometry::getHitNormal(uint triangle, math::vec3* hitNormal)
{
	auto normalBuffer = vertexBuffer("normal");
	auto& normalData = normalBuffer->data();
	auto normalPtr = &normalData[0];
	auto normalVertexSize = normalBuffer->vertexSize();
	auto normalOffset = normalBuffer->attribute("normal").offset;
	auto& indicesData = _indexBuffer->data();

	auto v0 = math::make_vec3(normalPtr + indicesData[triangle] * normalVertexSize + normalOffset);
	auto v1 = math::make_vec3(normalPtr + indicesData[triangle] * normalVertexSize + normalOffset);
	auto v2 = math::make_vec3(normalPtr + indicesData[triangle] * normalVertexSize + normalOffset);

	auto edge1 = math::normalize(v1 - v0);
	auto edge2 = math::normalize(v2 - v0);

	*hitNormal = math::cross(edge2, edge1);
}

void
Geometry::upload()
{
	for (const auto& vb : _vertexBuffers)
		vb->upload();

	_indexBuffer->upload();
}

void
Geometry::disposeIndexBufferData()
{
    _indexBuffer->disposeData();
}

void
Geometry::disposeVertexBufferData()
{
    for (auto vertexBuffer : _vertexBuffers)
        vertexBuffer->disposeData();
}

bool
Geometry::equals(Ptr geom) const
{
    bool vertexEquality = _vertexBuffers.size() == geom->_vertexBuffers.size();
    bool indexEquality = _indexBuffer == geom->_indexBuffer;
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

const render::VertexAttribute&
Geometry::getVertexAttribute(const std::string& attributeName) const
{
    for (auto vertexBuffer : _vertexBuffers)
        if (vertexBuffer->hasAttribute(attributeName))
            return vertexBuffer->attribute(attributeName);

    throw std::invalid_argument("attributeName = " + attributeName);
}
