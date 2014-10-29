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
#include "apple/dom/AppleWebViewDOMEngine.hpp"
#include "apple/dom/AppleWebViewDOMTouchEvent.hpp"

using namespace minko;
using namespace minko::dom;
using namespace apple;
using namespace apple::dom;

int
AppleWebViewDOMTouchEvent::clientX()
{
    return getProperty("changedTouches[" + std::to_string(_changedTouchesIndex) + "].clientX");
}

int
AppleWebViewDOMTouchEvent::clientY()
{
    return getProperty("changedTouches[" + std::to_string(_changedTouchesIndex) + "].clientY");
}

int
AppleWebViewDOMTouchEvent::pageX()
{
    return getProperty("changedTouches[" + std::to_string(_changedTouchesIndex) + "].pageX");
}

int
AppleWebViewDOMTouchEvent::pageY()
{
    return getProperty("changedTouches[" + std::to_string(_changedTouchesIndex) + "].pageY");
}

int
AppleWebViewDOMTouchEvent::layerX()
{
    return getProperty("changedTouches[" + std::to_string(_changedTouchesIndex) + "].layerX");
}

int
AppleWebViewDOMTouchEvent::layerY()
{
    return getProperty("changedTouches[" + std::to_string(_changedTouchesIndex) + "].layerY");
}

int
AppleWebViewDOMTouchEvent::screenX()
{
    return getProperty("changedTouches[" + std::to_string(_changedTouchesIndex) + "].screenX");
}

int
AppleWebViewDOMTouchEvent::screenY()
{
    return getProperty("changedTouches[" + std::to_string(_changedTouchesIndex) + "].screenY");
}

int
AppleWebViewDOMTouchEvent::identifier()
{
    return getProperty("changedTouches[" + std::to_string(_changedTouchesIndex) + "].identifier % 2147483647");
}

