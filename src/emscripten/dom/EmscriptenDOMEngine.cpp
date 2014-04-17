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

int
EmscriptenDOMEngine::_domUid = 0;

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
EmscriptenDOMEngine::createNewDom()
{
	std::string domName = "Minko.dom" + std::to_string(_domUid++);

	std::string eval = domName + " = {};";
	emscripten_run_script(eval.c_str());
	
	_currentDOM = EmscriptenDOM::create(domName);
}


minko::dom::AbstractDOM::Ptr
EmscriptenDOMEngine::mainDOM()
{
	return _currentDOM;
}

void
EmscriptenDOMEngine::enterFrame()
{
	std::string eval = "(Minko.loaded)";
	int loadedState = emscripten_run_script_int(eval.c_str());

	if (loadedState == 1)
	{
		if (_currentDOM->initialized())
			createNewDom();

		_currentDOM->initialized(true);

		_currentDOM->onload()->execute(_currentDOM, _currentDOM->fullUrl());
		_onload->execute(_currentDOM, _currentDOM->fullUrl());

		eval = "Minko.loaded = 0";
		emscripten_run_script(eval.c_str());
	}

	for(auto element : EmscriptenDOMElement::domElements)
	{
		element->update();
	}

	if (_currentDOM->initialized())
	{
		std::string eval = "(Minko.iframeElement.contentWindow.Minko.messagesToSend.length);";
		int l = emscripten_run_script_int(eval.c_str());

		if (l > 0)
		{
			for(int i = 0; i < l; ++i)
			{
				std::string eval = "(Minko.iframeElement.contentWindow.Minko.messagesToSend[" + std::to_string(i) + "])";
				char* charMessage = emscripten_run_script_string(eval.c_str());
				
				std::string message(charMessage);

				_currentDOM->onmessage()->execute(_currentDOM, message);
				_onmessage->execute(_currentDOM, message);
			}

			std::string eval = "Minko.iframeElement.contentWindow.Minko.messagesToSend = [];";
			emscripten_run_script(eval.c_str());
		}
	}
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

	eval += "Minko.redispatchKeyboardEvent = function(event)\n";
	eval += "{\n";
	eval += "	var eventCopy = document.createEvent('Event');\n";

	eval += "	eventCopy.initEvent(event.type, event.bubbles, event.cancelable);\n";

	eval += "	eventCopy.type = event.type;\n";
	eval += "	eventCopy.bubbles = event.bubbles;\n";
	eval += "	eventCopy.cancelable = event.cancelable;\n";
	eval += "	eventCopy.view = event.view;\n";
	eval += "	eventCopy.ctrlKey = event.ctrlKey;\n";
	eval += "	eventCopy.altKey = event.altKey;\n";
	eval += "	eventCopy.shiftKey = event.shiftKey;\n";
	eval += "	eventCopy.metaKey = event.metaKey;\n";
	eval += "	eventCopy.keyCode = event.keyCode;\n";
	eval += "	eventCopy.charCode = event.charCode;\n";
	eval += "	eventCopy.which = event.which;\n";
	eval += "	eventCopy.key = event.key;\n";
	eval += "	eventCopy.detail = event.detail;\n";
	eval += "	eventCopy.keyIdentifier = event.keyIdentifier;\n";

	eval += "	document.dispatchEvent(eventCopy);\n";
	eval += "}\n";


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

	eval += "	if (event.type == 'mousewheel')\n";
	eval += "	{\n";
	eval += "		eventCopy.detail = event.detail;\n";

	eval += "		eventCopy.wheelDelta = event.wheelDelta;\n";
	eval += "		eventCopy.wheelDeltaX = event.wheelDeltaX;\n";
	eval += "		eventCopy.wheelDeltaY = event.wheelDeltaY;\n";
	eval += "		eventCopy.wheelDeltaZ = event.wheelDeltaZ;\n";

	eval += "		eventCopy.delta = event.delta;\n";
	eval += "		eventCopy.deltaMode = event.deltaMode;\n";
	eval += "		eventCopy.deltaX = event.deltaX;\n";
	eval += "		eventCopy.deltaY = event.deltaY;\n";
	eval += "		eventCopy.deltaZ = event.deltaZ;\n";
	eval +=	"		event.preventDefault();\n";
	eval += "	}\n";

	eval += "	Minko.canvasElement.dispatchEvent(eventCopy);\n";

	eval += "}\n\n";

	eval += "Minko.iframeLoadHandler = function(event)\n";
	eval += "{\n";
	eval += "	if(Minko.loaded == -1)\n";
	eval += "		return;\n\n";

	eval += "	Minko.loaded = 1;\n";
	eval +=	"	if (!Minko.iframeElement.contentWindow.Minko)\n";
	eval += "		Minko.iframeElement.contentWindow.Minko = {};\n";

	eval += "	Minko.iframeElement.contentWindow.document.body.oncontextmenu = function(event){ event.preventDefault(); return false;};\n";

	eval += "	if (!Minko.iframeElement.contentWindow.Minko.onmessage)\n";
	eval +=	"	{\n";
	eval +=	"		Minko.iframeElement.contentWindow.Minko.onmessage = function(message)\n";
	eval +=	"		{\n";
	eval +=	"			console.log('MINKO: ' + message);\n";
	eval +=	"		}\n";
	eval +=	"	}\n";
	eval +=	"	Minko.iframeElement.contentWindow.Minko.messagesToSend = [];\n";

	eval += "	Minko.iframeElement.contentWindow.Minko.sendMessage = function(message)\n";
	eval +=	"	{\n";
	eval +=	"		Minko.iframeElement.contentWindow.Minko.messagesToSend.push(message);\n";
	eval +=	"	}\n";

	eval += "	Minko.iframeElement.contentWindow.addEventListener('mousemove',		Minko.redispatchMouseEvent);\n";
	eval += "	Minko.iframeElement.contentWindow.addEventListener('mouseup',		Minko.redispatchMouseEvent);\n";
	eval += "	Minko.iframeElement.contentWindow.addEventListener('mousedown',		Minko.redispatchMouseEvent);\n";
	eval += "	Minko.iframeElement.contentWindow.addEventListener('click',			Minko.redispatchMouseEvent);\n";
	eval += "	Minko.iframeElement.contentWindow.addEventListener('mouseover',		Minko.redispatchMouseEvent);\n";
	eval += "	Minko.iframeElement.contentWindow.addEventListener('mouseout',		Minko.redispatchMouseEvent);\n";
	eval += "	Minko.iframeElement.contentWindow.addEventListener('mousewheel',	Minko.redispatchMouseEvent);\n";
	eval += "	Minko.iframeElement.contentWindow.addEventListener('keydown',		Minko.redispatchKeyboardEvent);\n";
	eval += "	Minko.iframeElement.contentWindow.addEventListener('keyup',			Minko.redispatchKeyboardEvent);\n";
	eval += "	Minko.iframeElement.contentWindow.addEventListener('keypress',		Minko.redispatchKeyboardEvent);\n";
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

	createNewDom();

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