#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoParticles.hpp"

#include "GLFW/glfw3.h"

#define FRAMERATE 60

using namespace minko::component;
using namespace minko::math;

ParticleSystem::Ptr particleSystem;
Rendering::Ptr renderingComponent;

auto mesh = scene::Node::create("mesh");
auto group = scene::Node::create("group");
auto camera	= scene::Node::create("camera");

void
printFramerate(const unsigned int delay = 1)
{
	static auto start = clock();
	static auto numFrames = 0;

	auto time = clock();
	auto deltaT = (float)(clock() - start) / CLOCKS_PER_SEC;

	++numFrames;
	if (deltaT > delay)
	{
		std::cout << ((float)numFrames / deltaT) << " fps." << std::endl;
		start = time;
		numFrames = 0;
	}
}

int main(int argc, char** argv)
{
    glfwInit();
    auto window = glfwCreateWindow(800, 600, "Minko Examples", NULL, NULL);
    glfwMakeContextCurrent(window);

	auto context = render::OpenGLES2Context::create();
	auto assets	= AssetsLibrary::create(context)
		->registerParser<file::JPEGParser>("jpg")
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(context))
		//->geometry("sphere", geometry::SphereGeometry::create(context, 40))
		->queue("collage.jpg")
        ->queue("box3.png")
        ->queue("firefull.jpg")
		->queue("DirectionalLight.effect")
		//->queue("VertexNormal.effect")
		->queue("Texture.effect")
		->queue("Red.effect")
		->queue("Basic.effect")
		->queue("Particles.effect");
	
#ifdef DEBUG
	assets->defaultOptions()->includePaths().push_back("effect");
	assets->defaultOptions()->includePaths().push_back("texture");
#else
	assets->defaultOptions()->includePaths().push_back("../../effect");
	assets->defaultOptions()->includePaths().push_back("../../texture");
#endif

	auto _ = assets->complete()->connect([context](AssetsLibrary::Ptr assets)
	{
		auto root   = scene::Node::create("root");
		

		root->addChild(group)->addChild(camera);

        renderingComponent = Rendering::create(assets->context());
        renderingComponent->backgroundColor(0x000000FF);
		camera->addComponent(renderingComponent);
        camera->addComponent(Transform::create());
        camera->component<Transform>()->transform()
            ->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f));
        camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));


		mesh->addComponent(Transform::create());
		mesh->component<Transform>()->transform()
			->appendRotationZ(15)
			->appendTranslation(0.f, 0.f, -30.f);
		
		group->addChild(mesh);

		particleSystem = ParticleSystem::create(
			context,
			assets,
			6000,
			particle::sampler::RandomValue<float>::create(0.2, 0.8),
			particle::shape::Cylinder::create(1., 5., 5.),
			particle::StartDirection::NONE,
			0);

		particleSystem->isInWorldSpace(true);
		particleSystem->material()
				->set("material.diffuseColor",	Vector4::create(.8f, .3f, .1f, 1.f))
                ->set("material.diffuseMap",	assets->texture("firefull.jpg"));

		particleSystem->add(particle::modifier::StartForce::create(
			particle::sampler::RandomValue<float>::create(-2., 2.),
			particle::sampler::RandomValue<float>::create(8., 10.),
			particle::sampler::RandomValue<float>::create(-2., 2.)
			));

		particleSystem->add(particle::modifier::StartSize::create(
			particle::sampler::RandomValue<float>::create(0.1, .8)
			));

		particleSystem->add(particle::modifier::StartSprite::create(
			particle::sampler::RandomValue<float>::create(0., 4.)
			));

		particleSystem->add(particle::modifier::StartAngularVelocity::create(
			particle::sampler::RandomValue<float>::create(1., 5.)
			));

		mesh->addComponent(particleSystem);
		particleSystem->updateRate(60);
		particleSystem->play();
	});

	assets->load();

	while(!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            camera->component<Transform>()->transform()->appendTranslation(0.f, 0.f, -.1f);
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            camera->component<Transform>()->transform()->appendTranslation(0.f, 0.f, .1f);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            camera->component<Transform>()->transform()->appendTranslation(-.1f, 0.f, 0.f);
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            camera->component<Transform>()->transform()->appendTranslation(.1f, 0.f, 0.f);
		
		mesh->component<Transform>()->transform()->prependRotationX(.01);

	    renderingComponent->render();

	    printFramerate();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
