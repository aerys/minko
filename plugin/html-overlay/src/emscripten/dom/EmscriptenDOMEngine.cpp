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

#ifdef MINKO_PLUGIN_HTTP_LOADER
# include "minko/MinkoHTTP.hpp"
#endif

#include "minko/file/Options.hpp"
#include "minko/file/FileProtocol.hpp"
#include "minko/file/AssetLibrary.hpp"
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
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_visible(true)
{
}

void
EmscriptenDOMEngine::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;


	loadScript("script/minko.overlay.js");

	std::string eval = "Minko.init('emscripten');";
	emscripten_run_script(eval.c_str());

	visible(_visible);

	_canvasResizedSlot = _canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		std::string ev = "";

		ev += "Minko.iframe.style.width = '" + std::to_string(w) + "px';\n";
		ev += "Minko.iframe.style.height = '" + std::to_string(h) + "px';\n";

		emscripten_run_script(ev.c_str());
	});

	_enterFrameSlot = _sceneManager->frameBegin()->connect([&](std::shared_ptr<component::SceneManager>, float, float)
	{
		enterFrame();
	}, -100.f);
}

void
EmscriptenDOMEngine::loadScript(std::string filename)
{
    auto path = filename;
    auto options = _sceneManager->assets()->loader()->options()->clone();
    auto loader = file::Loader::create();

    options
#ifdef MINKO_PLUGIN_HTTP_LOADER
        ->registerProtocol<net::HTTPProtocol>("http")
        ->registerProtocol<net::HTTPProtocol>("https")
#endif //MINKO_PLUGIN_HTTP_LOADER
    	->loadAsynchronously(false)
    	->storeDataIfNotParsed(false);

    options->protocolFunction([=](const std::string&) -> std::shared_ptr<minko::file::AbstractProtocol>
    {
        auto defaultProtocol = options->getProtocol("http");

        defaultProtocol->options(options->clone());

        return defaultProtocol;
    });

    loader->options(options);

    auto loaderComplete = loader->complete()->connect([=](file::Loader::Ptr loaderThis)
    {
		const auto& data = loaderThis->files().at(path)->data();
		const auto eval = std::string(data.begin(), data.end());

		emscripten_run_script(eval.c_str());
	});

	loader
		->queue(path)
		->load();
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
		std::string eval = "Minko._tempMessagesToSend = Minko.messagesToSend.concat();Minko.messagesToSend = [];";
		emscripten_run_script(eval.c_str());

		eval = "(Minko._tempMessagesToSend.length);";
		int l = emscripten_run_script_int(eval.c_str());

		if (l > 0)
		{
			for(int i = 0; i < l; ++i)
			{
				eval = "(Minko._tempMessagesToSend[" + std::to_string(i) + "])";
				char* charMessage = emscripten_run_script_string(eval.c_str());

				std::string message(charMessage);
				
				_currentDOM->onmessage()->execute(_currentDOM, message);
				_onmessage->execute(_currentDOM, message);
			}

			eval = "delete Minko._tempMessagesToSend";
			emscripten_run_script(eval.c_str());
		}
	}
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

	eval += "Minko.loadUrlEmscripten('" + uri + "')\n";

	emscripten_run_script(eval.c_str());

	createNewDom();

	return _currentDOM;
}

void
EmscriptenDOMEngine::clear()
{
	std::string eval = "";

	eval += "var iframeParent = Minko.iframe.parentNode;\n";
	eval += "iframeParent.removeChild(Minko.iframe);";

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

void
EmscriptenDOMEngine::visible(bool value)
{
	if (_canvas != nullptr)
	{
		std::string eval = "";

		std::string visibility = "visible";

		if (!value)
			visibility = "hidden";

		eval += "Minko.iframe.style.visibility = '" + visibility + "';\n";

		emscripten_run_script(eval.c_str());
	}

	_visible = value;
}

bool
EmscriptenDOMEngine::visible()
{
	return _visible;
}