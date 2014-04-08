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

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoParticles.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Particles", 800, 600);
	auto sceneManager = SceneManager::create(canvas->context());
    auto assets = sceneManager->assets();
	
	// setup assets
	assets->loader()->options()
        ->resizeSmoothly(true)
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png");
    
    assets->loader()
        ->queue("texture/heal.png")
        ->queue("texture/fire_spritesheet.png")
        ->queue("effect/Basic.effect")
		->queue("effect/Particles.effect");

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto particlesNode = scene::Node::create("particlesNode")
		->addComponent(Transform::create());

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
		    Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f))
		))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, (float)PI * 0.25f, .1f, 1000.f));
	root->addChild(camera);
    
	std::cout << "Control the particle system's emittive state with the keyboard" << std::endl;

	bool toogleEmitting = false;

	auto _ = assets->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
        auto particles = ParticleSystem::create(
            assets,
            100.0f,
            particle::sampler::Constant<float>::create(2.0f),
            particle::shape::Sphere::create(0.5f),
            particle::StartDirection::UP,
            particle::sampler::Constant<float>::create(0.00f)
        );  
        
        auto startcolor	= Vector3::create(1.0f, 0.0f, 0.0f);
        auto endcolor	= Vector3::create(0.0f, 1.0f, 0.0f);

        particles->material()
			->diffuseMap(assets->texture("texture/heal.png"))
			->diffuseColor(0xffffffff);

        particles
            ->add(particle::modifier::StartSize::create(particle::sampler::Constant<float>::create(0.1f)))
            ->add(particle::modifier::SizeOverTime::create(particle::sampler::LinearlyInterpolatedValue<float>::create(1.0, 5.0f, 0.0f, 1.0f)))
            ->add(particle::modifier::VelocityOverTime::create(
                particle::sampler::Constant<float>::create(0.0f),
                particle::sampler::LinearlyInterpolatedValue<float>::create(1.0, 0.0f, 0.0f, 1.0f),
                particle::sampler::Constant<float>::create(0.0f)
            ))
        //->add(particle::modifier::ColorOverTime::create(particle::sampler::LinearlyInterpolatedValue<math::Vector3>::create(*startcolor, *endcolor)))
            ->add(particle::modifier::ColorBySpeed::create(particle::sampler::LinearlyInterpolatedValue<math::Vector3>::create(*startcolor, *endcolor, 0.0f, 1.0f)))
            ->add(particle::modifier::SizeBySpeed::create(particle::sampler::LinearlyInterpolatedValue<float>::create(0.05f, 2.0f)))
        /*->add(particle::modifier::StartRotation::create(particle::sampler::Constant<float>::create(PI * 0.25f)))
        ->add(particle::modifier::StartColor::create(
            particle::sampler::Constant<math::Vector3>::create(*color)
        ))
        ->add(particle::modifier::StartSprite::create(
            particle::sampler::RandomValue<float>::create(0.0f, 4.0f),
            assets->texture("texture/fire_spritesheet.png"), 2, 2))*/
            ->play();

        particlesNode->addComponent(particles);

		root->addChild(particlesNode);
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
	{
		if (toogleEmitting)
		{
			auto withParticles = scene::NodeSet::create(root)
				->descendants(true)
				->where([](scene::Node::Ptr n){ return n->hasComponent<component::ParticleSystem>(); });

			for (auto& n : withParticles->nodes())
			{
				auto particles = n->component<component::ParticleSystem>();

				particles->emitting(!particles->emitting());
			}
		}
		toogleEmitting = false;

		sceneManager->nextFrame(time, deltaTime);
	});

	auto keydown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
	{
		toogleEmitting = true;
	});

	assets->loader()->load();
	canvas->run();

	return 0;
}


