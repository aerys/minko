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

#if defined(CHROMIUM)
#include "chromium/ChromiumRenderHandler.hpp"
#include "minko/render/Texture.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/render/AbstractContext.hpp"
#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_render_handler.h"

using namespace minko;
using namespace chromium;

ChromiumRenderHandler::ChromiumRenderHandler(std::shared_ptr<AbstractCanvas> canvas, std::shared_ptr<render::AbstractContext> context) :
	_canvas(canvas), 
	_context(context),
	_textureBuffer(nullptr),
	renderTexture(nullptr),
	textureChanged(false)
{
	generateTexture();
}

ChromiumRenderHandler::~ChromiumRenderHandler()
{
}

bool
ChromiumRenderHandler::generateTexture()
{
	uint w = math::clp2(_canvas->width());
	uint h = math::clp2(_canvas->height());
	
	if (w > 2048)
		w = 2048;

	if (h > 2048)
		h = 2048;

	if (w == _texW && h == _texH)
		return false;

	_texW = w;
	_texH = h;

	if (renderTexture != nullptr)
		renderTexture->dispose();

	renderTexture = render::Texture::create(_context, _texW, _texH, false, false, false);
	
	if (_textureBuffer != nullptr)
	{
		_textureBuffer->clear();
		delete _textureBuffer;
	}

	const auto size = _texW * _texH * sizeof(int);
	_textureBuffer = new std::vector<unsigned char>(size, 0);
	
	renderTexture->data(&(*_textureBuffer)[0]);
	renderTexture->upload();

	return true;
}

bool
ChromiumRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
	uint w = _canvas->width();
	if (w > 2048)
		w = 2048;

	uint h = _canvas->height();
	if (h > 2048)
		h = 2048;

	rect = CefRect(0, 0, w, h);

	if (_lastW != w || _lastH != h)
	{
		generateTexture();
	}

	_lastW = w;
	_lastH = h;

	return true;
}

void
ChromiumRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
{
	if (width != _lastW || height != _lastH)
		return;
	if (_lastW > _texW || _lastH > _texH)
		return;

	if (renderTexture != NULL)
	{
		unsigned char* charBuffer = (unsigned char*) buffer;

		for (uint y = 0; y < _lastH; ++y)
		{
			if (y > 0)
				charBuffer += _lastW * sizeof(int);
			memcpy(&(*_textureBuffer)[y * _texW * sizeof(int)], charBuffer, _lastW * sizeof(int));
		}
		textureChanged = true;
	}
}

void
ChromiumRenderHandler::uploadTexture()
{
	if (textureChanged)
	{
		renderTexture->data(&(*_textureBuffer)[0]);
		renderTexture->upload();
	}
	textureChanged = false;
}

void
ChromiumRenderHandler::canvasResized(AbstractCanvas::Ptr canvas, uint w, uint h)
{
	generateTexture();
}
#endif
