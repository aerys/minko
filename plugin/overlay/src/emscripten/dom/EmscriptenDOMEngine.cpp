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

#if defined(EMSCRIPTEN)

#include "minko/Common.hpp"
#include "emscripten/dom/EmscriptenDOMEngine.hpp"
#include "emscripten/emscripten.h"

using namespace minko;
using namespace minko::dom;
using namespace emscripten;
using namespace emscripten::dom;

EmscriptenDOMEngine::EmscriptenDOMEngine()
{
	std::string eval = "";

	eval += "window.Minko = {};\n";

	eval += "var canvasElement = document.getElementById('canvas');\n";
	eval += "var iframeElement = document.createElement('iframe');\n";

	eval += "iframeElement.id = 'canvasiframe';\n";
	eval += "iframeElement.className = 'emscripten';\n";

	eval += "iframeElement.style.width = canvasElement.clientWidth + 'px';\n";
	eval += "iframeElement.style.height = canvasElement.clientHeight + 'px';\n";

	eval += "iframeElement.style.backgroundColor = 'transparent';\n";
	eval += "iframeElement.allowTransparency = 'true';\n";
	eval += "iframeElement.frameBorder = '0';\n";

	eval += "iframeElement.style.position = 'relative';\n";
	eval += "canvasElement.parentNode.style.position = 'relative';\n";

	eval += "canvasElement.style.position = 'absolute';\n";
	eval += "canvasElement.style.left = '0';\n";
	eval += "canvasElement.style.right = '0';\n";

	eval += "canvasElement.parentNode.appendChild(iframeElement);\n";

	eval += "Minko.iframeElement = iframeElement;\n";
	eval += "Minko.canvasElement = canvasElement;\n";

	eval += "Minko.getOffsetTop = function(element){\n";
	eval += "	var result = 0;\n";
	eval += "	while(element){\n";
	eval += "		result += element.offsetTop;\n";
	eval += "		element = element.offsetParent;\n";
	eval += "	}\n";
	eval += "	return result;\n";
	eval += "};\n";

	eval += "Minko.getOffsetLeft = function(element){\n";
	eval += "	var result = 0;\n";
	eval += "	while(element){\n";
	eval += "		result += element.offsetLeft;\n";
	eval += "		element = element.offsetParent;\n";
	eval += "	}\n";
	eval += "	return result;\n";
	eval += "};\n";

	eval += "Minko.redispatchMouseEvent = function(event){\n";
	eval += "	console.log(event.clientX, event.clientY);\n";

	eval += "	var pageX = 1 + Minko.getOffsetLeft(Minko.iframeElement) + (event.pageX || event.layerX);\n";
	eval += "	var pageY = 1 + Minko.getOffsetTop(Minko.iframeElement) + (event.pageY || event.layerY);\n";

	eval += "	var screenX = pageX - document.body.scrollLeft;\n";
	eval += "	var screenY = pageY - document.body.scrollTop;\n";

	eval += "	var eventCopy = document.createEvent('MouseEvents');\n";
	eval += "	eventCopy.initMouseEvent(event.type, event.bubbles, event.cancelable, event.view, event.detail,\n";
	eval += "		pageX, pageY, screenX, screenY, \n";
	eval += "		event.ctrlKey, event.altKey, event.shiftKey, event.metaKey, event.button, event.relatedTarget);\n";
	eval += "	Minko.canvasElement.dispatchEvent(eventCopy);\n";
	eval += "}\n";

	eval += "Minko.iframeLoadHandler = function(event){\n";
	eval += "	console.log('onload');\n";
	eval += "	iframeElement.contentWindow.onmousemove = Minko.redispatchMouseEvent;\n";
	eval += "	iframeElement.contentWindow.onmouseup = Minko.redispatchMouseEvent;\n";
	eval += "	iframeElement.contentWindow.onmousedown = Minko.redispatchMouseEvent;\n";
	eval += "	iframeElement.contentWindow.onclick = Minko.redispatchMouseEvent;\n";
	eval += "	iframeElement.contentWindow.onmouseover = Minko.redispatchMouseEvent;\n";
	eval += "	iframeElement.contentWindow.onmouseout = Minko.redispatchMouseEvent;\n";
	eval += "	iframeElement.contentWindow.onmousewheel = Minko.redispatchMouseEvent;\n";
	eval += "}\n";

	eval += "iframeElement.src = 'asset/html/menu.html';\n";

	eval += "iframeElement.onload = Minko.iframeLoadHandler;\n";

	emscripten_run_script(eval.c_str());
}

EmscriptenDOMEngine::Ptr
EmscriptenDOMEngine::create()
{
	EmscriptenDOMEngine::Ptr engine(new EmscriptenDOMEngine);
	return engine;
}

AbstractDOM::Ptr
EmscriptenDOMEngine::load(std::string uri)
{
	return nullptr;
}

void
EmscriptenDOMEngine::clear()
{
}

minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
EmscriptenDOMEngine::onload()
{
	return nullptr;
}

minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
EmscriptenDOMEngine::onmessage()
{
	return nullptr;
}
#endif