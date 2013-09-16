#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"

#include "GLFW/glfw3.h"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

uint g_frameId = 0;
component::SpotLight::Ptr g_spotLight = nullptr;

void
printFramerate(const unsigned int delay = 1)
{
	static auto start = time(NULL);
	static auto numFrames = 0;

	int secondTime = time(NULL);

	++numFrames;

	if ((secondTime - start) >= 1)
	{
		std::cout << numFrames << " fps." << std::endl;
		start = time(NULL);
		numFrames = 0;
	}
}

int main(int argc, char** argv)
{
	glfwInit();
	auto window = glfwCreateWindow(800, 600, "Minko - Cube Example", NULL, NULL);
	glfwMakeContextCurrent(window);

	auto sceneManager	= SceneManager::create(render::OpenGLES2Context::create());
    auto mesh			= scene::Node::create("mesh");
    auto lightNode		= scene::Node::create("directional light 1");
	auto pointLightNode	= scene::Node::create("plight");
	auto spotLightNode	= scene::Node::create("spotlight");
	auto sphereGeometry	= geometry::SphereGeometry::create(sceneManager->assets()->context(), 32, 16, true);

	sphereGeometry->computeTangentSpace(false);

	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("sphere", sphereGeometry)
		->queue("texture/box.png")
		->queue("texture/normalmap-cells.png")
		//->queue("texture/normalmap-squares.png")
		//->queue("texture/specularmap-squares.png")
		->queue("effect/Phong.effect");

#ifdef DEBUG
    sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#else
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/release");
#endif

	const float boxScale = 3.0f;

    auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		auto root   = scene::Node::create("root");
        auto camera	= scene::Node::create("camera");
 
		root->addComponent(sceneManager);
		
		auto ambientLight = scene::Node::create("ambient light");
		ambientLight->addComponent(AmbientLight::create(0.8f));
		root->addChild(ambientLight);

		// setup directional light
		auto directionalLight = DirectionalLight::create();
		lightNode->addComponent(Transform::create());
		lightNode->addComponent(directionalLight);
		//root->addChild(lightNode);

		// setup directional light 2
		auto lightNode2 = scene::Node::create("light2");
		directionalLight = DirectionalLight::create();
		directionalLight->color()->setTo(1.f, 0.f, 0.f);
		lightNode2->addComponent(directionalLight);
		lightNode2->addComponent(Transform::create());
		lightNode2->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(0.f, -1.f, 1.f));
		root->addChild(lightNode2);

		// setup point light 1
		auto pointLight		= component::PointLight::create();
		pointLight->color()->setTo(0.2f, 0.2f, 1.0f);
		pointLightNode->addComponent(pointLight);
		pointLightNode->addComponent(Transform::create());
		pointLightNode->component<Transform>()->transform()->appendTranslation(boxScale, 0.0f, 0.0f);
		root->addChild(pointLightNode);
		 
		// setup spot light
		/*auto spotLight*/ g_spotLight	= component::SpotLight::create(0.05f*PI, 0.075f*PI);
		g_spotLight->color()->setTo(0.8f, 0.8f, 0.0f);
		spotLightNode->addComponent(g_spotLight);
		spotLightNode->addComponent(Transform::create());
		spotLightNode->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(0.0f, 5.0f, 0.0f), Vector3::create(-1.0, 0.0, 0.0));
		root->addChild(spotLightNode);

		// setup camera
        auto renderingComponent = Renderer::create();
		renderingComponent->backgroundColor(0x7F7F7FFF);
        camera->addComponent(renderingComponent);
		camera->addComponent(Transform::create());
		camera->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(6.f, 6.f, -6.f));
		camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));
        root->addChild(camera);

		// setup mesh
		mesh->addComponent(Transform::create());
		mesh->component<Transform>()->transform()
			->appendScale(boxScale, boxScale, boxScale)
			->appendTranslation(0.0f, 0.f, 0.0f);

		mesh->addComponent(Surface::create(
			assets->geometry("sphere"),
			data::Provider::create()
				->set("material.diffuseColor",	Vector4::create(0.f, 0.f, 1.f, 1.f))
				->set("material.diffuseMap",	assets->texture("texture/box.png"))
				->set("material.normalMap",		assets->texture("texture/normalmap-cells.png"))
				//->set("material.normalMap",		assets->texture("texture/normalmap-squares.png"))
				//->set("material.specularMap",	assets->texture("texture/specularmap-squares.png"))
				->set("material.shininess",		32.f),
			assets->effect("effect/Phong.effect")
		));
		root->addChild(mesh);
	});

	sceneManager->assets()->load();

	while (!glfwWindowShouldClose(window))
	{
		//mesh->component<Transform>()->transform()->prependRotationY(-.01f);
		lightNode->component<Transform>()->transform()->prependRotationY(.01f);
		pointLightNode->component<Transform>()->transform()->appendRotationY(.01f);
		//spotLightNode->component<Transform>()->transform()->appendRotationY(.01f);

		sceneManager->nextFrame();
		printFramerate();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);

}
