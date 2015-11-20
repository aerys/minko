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

#include "minko/SDLWebGLBackend.hpp"
#include "minko/Canvas.hpp"

#if MINKO_PLATFORM & MINKO_PLATFORM_HTML5
# include "emscripten/emscripten.h"
# include "emscripten/html5.h"
#endif
#include "SDL.h"
#include "SDL_syswm.h"

using namespace minko;

Canvas::Ptr currentCanvas;
std::chrono::high_resolution_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();
int canvasHidden = 0;

SDLWebGLBackend::SDLWebGLBackend()
{
}

void
emscriptenMainLoop()
{
	auto t = std::chrono::high_resolution_clock::now();

	auto frameTime = 1e-6f * std::chrono::duration_cast<std::chrono::nanoseconds>(t - previousFrameTime).count();
	auto targetFrameTime = (1000.f / (currentCanvas->desiredFramerate())) - 2.0f;

	if (frameTime < targetFrameTime)
		return;

	previousFrameTime = t;

    currentCanvas->step();
}

EM_BOOL emscriptenVisibilityChangeHandler(int eventType, const EmscriptenVisibilityChangeEvent *e, void *userData)
{
    if (e->hidden == canvasHidden)
        return 0;

    if (!canvasHidden && e->hidden)
        currentCanvas->resetInputs();

    canvasHidden = e->hidden;

	return 0;
}

EM_BOOL emscriptenFocusBlurHandler(int eventType, const EmscriptenFocusEvent *focusEvent, void *userData)
{
    auto hidden = eventType == EMSCRIPTEN_EVENT_BLUR ? 1 : 0;

    if (!canvasHidden && hidden)
        currentCanvas->resetInputs();

    canvasHidden = hidden;

    return 0;
}

void
SDLWebGLBackend::initialize(std::shared_ptr<Canvas> canvas)
{
    // Nothing, because we already have the browser.
}

void
SDLWebGLBackend::swapBuffers(std::shared_ptr<Canvas> canvas)
{
    SDL_GL_SwapBuffers();
}

void
SDLWebGLBackend::run(std::shared_ptr<Canvas> canvas)
{
    currentCanvas = canvas;

    emscripten_set_visibilitychange_callback(0, false, emscriptenVisibilityChangeHandler);
    emscripten_set_focus_callback("#window", 0, false, emscriptenFocusBlurHandler);
    emscripten_set_blur_callback("#window", 0, false, emscriptenFocusBlurHandler);
    
    emscripten_set_main_loop(emscriptenMainLoop, 0, 1);
}

void
SDLWebGLBackend::wait(std::shared_ptr<Canvas> canvas, uint ms)
{
    // Nothing, because emscripten_set_main_loop calls step on a timer.
}
