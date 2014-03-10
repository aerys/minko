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

#include "minko/Minko.hpp"

#include "chromium/ChromiumApp.hpp"
#include "chromium/ChromiumPimpl.hpp"
#include "chromium/dom/ChromiumDOMEngine.hpp"
#include "chromium/dom/ChromiumDOMElement.hpp"
#include "chromium/dom/ChromiumDOMEngineV8Handler.hpp"
#include "minko/dom/AbstractDOMElement.hpp"

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_render_handler.h"

using namespace chromium;
using namespace chromium::dom;
using namespace minko;
using namespace minko::dom;

ChromiumDOMEngine::ChromiumDOMEngine() :
	_v8Handler(new ChromiumDOMEngineV8Handler()),
	_onload(minko::Signal<std::string>::create()),
	_onmessage(minko::Signal<std::string>::create())
{
	_impl = new ChromiumPimpl();
}

ChromiumDOMEngine::~ChromiumDOMEngine()
{
	std::cout << "toto" << std::endl;
}

ChromiumDOMEngine::Ptr
ChromiumDOMEngine::create()
{
	Ptr engine(new ChromiumDOMEngine());
	engine->_impl->domEngine = engine;
	engine->start();

	return engine;
}

void
ChromiumDOMEngine::initNewPage(CefRefPtr<CefV8Context> context)
{
	_currentV8Context = context;

	_currentV8Context->Enter();

	_minkoObject = CefV8Value::CreateObject(nullptr);
	window()->SetValue("Minko", _minkoObject, V8_PROPERTY_ATTRIBUTE_NONE);

	addLoadEventListener();
	addSendMessageFunction();

	_currentV8Context->Exit();
}


CefRefPtr<CefV8Value>
ChromiumDOMEngine::window()
{
	return _currentV8Context->GetGlobal();
}


AbstractDOMElement::Ptr
ChromiumDOMEngine::document()
{
	return ChromiumDOMElement::getDOMElementFromV8Object(_currentV8Context->GetGlobal()->GetValue("document"));
}

AbstractDOMElement::Ptr
ChromiumDOMEngine::body()
{
	return ChromiumDOMElement::getDOMElementFromV8Object(_currentV8Context->GetGlobal()->GetValue("document")->GetValue("body"));
}

void
ChromiumDOMEngine::addSendMessageFunction()
{
	std::string sendMessageFunctionName = "sendMessage";

	// [javascript] Minko.sendMessage('message');
	CefRefPtr<CefV8Value> sendMessageFunction = CefV8Value::CreateFunction(sendMessageFunctionName, _v8Handler.get());
	_minkoObject->SetValue(sendMessageFunctionName, sendMessageFunction, V8_PROPERTY_ATTRIBUTE_NONE);

	_onmessageSlot = _v8Handler->received()->connect([=](std::string functionName, CefV8ValueList arguments)
	{
		if (functionName == sendMessageFunctionName)
		{
			CefRefPtr<CefV8Value> messageV8Value = arguments[0];
			std::string message = messageV8Value->GetStringValue();

			_onmessage->execute(message);
		}
	});
}

void
ChromiumDOMEngine::addLoadEventListener()
{
	std::string loadFunctionName = "onLoadHandler";
	CefRefPtr<CefV8Value> onloadFunction = CefV8Value::CreateFunction(loadFunctionName, _v8Handler.get());

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString("load"));
	args.push_back(onloadFunction);

	window()->GetValue("addEventListener")->ExecuteFunction(nullptr, args);

	_onloadSlot = _v8Handler->received()->connect([=](std::string functionName, CefV8ValueList)
	{
		//fixme: pass page name, relative to working directory
		if (functionName == loadFunctionName)
			_onload->execute("");
	});
}

void
ChromiumDOMEngine::start()
{
#ifdef _WIN32
	_impl->mainArgs = new CefMainArgs(GetModuleHandle(NULL));
#else
	_impl->mainArgs = new CefMainArgs(0, 0);
#endif
	_impl->app = new ChromiumApp();

	CefExecuteProcess(*_impl->mainArgs, _impl->app.get(), nullptr);
}


