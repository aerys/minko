#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoMk.hpp"
#include "minko/MinkoBullet.hpp"
#include "minko/MinkoParticles.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"

#include <time.h>

#ifdef EMSCRIPTEN
# include "minko/MinkoWebGL.hpp"
# include "emscripten.h"
# include "SDL/SDL.h"
#else
# include "SDL2/SDL.h"
#endif

#ifdef MINKO_ANGLE
#include "SDL2/SDL_syswm.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#include "minko/component/SponzaLighting.hpp"
#include "minko/component/Fire.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const float WINDOW_WIDTH		= 1024.0f;
const float WINDOW_HEIGHT		= 500.0f;

const std::string MK_NAME			= "model/Sponza_lite_sphere.mk";
const std::string DEFAULT_EFFECT	= "effect/SponzaLighting.effect";
const std::string CAMERA_NAME		= "camera";

const float CAMERA_LIN_SPEED	= 0.05f;
const float CAMERA_ANG_SPEED	= PI * 2.f / 180.0f;
const float CAMERA_MASS			= 50.0f;
const float CAMERA_FRICTION		= 0.6f;

Renderer::Ptr			renderer			= nullptr;
auto					sponzaLighting		= SponzaLighting::create();
auto					mesh				= scene::Node::create("mesh");
auto					group				= scene::Node::create("group");
auto					camera				= scene::Node::create("camera");
auto					root				= scene::Node::create("root");
auto					speed				= 0.0f;
auto					angSpeed			= 0.0f;
float					rotationX			= 0.0f;
float					rotationY			= 0.0f;
float					mousePositionX		= 0.0f;
float					mousePositionY		= 0.0f;
Vector3::Ptr			target				= Vector3::create();
Vector3::Ptr			eye					= Vector3::create();
bullet::Collider::Ptr	cameraCollider		= nullptr;

#if defined EMSCRIPTEN
render::WebGLContext::Ptr       context;
#else
render::OpenGLES2Context::Ptr   context;
#endif

#if defined EMSCRIPTEN
void
resizeHandler(int width, int height)
{
	context->configureViewport(0, 0, width, height);
}

#endif
void
SDLMouseMoveHandler()
{
	int x;
	int y;
	if (!(SDL_GetMouseState(&x, &y) & SDL_BUTTON_LEFT))
	{
		mousePositionX = x;
		mousePositionY = y;

		return;
	}

	rotationY += (mousePositionX - x) * .0005;
	rotationX += (mousePositionY - y) * .0005;

	const float limit = 89 * PI / 180;

	if (rotationX < -limit)
		rotationX = -limit;
	else if (rotationX > +limit)
		rotationX = +limit;

}

#ifndef EMSCRIPTEN
void
SDL_KeyboardHandler(bool collider, std::shared_ptr<Matrix4x4> cameraTransform)
{
	if (!collider)
	{
	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
	if (keyboardState[SDL_SCANCODE_UP] ||
		keyboardState[SDL_SCANCODE_W] ||
		keyboardState[SDL_SCANCODE_Z])
		cameraTransform->prependTranslation(0.f, 0.f, -CAMERA_LIN_SPEED);
	else if (keyboardState[SDL_SCANCODE_DOWN] ||
		keyboardState[SDL_SCANCODE_S])
		cameraTransform->prependTranslation(0.f, 0.f, CAMERA_LIN_SPEED);
	if (keyboardState[SDL_SCANCODE_LEFT] ||
		keyboardState[SDL_SCANCODE_A] ||
		keyboardState[SDL_SCANCODE_Q])
		cameraTransform->prependRotation(-CAMERA_ANG_SPEED, Vector3::yAxis());
	else if (keyboardState[SDL_SCANCODE_RIGHT] ||
		keyboardState[SDL_SCANCODE_D])
		cameraTransform->prependRotation(CAMERA_ANG_SPEED, Vector3::yAxis());
	}
	else
	{

	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
	if (keyboardState[SDL_SCANCODE_UP] ||
		keyboardState[SDL_SCANCODE_W] ||
		keyboardState[SDL_SCANCODE_Z])
		// go forward
		cameraTransform->prependTranslation(Vector3::create(0.0f, 0.0f, -CAMERA_LIN_SPEED));
	else if (keyboardState[SDL_SCANCODE_DOWN] ||
		keyboardState[SDL_SCANCODE_S])
		// go backward
		cameraTransform->prependTranslation(Vector3::create(0.0f, 0.0f, CAMERA_LIN_SPEED));
	if (keyboardState[SDL_SCANCODE_LEFT] ||
		keyboardState[SDL_SCANCODE_A] ||
		keyboardState[SDL_SCANCODE_Q])
		cameraTransform->prependTranslation(-CAMERA_LIN_SPEED, 0.0f, 0.0f);
	else if (keyboardState[SDL_SCANCODE_RIGHT] ||
		keyboardState[SDL_SCANCODE_D])
		cameraTransform->prependTranslation(CAMERA_LIN_SPEED, 0.0f, 0.0f);

	eye = cameraTransform->translation();

	if (keyboardState[SDL_SCANCODE_SPACE] && eye->y() <= 0.5f)
		cameraTransform->prependTranslation(0.0f, 4 * CAMERA_LIN_SPEED, 0.0f);
	}
}
#else
void
SDL_KeyboardHandler(bool collider, std::shared_ptr<Matrix4x4> cameraTransform)
{
	return;
}
#endif

