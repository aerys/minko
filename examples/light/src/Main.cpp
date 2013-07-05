#include <time.h>
#include <exception>
#include <iostream>

#include "minko/Minko.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoWebGL.hpp"


#ifdef EMSCRIPTEN
	#include "GL/glut.h"
#else
	#include "GLFW/glfw3.h"
#endif

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

#ifdef EMSCRIPTEN
void
renderScene()
{
	mesh->controller<TransformController>()->transform()->prependRotationY(.01f);
	renderingController->render();

	//printFramerate();

	glutSwapBuffers();
	glutPostRedisplay();

}

void timerFunc(int)
{
	glutTimerFunc(1000 / FRAMERATE, timerFunc, 0);
	glutPostRedisplay();
}
#endif

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
std::cout << "Starting example" << std::endl;
#ifdef EMSCRIPTEN
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Minko Examples");

	auto context = render::WebGLContext::create();
#else
    glfwInit();
    auto window = glfwCreateWindow(800, 600, "Minko Examples", NULL, NULL);
    glfwMakeContextCurrent(window);

	auto context = render::OpenGLES2Context::create();
#endif
	
    context->setBlendMode(render::Blending::Mode::DEFAULT);

	auto assets	= AssetsLibrary::create(context)
		->registerParser<file::JPEGParser>("jpg")
        	->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(context))
		->geometry("sphere", geometry::SphereGeometry::create(context, 40))
		->queue("collage.jpg")
        ->queue("box3.png")
		->queue("DirectionalLight.effect")
		//->queue("VertexNormal.effect")
		//->queue("Texture.effect")
		//->queue("Red.effect")
		->queue("Basic.effect");

#ifdef DEBUG
	assets->defaultOptions()->includePaths().push_back("effect");
	assets->defaultOptions()->includePaths().push_back("texture");
#else
	assets->defaultOptions()->includePaths().push_back("../../effect");
	assets->defaultOptions()->includePaths().push_back("../../texture");
#endif

	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
		std::cout << "load complete" << std::endl;
		auto camera	= scene::Node::create("camera");
		auto root   = scene::Node::create("root");
		
		root->addChild(group)->addChild(camera);

        renderingComponent = Rendering::create(assets->context());
        renderingComponent->backgroundColor(0x7F7F7FFF);
		camera->addComponent(renderingComponent);
        camera->addComponent(Transform::create());
        camera->component<Transform>()->transform()
            ->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f));
        camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));

        root->addComponent(DirectionalLight::create());

        group->addChild(mesh);

		mesh->addComponent(Transform::create());
		mesh->addComponent(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material.diffuseColor",	Vector4::create(0.f, 0.f, 1.f, 1.f))
                ->set("material.diffuseMap",	assets->texture("box3.png"))
                ->set("material.specular",	    Vector3::create(.25f, .25f, .25f))
                ->set("material.shininess",	    30.f),
			assets->effect("directional light")
		));
	});
	assets->load();

#ifdef EMSCRIPTEN
	glutDisplayFunc(renderScene);
	glutMainLoop();
	return 0;
#else
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

        //group->component<Transform>()->transform()->appendRotationY(.01f);
        //camera->component<Transform>()->transform()->appendRotationY(0.01f);
        mesh->component<Transform>()->transform()->prependRotationY(.01f);

	    renderingComponent->render();

	    printFramerate();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
 
    glfwTerminate();

    exit(EXIT_SUCCESS);
#endif
}
