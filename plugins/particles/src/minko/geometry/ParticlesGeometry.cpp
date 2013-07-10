#include "ParticlesGeometry.hpp"

#include "minko/render/VertexStream.hpp"
#include "minko/render/IndexStream.hpp"

using namespace minko::geometry;

ParticlesGeometry::ParticlesGeometry(std::shared_ptr<render::AbstractContext> context)
	: Geometry ()
{
	_vertices = render::VertexStream::create(context);
	_indices = render::IndexStream::create(context, std::vector<unsigned short> (6));
	
	_vertices->addAttribute("offset", 2, 0);
	_vertices->addAttribute("position", 3, 2);

	initStreams(0);

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
		data()->set("geometry/vertex/attribute/" + std::get<0>(*attribute), _vertices);
		vertexSize(vertexSize() + std::get<1>(*attribute));
	}

	data()->set("geometry/vertex/size", vertexSize());

	_vertices->data().resize(vertexSize() * 4 * maxParticles);
	
	std::vector<float>& vsData = _vertices->data();

	for (unsigned int i = 0; i < maxParticles; ++i)
	{
		vsData[i * vertexSize() * 4] = -0.5;
		vsData[i * vertexSize() * 4 + 1] = -0.5;

		vsData[vertexSize() + i * vertexSize() * 4] = 0.5;
		vsData[vertexSize() + i * vertexSize() * 4 + 1] = -0.5;

		vsData[2 * vertexSize() + i * vertexSize() * 4] = -0.5;
		vsData[2 * vertexSize() + i * vertexSize() * 4 + 1] = 0.5;

		vsData[3 * vertexSize() + i * vertexSize() * 4] = 0.5;
		vsData[3 * vertexSize() + i * vertexSize() * 4 + 1] = 0.5;
	}
	
	_vertices->upload();
	
	std::vector<unsigned short>& isData = _indices->data();
	if (isData.size() != maxParticles * 6)
	{
		if (isData.size() < maxParticles * 6)
		{
			isData.resize(maxParticles * 6);
			for (unsigned int i = 0; i < maxParticles; ++i)
			{
				isData[i * 6] = i * 4;
				isData[i * 6 + 1] = i * 4 + 2;
				isData[i * 6 + 2] = i * 4 + 1;
				isData[i * 6 + 3] = i * 4 + 1;
				isData[i * 6 + 4] = i * 4 + 2; 
				isData[i * 6 + 5] = i * 4 + 3;
			}
		}
		else
			isData.resize(maxParticles * 6);

		_indices->upload();
	}
}