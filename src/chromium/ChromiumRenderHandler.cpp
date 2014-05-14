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
	_popupBuffer(nullptr),
	renderTexture(nullptr),
	textureChanged(false),
	_popupUpdated(false),
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
	
	if (w > 4096)
		w = 4096;

	if (h > 4096)
		h = 4096;

	if (w == _texW && h == _texH)
		return false;

	_texW = w;
	_texH = h;

	if (renderTexture != nullptr)
		renderTexture->dispose();

	renderTexture = render::Texture::create(_context, _texW, _texH, false, false, false);
	
	const auto size = _texW * _texH * sizeof(int);
	auto textureBuffer = new std::vector<unsigned char>(size, 0);
	
	renderTexture->data(&(*textureBuffer)[0]);
	renderTexture->upload();

	textureBuffer->clear();
	textureBuffer->shrink_to_fit();

	return true;
}


bool
ChromiumRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
	uint w = _canvas->width();
	if (w > 4096)
		w = 4096;

	uint h = _canvas->height();
	if (h > 4096)
		h = 4096;

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


		for (auto rect : dirtyRects)
			drawRect((unsigned char*)buffer, rect.x, rect.y, rect.width, rect.height);

		if (!_popupShown && _popupW != 0 && _popupH != 0)
		{
			drawRect((unsigned char*)buffer, _popupX, _popupY, _popupW, _popupH);

			_popupX = 0;
			_popupY = 0;
			_popupW = 0;
			_popupH = 0;
		}

		textureChanged = true;
	}
	else if (type == PaintElementType::PET_POPUP) //Popup
	{
		if (width != _popupW || height != _popupH)
			return;
				
		_popupBuffer = (unsigned char*)buffer;

		_popupUpdated = true;

		textureChanged = true;
	}

	uploadTexture();
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

		memcpy(&(renderTexture->data())[offset * sizeof(int)], charBuffer, _popupW * sizeof(int));
	}
}


void
ChromiumRenderHandler::drawRect(unsigned char* source, int x, int y, int w, int h)
{
	uint yMax = y + h;

	source += ((y * _lastW) + x) * sizeof(int);

	for (uint currentY = y; currentY < yMax && currentY < _lastH; ++currentY)
	{
		memcpy(&(renderTexture->data())[((currentY * _texW) + x) * sizeof(int)], source, w * sizeof(int));

		source += _lastW * sizeof(int);
	}
}

void
ChromiumRenderHandler::uploadTexture()
{
	if (textureChanged)
	{
		if (_popupShown && _popupUpdated)
			drawPopup();

		//renderTexture->data(&(*_textureBuffer)[0]);
		renderTexture->upload();
	}
	textureChanged = false;
	_popupUpdated = false;
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
