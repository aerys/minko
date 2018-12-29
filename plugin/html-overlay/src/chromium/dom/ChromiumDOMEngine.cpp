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

#include "minko/scene/Node.hpp"

#include "chromium/ChromiumApp.hpp"
#include "chromium/ChromiumPimpl.hpp"
#include "chromium/dom/ChromiumDOMEngine.hpp"
#include "chromium/dom/ChromiumDOMElement.hpp"
#include "minko/dom/AbstractDOMElement.hpp"
#include "minko/file/AssetLibrary.hpp"

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
	_cleared(false),
	_visible(true),
	_chromiumInitialized(false),
	_quad(nullptr)
{
	_impl = new ChromiumPimpl();
}

ChromiumDOMEngine::~ChromiumDOMEngine()
{
	clear();
}

ChromiumDOMEngine::Ptr
ChromiumDOMEngine::create(int argc, char** argv)
{
	Ptr engine(new ChromiumDOMEngine());
	engine->_impl->domEngine = engine;

	engine->start(argc, argv);

	return engine;
}

void
ChromiumDOMEngine::start(int argc, char** argv)
{
#ifdef _WIN32
	_impl->mainArgs = new CefMainArgs(GetModuleHandle(NULL));
#else
	_impl->mainArgs = new CefMainArgs(argc, argv);
#endif
	_impl->app = new ChromiumApp();

	_impl->app->enableInput(_visible);

	int result = CefExecuteProcess(*_impl->mainArgs, _impl->app.get(), nullptr);
}

void
ChromiumDOMEngine::initialize(AbstractCanvas::Ptr canvas, std::shared_ptr<component::SceneManager> sceneManager, minko::scene::Node::Ptr root, bool secure)
{
	_canvas = canvas;
	_sceneManager = sceneManager;

	if (_quad != nullptr)
		remove();

	if (!_chromiumInitialized)
	{
		std::shared_ptr<render::Texture> texture = _impl->app->initialize(canvas, _sceneManager->assets()->context(), _impl);

        _impl->app->secure(secure);
		_impl->renderHandler->visible(_visible);

		CefSettings settings;

		settings.windowless_rendering_enabled = 1;
		settings.command_line_args_disabled = 1;
		// CefString(&settings.resources_dir_path) = "/resource";
		// Use this if you wish to changes cef resources directory

		int result = CefInitialize(*_impl->mainArgs, settings, _impl->app.get(), nullptr);

		_overlayMaterial = material::BasicMaterial::create();
        _overlayMaterial->diffuseMap(texture);

		loadOverlayEffect();

		float wRatio = (float)canvas->width() / (float)math::clp2(canvas->width());
		float hRatio = (float)canvas->height() / (float)math::clp2(canvas->height());
		_overlayMaterial->data()->set("overlayRatio", math::vec2(wRatio, hRatio));
		
        auto overlayEffect = _sceneManager->assets()->effect("effect/HtmlOverlay.effect");
		
		if (overlayEffect && _quad == nullptr)
		{
			_quad = scene::Node::create("quad")
				->addComponent(component::Surface::create(
                geometry::QuadGeometry::create(_sceneManager->assets()->context()),
				_overlayMaterial,
				overlayEffect
			));

            // TODO: Redmine #4274
			//_quad->component<component::Surface>()->visible(_visible);
		}
		else
		{
			std::cout << "WARNING: HtmlOverlay.effect has not been loaded. Overlay will not be displayed" << std::endl;
		}

		_chromiumInitialized = true;
	}

	_canvasResizedSlot = _canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		_impl->browser->GetHost()->WasResized();

		float wRatio = (float)canvas->width() / (float)math::clp2(canvas->width());
		float hRatio = (float)canvas->height() / (float)math::clp2(canvas->height());

        _overlayMaterial->data()->set("overlayRatio", math::vec2(wRatio, hRatio));
        _overlayMaterial->diffuseMap(_impl->renderHandler->renderTexture);
	});

	_renderBeginSlot = _sceneManager->renderingBegin()->connect([&](std::shared_ptr<component::SceneManager>, uint, render::AbstractTexture::Ptr)
	{
		enterFrame();
	}, -100.0f);

	root->addChild(_quad);
}

void
ChromiumDOMEngine::remove()
{
	_canvasResizedSlot = nullptr;
	_renderBeginSlot = nullptr;
	_enterFrameSlot = nullptr;
	_endFrameSlot = nullptr;

	if (_quad->parent() != nullptr)
		_quad->parent()->removeChild(_quad);
}

void
ChromiumDOMEngine::loadOverlayEffect()
{
    auto overlayEffectLoader = file::Loader::create(_sceneManager->assets()->loader());

    overlayEffectLoader->options(overlayEffectLoader->options());
	overlayEffectLoader->options()->loadAsynchronously(false);

	overlayEffectLoader->queue("effect/HtmlOverlay.effect")->load();
}

void
ChromiumDOMEngine::enterFrame()
{
	CefDoMessageLoopWork();

	for (auto dom : _doms)
		dom->update();
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

AbstractDOM::Ptr
ChromiumDOMEngine::mainDOM()
{
	return _impl->mainDOM;
}

void
ChromiumDOMEngine::loadHttp(std::string url)
{
	std::cout << url << std::endl;
	_impl->browser->GetMainFrame()->LoadURL(url);
}

void
ChromiumDOMEngine::loadLocal(std::string filename)
{
	std::string path = "file://" + file::File::getBinaryDirectory() + "/";

	path += "asset/";

	std::string url = path + filename;

	loadHttp(url);
}


void
ChromiumDOMEngine::visible(bool value)
{
    // TODO: Redmine #4274
    /*
	if (_quad != nullptr && _quad->hasComponent<component::Surface>())
		_quad->component<component::Surface>()->visible(value);
    */

	if (_impl != nullptr)
	{
		if (_impl->app != nullptr)
			_impl->app->enableInput(value);
		if (_impl->renderHandler != nullptr)
			_impl->renderHandler->visible(value);
	}

	_visible = value;

}

bool
ChromiumDOMEngine::visible()
{
	return _visible;
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
	ChromiumDOMMouseEvent::clearAll();
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