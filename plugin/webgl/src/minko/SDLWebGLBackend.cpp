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

#include "emscripten/emscripten.h"
#include "SDL.h"

using namespace minko;

Canvas::Ptr currentCanvas;

void
emscriptenMainLoop()
{
    currentCanvas->step();
}

void
SDLWebGLBackend::initialize(std::shared_ptr<Canvas> canvas)
{
    SDL_GLContext glContext = SDL_GL_CreateContext(canvas->window());

    if (!glContext)
        throw std::runtime_error("Could not create WebGL context backend.");
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
    emscripten_set_main_loop(emscriptenMainLoop, 0, 1);
}

void
SDLWebGLBackend::wait(std::shared_ptr<Canvas> canvas, uint ms)
{
	// Nothing, because emscripten_set_main_loop calls step on a timer.
}
