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

#pragma once

#include "minko/Minko.hpp"
#include "minko/Signal.hpp"
#include "minko/render/AbstractContext.hpp"

#include <GL/osmesa.h>

class MinkoOffscreen
{
private:
	static bool                                 _active;
	static minko::render::AbstractContext::Ptr  _context;
	static float                                _framerate;
	static minko::Signal<>::Ptr                 _enterFrame;
	static std::unique_ptr<GLfloat[]>           _backBuffer;
	static OSMesaContext                        _offscreenContext;
	static std::unique_ptr<char[]>              _pixels;
	static minko::uint							_width;
	static minko::uint							_height;

public:
	inline static
	bool
	active()
	{
		return _active;
	}

	inline static
	minko::Signal<>::Ptr
	enterFrame()
	{
		return _enterFrame;
	}

	inline static
	minko::render::AbstractContext::Ptr
	context()
	{
		return _context;
	}

	inline static
	float
	framerate()
	{
		return _framerate;
	}

	static
	void
	run();

	static
	void
	initialize(const std::string& windowTitle, unsigned int width = 0, unsigned int height = 0, bool useStencil = false);

	static
	void
	takeScreenshot();

private:
	static
	void
	step();

	static
	void
	initializeContext(const std::string& windowTitle, unsigned int width, unsigned int height, bool useStencil);
};
