#include "ParticlesGeometry.hpp"

#include "minko/render/ParticleVertexStream.hpp"
#include "minko/render/ParticleIndexStream.hpp"

using namespace minko::geometry;

ParticlesGeometry::ParticlesGeometry(std::shared_ptr<render::AbstractContext> context)
	: Geometry ()
{
	_vertices = render::ParticleVertexStream::create(context);
	_indices = render::ParticleIndexStream::create(context);

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
		data()->set("geometry/vertex/attribute/" + std::get<0>(*attribute),
					std::static_pointer_cast<render::VertexStream>(_vertices));
		vertexSize(vertexSize() + std::get<1>(*attribute));
	}

	data()->set("geometry/vertex/size", vertexSize());
	
	_vertices->resize(maxParticles, vertexSize());
	_indices->resize(maxParticles);
}