template <typename T>
static
void
read(std::stringstream& stream, T& value)
{
	stream.read(reinterpret_cast<char*>(&value), sizeof (T));
}

template <typename T>
static
T
swap_endian(T u)
{
	union
	{
		T u;
		unsigned char u8[sizeof(T)];
	} source, dest;

	source.u = u;

	for (size_t k = 0; k < sizeof(T); k++)
		dest.u8[k] = source.u8[sizeof(T) - k - 1];

	return dest.u;
}

template <typename T>
T
readAndSwap(std::stringstream& stream)
{
	T value;
	stream.read(reinterpret_cast<char*>(&value), sizeof (T));

	return swap_endian(value);
}

bullet::AbstractPhysicsShape::Ptr
deserializeShape(Qark::Map&			shapeData,
				 scene::Node::Ptr&	node)
{
	bullet::AbstractPhysicsShape::Ptr deserializedShape;

	int type = Any::cast<int>(shapeData["type"]);

	double rx	= 0.0;
	double ry	= 0.0;
	double rz	= 0.0;
	double h	= 0.0;
	double r	= 0.0;

	std::stringstream stream;
	switch (type)
	{
	case 101: // multiprofile
		deserializedShape = deserializeShape(Any::cast<Qark::Map&>(shapeData["shape"]), node);
		break;
	case 2: // BOX
		{
			Qark::ByteArray& source = Any::cast<Qark::ByteArray&>(shapeData["data"]);
			stream.write(&*source.begin(), source.size());

			rx = readAndSwap<double>(stream);
			ry = readAndSwap<double>(stream);
			rz = readAndSwap<double>(stream);

			deserializedShape = bullet::BoxShape::create(rx, ry, rz);
		}
		break;
	case 5 : // CONE
		{
			Qark::ByteArray& source = Any::cast<Qark::ByteArray&>(shapeData["data"]);
			stream.write(&*source.begin(), source.size());

			r = readAndSwap<double>(stream);
			h = readAndSwap<double>(stream);

			deserializedShape = bullet::ConeShape::create(r, h);
		}
		break;
	case 6 : // BALL
		{
			Qark::ByteArray& source = Any::cast<Qark::ByteArray&>(shapeData["data"]);
			stream.write(&*source.begin(), source.size());

			r = readAndSwap<double>(stream);

			deserializedShape = bullet::SphereShape::create(r);
		}
		break;
	case 7 : // CYLINDER
		{
			Qark::ByteArray& source = Any::cast<Qark::ByteArray&>(shapeData["data"]);
			stream.write(&*source.begin(), source.size());

			r = readAndSwap<double>(stream);
			h = readAndSwap<double>(stream);

			deserializedShape = bullet::CylinderShape::create(r, h, r);
		}
		break;
	case 100 : // TRANSFORM
		{
			deserializedShape		= deserializeShape(Any::cast<Qark::Map&>(shapeData["subGeometry"]), node);

			auto delta				= deserialize::TypeDeserializer::matrix4x4(shapeData["delta"]);
			auto modelToWorld		= node->component<Transform>()->modelToWorldMatrix(true);

			deserializedShape->initialize(delta, modelToWorld);
		}
		break;
	default:
		deserializedShape = nullptr;
	}

	return deserializedShape;
}

