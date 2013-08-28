#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoWebGL.hpp"

#include "GL/glut.h"

#ifdef EMSCRIPTEN
#include "minko/MinkoWebGL.hpp"
#include "emscripten.h"
#endif

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const int WINDOW_WIDTH		= 1024;
const int WINDOW_HEIGHT		= 768;

scene::Node::Ptr				root	= scene::Node::create();
scene::Node::Ptr				mesh	= scene::Node::create();
render::AbstractContext::Ptr	context = nullptr;

void
renderScene()
{
	// render our scene here...
	root->component<SceneManager>()->nextFrame();

	mesh->component<Transform>()->transform()->appendRotationY(.01f);

	glutSwapBuffers();
	glutPostRedisplay();
}

void
initializeScene()
{
	// init. scene here...
	auto sceneManager = SceneManager::create(context);

	root->addComponent(sceneManager);

	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->queue("texture/box.png")
		->queue("effect/Basic.effect");
#ifdef DEBUG
    sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#endif

	// setup camera
	auto camera = scene::Node::create();
	camera->addComponent(Renderer::create());
	camera->addComponent(Transform::create());
	camera->component<Transform>()->transform()
		->lookAt(Vector3::zero(), Vector3::create(3.f, 3.f, 3.f));
	camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));
	root->addChild(camera);

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		std::cout << "loaded" << std::endl;

		// setup mesh
		mesh->addComponent(Transform::create());
		mesh->addComponent(Surface::create(
			geometry::CubeGeometry::create(context),
			data::Provider::create()
				->set("material.diffuseColor",	Vector4::create(0.f, 0.f, 1.f, 1.f))
				->set("material.diffuseMap",	sceneManager->assets()->texture("texture/box.png")),
			sceneManager->assets()->effect("effect/Basic.effect")
		));
		root->addChild(mesh);
	});

	sceneManager->assets()->load();
}

int
main(int argc, char** argv)
{
	// GLUT init.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Flash Conference Example");

	// main loop/context init.
#if defined EMSCRIPTEN
	context = render::WebGLContext::create();
	emscripten_set_main_loop(renderScene, 0, true);
#else
	context = render::OpenGLES2Context::create();
	glutDisplayFunc(renderScene);
#endif

	initializeScene();

	// start the main loop
	glutMainLoop();
}
