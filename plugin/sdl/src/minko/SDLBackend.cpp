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

#include "minko/SDLBackend.hpp"
#include "minko/Canvas.hpp"
#include "minko/log/Logger.hpp"

#include "SDL.h"

using namespace minko;

SDLBackend::SDLBackend()
{

}

void
SDLBackend::initialize(std::shared_ptr<Canvas> canvas)
{
    SDL_GLContext glContext = SDL_GL_CreateContext(canvas->window());

    if (!glContext)
    {
        const auto error = SDL_GetError();

        LOG_ERROR(error);

        throw std::runtime_error("Could not create default context");
    }
}

void
SDLBackend::swapBuffers(std::shared_ptr<Canvas> canvas)
{
    SDL_GL_SwapWindow(canvas->window());
}

void
SDLBackend::run(std::shared_ptr<Canvas> canvas)
{
    while (canvas->active())
    {
        canvas->step();
    }
}

void
SDLBackend::wait(std::shared_ptr<Canvas> canvas, uint ms)
{
    SDL_Delay(ms);
}
