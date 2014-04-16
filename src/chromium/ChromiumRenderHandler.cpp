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
	_popupBuffer(nullptr),
	renderTexture(nullptr),
	textureChanged(false),
	_popupShown(false)
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
ChromiumRenderHandler::OnPopupShow(CefRefPtr<CefBrowser> browser, bool shown)
{
	_popupShown = shown;

	if (!_popupShown)
	{
		_popupBuffer = nullptr;

		_popupX = 0;
		_popupY = 0;
		_popupW = 0;
		_popupH = 0;
	}
}

void
ChromiumRenderHandler::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect &rect)
{
	_popupX = rect.x;
	_popupY = rect.y;
	_popupW = rect.width;
	_popupH = rect.height;
}

void
ChromiumRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
{
	if (renderTexture == NULL)
		return;

	if (type == PaintElementType::PET_VIEW) //Main View
	{
		if (width != _lastW || height != _lastH)
			return;
		if (_lastW > _texW || _lastH > _texH)
			return;

		unsigned char* charBuffer = (unsigned char*)buffer;

		for (uint y = 0; y < _lastH; ++y)
		{
			if (y > 0)
				charBuffer += _lastW * sizeof(int);
			memcpy(&(*_textureBuffer)[y * _texW * sizeof(int)], charBuffer, _lastW * sizeof(int));
		}
		textureChanged = true;
	}
	else if (type == PaintElementType::PET_POPUP) //Popup
	{
		if (width != _popupW || height != _popupH)
			return;
				
		_popupBuffer = (unsigned char*)buffer;

		textureChanged = true;
	}
}

void
ChromiumRenderHandler::drawPopup()
{
	if (!_popupShown || _popupBuffer == nullptr)
		return;

	unsigned char* charBuffer = _popupBuffer;

	int popupYMax = _popupY + _popupH;

	for (uint y = _popupY; y < popupYMax && y < _lastH; ++y)
	{
		if (y > _popupY)
			charBuffer += _popupW * sizeof(int);
		
		uint offset = (y * _texW) + _popupX;

		memcpy(&(*_textureBuffer)[offset * sizeof(int)], charBuffer, _popupW * sizeof(int));
	}
}

void
ChromiumRenderHandler::uploadTexture()
{
	if (textureChanged)
	{
		if (_popupShown)
			drawPopup();

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

void
ChromiumRenderHandler::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor)
{
#if defined(_MSC_VER) 
	SetCursor(cursor);
#endif
}
#endif
