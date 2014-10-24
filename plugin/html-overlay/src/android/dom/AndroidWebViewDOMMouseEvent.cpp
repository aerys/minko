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

#include "minko/Common.hpp"
#include "android/dom/AndroidWebViewDOMMouseEvent.hpp"
#include "android/dom/AndroidWebViewDOMElement.hpp"

using namespace minko;
using namespace minko::dom;
using namespace android;
using namespace android::dom;

int
AndroidWebViewDOMMouseEvent::clientX()
{
	return _clientX;
}

int
AndroidWebViewDOMMouseEvent::clientY()
{
	return _clientY;
}

int
AndroidWebViewDOMMouseEvent::pageX()
{
	return _pageX;
}

int
AndroidWebViewDOMMouseEvent::pageY()
{
	return _pageY;
}

int
AndroidWebViewDOMMouseEvent::layerX()
{
	return _layerX;
}

int
AndroidWebViewDOMMouseEvent::layerY()
{
	return _layerY;
}

int
AndroidWebViewDOMMouseEvent::screenX()
{
	return _screenX;
}

int
AndroidWebViewDOMMouseEvent::screenY()
{
	return _screenY;
}

void
AndroidWebViewDOMMouseEvent::clientX(int value)
{
    _clientX = value;
}

void
AndroidWebViewDOMMouseEvent::clientY(int value)
{
    _clientY = value;
}

void
AndroidWebViewDOMMouseEvent::pageX(int value)
{
	_pageX = value;
}

void
AndroidWebViewDOMMouseEvent::pageY(int value)
{
    _pageY = value;
}

void
AndroidWebViewDOMMouseEvent::layerX(int value)
{
	_layerX = value;
}

void
AndroidWebViewDOMMouseEvent::layerY(int value)
{
	_layerY = value;
}

void
AndroidWebViewDOMMouseEvent::screenX(int value)
{
	_screenX = value;
}

void
AndroidWebViewDOMMouseEvent::screenY(int value)
{
	_screenY = value;
}