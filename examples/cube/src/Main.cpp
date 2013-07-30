#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"

#include "GLFW/glfw3.h"

using namespace minko::component;
using namespace minko::math;

int main(int argc, char** argv)
{
	glfwInit();
	auto window = glfwCreateWindow(800, 600, "Minko - Cube Example", NULL, NULL);
	glfwMakeContextCurrent(window);

	auto context = render::OpenGLES2Context::create();
    auto renderingComponent = Rendering::create(context);
    auto mesh = scene::Node::create("mesh");
    auto assets	= file::AssetLibrary::create(context)
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(context))
		->queue("texture/box.png")
		->queue("effect/Basic.effect");

#ifdef DEBUG
    assets->defaultOptions()->includePaths().insert("bin/debug");
#endif

    std::cout << context->driverInfo() << std::endl;

    assets->defaultOptions()->generateMipmaps(true);

    auto _ = assets->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		auto root   = scene::Node::create("root");
        auto camera	= scene::Node::create("camera");
		
        renderingComponent->backgroundColor(0x7F7F7FFF);
        camera->addComponent(renderingComponent);
		camera->addComponent(Transform::create());
		camera->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f));
		camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));
        root->addChild(camera);

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

	assets->load();

	while (!glfwWindowShouldClose(window))
	{
		mesh->component<Transform>()->transform()->prependRotationY(.01f);

		renderingComponent->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}