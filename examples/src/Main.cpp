#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoMk.hpp"

#include "GLFW/glfw3.h"

#define FRAMERATE 60

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

void
testMk(AssetsLibrary::Ptr assets)
{
	std::shared_ptr<data::HalfEdgeCollection> halfEdgeCollection = data::HalfEdgeCollection::create(assets->geometry("cube")->indices());
}

int main(int argc, char** argv)
{
    glfwInit();
	GLFWwindow* window = glfwCreateWindow(800, 600, "Minko Examples", NULL, NULL);
    glfwMakeContextCurrent(window);

	auto context = render::OpenGLES2Context::create();
	auto assets	= AssetsLibrary::create(context)
		->registerParser<file::JPEGParser>("jpg")
		->registerParser<file::PNGParser>("png")
		->registerParser<file::MkParser>("mk")
		->geometry("cube", geometry::CubeGeometry::create(context))
		->geometry("sphere", geometry::SphereGeometry::create(context, 40))
        ->queue("box3.png")
		->queue("DirectionalLight.effect")
		->queue("VertexNormal.effect")
		->queue("Texture.effect")
		->queue("Red.effect")
		->queue("Basic.effect")
		->queue("models/model.mk");

	//#ifdef DEBUG
	assets->defaultOptions()->includePaths().push_back("effect");
	assets->defaultOptions()->includePaths().push_back("texture");
	//#else
	//assets->defaultOptions()->includePaths().push_back("../../effect");
	//assets->defaultOptions()->includePaths().push_back("../../texture");
	//#endif

	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
		auto root   = scene::Node::create("root");

		root->addChild(group)->addChild(camera);
		
		renderingComponent = Rendering::create(assets->context());
        renderingComponent->backgroundColor(0x7F7F7FFF);
		camera->addComponent(renderingComponent);
        camera->addComponent(Transform::create());
        camera->component<Transform>()->transform()->appendTranslation(0.f, 2.f, 0.75);
        camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));

        root->addComponent(DirectionalLight::create());
		group->addComponent(Transform::create());
		group->addChild(mesh);
//		mesh->addComponent(Transform::create());
//		mesh->addComponent(Surface::create(
//			assets->geometry("cube"),
//			data::Provider::create()
//				->set("material.diffuseColor",	Vector4::create(0.f, 0.f, 1.f, 1.f))
//                ->set("material.diffuseMap",	assets->texture("box3.png"))
//               ->set("material.specular",	    Vector3::create(.25f, .25f, .25f))
//               ->set("material.shininess",	    30.f),
//			assets->effect("directional light")));

		group->addChild(assets->node("models/model.mk"));
	});

	assets->load();

	while(!glfwWindowShouldClose(window))
    {
		//camera->component<Transform>()->transform()->appendTranslation(0.f, 0.f, 0.01f);
        group->component<Transform>()->transform()->prependRotationY(.01f);
		renderingComponent->render();

	    printFramerate();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();

    exit(EXIT_SUCCESS);
}