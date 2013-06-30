#include <time.h>

#include "minko/Minko.hpp"

using namespace minko;

RenderingController::Ptr renderingController;
auto mesh = Node::create("mesh");
auto group = Node::create("group");

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
	//mesh->controller<TransformController>()->transform()->prependRotationY(.001f);

	renderingController->render();

	printFramerate();

	glutSwapBuffers();
	glutPostRedisplay();
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

	auto context = OpenGLES2Context::create();
	auto assets	= AssetsLibrary::create(context)
		->registerParser<file::EffectParser>("effect")
		->geometry("cube", CubeGeometry::create(context))
		->geometry("sphere", SphereGeometry::create(context))
		->queue("DirectionalLight.effect")
		->queue("Red.effect")
		->queue("Basic.effect");

	assets->defaultOptions()->includePath("effects");

	auto _ = assets->complete()->connect([](AssetsLibrary::Ptr assets)
	{
		auto camera	= Node::create("camera");
		auto root   = Node::create("root");

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
			assets->effect("directional light")
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

	glutDisplayFunc(renderScene);
	glutMainLoop();

	return 0;
}
