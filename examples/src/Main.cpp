#include "minko/Minko.hpp"

using namespace minko::scene;
using namespace minko::math;
using namespace minko::render;

void clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
}

void setupProjection()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 500.0);
}

void setupModelView()
{
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
  gluLookAt(2, 2, 2, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glScalef(.005,.005,.005);
  glRotatef(20, 0, 1, 0);
  glRotatef(30, 0, 0, 1);
  glRotatef(5, 1, 0, 0);
  glTranslatef(-300, 0, 0);
}

void draw()
{
  glColor3f(1,1,1);
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'M');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'i');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'n');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'k');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'o');

  glutSwapBuffers();
}

void render()
{
  clear();
  setupProjection();
  setupModelView();
  draw();
}

/* Set up everything, and start the GLUT main loop. */
int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(300, 200);
  glutCreateWindow("Minko Examples");

  context::OpenGLESContext::ptr  oglContext  = OpenGLESContext::create();
  Node::ptr                      camera      = Node::create("camera");
  Node::ptr                      mesh        = Node::create("mesh");
  Node::ptr                      root        = Node::create("root", {camera, mesh});

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

  mesh->addController(SurfaceController::create(
    CubeGeometry::create(),
    data::DataProvider::create(),
    fx
  ));

  mesh->controller<SurfaceController>()->material()->setProperty("diffuseMaterial/rgba", 0x0000ff);
    
  return 0;
}
