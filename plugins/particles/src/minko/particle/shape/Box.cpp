#include "minko/particle/shape/Box.hpp"
#include "minko/particle/ParticleData.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::shape;

Box::Box(float	width,
		 float 	height,
		 float	length, 
		 bool 	limitToSides)
	: _width (width),
	  _height (height),
	  _length (length),
	  _limitToSides (limitToSides)
{}

void
Box::initPosition(ParticleData& particle) const
{
	if (_limitToSides)
	{
		particle.x = rand01() < 0.5 ? -_width / 2: _width / 2;
		particle.y = rand01() < 0.5 ? -_height / 2: _height / 2;
		particle.z = rand01() < 0.5 ? -_length / 2: _length / 2;
	}
	else
	{
		particle.x = rand01() * _width -_width / 2;
		particle.y = rand01() * _height -_height / 2;
		particle.z = rand01() * _length -_length / 2;
	}
}

