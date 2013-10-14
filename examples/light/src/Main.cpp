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

#ifndef EMSCRIPTEN
void
SDL_KeyboardHandler(scene::Node::Ptr		root,
					data::Provider::Ptr		data,
					file::AssetLibrary::Ptr	assets)
{
	static const std::string& normalMapPropName	= "material.normalMap";
	static const std::string& normalMapFilename	= "texture/normalmap-cells.png";

	static const uint MAX_NUM_LIGHTS	= 4;
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
		if (newLights.empty())
		{
			std::cout << "no random light to remove" << std::endl;
			return;
		}

		root->removeChild(newLights.back());
		newLights.resize(newLights.size()-1);
	}
	else if (keyboardState[SDL_SCANCODE_A])
	{
		if (newLights.size() == MAX_NUM_LIGHTS)
		{
			std::cout << "cannot add more lights" << std::endl;
			return;
		}

		std::stringstream stream;
		stream << "newLight_" << newLights.size();
		
		newLights.push_back(scene::Node::create(stream.str()));

		auto pointLight = PointLight::create();
		pointLight->color()->setTo(rand()/(float)RAND_MAX, rand()/(float)RAND_MAX, rand()/(float)RAND_MAX);

		newLights.back()->addComponent(pointLight);

		const float	theta		= 2.0f * PI * rand() / (float)RAND_MAX;
		const float	phi			= PI * rand() / (float)RAND_MAX;
		auto		direction	= Vector3::create(cosf(theta)*sinf(phi), sinf(theta)*sinf(phi), cosf(phi));
		direction				= direction * 5.0;

		newLights.back()->addComponent(Transform::create());
		newLights.back()->component<Transform>()->transform()
			->appendTranslation(direction);

		root->addChild(newLights.back());
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
	auto root				= scene::Node::create("root");
    auto mesh				= scene::Node::create("mesh");
	auto meshData			= data::Provider::create();
	auto ambientLightNode	= scene::Node::create("ambientLight");
    auto dirLightNode1		= scene::Node::create("directionalLight1");
	auto dirLightNode2		= scene::Node::create("directionalLight2");
	auto pointLightNode		= scene::Node::create("pointLight");
	auto spotLightNode		= scene::Node::create("spotLight");
	auto sphereGeometry		= geometry::SphereGeometry::create(sceneManager->assets()->context(), 32, 16, true);

	const bool blackOut = false;
	std::cout << "Press [SPACE]\tto toogle normal mapping\nPress [A]\tto add random light\nPress [R]\tto remove random light" << std::endl;


	sphereGeometry->computeTangentSpace(false);

	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("sphere", sphereGeometry)
		->queue("texture/box.png")
		->queue("texture/normalmap-cells.png")
		//->queue("texture/normalmap-squares.png")
		//->queue("texture/specularmap-squares.png")
		->queue("effect/Phong.effect");

#ifdef DEBUG
    sceneManager->assets()->defaultOptions()->includePaths().insert("bin/debug");
#else
	sceneManager->assets()->defaultOptions()->includePaths().insert("bin/release");
#endif

	const float boxScale = 3.0f;

    auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
        auto camera	= scene::Node::create("camera");
 
		root->addComponent(sceneManager);
		
		// ambient light
		ambientLightNode->addComponent(AmbientLight::create(0.8f));
		if (!blackOut)
			root->addChild(ambientLightNode);

		// directional light
		auto directionalLight = DirectionalLight::create();
		dirLightNode1->addComponent(Transform::create());
		dirLightNode1->addComponent(directionalLight);
		if (!blackOut)
			root->addChild(dirLightNode1);

		// directional light 2
		directionalLight = DirectionalLight::create();
		directionalLight->color()->setTo(1.f, 0.f, 0.f);
		dirLightNode2->addComponent(directionalLight);
		dirLightNode2->addComponent(Transform::create());
		dirLightNode2->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(0.f, -1.f, 1.f));
		if (!blackOut)
			root->addChild(dirLightNode2);

		// setup point light 1
		auto pointLight	= component::PointLight::create();
		pointLight->color()->setTo(0.2f, 0.2f, 1.0f);
		pointLightNode->addComponent(pointLight);
		pointLightNode->addComponent(Transform::create());
		pointLightNode->component<Transform>()->transform()->appendTranslation(boxScale, 0.0f, 0.0f);
		if (!blackOut)
			root->addChild(pointLightNode);
		 
		// setup spot light
		auto spotLight	= component::SpotLight::create(0.05f*PI, 0.075f*PI);
		spotLight->color()->setTo(0.8f, 0.8f, 0.0f);
		spotLightNode->addComponent(spotLight);
		spotLightNode->addComponent(Transform::create());
		spotLightNode->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(0.0f, 5.0f, 0.0f), Vector3::create(-1.0, 0.0, 0.0));
		if (!blackOut)
			root->addChild(spotLightNode);

		// setup camera
        auto renderingComponent = Renderer::create();
		renderingComponent->backgroundColor(0x7F7F7FFF);
        camera->addComponent(renderingComponent);
		camera->addComponent(Transform::create());
		camera->component<Transform>()->transform()
			->lookAt(Vector3::zero(), Vector3::create(6.f, 6.f, -6.f));
		camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));
        root->addChild(camera);

		// setup mesh
		mesh->addComponent(Transform::create());
		mesh->component<Transform>()->transform()
			->appendScale(boxScale, boxScale, boxScale)
			->appendTranslation(0.0f, 0.f, 0.0f);

		// setup mesh material
		meshData
			->set("material.diffuseColor",	Vector4::create(0.f, 0.f, 1.f, 1.f))
			->set("material.diffuseMap",	assets->texture("texture/box.png"))
			->set("material.normalMap",		assets->texture("texture/normalmap-cells.png"))
			//->set("material.normalMap",		assets->texture("texture/normalmap-squares.png"))
			//->set("material.specularMap",	assets->texture("texture/specularmap-squares.png"))
			->set("material.shininess",		32.f);

		mesh->addComponent(Surface::create(
			assets->geometry("sphere"),
			meshData,
			assets->effect("effect/Phong.effect")
		));
		root->addChild(mesh);
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
				SDL_KeyboardHandler(root, meshData, sceneManager->assets());
#endif // EMSCRIPTEN
				break;
			default:
				break;
			}
		}


		dirLightNode1->component<Transform>()->transform()->prependRotationY(.01f);
		pointLightNode->component<Transform>()->transform()->appendRotationY(.01f);

		const float ampl		= 0.5f + 0.5f * cosf((float)(clock() - startTime) * 0.01f);
		const float	outerAng	= PI * 0.01f * (1.0f + 49.0f * ampl);
		const float innerAng	= 0.8f * outerAng;

		dirLightNode1->component<DirectionalLight>()->diffuse(ampl);
		spotLightNode->component<SpotLight>()->innerConeAngle(innerAng);
		spotLightNode->component<SpotLight>()->outerConeAngle(outerAng);

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
