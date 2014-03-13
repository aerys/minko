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
using namespace minko::component;
using namespace minko::dom;
using namespace emscripten;
using namespace emscripten::dom;

//TODO with embind (https://github.com/kripken/emscripten/wiki/embind) :

//DOMEngineHelper(engineId);
//DOMEngine.onLoad = {EmscriptenDOMEngine.getEngine(engineId).onload.execute()}

// JS : Minko.DOMEngineHelper = new Module.DOMEngineHelper(this.engineId);
// JS : iFrame.onload = {Minko.DOMEngineHelper.onload()}

EmscriptenDOMEngine::EmscriptenDOMEngine() :
	_loadedPreviousFrameState(0),
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create())
{
}

void
EmscriptenDOMEngine::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;
	initJavascript();

	_canvasResizedSlot = _canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		std::string eval = "";

		eval += "Minko.iframeElement.style.width = '" + std::to_string(w) + "px';\n";
		eval += "Minko.iframeElement.style.height = '" + std::to_string(h) + "px';\n";

		emscripten_run_script(eval.c_str());
	});

	_enterFrameSlot = _sceneManager->frameBegin()->connect([&](std::shared_ptr<component::SceneManager>, float, float)
	{
		enterFrame();
	});
}

void
EmscriptenDOMEngine::enterFrame()
{
	std::string eval = "(Minko.loaded)";
	int loadedState = emscripten_run_script_int(eval.c_str());

	if (loadedState == 1)
	{
		if (_currentDOM->initialized())
			_currentDOM = EmscriptenDOM::create();

		_currentDOM->initialized(true);

		_currentDOM->onload()->execute(_currentDOM, _currentDOM->fullUrl());
		_onload->execute(_currentDOM, _currentDOM->fullUrl());
	}

	eval = "Minko.loaded = 0";
	emscripten_run_script(eval.c_str());
}

void
EmscriptenDOMEngine::initJavascript()
{
	std::string eval = "";

	eval += "window.Minko = {};\n";
	eval +=	"Minko.loaded = -1\n\n";

	eval += "var canvasElement = document.getElementById('canvas');\n";
	eval += "var iframeElement = document.createElement('iframe');\n\n";

	eval += "iframeElement.id = 'canvasiframe';\n";
	eval += "iframeElement.className = 'emscripten';\n\n";

	eval += "iframeElement.style.width = canvasElement.clientWidth + 'px';\n";
	eval += "iframeElement.style.height = canvasElement.clientHeight + 'px';\n\n";

	eval += "iframeElement.style.backgroundColor = 'transparent';\n";
	eval += "iframeElement.allowTransparency = 'true';\n";
	eval += "iframeElement.frameBorder = '0';\n\n";

	eval += "iframeElement.style.position = 'relative';\n";
	eval += "canvasElement.parentNode.style.position = 'relative';\n\n";

	eval += "canvasElement.style.position = 'absolute';\n";
	eval += "canvasElement.style.left = '0';\n";
	eval += "canvasElement.style.right = '0';\n\n";

	eval += "canvasElement.parentNode.appendChild(iframeElement);\n\n";

	eval += "Minko.iframeElement = iframeElement;\n";
	eval += "Minko.canvasElement = canvasElement;\n\n";

	eval += "Minko.getOffsetTop = function(element)\n";
	eval += "{\n";
	eval += "	var result = 0;\n";
	eval += "	while(element){\n";
	eval += "		result += element.offsetTop;\n";
	eval += "		element = element.offsetParent;\n";
	eval += "	}\n";
	eval += "	return result;\n";
	eval += "};\n\n";

	eval += "Minko.getOffsetLeft = function(element)";
	eval += "{\n";
	eval += "	var result = 0;\n";
	eval += "	while(element){\n";
	eval += "		result += element.offsetLeft;\n";
	eval += "		element = element.offsetParent;\n";
	eval += "	}\n";
	eval += "	return result;\n";
	eval += "};\n\n";

	eval += "Minko.redispatchMouseEvent = function(event)\n";
	eval += "{\n";
	eval += "	var pageX = 1 + Minko.getOffsetLeft(Minko.iframeElement) + (event.pageX || event.layerX);\n";
	eval += "	var pageY = 1 + Minko.getOffsetTop(Minko.iframeElement) + (event.pageY || event.layerY);\n";

	eval += "	var screenX = pageX - document.body.scrollLeft;\n";
	eval += "	var screenY = pageY - document.body.scrollTop;\n";

	eval += "	var eventCopy = document.createEvent('MouseEvents');\n";
	eval += "	eventCopy.initMouseEvent(event.type, event.bubbles, event.cancelable, event.view, event.detail,\n";
	eval += "		pageX, pageY, screenX, screenY, \n";
	eval += "		event.ctrlKey, event.altKey, event.shiftKey, event.metaKey, event.button, event.relatedTarget);\n";
	eval += "	Minko.canvasElement.dispatchEvent(eventCopy);\n";
	eval += "}\n\n";

	eval += "Minko.iframeLoadHandler = function(event)\n";
	eval += "{\n";
	eval += "	if(Minko.loaded == -1)\n";
	eval += "		return;\n\n";

	eval += "	Minko.loaded = 1;\n";
	eval += "	console.log('onload');\n";

	eval += "	Minko.document = iframeElement.contentDocument;\n";
	eval += "	Minko.window = iframeElement.contentWindow;\n";

	eval += "	Minko.window.addEventListener('mousemove',	Minko.redispatchMouseEvent);\n";
	eval += "	Minko.window.addEventListener('mouseup',	Minko.redispatchMouseEvent);\n";
	eval += "	Minko.window.addEventListener('mousedown',	Minko.redispatchMouseEvent);\n";
	eval += "	Minko.window.addEventListener('click',		Minko.redispatchMouseEvent);\n";
	eval += "	Minko.window.addEventListener('mouseover',	Minko.redispatchMouseEvent);\n";
	eval += "	Minko.window.addEventListener('mouseout',	Minko.redispatchMouseEvent);\n";
	eval += "	Minko.window.addEventListener('mousewheel',	Minko.redispatchMouseEvent);\n";
	eval += "}\n\n";

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
	bool isHttp		= uri.substr(0, 7) == "http://";
	bool isHttps	= uri.substr(0, 8) == "https://";

	if (!isHttp && !isHttps)
		uri = "asset/" + uri;

	std::string eval = "";

	eval += "Minko.loaded = 0;\n";
	eval += "Minko.iframeElement.src = '" + uri + "';\n";

	emscripten_run_script(eval.c_str());

	_currentDOM = EmscriptenDOM::create();

	return _currentDOM;
}

void
EmscriptenDOMEngine::clear()
{
	std::string eval = "";

	eval += "var iframeParent = Minko.iframeElement.parentNode;\n";
	eval += "iframeParent.removeChild(Minko.iframeElement);";

	emscripten_run_script(eval.c_str());
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
EmscriptenDOMEngine::onload()
{
	return _onload;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
EmscriptenDOMEngine::onmessage()
{
	return _onmessage;
}
#endif