void
ChromiumDOMEngine::initialize(AbstractCanvas::Ptr canvas, std::shared_ptr<component::SceneManager> sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;
	std::shared_ptr<render::Texture> texture = _impl->app->initialize(canvas, _sceneManager->assets()->context(), _impl);

	CefSettings settings;

	settings.single_process = 1;

	int result = CefInitialize(*_impl->mainArgs, settings, _impl->app.get(), nullptr);
	
	auto overlayEffect = _sceneManager->assets()->effect("effect/Overlay.effect");

	if (!overlayEffect)
		throw std::logic_error("Overlay.effect has not been loaded.");

	_overlayMaterial = material::BasicMaterial::create()->diffuseMap(texture);

	auto quad = scene::Node::create("quad")
		->addComponent(component::Surface::create(
		geometry::QuadGeometry::create(sceneManager->assets()->context()),
		_overlayMaterial,
		overlayEffect
		));

	_sceneManager->getTarget(0)->addChild(quad);

	float wRatio = (float)canvas->width() / (float)math::clp2(canvas->width());
	float hRatio = (float)canvas->height() / (float)math::clp2(canvas->height());
	_overlayMaterial->set("overlayRatio", math::Vector2::create(wRatio, hRatio));

	_canvasResizedSlot = _canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		_overlayMaterial->unset("diffuseMap");
		_impl->browser->GetHost()->WasResized();

		float wRatio = (float)canvas->width() / (float)math::clp2(canvas->width());
		float hRatio = (float)canvas->height() / (float)math::clp2(canvas->height());
		_overlayMaterial->set("overlayRatio", math::Vector2::create(wRatio, hRatio));
		_overlayMaterial->diffuseMap(_impl->renderHandler->renderTexture);
		enterFrame();
	});

	_enterFrameSlot = _sceneManager->frameBegin()->connect([&](std::shared_ptr<component::SceneManager>)
	{
		enterFrame();
	});
}

void
ChromiumDOMEngine::enterFrame()
{
	CefDoMessageLoopWork();
	_impl->renderHandler->uploadTexture();
}

void
ChromiumDOMEngine::load(std::string uri)
{
	bool isHttp		= uri.substr(0, 7) == "http://";
	bool isHttps	= uri.substr(0, 8) == "https://";
	bool isFile		= uri.substr(0, 7) == "file://";

	if (isHttp || isHttps || isFile)
		loadHttp(uri);
	else
		loadLocal(uri);
}

void
ChromiumDOMEngine::loadHttp(std::string url)
{
	_impl->browser.get()->GetMainFrame()->LoadURL(url);
}

void
ChromiumDOMEngine::loadLocal(std::string filename)
{
	std::string path = getWorkingDirectory() + "/";

	//Fixme : find a more elegant way ?
#if DEBUG
	path += "../../../asset/";
#endif

	std::string url = "file://" + path + "/" + filename;

	loadHttp(url);
}

void
ChromiumDOMEngine::unload()
{
	_currentV8Context = nullptr;
	_v8Handler = nullptr;
	_minkoObject = nullptr;

	ChromiumDOMElement::clearAll();
	ChromiumDOMEvent::clearAll();

	_impl->mainArgs = nullptr;
	_impl->browser = nullptr;
	_impl->app = nullptr;
	_impl->renderHandler = nullptr;
	_impl->renderProcessHandler = nullptr;
	_impl->domEngine = nullptr;
	_impl->v8Context = nullptr;

	CefShutdown();
}

AbstractDOMElement::Ptr
ChromiumDOMEngine::createElement(std::string element)
{
	CefRefPtr<CefV8Value> document = window()->GetValue("document");
	CefRefPtr<CefV8Value> func = document->GetValue("createElement");

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString(element));

	CefRefPtr<CefV8Value> result = func->ExecuteFunction(document, args);

	return ChromiumDOMElement::create(result);
}

AbstractDOMElement::Ptr
ChromiumDOMEngine::getElementById(std::string id)
{
	CefRefPtr<CefV8Value> document = window()->GetValue("document");
	CefRefPtr<CefV8Value> func = document->GetValue("getElementById");

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString(id));

	CefRefPtr<CefV8Value> result = func->ExecuteFunction(document, args);

	return ChromiumDOMElement::getDOMElementFromV8Object(result);
}

std::list<AbstractDOMElement::Ptr>
ChromiumDOMEngine::getElementsByClassName(std::string className)
{
	CefRefPtr<CefV8Value> document = window()->GetValue("document");
	CefRefPtr<CefV8Value> func = document->GetValue("getElementsByClassName");

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString(className));

	CefRefPtr<CefV8Value> result = func->ExecuteFunction(document, args);

	return ChromiumDOMElement::v8ElementArrayToList(result);
}

std::list<AbstractDOMElement::Ptr>
ChromiumDOMEngine::getElementsByTagName(std::string tagName)
{
	CefRefPtr<CefV8Value> document = window()->GetValue("document");
	CefRefPtr<CefV8Value> func = document->GetValue("getElementsByTagName");

	CefV8ValueList args;
	args.push_back(CefV8Value::CreateString(tagName));

	CefRefPtr<CefV8Value> result = func->ExecuteFunction(document, args);

	return ChromiumDOMElement::v8ElementArrayToList(result);
}

