#include "ParticlesGeometry.hpp"

#include "minko/render/ParticleVertexBuffer.hpp"
#include "minko/render/ParticleIndexBuffer.hpp"

using namespace minko::geometry;

ParticlesGeometry::ParticlesGeometry(std::shared_ptr<render::AbstractContext> context)
	: Geometry ()
{
	_vertices = render::ParticleVertexBuffer::create(context);
	_indices = render::ParticleIndexBuffer::create(context);

	indices(_indices);
}


void
ParticlesGeometry::initStreams(unsigned int maxParticles)
{
	if (maxParticles == 0)
		return;

	vertexSize(0);

	for (auto attribute : _vertices->attributes())
	{
		data()->set("geometry.vertex.attribute." + std::get<0>(*attribute),
					std::static_pointer_cast<render::VertexBuffer>(_vertices));
		vertexSize(vertexSize() + std::get<1>(*attribute));
	}

	data()->set("geometry.vertex.size", vertexSize());
	
	_vertices->resize(maxParticles, vertexSize());
	_indices->resize(maxParticles);
}