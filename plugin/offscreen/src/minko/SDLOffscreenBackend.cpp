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

#include "minko/SDLOffscreenBackend.hpp"
#include "minko/MinkoSDL.hpp"

#include <GL/osmesa.h>

using namespace minko;

void
SDLOffscreenBackend::initialize(std::shared_ptr<Canvas> canvas)
{
	try
	{
		_backBuffer.reset(new std::vector<float>(canvas->width() * canvas->height() * 4));
	}
	catch (const std::bad_alloc&)
	{
		throw std::runtime_error("Could not create offscreen backbuffer");
	}

	OSMesaContext offscreenContext = OSMesaCreateContextExt(GL_RGBA, 32, 0, 0, NULL);

	if (!offscreenContext)
		throw std::runtime_error("Could not create offscreen context");

	if (!OSMesaMakeCurrent(offscreenContext, &*_backBuffer->begin(), GL_FLOAT, canvas->width(), canvas->height()))
		throw std::runtime_error("Could not make offscreen context current");
}

void
SDLOffscreenBackend::swapBuffers(std::shared_ptr<Canvas> canvas)
{
	// Nothing.
}
