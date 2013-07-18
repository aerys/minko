#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"

#include "GLFW/glfw3.h"

using namespace minko::component;
using namespace minko::math;

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
    auto window = glfwCreateWindow(800, 600, "Minko - Cube Example", NULL, NULL);
    glfwMakeContextCurrent(window);

	auto context = render::OpenGLES2Context::create();
	auto assets	= AssetsLibrary::create(context)
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(context))
        ->queue("texture/box.png")
		->queue("Basic.effect");

	assets->defaultOptions()->includePaths().insert(MINKO_FRAMEWORK_EFFECTS_PATH);

	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
		auto root   = scene::Node::create("root");

		root->addChild(group)->addChild(camera);

        renderingComponent = Rendering::create(assets->context());
        renderingComponent->backgroundColor(0x7F7F7FFF);
		camera->addComponent(renderingComponent);
        camera->addComponent(Transform::create());
        camera->component<Transform>()->transform()
            ->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f));
        camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));

        group->addChild(mesh);

		mesh->addComponent(Transform::create());
		mesh->addComponent(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material.diffuseColor",	Vector4::create(0.f, 0.f, 1.f, 1.f))
				->set("material.diffuseMap",	assets->texture("texture/box.png")),
			assets->effect("basic")
		));
	});

	assets->load();

	while(!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            camera->component<Transform>()->transform()->prependTranslation(0.f, 0.f, -.1f);
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            camera->component<Transform>()->transform()->prependTranslation(0.f, 0.f, .1f);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            camera->component<Transform>()->transform()->appendTranslation(-.1f, 0.f, 0.f);
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            camera->component<Transform>()->transform()->appendTranslation(.1f, 0.f, 0.f);
        
		mesh->component<Transform>()->transform()->prependRotationY(.01f);

	    renderingComponent->render();

	    printFramerate();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();

    exit(EXIT_SUCCESS);
}