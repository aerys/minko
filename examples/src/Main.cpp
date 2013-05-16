#include <time.h>

#include "minko/Minko.hpp"

using namespace minko::scene;
using namespace minko::math;
using namespace minko::render;
using namespace minko::render::context;

RenderingController::ptr renderingController;
clock_t start;
unsigned int numFrames = 0;
auto mesh = Node::create("mesh");
auto group = Node::create("group");

void
renderScene()
{
  //group->controller<TransformController>()->transform()->appendTranslation(1, 0, 0);
  //mesh->controller<TransformController>()->transform()->appendRotation(.1, Vector3::yAxis());

  for (auto node : group->children())
	  node->controller<TransformController>()->transform()->appendTranslation(1, 0, 0);
  
  renderingController->render();

  glutPostRedisplay();

  if (numFrames % 100 == 0)
    std::cout << (float)numFrames / ((float)(clock() - start) / CLOCKS_PER_SEC) << " fps." << std::endl;
  ++numFrames;

  //std::cout << std::to_string(group->controller<TransformController>()->transform()) << std::endl;
  //std::cout << std::to_string(mesh->controller<TransformController>()->modelToWorldMatrix()) << std::endl;
}

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("OpenGL - First window demo");
  glutDisplayFunc(renderScene);

#ifdef _WIN32
  glewInit();
#endif

  auto oglContext   = OpenGLESContext::create();
  auto camera       = Node::create("camera");
  auto root         = Node::create("root");

  root->addChild(group)->addChild(camera);

  for (auto i = 0; i < 40000; ++i)
    group->addChild(Node::create()->addController(TransformController::create()));

  camera->addController(renderingController = RenderingController::create(oglContext));

  /*auto shader = GLSLProgram::fromFiles(
    oglContext,
    "../shaders/Basic.vertex.glsl",
    "../shaders/Red.fragment.glsl"
  );

  std::vector<GLSLProgram::ptr> shaders;
  shaders.push_back(shader);

  mesh->addController(SurfaceController::create(
    CubeGeometry::create(),
    data::DataProvider::create(),
    fx
  ));

  auto fx = Effect::create(mesh->bindings(), shaders)
    ->bind("diffuseMaterial/rgba",          "diffuseColor")
    ->bind("transform/modelToWorldMatrix",  "modelToWorldMatrix")
    ->bind("transform/worldToScreenMatrix", "worldToScreenMatrix");
  */

  mesh->addController(TransformController::create());
  group->addController(TransformController::create());

  group->addChild(mesh);
  
 // std::cout << std::to_string(mesh->bindings()->getProperty<Matrix4x4::ptr>("transform/modelToWorldMatrix")) << std::endl;

  start = clock();

  renderScene();
  glutMainLoop();

  return 0;
}
