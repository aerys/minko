/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/SDLAngleBackend.hpp"

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

using namespace minko;

void
SDLAngleBackend::initialize(std::shared_ptr<Canvas> canvas)
{
    EGLint configAttribList[] =
    {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 8,
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
        throw std::runtime_error("Could not create Angle context backend");

    EGLNativeWindowType hWnd = info.info.win.window;

    ESContext* es_context = new ESContext();
    es_context->width = width;
    es_context->height = height;
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
    if (display == EGL_NO_DISPLAY)
        throw std::runtime_error("Could not create Angle context backend");

    // Initialize EGL
    if (!eglInitialize(display, &majorVersion, &minorVersion))
        throw std::runtime_error("Could not create Angle context backend");

    // Get configs
    if (!eglGetConfigs(display, NULL, 0, &numConfigs))
        throw std::runtime_error("Could not create Angle context backend");

    // Choose config
    if (!eglChooseConfig(display, configAttribList, &config, 1, &numConfigs))
        throw std::runtime_error("Could not create Angle context backend");

    // Create a surface
    surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hWnd, surfaceAttribList);
    if (surface == EGL_NO_SURFACE)
        throw std::runtime_error("Could not create Angle context backend");

    // Create a GL context
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT)
        throw std::runtime_error("Could not create Angle context backend");

    // Make the context current
    if (!eglMakeCurrent(display, surface, surface, context))
        throw std::runtime_error("Could not create Angle context backend");

    es_context->eglDisplay = display;
    es_context->eglSurface = surface;
    es_context->eglContext = context;

    _context = es_context;
}

void
SDLAngleBackend::swapBuffers(std::shared_ptr<Canvas> canvas)
{
	auto context = reinterpret_cast<ESContext>(_context);
    eglSwapBuffers(context->eglDisplay, context->eglSurface);
}