std::shared_ptr<bullet::Collider>
deserializeBullet(Qark::Map&						nodeInformation,
				  file::MkParser::ControllerMap&	controllerMap,
				  file::MkParser::NodeMap&			nodeMap,
				  scene::Node::Ptr&					node)
{
	Qark::Map& colliderData = Any::cast<Qark::Map&>(nodeInformation["defaultCollider"]);
	Qark::Map& shapeData	= Any::cast<Qark::Map&>(colliderData["shape"]);

	bullet::AbstractPhysicsShape::Ptr shape = deserializeShape(shapeData, node);

	float mass			= 1.0f;
	double vx			= 0.0;
	double vy			= 0.0;
	double vz			= 0.0;
	double avx			= 0.0;
	double avy			= 0.0;
	double avz			= 0.0;
	bool sleep			= false;
	bool rotate			= false;
	bool trigger		= false;
	double friction		= 0.5; // bullet's advices
	double restitution	= 0.0; // bullet's advices

	if (shapeData.find("materialProfile") != shapeData.end())
	{
		Qark::ByteArray& materialProfileData = Any::cast<Qark::ByteArray&>(shapeData["materialProfile"]);
		std::stringstream	stream;
		stream.write(&*materialProfileData.begin(), materialProfileData.size());

		double density	= readAndSwap<double>(stream);
		mass			= density * shape->volume();
		friction		= readAndSwap<double>(stream);
		restitution		= readAndSwap<double>(stream);
	}

	if (shapeData.find("logicProfile") != shapeData.end())
	{
		Qark::ByteArray& logicProfileData = Any::cast<Qark::ByteArray&>(shapeData["logicProfile"]);
		std::stringstream	stream;
		stream.write(&*logicProfileData.begin(), logicProfileData.size());

		trigger	= readAndSwap<bool>(stream);
	}

	if (colliderData.find("dynamics") == colliderData.end())
		mass = 0.0; // static object
	else
	{
		Qark::ByteArray& dynamicsData = Any::cast<Qark::ByteArray&>(colliderData["dynamics"]);
		std::stringstream	stream;
		stream.write(&*dynamicsData.begin(), dynamicsData.size());

		vx		= readAndSwap<double>(stream);
		vy		= readAndSwap<double>(stream);
		vz		= readAndSwap<double>(stream);
		avx		= readAndSwap<double>(stream);
		avy		= readAndSwap<double>(stream);
		avz		= readAndSwap<double>(stream);
		sleep	= readAndSwap<bool>(stream);
		rotate	= readAndSwap<bool>(stream);
	}


	bullet::ColliderData::Ptr data = bullet::ColliderData::create(mass, shape);

	data->linearVelocity(vx, vy, vz);
	data->angularVelocity(avx, avy, avz);
	data->friction(friction);
	data->restitution(restitution);
	data->triggerCollisions(trigger);

	if (!rotate)
		data->angularFactor(0.0f, 0.0f, 0.0f);
	//collider->disableDeactivation(sleep == false);
	data->disableDeactivation(true);

	return bullet::Collider::create(data);
}

component::bullet::Collider::Ptr
initializeDefaultCameraCollider()
{
	auto shape		= bullet::BoxShape::create(0.2f, 0.3f, 0.2f);
	auto data		= bullet::ColliderData::create(CAMERA_MASS, shape);

	data->restitution(0.5f);
	data->angularFactor(0.0f, 0.0f, 0.0f);
	data->friction(CAMERA_FRICTION);
	data->disableDeactivation(true);

	return bullet::Collider::create(data);
}

void
initializeCamera(scene::Node::Ptr group)
{
	auto cameras = scene::NodeSet::create(group)
		->descendants(true)
		->where([](scene::Node::Ptr node)
				{
					return node->name() == CAMERA_NAME;
				});

	bool cameraInGroup = false;
	if (cameras->nodes().empty())
	{
		// default camera
		camera = scene::Node::create(CAMERA_NAME);

		camera->addComponent(Transform::create());
		camera->component<Transform>()->transform()
			->appendTranslation(0.0f, 0.75f, 5.0f)
			->appendRotationY(PI * 0.5);

		cameraCollider = initializeDefaultCameraCollider();
		camera->addComponent(cameraCollider);
	}
	else
	{
		// set-up camera from the mk file
		camera = cameras->nodes().front();
		cameraInGroup = true;

		if (camera->hasComponent<component::bullet::Collider>())
			cameraCollider = camera->component<component::bullet::Collider>();
	}

	if (!camera->hasComponent<Transform>())
		throw std::logic_error("Camera (deserialized or created) must have a Transform.");

	camera->addComponent(renderer);
	camera->addComponent(PerspectiveCamera::create(.785f, WINDOW_WIDTH / WINDOW_HEIGHT, .1f, 1000.f));
	root->addChild(camera);
}

