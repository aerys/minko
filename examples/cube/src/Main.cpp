#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"

#include "SDL2/SDL.h"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

int main(int argc, char** argv)
{

	if (SDL_Init(SDL_INIT_VIDEO) == -1)
		exit(-1);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window* window = SDL_CreateWindow("Minko - Cube Example",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			800, 600, SDL_WINDOW_OPENGL);

	SDL_GLContext glcontext = SDL_GL_CreateContext(window);

	auto sceneManager = SceneManager::create(render::OpenGLES2Context::create());
    auto mesh = scene::Node::create("mesh");

	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->queue("texture/box.png")
		->queue("effect/Basic.effect");

#ifdef DEBUG
    sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#else 
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/release");
#endif

    auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		auto root   = scene::Node::create("root");
        auto camera	= scene::Node::create("camera");
		
		root->addComponent(sceneManager);

		// setup camera
        auto renderer = Renderer::create();
		renderer->backgroundColor(0x7F7F7FFF);
        camera->addComponent(renderer);
		camera->addComponent(Transform::create());
		camera->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f));
		camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));
        root->addChild(camera);

		// setup mesh
		mesh->addComponent(Transform::create());
		mesh->addComponent(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material.diffuseColor",	Vector4::create(0.f, 0.f, 1.f, 1.f))
				->set("material.diffuseMap",	assets->texture("texture/box.png")),
			assets->effect("effect/Basic.effect")
		));
		root->addChild(mesh);
	});

	sceneManager->assets()->load();

	bool done = false;
	while (!done)
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				done = true;
				break;
			default:
				break;
			}
		}

		mesh->component<Transform>()->transform()->prependRotationY(.01f);

		sceneManager->nextFrame();

		SDL_GL_SwapWindow(window);
	}

	SDL_Quit();

	exit(EXIT_SUCCESS);
}
