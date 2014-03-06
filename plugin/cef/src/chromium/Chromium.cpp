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

#include <signal.h>

#include "minko/Minko.hpp"
#include "chromium/Chromium.hpp"

#include "chromium/ChromiumApp.hpp"
#include "chromium/ChromiumRenderHandler.hpp"
#include "chromium/ChromiumClient.hpp"

#include "chromium/ChromiumPimpl.hpp"

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_render_handler.h"

using namespace minko;
using namespace chromium;

Chromium::Chromium()
{
	_impl = new ChromiumPimpl();
	start(0, 0);
}

Chromium::Chromium(int argc, char** argv)
{
	_impl = new ChromiumPimpl();
	start(argc, argv);
}

void
Chromium::start(int argc, char** argv)
{
#ifdef _WIN32
	_impl->mainArgs = new CefMainArgs(GetModuleHandle(NULL));
#else
	_impl->mainArgs = new CefMainArgs(argc, argv);
#endif
	_impl->app = new ChromiumApp();

	CefExecuteProcess(*_impl->mainArgs, _impl->app.get(), nullptr);
}

void
Chromium::beforeAppCloses()
{
	_impl->mainArgs = nullptr;
	_impl->browser = nullptr;
	_impl->app = nullptr;
	_impl->renderHandler = nullptr;
	_impl->renderProcessHandler = nullptr;
	_impl->v8Engine = nullptr;
	_impl->v8Context = nullptr;

	CefShutdown();
}

void
Chromium::initialize(std::shared_ptr<AbstractCanvas> canvas, std::shared_ptr<component::SceneManager> sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;
	_context = _sceneManager->assets()->context();
	std::shared_ptr<render::Texture> texture = _impl->app->initialize(canvas, _context, _impl);

	CefSettings settings;

	settings.single_process = 1;

	int result = CefInitialize(*_impl->mainArgs, settings, _impl->app.get(), nullptr);
	
	auto overlayEffect = sceneManager->assets()->effect("effect/Overlay.effect");

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
Chromium::load(std::string uri)
{
	int httpIndex = uri.find_first_of("http://");
	int httpsIndex = uri.find_first_of("https://");
	int fileIndex = uri.find_first_of("file://");

	if (fileIndex == 0)
		loadHttp(uri);
	else
		loadLocal(uri);
}

void
Chromium::loadHttp(std::string url)
{
	_impl->browser.get()->GetMainFrame()->LoadURL(url);
}

void
Chromium::loadLocal(std::string filename)
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
Chromium::executeJavascript(std::string code)
{
	_impl->browser.get()->GetMainFrame()->ExecuteJavaScript(code, "", 0);
}

void
Chromium::enterFrame()
{
	CefDoMessageLoopWork();
	_impl->renderHandler->uploadTexture();
}