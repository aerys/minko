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
  renderingController->render();

  printFramerate();

  glutSwapBuffers();
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
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
    "shaders/Basic.vertex.glsl",
    "shaders/Red.fragment.glsl"
  );

  std::cout << "== vertex shader compilation logs ==" << std::endl;
  std::cout << oglContext->getShaderCompilationLogs(shader->vertexShader()) << std::endl;
  std::cout << "== fragment shader compilation logs ==" << std::endl;
  std::cout << oglContext->getShaderCompilationLogs(shader->fragmentShader()) << std::endl;
  std::cout << "== program info logs ==" << std::endl;
  std::cout << oglContext->getProgramInfoLogs(shader->program()) << std::endl;

  std::vector<GLSLProgram::ptr> shaders;
  shaders.push_back(shader);

  mesh->addController(SurfaceController::create(
    CubeGeometry::create(oglContext),
    data::DataProvider::create()
//		->setProperty("material/diffuse/rgba",			Vector4::create(1.f, 1.f, 1.f, 1.f))
		->setProperty("transform/worldToScreenMatrix",	Matrix4x4::create()->perspectiveFoV(.785f, 800.f / 600.f, .01f, 1000.f)),
    Effect::create(shaders)
//		->bindInput("material/diffuse/rgba",				"diffuseColor")
		->bindInput("transform/modelToWorldMatrix",		"modelToWorldMatrix")
		->bindInput("transform/worldToScreenMatrix",		"worldToScreenMatrix")
  ));

  mesh->addController(TransformController::create());
  mesh->controller<TransformController>()->transform()->appendTranslation(0., 0., 10.);

  group->addController(TransformController::create());

  group->addChild(mesh);
  
  glutDisplayFunc(renderScene);
  glutMainLoop();

  return 0;
}
