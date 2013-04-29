#include <iostream>
#include <functional>
#include <exception>
#include <stdexcept>
#include <string>

#include <sys/time.h>

#include "minko/Minko.hpp"

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
  Node::ptr           scene     = Node::create("scene");
  Node::ptr           camera    = Node::create("camera");
  Node::ptr           mesh      = Node::create("mesh");
  DataProvider::ptr   material  = DataProvider::create();

  Matrix4x4::ptr      m         = Matrix4x4::create();

  m->view(Vector3::zero(), Vector3::zAxis(), Vector3::yAxis());

  std::cout << std::to_string(*m) << std::endl;

  //try
  {
    (*material)["diffuseColor"] = 0x0000ffff;

    mesh->addController(SurfaceController::create(CubeGeometry::create(), material));
    camera->addController(RenderingController::create(OpenGLESContext::create()));

    scene->descendantAdded()->add([](Node::ptr node, Node::ptr descendant)
    {
      std::cout << "descendant added: " << descendant->name() << std::endl;
    });

    scene->descendantRemoved()->add([](Node::ptr node, Node::ptr descendant)
    {
      std::cout << "descendant removed: " << descendant->name() << std::endl;
    });

    mesh->bindings()->propertyChangedSignal("diffuseColor")->add([](DataBindings::ptr b, DataProvider::ptr p, const std::string& propertyName)
    {
      std::cout << "bindings property changed: " << propertyName << std::endl;
    });

    //material->setProperty("diffuseColor", 0x000000ff);
    (*material)["diffuseColor"] = 0x00ff00ff;

    scene->addChild(mesh);
    scene->addChild(camera);

    camera->parent()->removeChild(camera);
  }
  /*catch (const std::invalid_argument& ia)
  {
    std::cerr << "Invalid argument:" << ia.what() << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }*/

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(300, 200);
  glutCreateWindow("Hello World!");

  glutDisplayFunc(render);

  glutMainLoop();

  return 0;
} /* end func main */
