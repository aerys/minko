#include <time.h>

#include "minko/Minko.hpp"

using namespace minko::scene;
using namespace minko::math;
using namespace minko::render;
using namespace minko::render::context;

RenderingController::ptr renderingController;
clock_t start = clock();
unsigned int numFrames = 0;

void
renderScene()
{
  renderingController->render();

  glutPostRedisplay();

  //std::cout << "frame " << numFrames << std::endl;
  ++numFrames;
  if (numFrames % 100 == 0)
  {
    std::cout << (float)numFrames / ((float)(clock() - start) / CLOCKS_PER_SEC) << " fps." << std::endl;
  }
}

/* Main method - main entry point of application
the freeglut library does the window creation work for us, 
regardless of the platform. */
int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE);
  glutInitWindowSize(500,500);
  glutInitWindowPosition(100,100);
  glutCreateWindow("OpenGL - First window demo");
  glutDisplayFunc(renderScene);

  auto oglContext   = OpenGLESContext::create();
  auto camera       = Node::create("camera");
  auto mesh         = Node::create("mesh");
  auto group        = Node::create("group", {mesh});
  auto root         = Node::create("root", {group, camera});

  for (auto i = 0; i < 10000; ++i)
  {
    std::cout << i << std::endl;
    group->addChild(Node::create("test" + std::to_string(i))->addController(TransformController::create()));
  }

  renderingController = RenderingController::create(oglContext);

  camera->addController(renderingController);

  auto shader = GLSLProgram::fromFiles(
    oglContext,
    "../shaders/Basic.vertex.glsl",
    "../shaders/Red.fragment.glsl"
  );

  auto fx = Effect::create(mesh->bindings(), {shader})
    ->bind("diffuseMaterial/rgba",          "diffuseColor")
    ->bind("transform/modelToWorldMatrix",  "modelToWorldMatrix")
    ->bind("transform/worldToScreenMatrix", "worldToScreenMatrix");

  //mesh->addController(TransformController::create());
  group->addController(TransformController::create());

  /*mesh->addController(SurfaceController::create(
    CubeGeometry::create(),
    data::DataProvider::create(),
    fx
  ));*/

  //renderScene();
  glutMainLoop();

  return 0;
}
