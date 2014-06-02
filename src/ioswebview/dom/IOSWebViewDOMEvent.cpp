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

#if defined(TARGET_IPHONE_SIMULATOR) or defined(TARGET_OS_IPHONE) // iOS

#include "minko/Common.hpp"
#include "ioswebview/dom/IOSWebViewDOMEvent.hpp"
#include "ioswebview/dom/IOSWebViewDOMElement.hpp"
#include "ioswebview/dom/IOSWebViewDOMEngine.hpp"

using namespace minko;
using namespace minko::dom;
using namespace ioswebview;
using namespace ioswebview::dom;

void
IOSWebViewDOMEvent::preventDefault()
{
	std::cerr << "Warning : AbstractDOMEvent::preventDefault will have no effect" << std::endl;
	std::string js = _jsAccessor + ".preventDefault()";
	
    _engine->eval(js);
}

void
IOSWebViewDOMEvent::stopPropagation()
{
	std::cerr << "Warning : AbstractDOMEvent::stopPropagation will have no effect" << std::endl;
	std::string js = _jsAccessor + ".stopPropagation()";
	
    _engine->eval(js);
}

std::string
IOSWebViewDOMEvent::type()
{
	std::string js = "(" + _jsAccessor + ".type)";
    std::string result = _engine->eval(js);

	return std::string(result);
}

minko::dom::AbstractDOMElement::Ptr
IOSWebViewDOMEvent::target()
{
	return IOSWebViewDOMElement::getDOMElement(_jsAccessor + ".target", _engine);
}

int
IOSWebViewDOMEvent::clientX()
{
	std::string js = "(" + _jsAccessor + ".touches[0]." + "clientX)";
	int result = atoi(_engine->eval(js).c_str());
    
	return result;
}

int
IOSWebViewDOMEvent::clientY()
{
	std::string js = "(" + _jsAccessor + ".touches[0]" + ".clientY)";
	int result = atoi(_engine->eval(js).c_str());
    
	return result;
}

int
IOSWebViewDOMEvent::pageX()
{
	std::string js = "(" + _jsAccessor + ".pageX)";
	int result = atoi(_engine->eval(js).c_str());
    
	return result;
}

int
IOSWebViewDOMEvent::pageY()
{
	std::string js = "(" + _jsAccessor + ".pageY)";
	int result = atoi(_engine->eval(js).c_str());
    
	return result;
}

int
IOSWebViewDOMEvent::layerX()
{
	std::string js = "(" + _jsAccessor + ".layerX)";
	int result = atoi(_engine->eval(js).c_str());
    
	return result;
}

int
IOSWebViewDOMEvent::layerY()
{
	std::string js = "(" + _jsAccessor + ".layerY)";
	int result = atoi(_engine->eval(js).c_str());
    
	return result;
}

int
IOSWebViewDOMEvent::screenX()
{
	std::string js = "(" + _jsAccessor + ".screenX)";
	int result = atoi(_engine->eval(js).c_str());
    
	return result;
}

int
IOSWebViewDOMEvent::screenY()
{
	std::string js = "(" + _jsAccessor + ".screenY)";
	int result = atoi(_engine->eval(js).c_str());
    
	return result;
}
#endif