#include "minko/Minko.hpp"

using namespace minko::scene;
using namespace minko::math;

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
  Node::ptr           camera    = Node::create("camera");
  Node::ptr           mesh      = Node::create("mesh");
  DataProvider::ptr   material  = data::DataProvider::create();

  Matrix4x4::ptr      m1        = Matrix4x4::create();
  Matrix4x4::ptr      m2        = Matrix4x4::create();

  m1->appendRotation(Vector4::create(0, 1, 0, .5));
  m2->append(Matrix4x4::create()->rotationY(.5));

  std::cout << std::to_string(*m1) << std::endl;
  std::cout << std::to_string(*m2) << std::endl;

  /*glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(300, 200);
  glutCreateWindow("Hello World!");

  glutDisplayFunc(render);

  glutMainLoop();*/

  return 0;
} /* end func main */
