#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"

#include "GLFW/glfw3.h"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

int main(int argc, char** argv)
{
	glfwInit();
	auto window = glfwCreateWindow(800, 600, "Minko - Cube Example", NULL, NULL);
	glfwMakeContextCurrent(window);

	auto sceneManager = SceneManager::create(render::OpenGLES2Context::create());
    auto mesh = scene::Node::create("mesh");
    auto lightNode = scene::Node::create("directional light 1");

	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->queue("texture/box.png")
		->queue("effect/Phong.effect");

#ifdef DEBUG
    sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#endif

    auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		auto root   = scene::Node::create("root");
        auto camera	= scene::Node::create("camera");
 
		root->addComponent(sceneManager);
		
		auto ambientLight = scene::Node::create("ambient light");
		ambientLight->addComponent(AmbientLight::create());
		root->addChild(ambientLight);

		// setup directional light
		auto directionalLight = DirectionalLight::create();
		lightNode->addComponent(Transform::create());
		lightNode->addComponent(directionalLight);
		root->addChild(lightNode);

		// setup directional light 2
		auto lightNode2 = scene::Node::create("light2");
		directionalLight = DirectionalLight::create();
		directionalLight->color()->setTo(1.f, 0.f, 0.f);
		lightNode2->addComponent(directionalLight);
		lightNode2->addComponent(Transform::create());
		lightNode2->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(0.f, -1.f, 1.f));
		root->addChild(lightNode2);

		// setup camera
        auto renderingComponent = Renderer::create();
		renderingComponent->backgroundColor(0x7F7F7FFF);
        camera->addComponent(renderingComponent);
		camera->addComponent(Transform::create());
		camera->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(3.f, 3.f, -3.f));
		camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));
        root->addChild(camera);

		// setup mesh
		mesh->addComponent(Transform::create());
		mesh->addComponent(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material.diffuseColor",	Vector4::create(0.f, 0.f, 1.f, 1.f))
				->set("material.diffuseMap",	assets->texture("texture/box.png"))
				->set("material.shininess",		32.f),
			assets->effect("effect/Phong.effect")
		));
		root->addChild(mesh);
	});

	sceneManager->assets()->load();

	while (!glfwWindowShouldClose(window))
	{
		mesh->component<Transform>()->transform()->prependRotationY(-.01f);
		lightNode->component<Transform>()->transform()->prependRotationY(.01f);

		sceneManager->nextFrame();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