void
initializePhysics()
{
	auto physicWorld = bullet::PhysicsWorld::create(renderer);

	physicWorld->setGravity(math::Vector3::create(0.f, -9.8f, 0.f));
	root->addComponent(physicWorld);
}

void
printFramerate(const unsigned int delay = 1)
{
	static auto start = time(NULL);
	static auto numFrames = 0;

	int secondTime = time(NULL);

	++numFrames;

	if ((secondTime - start) >= 1)
	{
		std::cout << numFrames << " fps." << std::endl;
		start = time(NULL);
		numFrames = 0;
	}
}


#ifdef MINKO_ANGLE
typedef struct
{
   /// Window width
   GLint       width;
   /// Window height
   GLint       height;
   /// Window handle
   EGLNativeWindowType  hWnd;
   /// EGL display
   EGLDisplay  eglDisplay;
   /// EGL context
   EGLContext  eglContext;
   /// EGL surface
   EGLSurface  eglSurface;
} ESContext; 

ESContext* initContext(SDL_Window* window)
{
	EGLint configAttribList[] =
	{
		EGL_RED_SIZE,       8,
		EGL_GREEN_SIZE,     8,
		EGL_BLUE_SIZE,      8,
		EGL_ALPHA_SIZE,     8,
		EGL_DEPTH_SIZE,     16,
		EGL_STENCIL_SIZE,   8,
		EGL_SAMPLE_BUFFERS, 0,
		EGL_NONE
	};
	EGLint surfaceAttribList[] =
	{
		EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
		EGL_NONE, EGL_NONE
	};

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (!SDL_GetWindowWMInfo(window, &info))
		return GL_FALSE;
	EGLNativeWindowType hWnd = info.info.win.window;

	ESContext* es_context = new ESContext();
	es_context->width = 800;
	es_context->height = 600;
	es_context->hWnd = hWnd;

	EGLDisplay display;
	EGLint numConfigs;
	EGLint majorVersion;
	EGLint minorVersion;
	EGLContext context;
	EGLSurface surface;
	EGLConfig config;
	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE }; 

	display = eglGetDisplay(GetDC(hWnd)); // EGL_DEFAULT_DISPLAY
	if ( display == EGL_NO_DISPLAY )
	{
		return EGL_FALSE;
	}

	// Initialize EGL
	if ( !eglInitialize(display, &majorVersion, &minorVersion) )
	{
		return EGL_FALSE;
	}

	// Get configs
	if ( !eglGetConfigs(display, NULL, 0, &numConfigs) )
	{
		return EGL_FALSE;
	}

	// Choose config
	if ( !eglChooseConfig(display, configAttribList, &config, 1, &numConfigs) )
	{
		return EGL_FALSE;
	}

	// Create a surface
	surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hWnd, surfaceAttribList);
	if ( surface == EGL_NO_SURFACE )
	{
		return EGL_FALSE;
	}

	// Create a GL context
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs );
	if ( context == EGL_NO_CONTEXT )
	{
		return EGL_FALSE;
	}   

	// Make the context current
	if ( !eglMakeCurrent(display, surface, surface, context) )
	{
		return EGL_FALSE;
	}
	es_context->eglDisplay = display;
	es_context->eglSurface = surface;
	es_context->eglContext = context;
	return es_context;
}
#endif

