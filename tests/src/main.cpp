#include "gtest/gtest.h"

#include "minko/Minko.hpp"
#include "minko/MinkoTests.hpp"

#ifdef EMSCRIPTEN
# include "SDL/SDL.h"
#else
# include "SDL2/SDL.h"
#endif

#ifdef MINKO_ANGLE
# include "SDL2/SDL_syswm.h"
# include <EGL/egl.h>
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#endif

using namespace minko;

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

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);

		if (SDL_Init(SDL_INIT_VIDEO) == -1)
		exit(-1);

#ifdef EMSCRIPTEN
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_WM_SetCaption("Minko - Cube Example", "Minko");
	SDL_Surface *screen = SDL_SetVideoMode(800,
		600,
		0, SDL_OPENGL);

	std::cout << "WebGL context created" << std::endl;
	context = render::WebGLContext::create();
#else
	SDL_Window* window = SDL_CreateWindow(
		"Minko - Cube Example",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800, 600,
		SDL_WINDOW_OPENGL
	);
#endif

#ifdef MINKO_ANGLE
	ESContext* context;
	if (!(context = initContext(window)))
		throw std::runtime_error("Could not create eglContext");

	std::cout << "EGLContext Initialized" << std::endl;
#else
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
#endif

	MinkoTests::context(render::OpenGLES2Context::create());

	return RUN_ALL_TESTS();
}
