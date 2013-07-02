#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoMk.hpp"


#define FRAMERATE 60


using namespace minko::parser::mk;
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
renderScene()
{
	mesh->controller<TransformController>()->transform()->prependRotationY(.01f);

	renderingController->render();

	printFramerate();

	glutSwapBuffers();
}

void timerFunc(int)
{
	glutTimerFunc(1000 / FRAMERATE, timerFunc, 0);
	glutPostRedisplay();
}

void screenshotFunc(int)
{
	const int width = 800, height = 600;

	char* pixels = new char[3 * width * height];

	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	int i, j;
	FILE *fp = fopen("screenshot.ppm", "wb"); /* b - binary mode */
	fprintf(fp, "P6\n%d %d\n255\n", width, height);

	for (j = 0; j < height; ++j)
	{
		for (i = 0; i < width; ++i)
		{
			static unsigned char color[3];
			color[0] = pixels[(width * j + i) * 3 + 0];  /* red */
			color[1] = pixels[(width * j + i) * 3 + 1];  /* green */
			color[2] = pixels[(width * j + i) * 3 + 2];  /* blue */
			(void) fwrite(color, 1, 3, fp);
		}
	}

	fclose(fp);

	delete[] pixels;
}

int main(int argc, char** argv)
{
	// glut/glew init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Minko Examples");

#ifdef _WIN32
	glewInit();
#endif
	// !glut/glew init

	auto context = render::OpenGLES2Context::create();
	auto assets	= AssetsLibrary::create(context)
		->registerParser<file::EffectParser>("effect")
		->geometry("cube", geometry::CubeGeometry::create(context))
		->geometry("sphere", geometry::SphereGeometry::create(context))
		->queue("DirectionalLight.effect")
		->queue("Red.effect")
		->queue("Basic.effect");

	assets->defaultOptions()->includePath("effects");

	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
		auto camera	= scene::Node::create("camera");
		auto root   = scene::Node::create("root");

		root->addChild(group)->addChild(camera);

		camera->addController(renderingController = RenderingController::create(assets->context()));

		auto view = Matrix4x4::create()->perspective(.785f, 800.f / 600.f, .1f, 1000.f);
		auto color = Vector4::create(0.f, 0.f, 1.f, 1.f);
		auto lightDirection = Vector3::create(0.f, -1.f, -1.f);

		mesh->addController(TransformController::create());
		mesh->controller<TransformController>()->transform()->appendTranslation(0.f, 0.f, -3.f);
		mesh->addController(SurfaceController::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material/diffuse/rgba",			color)
				->set("transform/worldToScreenMatrix",	view)
				->set("light/direction",				lightDirection),
			assets->effect("basic")
		));

		group->addChild(mesh);

		/*
		for (auto i = 0; i < 50000; ++i)
		{
			group->addChild(Node::create()->addController(SurfaceController::create(
				assets->geometry("cube"),
				data::Provider::create()
					->set("material/diffuse/rgba",			color)
					->set("transform/worldToScreenMatrix",	view)
					->set("light/direction",				lightDirection),
				assets->effect("directional light")
			)));
		}
		*/
	});

	assets->load();
	HalfEdgeCollection* halfEdgeCollection = new HalfEdgeCollection(assets->geometry("sphere")->indices());

	// auto oglContext = context;
	// auto fx = assets->effect("directional light");

	// std::cout << "== vertex shader compilation logs ==" << std::endl;
	// std::cout << oglContext->getShaderCompilationLogs(fx->shaders()[0]->vertexShader()) << std::endl;
	// std::cout << "== fragment shader compilation logs ==" << std::endl;
	// std::cout << oglContext->getShaderCompilationLogs(fx->shaders()[0]->fragmentShader()) << std::endl;
	// std::cout << "== program info logs ==" << std::endl;
	// std::cout << oglContext->getProgramInfoLogs(fx->shaders()[0]->program()) << std::endl;

	glutTimerFunc(1000 / FRAMERATE, timerFunc, 0);
	glutTimerFunc(1000, screenshotFunc, 0);

	glutDisplayFunc(renderScene);
	glutMainLoop();

	return 0;
}
