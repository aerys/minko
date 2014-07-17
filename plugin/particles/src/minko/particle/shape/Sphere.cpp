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

#include "minko/particle/shape/Sphere.hpp"
#include "minko/particle/ParticleData.hpp"

using namespace minko;
using namespace minko::particle;
using namespace minko::particle::shape;

Sphere::Sphere(float radius,
		   	   float innerRadius) :
    EmitterShape(),
    _radius (radius),
    _innerRadius (innerRadius)
{
}
	
void
Sphere::initPosition(ParticleData& particle) const
{
	float u			= tools::rand01();
	float sqrt1mu2	= sqrtf(1.0f - u * u);
	
	float theta		= (tools::rand01() * 2.0f - 1.0f) * float(M_PI);
	
	float cosTheta	= cosf(theta);
	float sinTheta	= sinf(theta);
	
	float r			= _innerRadius + sqrt(tools::rand01()) * (_radius - _innerRadius);
	
	r = tools::rand01() > 0.5f ? r : -r;

	particle.x = r * sqrt1mu2 * cosTheta;
	particle.y = r * sqrt1mu2 * sinTheta;
	particle.z = r * u;
}
