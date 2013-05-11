#include "minko/Minko.hpp"

using namespace minko::scene;
using namespace minko::math;
using namespace minko::render;

RenderingController::ptr renderingController;

void
renderScene()
{
  glClearColor(0., 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
  glBegin(GL_POLYGON);
      glVertex2f(-0.5, -0.5);
      glVertex2f(-0.5, 0.5);
      glVertex2f(0.5, 0.5);
      glVertex2f(0.5, -0.5);
  glEnd();
  glFlush();

  renderingController->render();

  glutPostRedisplay();

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

  context::OpenGLESContext::ptr  oglContext   = OpenGLESContext::create();
  Node::ptr                      camera       = Node::create("camera");
  Node::ptr                      mesh         = Node::create("mesh");
  Node::ptr                      group        = Node::create("group", {mesh});
  Node::ptr                      root         = Node::create("root", {group, camera});

  renderingController = RenderingController::create(oglContext);
  camera->addController(renderingController);

  GLSLProgram::ptr shader = GLSLProgram::fromFiles(
    oglContext,
    "../shaders/Basic.vertex.glsl",
    "../shaders/Red.fragment.glsl"
  );

  Effect::ptr fx = Effect::create(mesh->bindings(), {shader})
    ->bind("diffuseMaterial/rgba", "diffuseColor")
    ->bind("transform/modelToWorldMatrix", "modelToWorldMatrix")
    ->bind("transform/worldToScreenMatrix", "worldToScreenMatrix");

  mesh->addController(TransformController::create());
  group->addController(TransformController::create());

  mesh->addController(SurfaceController::create(
    CubeGeometry::create(),
    data::DataProvider::create(),
    fx
  ));

//  renderScene();
  glutMainLoop();

  return 0;
}
