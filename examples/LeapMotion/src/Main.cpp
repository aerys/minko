#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"

#ifdef EMSCRIPTEN
# include "SDL/SDL.h"
#else
# include "Leap/Leap.h"
# include "SDL2/SDL.h"
# ifdef MINKO_ANGLE
#  include "SDL2/SDL_syswm.h"
#  include <EGL/egl.h>
#  include <GLES2/gl2.h>
#  include <GLES2/gl2ext.h>
# endif
#endif

using namespace minko;
using namespace minko::component;
using namespace minko::math;

void explode(scene::Node::Ptr node, float magnitude)
{
	auto pTransform = node->component<Transform>()->transform();
	for (scene::Node::Ptr child : node->children())
	{
		auto cTransform = child->component<Transform>()->transform();
		auto direction = cTransform->translation();
		cTransform->appendTranslation(direction * magnitude);
		explode(child, magnitude);
	}
}

typedef struct s_boundingBox
{
    Vector3::Ptr        corner1;
    Vector3::Ptr        corner2;
    scene::Node::Ptr    node;
}boundingBox;

//std::vector<boundingBox>
void
getBoundingBoxes(scene::Node::Ptr node, std::vector<boundingBox> boxList)
{
    auto surface = node->component<Surface>();
    if (surface != nullptr)
    {
        auto geometry = surface->geometry();
        auto provider = geometry->data();
        auto vertices = provider->get<render::VertexBuffer::Ptr>("geometry.vertex.attribute.position")->data();
        Vector3::Ptr corner1 = Vector3::create(vertices[0], vertices[1], vertices[2]);
        Vector3::Ptr corner2 = Vector3::create(vertices[0], vertices[1], vertices[2]);
        
        for (int i = 0; i < vertices.size(); i += 3)
        {
            if (vertices[i] > corner1->x())
                corner1->x(vertices[i]);
            if (vertices[i+1] > corner1->y())
                corner1->y(vertices[i+1]);
            if (vertices[i+2] > corner1->z())
                corner1->z(vertices[i+2]);
            
            if (vertices[i] < corner2->x())
                corner2->x(vertices[i]);
            if (vertices[i+1] < corner1->y())
                corner2->y(vertices[i+1]);
            if (vertices[i+2] < corner1->z())
                corner2->z(vertices[i+2]);
        }
        boundingBox box;
        box.corner1 = corner1;
        box.corner2 = corner2;
        box.node = node;
        boxList.push_back(box);
    }
    for (scene::Node::Ptr child : node->children())
        getBoundingBoxes(child, boxList);
}

int main(int argc, char** argv)
{
	std::vector<boundingBox>boxList;
    
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
		exit(-1);

#ifdef EMSCRIPTEN
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_WM_SetCaption("Minko - Cube Example", "Minko");
	SDL_Surface *screen = SDL_SetVideoMode(800,
		600,
		0, SDL_OPENGL);

	std::cout << "WebGL context created" << std::endl;
	//context = render::WebGLContext::create();
#else
	SDL_Window* window = SDL_CreateWindow(
		"Minko - Cube Example",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800, 600,
		SDL_WINDOW_OPENGL
	);

# ifdef MINKO_ANGLE
	ESContext* context;
	if (!(context = initContext(window)))
		throw std::runtime_error("Could not create eglContext");

	std::cout << "EGLContext Initialized" << std::endl;
# else
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
# endif
#endif

	auto sceneManager = SceneManager::create(render::OpenGLES2Context::create());
	auto baseNode = scene::Node::create("baseNode");
	auto mesh = scene::Node::create("mesh");
	auto mesh2 = scene::Node::create("mesh2");
	auto mesh3 = scene::Node::create("mesh3");
	auto pointer = scene::Node::create("pointer");
	auto selectedMesh = mesh;

	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context()))
		->queue("texture/box.png")
		->queue("effect/Basic.effect");

