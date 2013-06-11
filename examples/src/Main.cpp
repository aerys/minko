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
	//mesh->controller<TransformController>()->transform()->prependRotationY(.1);
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

  auto oglContext   = OpenGLESContext::create();

//  oglContext->configureViewport(0, 0, 800, 600);

  auto camera       = Node::create("camera");
  auto root         = Node::create("root");

  root->addChild(group)->addChild(camera);

  camera->addController(renderingController = RenderingController::create(oglContext));

  auto shader = GLSLProgram::fromFiles(
    oglContext,
    "../shaders/Basic.vertex.glsl",
    "../shaders/Basic.fragment.glsl"
  );

  std::cout << "== vertex shader compilation logs ==" << std::endl;
  std::cout << oglContext->getShaderCompilationLogs(shader->vertexShader()) << std::endl;
  std::cout << "== fragment shader compilation logs ==" << std::endl;
  std::cout << oglContext->getShaderCompilationLogs(shader->fragmentShader()) << std::endl;
  std::cout << "== program info logs ==" << std::endl;
  std::cout << oglContext->getProgramInfoLogs(shader->program()) << std::endl;

  std::vector<GLSLProgram::ptr> shaders;
  shaders.push_back(shader);

  auto cubeGeometry = CubeGeometry::create(oglContext);
  auto fx = Effect::create(shaders)
	->bindInput("material/diffuse/rgba",			"diffuseColor")
	->bindInput("transform/modelToWorldMatrix",		"modelToWorldMatrix")
	->bindInput("transform/worldToScreenMatrix",	"worldToScreenMatrix");

  auto viewMatrix = Matrix4x4::create()->perspectiveFoV(.785f, 800.f / 600.f, .1f, 1000.f);

  mesh->addController(SurfaceController::create(
    cubeGeometry,
    data::DataProvider::create()
		->setProperty("material/diffuse/rgba",			Vector4::create(0.f, 0.f, 1.f, 1.f))
		->setProperty("transform/worldToScreenMatrix",	viewMatrix),
    fx
  ));

  mesh->addController(TransformController::create());
  mesh->controller<TransformController>()->transform()->appendTranslation(0.f, 0.f, -10.f);
  //group->addChild(mesh);

  for (auto i = 0; i < 10000; ++i)
  {
	  auto cube = Node::create("cube" + std::to_string(i));

	  cube->addController(SurfaceController::create(
		cubeGeometry,
		data::DataProvider::create()
			->setProperty("material/diffuse/rgba",			Vector4::create(1.f, 0.f, 0.f, 1.f))
			->setProperty("transform/worldToScreenMatrix",	viewMatrix),
		fx
	  ));

	  cube->addController(TransformController::create());
	  cube->controller<TransformController>()->transform()->appendTranslation(
	    -10.f + (((float)rand() / RAND_MAX) - .5f) * 40.f,
		-10.f + (((float)rand() / RAND_MAX) - .5f) * 40.f,
		-10.f + (((float)rand() / RAND_MAX) - .5f) * 40.f
	  );

	  group->addChild(cube);
  }

  group->addController(TransformController::create());

  glutDisplayFunc(renderScene);
  glutMainLoop();

  return 0;
}
