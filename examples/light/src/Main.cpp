#include <ctime>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"

#ifdef EMSCRIPTEN
# include "SDL/SDL.h"
#else
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

scene::Node::Ptr
createPointLight(Vector3::Ptr color, Vector3::Ptr position, file::AssetLibrary::Ptr assets)
{
	auto pointLight = scene::Node::create("pointLight")
		->addComponent(PointLight::create(10.f))
		->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(position)))
		->addComponent(Surface::create(
			assets->geometry("quad"),
			material::Material::create()
				->set("diffuseMap",		assets->texture("texture/sprite-pointlight.png"))
				->set("diffuseTint",	Vector4::create(color->x(), color->y(), color->z(), 1.f)),
			assets->effect("effect/Sprite.effect")
		));
	pointLight->component<PointLight>()->color(color);
	pointLight->component<PointLight>()->diffuse(.1f);

	return pointLight;
}

float
hue2rgb(float p, float q, float t)
{
	if (t < 0.f)
		t += 1.f;
	if (t > 1.f)
		t -= 1.f;
	if (t < 1.f/6.f)
		return p + (q - p) * 6.f * t;
	if (t < 1.f/2.f)
		return q;
	if (t < 2.f/3.f)
		return p + (q - p) * (2.f/3.f - t) * 6.f;

	return p;
}

Vector3::Ptr
hslToRgb(float h, float s, float l)
{
    float r, g, b;

    if (s == 0)
        r = g = b = l; // achromatic
    else
	{
        float q = l < 0.5f ? l * (1.f + s) : l + s - l * s;
        float p = 2.f * l - q;

        r = hue2rgb(p, q, h + 1.f/3.f);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.f/3.f);
    }

	return Vector3::create(r, g, b);
}

#ifndef EMSCRIPTEN
void
SDL_KeyboardHandler(scene::Node::Ptr		root,
					data::Provider::Ptr		data,
					file::AssetLibrary::Ptr	assets)
{
	static const std::string& normalMapPropName	= "normalMap";
	static const std::string& normalMapFilename	= "texture/normalmap-cells.png";

	static const uint MAX_NUM_LIGHTS	= 40;
	static std::vector<scene::Node::Ptr> newLights;

	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
	if (keyboardState[SDL_SCANCODE_SPACE])
	{
		bool hasNormalMap = data->hasProperty(normalMapPropName);

		std::cout << "mesh does" << (!hasNormalMap ? " not " : " ") << "have a normal map:\t" << (hasNormalMap ? "remove" : "add") << " it" << std::endl;
		if (hasNormalMap)
			data->unset(normalMapPropName);
		else
			data->set(normalMapPropName, assets->texture(normalMapFilename));
	}
	else if (keyboardState[SDL_SCANCODE_R])
	{
		auto lights = root->children()[4];

		if (lights->children().size() == 0)
		{
			std::cout << "no random light to remove" << std::endl;
			return;
		}

		lights->removeChild(lights->children().back());
	}
	else if (keyboardState[SDL_SCANCODE_A])
	{
		if (root->children()[4]->children().size() == MAX_NUM_LIGHTS)
		{
			std::cout << "cannot add more lights" << std::endl;
			return;
		}

		auto r = rand() / (float)RAND_MAX;
		auto theta = 2.0f * PI *  r;
		auto color = hslToRgb(r, 1.f, .5f);
		auto pos = Vector3::create(
			cosf(theta) * 5.f + rand() / ((float)RAND_MAX * 3.f),
			2.5f + rand() / (float)RAND_MAX,
			sinf(theta) * 5.f + rand() / ((float)RAND_MAX * 3.f)
		);

		root->children()[4]->addChild(createPointLight(color, pos, assets));
	}
}
#endif // EMSCRIPTEN

int main(int argc, char** argv)
{
#ifdef EMSCRIPTEN
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_WM_SetCaption("Minko - Light Example", "Minko");
	SDL_Surface *screen = SDL_SetVideoMode(800,
		600,
		0, SDL_OPENGL);

	std::cout << "WebGL context created" << std::endl;
	context = render::WebGLContext::create();
#else
	SDL_Window* window = SDL_CreateWindow(
		"Minko - Light Example",
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

	const clock_t startTime	= clock();

	auto sceneManager		= SceneManager::create(render::OpenGLES2Context::create());
	auto root				= scene::Node::create("root")->addComponent(sceneManager);
	auto sphereGeometry		= geometry::SphereGeometry::create(sceneManager->assets()->context(), 32, 32, true);
	auto sphereMaterial		= material::Material::create()
		->set("diffuseColor",	Vector4::create(1.f, 1.f, 1.f, 1.f))
		->set("shininess",		32.f);
		//->set("normalMap",		assets->texture("texture/normalmap-cells.png")),
	auto lights				= scene::Node::create("lights");

	std::cout << "Press [SPACE]\tto toogle normal mapping\nPress [A]\tto add random light\nPress [R]\tto remove random light" << std::endl;

	sphereGeometry->computeTangentSpace(false);

	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("quad", geometry::QuadGeometry::create(sceneManager->assets()->context()))
		->geometry("sphere", sphereGeometry)
		//->queue("texture/box.png")
		->queue("texture/normalmap-cells.png")
		->queue("texture/window-normal.png")
		//->queue("texture/specularmap-squares.png")
		->queue("texture/sprite-pointlight.png")
		->queue("effect/Basic.effect")
		->queue("effect/Sprite.effect")
		->queue("effect/Phong.effect");

#ifdef DEBUG
    sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#else
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/release");
#endif

    auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		// camera
        auto camera	= scene::Node::create("camera")
			->addComponent(Renderer::create())
			->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f))
			->addComponent(Transform::create(Matrix4x4::create()->lookAt(Vector3::create(0.f, 2.f), Vector3::create(10.f, 10.f, 10.f))));
		//camera->component<Renderer>()->backgroundColor(0x7f7f7fff);
		root->addChild(camera);

		// ground
		auto ground = scene::Node::create("ground")
			->addComponent(Surface::create(
				assets->geometry("quad"),
				material::Material::create()
					->set("diffuseColor",	Vector4::create(1.f, 1.f, 1.f, 1.f)),
					//->set("normalMap",		assets->texture("texture/window-normal.png")),
				assets->effect("effect/Phong.effect")
			))
			->addComponent(Transform::create(Matrix4x4::create()->appendScale(50.f)->appendRotationX(-1.57f)));
		root->addChild(ground);

		// sphere
		auto sphere = scene::Node::create("sphere")
			->addComponent(Surface::create(
				assets->geometry("sphere"),
				sphereMaterial,
				assets->effect("effect/Phong.effect")
			))
			->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(0.f, 2.f, 0.f)->prependScale(3.f)));
		root->addChild(sphere);

		// spotLight
		auto spotLight = scene::Node::create("spotLight")
			->addComponent(SpotLight::create(.15f, .4f))
			->addComponent(Transform::create(Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(15.f, 20.f, 0.f))));
		spotLight->component<SpotLight>()->diffuse(.4f);
		root->addChild(spotLight);

		lights->addComponent(Transform::create());
		root->addChild(lights);
	});

	sceneManager->assets()->load();

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
			case SDL_KEYDOWN:
#ifndef EMSCRIPTEN
				SDL_KeyboardHandler(root, sphereMaterial, sceneManager->assets());
#endif // EMSCRIPTEN
				break;
			default:
				break;
			}
		}

		lights->component<Transform>()->transform()->appendRotationY(.005f);

		sceneManager->nextFrame();
		//printFramerate();

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