#ifdef DEBUG
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#else 
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/release");
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
			->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 15.f));
		camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));
		root->addChild(camera);

		baseNode->addComponent(Transform::create());

		// setup mesh
		mesh->addComponent(Transform::create());
		mesh->component<Transform>()->transform()->appendTranslation(2.5f, 0.f, 0.f);
		mesh->addComponent(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material.diffuseColor",	Vector4::create(0.f, 0.f, 1.f, 1.f)),
			assets->effect("effect/Basic.effect")
		));
		mesh2->addComponent(Transform::create());
		mesh2->component<Transform>()->transform()->appendTranslation(-2.5f, 0.f, 0.f);
		mesh2->addComponent(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material.diffuseColor",	Vector4::create(1.f, 0.f, 0.f, 1.f)),
			assets->effect("effect/Basic.effect")
		));

		mesh3->addComponent(Transform::create());
		mesh3->component<Transform>()->transform()->appendTranslation(0.f, -2.5f, 0.f);
		mesh3->addComponent(Surface::create(
			assets->geometry("cube"),
			data::Provider::create()
				->set("material.diffuseColor",	Vector4::create(0.f, 1.f, 0.f, 1.f)),
			assets->effect("effect/Basic.effect")
		));

		baseNode->addChild(mesh);
		baseNode->addChild(mesh2);
		mesh2->addChild(mesh3);

		pointer->addComponent(Transform::create());
		pointer->component<Transform>()->transform()->prependScale(0.3f, 0.3f, 0.3f)->appendTranslation(0, 0, 5.0f);
		pointer->addComponent(Surface::create(
			assets->geometry("sphere"),
			data::Provider::create()
			->set("material.diffuseColor", Vector4::create(1.f, 1.f, 1.f, 0.5f)),
			assets->effect("effect/Basic.effect")
		));

		root->addChild(baseNode);
		root->addChild(pointer);
        
        getBoundingBoxes(root, boxList);
	});
    
	sceneManager->assets()->load();

	Leap::Controller* controller = new Leap::Controller();
	controller->enableGesture(Leap::Gesture::TYPE_SWIPE);
	controller->enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
	Leap::Frame lastFrame;
	float speed = 0.f;
	float scaleSpeed = 1.0f;
	float lastgap = 0.0f;
	const float delta = 5.f;

	bool done = false;
	while (!done)
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				done = true;
				break;
			default:
				break;
			}
		}

		Leap::Frame frame = controller->frame();

		const Leap::GestureList gestures = frame.gestures();

		const Leap::HandList hands = frame.hands();

		

		if (hands.count() >= 2)
		{
			Leap::Hand lhand = hands.leftmost();
			Leap::Hand rhand = hands.rightmost();

			float gap = (rhand.palmPosition() - lhand.palmPosition()).magnitude();;

			if (lhand.palmNormal().x > 0.7f && rhand.palmNormal().x < -0.7f)
			{
				if (gap > lastgap + delta)
				{
					explode(baseNode, 0.1f);
					//scaleSpeed = scaleSpeed + (1.5f - scaleSpeed) * 0.01f;
				}
				else if (gap < lastgap - delta)
				{
					explode(baseNode, -0.1f);
					//scaleSpeed = scaleSpeed + (0.5f - scaleSpeed) * 0.01f;
				}

			}
			lastgap = gap;

			//auto rotation = mesh->component<Transform>()->transform()->rotationQuaternion();
			
		}
		else
		{
			for (int g = 0; g < gestures.count(); g++)
			{
				Leap::Gesture gesture = gestures[g];
				switch (gesture.type())
				{
				case Leap::Gesture::TYPE_SWIPE:
					{
						Leap::SwipeGesture swipe = gesture;
						speed = speed + (swipe.speed() / 100.f * swipe.direction().x - speed) * 0.005f;
                        std::cout << swipe.speed() << std::endl;
						break;
					}
				case Leap::Gesture::TYPE_SCREEN_TAP:
					{
						pointer->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(0.f, 1.f, 0.f, 0.5f));
                        Leap::ScreenTapGesture screenTape = gesture;
						break;
					}
				default:
					break;
				}
				
			}

			Leap::PointableList pointables = frame.pointables();
			if (pointables.count() >=4)
				speed = 0.f;
			else if (pointables.count() >= 1)
			{
				auto finger = pointables[0];
				auto tip = finger.tipPosition() * 0.1f;
				tip.y -= 15.f;
				pointer->component<Transform>()->transform()->identity()->prependScale(0.3f, 0.3f, 0.3f)->prependTranslation(tip.x, tip.y, 5.0f);
			}
		}

		selectedMesh->component<Transform>()->transform()->prependRotationY(0.01f * speed)->prependScale(scaleSpeed, scaleSpeed, scaleSpeed);

		speed = speed * 0.999f;
		scaleSpeed = scaleSpeed + (1.f - scaleSpeed) * 0.1f;
		//std::cout << scaleSpeed << std::endl;

		sceneManager->nextFrame();

		pointer->component<Surface>()->material()->set("material.diffuseColor", Vector4::create(1.f, 1.f, 1.f, 0.5f));
#ifdef MINKO_ANGLE
		eglSwapBuffers(context->eglDisplay, context->eglSurface);
#elif defined(EMSCRIPTEN)
		SDL_GL_SwapBuffers();
#else
		SDL_GL_SwapWindow(window);
#endif
	}

	SDL_Quit();

	exit(EXIT_SUCCESS);
}