int
main(int argc, char** argv)
{
	file::MkParser::registerController(
		"colliderController",
		std::bind(
			deserializeBullet,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4
		)
	);

	SDL_Init(SDL_INIT_VIDEO);

#ifdef EMSCRIPTEN
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_WM_SetCaption("Minko - Sponza Example", "Minko");
	SDL_Surface *screen = SDL_SetVideoMode(WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0, SDL_OPENGL);

	std::cout << "WebGL context created" << std::endl;
	context = render::WebGLContext::create();
#else
	SDL_Window *window = SDL_CreateWindow("Minko - Sponza Example",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			SDL_WINDOW_OPENGL);

# ifdef MINKO_ANGLE
	ESContext* escontext;
	if (!(escontext = initContext(window)))
		throw std::runtime_error("Could not create eglContext");

	std::cout << "EGLContext Initialized" << std::endl;
# else
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
# endif
	std::cout << "OpenGL ES2 context created" << std::endl;
	context = render::OpenGLES2Context::create();
#endif

	std::cout << context->driverInfo() << std::endl;

	auto sceneManager = SceneManager::create(context);

	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->registerParser<file::JPEGParser>("jpg")
		->registerParser<file::MkParser>("mk")
		->geometry("cube", geometry::CubeGeometry::create(context));

#ifdef EMSCRIPTEN
	sceneManager->assets()->defaultOptions()->includePaths().insert("assets");
#endif

#ifdef DEBUG
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#else
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/release");
#endif

	// load sponza lighting effect and set it as the default effect
	sceneManager->assets()
		->load("effect/SponzaLighting.effect")
		->load("effect/Basic.effect");
	sceneManager->assets()->defaultOptions()->effect(sceneManager->assets()->effect("effect/SponzaLighting.effect"));

	// load other assets
	sceneManager->assets()
		->queue("texture/firefull.jpg")
		->queue("effect/Particles.effect")
		->queue(MK_NAME);

	sceneManager->assets()->defaultOptions()->generateMipmaps(true);

	renderer = Renderer::create();

	initializePhysics();

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		scene::Node::Ptr mk = assets->node(MK_NAME);
		initializeCamera(mk);

		root->addChild(group);
		root->addComponent(sceneManager);
		root->addComponent(sponzaLighting);

		group->addComponent(Transform::create());
		group->addChild(mk);
		return;
		scene::NodeSet::Ptr fireNodes = scene::NodeSet::create(group)
			->descendants()
			->where([](scene::Node::Ptr node)
		{
			return node->name() == "fire";
		});

		auto fire = Fire::create(assets);
		for (auto fireNode : fireNodes->nodes())
		{
			fireNode->addComponent(fire);

			auto test = scene::Node::create()
				->addComponent(Transform::create())
				->addComponent(Surface::create(
					assets->geometry("cube"),
					data::Provider::create()->set("material.diffuseColor", Vector4::create(1, 0, 0, 1)),
					assets->effect("effect/Basic.effect")
				));

			test->component<Transform>()->transform()->copyFrom(fireNode->component<Transform>()->transform());
			root->addChild(test);

			std::cout << fireNode->component<Transform>()->transform()->translation()->toString() << std::endl;
		}
	});

	sceneManager->assets()->load();

	std::cout << "start rendering" << std::endl << std::flush;

	bool done = false;

	while (!done)
	{
		auto cameraTransform = camera->component<Transform>()->transform();

		if (cameraCollider == nullptr)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
			switch (event.type)
			{
				case SDL_MOUSEMOTION:
				SDLMouseMoveHandler();
				break;
				case SDL_QUIT:
				done = true;
				break;
				default:
				break;
			}
			}
			SDL_KeyboardHandler(false, cameraTransform);
		}
		else
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
				case SDL_MOUSEMOTION:
					SDLMouseMoveHandler();
					break;
				case SDL_KEYDOWN:
					break;
				case SDL_QUIT:
					done = true;
					break;
				default:
					break;
				}
			}
			SDL_KeyboardHandler(true, cameraTransform);

			// look around
			eye = cameraTransform->translation();

			target->setTo(
				eye->x() + sinf(rotationY) * cosf(rotationX),
				eye->y() + sinf(rotationX),
				eye->z() + cosf(rotationY) * cosf(rotationX)
			);

			cameraTransform->lookAt(target, eye, Vector3::upAxis());

			cameraCollider->synchronizePhysicsWithGraphics();
		}
		
		
		sceneManager->nextFrame();
		sponzaLighting->step();

#ifdef MINKO_ANGLE
		eglSwapBuffers(escontext->eglDisplay, escontext->eglSurface); 
#elif defined(EMSCRIPTEN)
		SDL_GL_SwapBuffers();
#else
		SDL_GL_SwapWindow(window);
#endif
		SDL_PumpEvents();
	}

#ifndef EMSCRIPTEN
	SDL_DestroyWindow(window);
#endif
	SDL_Quit();

	std::exit(EXIT_SUCCESS);
}
