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
#include "osxwebview/dom/OSXWebViewDOMMouseEvent.hpp"
#include "osxwebview/dom/OSXWebViewDOMElement.hpp"
#include "osxwebview/dom/OSXWebViewDOMEngine.hpp"

using namespace minko;
using namespace minko::dom;
using namespace osxwebview;
using namespace osxwebview::dom;

void
OSXWebViewDOMMouseEvent::preventDefault()
{
    std::cerr << "Warning : AbstractDOMEvent::preventDefault will have no effect" << std::endl;
    std::string js = _jsAccessor + ".preventDefault()";
    
    _engine->eval(js);
}

void
OSXWebViewDOMMouseEvent::stopPropagation()
{
    std::cerr << "Warning : AbstractDOMEvent::stopPropagation will have no effect" << std::endl;
    std::string js = _jsAccessor + ".stopPropagation()";
    
    _engine->eval(js);
}

std::string
OSXWebViewDOMMouseEvent::type()
{
    std::string js = "(" + _jsAccessor + ".type)";
    std::string result = _engine->eval(js);
    
    return std::string(result);
}

minko::dom::AbstractDOMElement::Ptr
OSXWebViewDOMMouseEvent::target()
{
    return OSXWebViewDOMElement::getDOMElement(_jsAccessor + ".target", _engine);
}

int
OSXWebViewDOMMouseEvent::clientX()
{
    std::string js = "(" + _jsAccessor + ".clientX)";
    int result = atoi(_engine->eval(js).c_str());
    
    return result;
}

int
OSXWebViewDOMMouseEvent::clientY()
{
    std::string js = "(" + _jsAccessor + ".clientY)";
    int result = atoi(_engine->eval(js).c_str());
    
    return result;
}

int
OSXWebViewDOMMouseEvent::pageX()
{
    std::string js = "(" + _jsAccessor + ".pageX)";
    int result = atoi(_engine->eval(js).c_str());
    
    return result;
}

int
OSXWebViewDOMMouseEvent::pageY()
{
    std::string js = "(" + _jsAccessor + ".pageY)";
    int result = atoi(_engine->eval(js).c_str());
    
    return result;
}

int
OSXWebViewDOMMouseEvent::layerX()
{
    std::string js = "(" + _jsAccessor + ".layerX)";
    int result = atoi(_engine->eval(js).c_str());
    
    return result;
}

int
OSXWebViewDOMMouseEvent::layerY()
{
    std::string js = "(" + _jsAccessor + ".layerY)";
    int result = atoi(_engine->eval(js).c_str());
    
    return result;
}

int
OSXWebViewDOMMouseEvent::screenX()
{
    std::string js = "(" + _jsAccessor + ".screenX)";
    int result = atoi(_engine->eval(js).c_str());
    
    return result;
}

int
OSXWebViewDOMMouseEvent::screenY()
{
    std::string js = "(" + _jsAccessor + ".screenY)";
    int result = atoi(_engine->eval(js).c_str());
    
    return result;
}