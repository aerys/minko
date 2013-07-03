#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoDevIL.hpp"

#include "GLFW/glfw3.h"

#define FRAMERATE 60

using namespace minko::controller;
using namespace minko::math;

RenderingController::Ptr renderingController;
auto mesh = scene::Node::create("mesh");
auto group = scene::Node::create("group");

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

/*void screenshotFunc(int)
{
	const int width = 800, height = 600;

	char* pixels = new char[3 * width * height];

	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	int i, j;
	FILE *fp = fopen("screenshot.ppm", "wb");
	fprintf(fp, "P6\n%d %d\n255\n", width, height);

	for (j = 0; j < height; ++j)
	{
		for (i = 0; i < width; ++i)
		{
			static unsigned char color[3];
			color[0] = pixels[(width * j + i) * 3 + 0];
			color[1] = pixels[(width * j + i) * 3 + 1];
			color[2] = pixels[(width * j + i) * 3 + 2];
			(void) fwrite(color, 1, 3, fp);
		}
	}

	fclose(fp);

	delete[] pixels;
}*/

int main(int argc, char** argv)
{
    glfwInit();
    auto window = glfwCreateWindow(800, 600, "Minko Examples", NULL, NULL);
    glfwMakeContextCurrent(window);

	auto context = render::OpenGLES2Context::create();

    context->setBlendMode(render::Blending::Mode::DEFAULT);

	auto assets	= AssetsLibrary::create(context)
		->registerParser<file::EffectParser>("effect")
		->registerParser<file::JPEGParser>("jpg")
        ->registerParser<file::DevILPNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(context))
		//->geometry("sphere", geometry::SphereGeometry::create(context, 40))
		->queue("textures/collage.jpg")
        ->queue("textures/box3.png")
		->queue("effects/Texture.effect")
		//->queue("effects/Red.effect")
		->queue("effects/Basic.effect");

	/*assets->defaultOptions()->includePath("effects");*/

	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
		auto camera	= scene::Node::create("camera");
		auto root   = scene::Node::create("root");

		root->addChild(group)->addChild(camera);

        renderingController = RenderingController::create(assets->context());
        renderingController->backgroundColor(0x7F7F7FFF);
		camera->addController(renderingController);

		auto view = Matrix4x4::create()->perspective(.785f, 800.f / 600.f, .1f, 1000.f);
		auto color = Vector4::create(0.f, 0.f, 1.f, .1f);
		auto lightDirection = Vector3::create(0.f, -1.f, -1.f);

		mesh->addController(TransformController::create());
		mesh->controller<TransformController>()->transform()->appendTranslation(0.f, 0.f, -3.f);
		mesh->addController(SurfaceController::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material/diffuse/rgba",			color)
				->set("transform/worldToScreenMatrix",	view)
				->set("light/direction",				lightDirection)
				->set("material/diffuse/map",			assets->texture("textures/box3.png")),
			assets->effect("texture")
		));

		group->addChild(mesh);

	});

	assets->load();

	// auto oglContext = context;
	// auto fx = assets->effect("directional light");

	// std::cout << "== vertex shader compilation logs ==" << std::endl;
	// std::cout << oglContext->getShaderCompilationLogs(fx->shaders()[0]->vertexShader()) << std::endl;
	// std::cout << "== fragment shader compilation logs ==" << std::endl;
	// std::cout << oglContext->getShaderCompilationLogs(fx->shaders()[0]->fragmentShader()) << std::endl;
	// std::cout << "== program info logs ==" << std::endl;
	// std::cout << oglContext->getProgramInfoLogs(fx->shaders()[0]->program()) << std::endl;

	//glutTimerFunc(1000 / FRAMERATE, timerFunc, 0);
	//glutTimerFunc(1000, screenshotFunc, 0);

	while(!glfwWindowShouldClose(window))
    {
        mesh->controller<TransformController>()->transform()->prependRotationY(.01f);

	    renderingController->render();

	    printFramerate();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();

    exit(EXIT_SUCCESS);
}
