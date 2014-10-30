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

#include "minko/Common.hpp"
#include "apple/dom/AppleWebViewDOMEvent.hpp"
#include "apple/dom/AppleWebViewDOMElement.hpp"
#include "apple/dom/AppleWebViewDOMEngine.hpp"

using namespace minko;
using namespace minko::dom;
using namespace apple;
using namespace apple::dom;

std::string
AppleWebViewDOMEvent::type()
{
    std::string js = "(" + _jsAccessor + ".type)";
    std::string result = _engine->eval(js);

    return std::string(result);
}

minko::dom::AbstractDOMElement::Ptr
AppleWebViewDOMEvent::target()
{
    return AppleWebViewDOMElement::getDOMElement(_jsAccessor + ".target", _engine);
}

int
AppleWebViewDOMEvent::getProperty(const std::string& property)
{
    std::string eval = "(" + _jsAccessor + "." + property + ")";
    int result = atoi(_engine->eval(eval).c_str());
    return result;
}
