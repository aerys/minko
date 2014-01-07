/*
Copyright (c) 2013 Aerys

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

#include "minko/MinkoOffscreen.hpp"
#include "minko/render/OpenGLES2Context.hpp"

#include <ctime>

bool
MinkoOffscreen::_active = false;

minko::Signal<>::Ptr
MinkoOffscreen::_enterFrame = nullptr;

minko::render::AbstractContext::Ptr
MinkoOffscreen::_context = nullptr;

float
MinkoOffscreen::_framerate = 0.f;

std::unique_ptr<GLfloat[]>
MinkoOffscreen::_backBuffer = nullptr;

OSMesaContext
MinkoOffscreen::_offscreenContext = nullptr;

std::unique_ptr<char[]>
MinkoOffscreen::_pixels = nullptr;

minko::uint
MinkoOffscreen::_width = 0;

minko::uint
MinkoOffscreen::_height = 0;

void
MinkoOffscreen::run()
{
	_active = true;
	_framerate = 0.f;

	while (_active)
		step();
}

void
MinkoOffscreen::initialize(const std::string& windowTitle, unsigned int width, unsigned int height, bool useStencil)
{
	_active = false;
	_framerate = 0.f;
	_width = width;
	_height = height;

	_enterFrame = minko::Signal<>::create();

	initializeContext(windowTitle, width, height, useStencil);
}

void
MinkoOffscreen::step()
{
	auto stepStartTime = std::clock();

	_enterFrame->execute();

	_framerate = 1000.f / (1000.f * (std::clock() - stepStartTime) / CLOCKS_PER_SEC);
}

void
MinkoOffscreen::initializeContext(const std::string& windowTitle, unsigned int width, unsigned int height, bool useStencil)
{
	_backBuffer.reset(new GLfloat[width * height * 4]);
	_pixels.reset(new char[width * height * 3]);

	if (!_backBuffer)
		throw std::runtime_error("Could not create offscreen backbuffer");

	_offscreenContext = OSMesaCreateContextExt(GL_RGBA, 32, 0, 0, NULL);

	if (!_offscreenContext)
		throw std::runtime_error("Could not create offscreen context");

	if (!OSMesaMakeCurrent(_offscreenContext, _backBuffer.get(), GL_FLOAT, width, height))
		throw std::runtime_error("Could not make offscreen context current");

	_context = minko::render::OpenGLES2Context::create();
}

void
MinkoOffscreen::takeScreenshot()
{
	glReadPixels(0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, _pixels.get());

	int i, j;
	FILE* fp = fopen("screenshot.ppm", "wb");
	fprintf(fp, "P6\n%d %d\n255\n", _width, _height);

	unsigned long long avgColor = 0;

	for (j = 0; j < _height; ++j)
	{
		for (i = 0; i < _width; ++i)
		{
			static unsigned char color[3];
			color[0] = _pixels[(_width * j + i) * 3 + 0];
			color[1] = _pixels[(_width * j + i) * 3 + 1];
			color[2] = _pixels[(_width * j + i) * 3 + 2];
			(void) fwrite(color, 1, 3, fp);

			avgColor += (color[0] + color[1] + color[2]) / 3;
		}
	}

	avgColor /= (_height * _width);

	std::cout << "avgColor: " << avgColor << std::endl;

	fclose(fp);

	// FIXME: Use PNGWriter.
}
