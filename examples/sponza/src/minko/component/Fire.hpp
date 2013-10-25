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
#include "minko/file/AssetLibrary.hpp"
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
		    create(std::shared_ptr<file::AssetLibrary> assets)
		    {
                auto fire = std::shared_ptr<Fire>(new Fire(assets));

                fire->initialize();

			    return fire;
		    }

	    private:
		    Fire(std::shared_ptr<file::AssetLibrary> assets) :
                ParticleSystem(
                    assets->context(),
		            assets,
		            200,
		            particle::sampler::RandomValue<float>::create(1, 1.3f),
		            particle::shape::Sphere::create(0.05f),
		            particle::StartDirection::NONE,
		            0
                )
		    {
                const float SCALE = 0.05f;

                material()
		            ->set("material.diffuseColor",	math::Vector4::create(.3f, .07f, .02f, 1.f))
		            ->set("material.diffuseMap",	assets->texture("texture/firefull.jpg"));

               add(particle::modifier::StartForce::create(
		            particle::sampler::RandomValue<float>::create(-.2f * SCALE, .2f * SCALE),
		            particle::sampler::RandomValue<float>::create(6.f * SCALE, 8.f * SCALE),
		            particle::sampler::RandomValue<float>::create(-.2f * SCALE, .2f * SCALE)
	           ));

	           add(particle::modifier::StartSize::create(
		            particle::sampler::RandomValue<float>::create(1.3f * SCALE, 1.6f * SCALE)
	           ));

	           add(particle::modifier::StartSprite::create(
		            particle::sampler::RandomValue<float>::create(0.f * SCALE, 4.f * SCALE)
	           ));

	           add(particle::modifier::StartAngularVelocity::create(
		            particle::sampler::RandomValue<float>::create(0.1f * SCALE, 2.f * SCALE)
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
