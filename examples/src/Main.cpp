#include <time.h>

#include "minko/Minko.hpp"

using namespace minko::scene;
using namespace minko::math;
using namespace minko::render;
using namespace minko::render::context;

RenderingController::ptr renderingController;
unsigned int numFrames = 0;
auto mesh = Node::create("mesh");
auto group = Node::create("group");

void
printFramerate(const unsigned int delay = 1)
{
	static auto start = clock();

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
	/*auto mesh = NodeSet::create(group)
		->descendants()
		->where([](Node::ptr node)
			{
				return node->hasController<TransformController>()
					&& node->hasController<SurfaceController>();
			});*/

	mesh->controller<TransformController>()->transform()->prependRotationY(.001);
	//mesh->controller<TransformController>()->transform()->appendTranslation(0.f, 0.f, .1f);

	//group->controller<TransformController>()->transform()->prependRotationY(.1);

  renderingController->render();

  printFramerate();

  glutSwapBuffers();
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
	srand(clock());

  glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_SINGLE);

  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutCreateWindow("Minko Examples");

#ifdef _WIN32
  glewInit();
#endif


  auto oglContext   = OpenGLES2Context::create();
  auto camera       = Node::create("camera");
  auto root         = Node::create("root");

  root->addChild(group)->addChild(camera);

  camera->addController(renderingController = RenderingController::create(oglContext));

  file::FileLoader loader;
  file::EffectParser parser;

  parser.parse(oglContext, loader.load("effects/DirectionalLight.effect"));

  auto fx = parser.effect();

  std::cout << "== vertex shader compilation logs ==" << std::endl;
  std::cout << oglContext->getShaderCompilationLogs(fx->shaders()[0]->vertexShader()) << std::endl;
  std::cout << "== fragment shader compilation logs ==" << std::endl;
  std::cout << oglContext->getShaderCompilationLogs(fx->shaders()[0]->fragmentShader()) << std::endl;
  std::cout << "== program info logs ==" << std::endl;
  std::cout << oglContext->getProgramInfoLogs(fx->shaders()[0]->program()) << std::endl;

  auto viewMatrix = Matrix4x4::create()->perspective(.785f, 800.f / 600.f, .1f, 1000.f);
  auto cubeGeometry = CubeGeometry::create(oglContext);

  mesh->addController(TransformController::create());
  mesh->controller<TransformController>()->transform()->appendTranslation(0.f, 0.f, -3.f);
  mesh->addController(SurfaceController::create(
    cubeGeometry,
    data::DataProvider::create()
		->setProperty("material/diffuse/rgba",			Vector4::create(0.f, 0.f, 1.f, 1.f))
		->setProperty("transform/worldToScreenMatrix",	viewMatrix)
		->setProperty("light/direction",					Vector3::create(0.f, -1.f, -1.f)),
    fx
  ));

  group->addChild(mesh);

  /*for (auto i = 0; i < 10000; ++i)
  {
	  auto cube = Node::create("cube" + std::to_string(i));

	  cube->addController(TransformController::create());
	  cube->controller<TransformController>()->transform()->appendTranslation(
	    -10.f + (((float)rand() / RAND_MAX) - .5f) * 40.f,
		-10.f + (((float)rand() / RAND_MAX) - .5f) * 40.f,
		-10.f + (((float)rand() / RAND_MAX) - .5f) * 40.f
	  );

	  cube->addController(SurfaceController::create(
		cubeGeometry,
		data::DataProvider::create()
			->setProperty("material/diffuse/rgba",			Vector4::create(1.f, 0.f, 0.f, 1.f))
			->setProperty("transform/worldToScreenMatrix",	viewMatrix),
		fx
	  ));

	  group->addChild(cube);
  }*/

  group->addController(TransformController::create());

  glutDisplayFunc(renderScene);
  glutMainLoop();

  return 0;
}
