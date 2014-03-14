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

#if defined(CHROMIUM)
#include "minko/Minko.hpp"

#include "chromium/ChromiumApp.hpp"
#include "chromium/ChromiumPimpl.hpp"
#include "chromium/dom/ChromiumDOMEngine.hpp"
#include "chromium/dom/ChromiumDOMElement.hpp"
#include "minko/dom/AbstractDOMElement.hpp"

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_render_handler.h"

using namespace chromium;
using namespace chromium::dom;
using namespace minko;
using namespace minko::dom;

ChromiumDOMEngine::ChromiumDOMEngine() :
	_onload(minko::Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(minko::Signal<AbstractDOM::Ptr, std::string>::create()),
	_cleared(false)
{
	_impl = new ChromiumPimpl();
}

ChromiumDOMEngine::~ChromiumDOMEngine()
{
	clear();
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
	settings.no_sandbox = 1;

	int result = CefInitialize(*_impl->mainArgs, settings, _impl->app.get(), nullptr);
	
	auto options = minko::file::Options::create(_sceneManager->assets()->defaultOptions());
	options->loadAsynchronously(false);
	
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

	_enterFrameSlot = _sceneManager->frameBegin()->connect([&](std::shared_ptr<component::SceneManager>, float, float)
	{
		enterFrame();
	});
}

void
ChromiumDOMEngine::enterFrame()
{
	CefDoMessageLoopWork();
	for (auto dom : _doms)
		dom->update();
	_impl->renderHandler->uploadTexture();
}

AbstractDOM::Ptr
ChromiumDOMEngine::load(std::string uri)
{
	bool isHttp		= uri.substr(0, 7) == "http://";
	bool isHttps	= uri.substr(0, 8) == "https://";
	bool isFile		= uri.substr(0, 7) == "file://";

	if (isHttp || isHttps || isFile)
		loadHttp(uri);
	else
		loadLocal(uri);

	_impl->mainDOM = ChromiumDOM::create(shared_from_this());

	return _impl->mainDOM;
}

void
ChromiumDOMEngine::registerDom(ChromiumDOM::Ptr dom)
{
	_doms.push_back(dom);
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

	std::string url = path + "/" + filename;

	loadHttp(url);
}

void
ChromiumDOMEngine::clear()
{
	if (_cleared)
		return;

	_impl->mainDOM = nullptr;

	for (ChromiumDOM::Ptr dom : _doms)
		dom->clear();

	_doms.clear();

	ChromiumDOMElement::clearAll();
	ChromiumDOMEvent::clearAll();
	_impl->mainArgs = nullptr;
	_impl->browser = nullptr;
	_impl->app = nullptr;
	_impl->renderHandler = nullptr;
	_impl->renderProcessHandler = nullptr;
	_impl->domEngine = nullptr;
	_impl->v8Context = nullptr;

	_cleared = true;

	CefShutdown();
}
#endif