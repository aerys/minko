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

#pragma once

#include "minko/Common.hpp"

#include "minko/MinkoParticles.hpp"
#include "minko/AssetsLibrary.hpp"
#include "minko/math/Vector4.hpp"

namespace minko
{
    namespace component
    {
	    class Fire :
            public ParticleSystem
	    {
	    public:
		    typedef std::shared_ptr<Fire> Ptr;

	    public:
		    inline static
		    Ptr
		    create(std::shared_ptr<AssetsLibrary> assets)
		    {
                auto fire = std::shared_ptr<Fire>(new Fire(assets));

                fire->initialize();

			    return fire;
		    }

	    private:
		    Fire(std::shared_ptr<AssetsLibrary> assets) :
                ParticleSystem(
                    assets->context(),
		            assets,
		            200,
		            particle::sampler::RandomValue<float>::create(1, 1.3),
		            particle::shape::Sphere::create(1 * 0.05),
		            particle::StartDirection::NONE,
		            0
                )
		    {
                const float SCALE = 0.05;

                material()
		            ->set("material.diffuseColor",	math::Vector4::create(.3f, .07f, .02f, 1.f))
		            ->set("material.diffuseMap",	assets->texture("texture/firefull.jpg"));

               add(particle::modifier::StartForce::create(
		            particle::sampler::RandomValue<float>::create(-.2 * SCALE, .2 * SCALE),
		            particle::sampler::RandomValue<float>::create(6. * SCALE, 8. * SCALE),
		            particle::sampler::RandomValue<float>::create(-.2 * SCALE, .2 * SCALE)
	           ));

	           add(particle::modifier::StartSize::create(
		            particle::sampler::RandomValue<float>::create(1.3 * SCALE, 1.6 * SCALE)
	           ));

	           add(particle::modifier::StartSprite::create(
		            particle::sampler::RandomValue<float>::create(0. * SCALE, 4. * SCALE)
	           ));

	           add(particle::modifier::StartAngularVelocity::create(
		            particle::sampler::RandomValue<float>::create(0.1 * SCALE, 2. * SCALE)
	           ));

	           add(particle::modifier::SizeOverTime::create());
	           add(particle::modifier::ColorOverTime::create());

	           updateRate(30);
	           fastForward(2, 30);
	           play();
		    }
	    };
    }
}
