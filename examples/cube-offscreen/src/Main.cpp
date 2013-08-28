#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"

#include <GL/osmesa.h>
#include <iostream>

using namespace minko;
using namespace minko::component;
using namespace minko::math;

#define WIDTH 800
#define HEIGHT 600

void takeScreenshot()
{
  const int width = WIDTH, height = HEIGHT;

  char* pixels = new char[3 * width * height];

  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  int i, j;
  FILE *fp = fopen("screenshot.ppm", "wb");
  fprintf(fp, "P6\n%d %d\n255\n", width, height);

  unsigned long long avgColor = 0;

  for (j = 0; j < height; ++j)
  {
    for (i = 0; i < width; ++i)
    {
      static unsigned char color[3];
      color[0] = pixels[(width * j + i) * 3 + 0];
      color[1] = pixels[(width * j + i) * 3 + 1];
      color[2] = pixels[(width * j + i) * 3 + 2];
      (void) fwrite(color, 1, 3, fp);

      avgColor += (color[0] + color[1] + color[2]) / 3;
    }
  }

  avgColor /= (height * width);

  std::cout << "avgColor: " << avgColor << std::endl;

  fclose(fp);

  delete[] pixels;
}

int main(int argc, char** argv)
{
  void* buffer = (GLfloat *) malloc(WIDTH * HEIGHT * 4 * sizeof(GLfloat));
  if (!buffer)
  {
    printf("Alloc image buffer failed!\n");
    return 0;
  }
  OSMesaContext context = OSMesaCreateContextExt(GL_RGBA, 32, 0, 0, NULL);
  if (!context)
  {
    printf("OSMesaCreateContext failed!\n");
    return 0;
  }

  if (!OSMesaMakeCurrent(context, buffer, GL_FLOAT, WIDTH, HEIGHT))
  {
    printf("OSMesaMakeCurrent failed!\n");
    return 0;
  }

	auto sceneManager = SceneManager::create(render::OpenGLES2Context::create());
    auto mesh = scene::Node::create("mesh");

	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->queue("texture/box.png")
		->queue("effect/Basic.effect");

#ifdef DEBUG
    sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#endif

    auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		auto root   = scene::Node::create("root");
                auto camera	= scene::Node::create("camera");

		root->addComponent(sceneManager);

		// setup camera
                auto renderer = Renderer::create();
		renderer->backgroundColor(0x7F7F7FFF);
                camera->addComponent(renderer);
		camera->addComponent(Transform::create());
		camera->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f));
		camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));
                root->addChild(camera);

		// setup mesh
		mesh->addComponent(Transform::create());
		mesh->addComponent(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material.diffuseColor",	Vector4::create(0.f, 0.f, 1.f, 1.f))
				->set("material.diffuseMap",	assets->texture("texture/box.png")),
			assets->effect("effect/Basic.effect")
		));
		root->addChild(mesh);
	});

	sceneManager->assets()->load();

	while (true)
	{
		mesh->component<Transform>()->transform()->prependRotationY(.01f);

		sceneManager->nextFrame();

                static int loop = 0;
                if (++loop % 100 == 0)
                  takeScreenshot();
	}

        OSMesaDestroyContext(context);

	exit(EXIT_SUCCESS);
}
