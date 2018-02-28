#include "minko/geometry/PointGeometry.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/IndexBuffer.hpp"

using namespace minko;
using namespace minko::geometry;
using namespace minko::render;

PointGeometry::PointGeometry() :
    Geometry("point"),
    _numPoints(0),
    _vertexBuffer(),
    _indexBuffer()
{
}

PointGeometry::Ptr
PointGeometry::addPoint(const math::vec3& position)
{
    ++_numPoints;

    const auto currentIndex = _vertexBuffer->numVertices();

    auto& indexData = _indexBuffer->data();
    auto newIndexData = std::vector<unsigned short> {
        0, 1, 2, 0, 2, 3
    };
    std::transform(
        newIndexData.begin(), newIndexData.end(), newIndexData.begin(),
        [&currentIndex](int index) { return currentIndex + index; }
    );
    indexData.insert(indexData.end(), newIndexData.begin(), newIndexData.end());

    static const auto halfSize = .5f;
    auto& vertexData = _vertexBuffer->data();
    auto newVertexData = std::vector<float> {
        position.x - halfSize, position.y - halfSize, position.z, 0.f, 0.f,
        position.x - halfSize, position.y + halfSize, position.z, 0.f, 1.f,
        position.x + halfSize, position.y + halfSize, position.z, 1.f, 1.f,
        position.x + halfSize, position.y - halfSize, position.z, 0.f, 1.f
    };
    vertexData.insert(vertexData.end(), newVertexData.begin(), newVertexData.end());

    return std::static_pointer_cast<PointGeometry>(shared_from_this());
}

PointGeometry::Ptr
PointGeometry::clearPoints()
{
    _numPoints = 0;

    _vertexBuffer->disposeData();
    _indexBuffer->disposeData();

    return std::static_pointer_cast<PointGeometry>(shared_from_this());
}

void
PointGeometry::upload()
{
    _indexBuffer->upload();
    _vertexBuffer->upload();

    if (!hasVertexBuffer(_vertexBuffer))
        addVertexBuffer(_vertexBuffer);
    if (indices() != _indexBuffer)
        indices(_indexBuffer);
}

void
PointGeometry::initialize(AbstractContext::Ptr context)
{
    _vertexBuffer = VertexBuffer::create(context);
    _vertexBuffer->addAttribute("position", 3, 0);
    _vertexBuffer->addAttribute("uv", 2, 3);

    _indexBuffer = IndexBuffer::create(context);
}
