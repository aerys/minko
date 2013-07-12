#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoMk.hpp"

#include "GLFW/glfw3.h"

#define FRAMERATE 60

using namespace minko::controller;
using namespace minko::math;

RenderingController::Ptr renderingController;
auto mesh	= scene::Node::create("mesh");
auto group	= scene::Node::create("group");

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
		->registerParser<file::EffectParser>("effect")
		->registerParser<file::JPEGParser>("jpg")
		->registerParser<file::PNGParser>("png")
		->registerParser<file::MkParser>("mk")
		->geometry("cube", geometry::CubeGeometry::create(context))
		->geometry("sphere", geometry::SphereGeometry::create(context, 40))
		->queue("collage.jpg")
        ->queue("box3.png")
		->queue("DirectionalLight.effect")
		->queue("VertexNormal.effect")
		->queue("Texture.effect")
		->queue("Red.effect")
		->queue("Basic.effect")
		->queue("models/sponza.mk");

	assets->defaultOptions()->includePaths().push_back("effects");
	assets->defaultOptions()->includePaths().push_back("textures");

	std::vector<unsigned short> t(42);
	auto ib = resource::IndexStream::create(context, t);

	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
		auto camera	= scene::Node::create("camera");
		auto root   = scene::Node::create("root");

		root->addChild(group)->addChild(camera);

        renderingController = RenderingController::create(assets->context());
        renderingController->backgroundColor(0x7F7F7FFF);
		camera->addController(renderingController);

		group->addController(TransformController::create());
		group->controller<TransformController>()->transform()->appendTranslation(0.f, -4.f, -0.f);
		group->controller<TransformController>()->transform()->prependRotationY(3.14/4);

		group->addChild(assets->node("models/sponza.mk"));
	});

	assets->load();

	while(!glfwWindowShouldClose(window))
    {
		group->controller<TransformController>()->transform()->prependRotationY(.01f);
	    renderingController->render();

	    printFramerate();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();

    exit(EXIT_SUCCESS);
}