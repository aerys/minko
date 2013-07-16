#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"

#include "GLFW/glfw3.h"

#define FRAMERATE 60

using namespace minko::controller;
using namespace minko::math;

RenderingController::Ptr renderingController;
auto mesh = scene::Node::create("mesh");
auto group = scene::Node::create("group");
render::OpenGLES2Context::Ptr context = nullptr;

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

	context = render::OpenGLES2Context::create();
	auto assets	= AssetsLibrary::create(context)
		->registerParser<file::JPEGParser>("jpg")
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(context))
		->geometry("sphere", geometry::SphereGeometry::create(context, 40))
		->queue("collage.jpg")
        ->queue("box3.png")
		->queue("window-diffuse.png")
		->queue("window-normal.png")
		->queue("window-specular.png")
		->queue("DirectionalLight.effect");
		//->queue("VertexNormal.effect")
		//->queue("Texture.effect")
		//->queue("Red.effect")
		//->queue("Basic.effect");

#ifdef DEBUG
	assets->defaultOptions()->includePaths().push_back("effect");
	assets->defaultOptions()->includePaths().push_back("texture");
#else
	assets->defaultOptions()->includePaths().push_back("../../effect");
	assets->defaultOptions()->includePaths().push_back("../../texture");
#endif

	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
		context->compileShader(0);
		std::cout << "compilation log[0]\n----------------\n" 
			<< context->getShaderCompilationLogs(0) 
			<< std::endl;
			
		auto camera	= scene::Node::create("camera");
		auto root   = scene::Node::create("root");

		root->addChild(group)->addChild(camera);

        renderingController = RenderingController::create(assets->context());
        renderingController->backgroundColor(0x7F7F7FFF);
		camera->addController(renderingController);

        auto view = Matrix4x4::create()->perspective(.785f, 800.f / 600.f, .1f, 1000.f)->prependTranslation(0.f, 0.f, -3.f);
		auto color = Vector4::create(0.f, 0.f, 1.f, 1.f);
		auto lightDirection = Vector3::create(-1.f, 0.f, -1.f);

		mesh->addController(Transform::create());
		//mesh->controller<Transform>()->transform()->appendTranslation(0.f, 0.f, -3.f);
		mesh->addController(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material.diffuseColor",			color)
                ->set("material.diffuseMap",			assets->texture("window-diffuse.png"))
				->set("material.specularMap",			assets->texture("window-specular.png"))
				->set("material.normalMap",				assets->texture("window-normal.png"))
                ->set("material.specular",	            Vector3::create(.25f, .25f, .25f))
                ->set("material.shininess",	            30.f)
				->set("transform.worldToScreenMatrix",  view)
				->set("light.ambient",				    Vector3::create(.25f, .25f, .25f))
				->set("light.direction",				lightDirection)
                ->set("light.diffuse",				    Vector3::create(1.f, 1.f, 1.f))
                ->set("light.specular",				    Vector3::create(1.f, 1.f, 1.f))
                ->set("camera.position",				Vector3::create(0., 0., 3.f)),
			assets->effect("directional light")
		));

		//mesh->controller<Surface>()->geometry()->computeNormals();
		mesh->controller<Surface>()->geometry()->data()->unset("geometry.vertex.attribute.normal");
		mesh->controller<Surface>()->geometry()->computeTangentSpace(true);

		group->addChild(mesh);

        /*
		mesh = scene::Node::create();
		mesh->addController(Transform::create());
		mesh->controller<Transform>()->transform()->appendTranslation(-.75f, 0.f, 0.f);
		mesh->addController(Surface::create(
			assets->geometry("sphere"),
			data::Provider::create()
				->set("material/diffuse/rgba",			color)
                ->set("material/phong/exponent",        50.f)
				->set("transform/worldToScreenMatrix",	view)
				->set("light/direction",				lightDirection)
				->set("light/ambient/rgba",				Vector3::create(.25f, .25f, .25f))
				->set("material/diffuse/map",			assets->texture("box3.png")),
			assets->effect("directional light")
		));
        */

		//group->addChild(mesh);

		group->addController(Transform::create());

		/*
		for (auto i = 0; i < 10000; ++i)
		{
			mesh = scene::Node::create();
			mesh->addController(Transform::create());
			mesh->controller<Transform>()->transform()->appendTranslation(0.f, 0.f, -3.f);
			mesh->addController(Surface::create(
				assets->geometry("sphere"),
				data::Provider::create()
					->set("material/diffuse/rgba",			color)
					->set("transform/worldToScreenMatrix",	view)
					->set("light/direction",				lightDirection)
					->set("material/diffuse/map",			assets->texture("box3.png")),
				assets->effect("texture")
			));
			group->addChild(mesh);
		}
		*/
	});

	assets->load();

	//glutTimerFunc(1000 / FRAMERATE, timerFunc, 0);
	//glutTimerFunc(1000, screenshotFunc, 0);

	while(!glfwWindowShouldClose(window))
    {
        //group->controller<Transform>()->transform()->appendRotationY(.01f);
        mesh->controller<Transform>()->transform()->prependRotationY(.01f);

	    renderingController->render();

	    printFramerate();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();

    exit(EXIT_SUCCESS);
}