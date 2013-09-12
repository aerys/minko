#include "ParticlesGeometry.hpp"

#include "minko/render/ParticleVertexBuffer.hpp"
#include "minko/render/ParticleIndexBuffer.hpp"

using namespace minko;
using namespace minko::geometry;

void
ParticlesGeometry::initialize(std::shared_ptr<render::AbstractContext> context)
{
	_vertices = render::ParticleVertexBuffer::create(context);
	_indices = render::ParticleIndexBuffer::create(context);	
	
	addVertexBuffer(_vertices);
	indices(_indices);
}

void
ParticlesGeometry::initStreams(unsigned int maxParticles)
{
	if (maxParticles == 0)
		return;
	
	_vertices->resize(maxParticles, vertexSize());
	_indices->resize(maxParticles);
}
