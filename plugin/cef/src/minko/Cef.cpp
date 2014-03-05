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
#include "minko/Cef.hpp"

#include "minko/App.hpp"
#include "minko/RenderHandler.hpp"
#include "minko/BrowserClient.hpp"

#include "minko/CefPimpl.hpp"

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_render_handler.h"

using namespace minko;

Cef::Cef()
{
	_impl = new CefPimpl();
}

bool
Cef::load(int argc, char** argv)
{
#ifdef _WIN32
	_impl->cefMainArgs = new CefMainArgs(GetModuleHandle(NULL));
#else
	_impl->cefMainArgs = new CefMainArgs(argc, argv);
#endif
	_impl->cefApp = new App();

	cefProcessResult = CefExecuteProcess(*_impl->cefMainArgs, _impl->cefApp.get(), nullptr);

	if (cefProcessResult >= 0)
	{
		return false;
	}
	return true;
}

void
Cef::unload()
{
	_impl->cefMainArgs = nullptr;
	_impl->cefBrowser = nullptr;
	_impl->cefApp = nullptr;
	_impl->cefRenderHandler = nullptr;
	CefShutdown();
	//delete _impl->cefRenderHandler;
}

void
Cef::initialize(std::shared_ptr<AbstractCanvas> canvas, std::shared_ptr<component::SceneManager> sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;
	_context = _sceneManager->assets()->context();
	std::shared_ptr<render::Texture> texture = _impl->cefApp->initialize(canvas, _context, _impl);

	CefSettings settings;

	settings.single_process = 1;

	cefProcessResult = CefInitialize(*_impl->cefMainArgs, settings, _impl->cefApp.get(), nullptr);
	
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
		_impl->cefBrowser->GetHost()->WasResized();

		float wRatio = (float)canvas->width() / (float)math::clp2(canvas->width());
		float hRatio = (float)canvas->height() / (float)math::clp2(canvas->height());
		_overlayMaterial->set("overlayRatio", math::Vector2::create(wRatio, hRatio));
		_overlayMaterial->diffuseMap(_impl->cefRenderHandler->renderTexture);
		enterFrame();
	});

	_enterFrameSlot = _sceneManager->frameBegin()->connect([&](std::shared_ptr<component::SceneManager>)
	{
		enterFrame();
	});
}

void
Cef::setURL(std::string url)
{
	_impl->cefBrowser.get()->GetMainFrame()->LoadURL(url);
}

void
Cef::loadLocal(std::string filename)
{
	std::string path = getWorkingDirectory() + "/";

//Fixme : find a more elegant way ?
#if DEBUG
	path += "../../../asset/";
#endif

	std::string url = "file://" + path + "/" + filename;

	setURL(url);
}

void
Cef::setHTML(std::string html)
{
	executeJavascript("var doc = document.open('text/html', 'replace'); doc.write(\"" + html + "\");doc.close();");
}

/*void
Cef::addEventListener(std::string type, std::string id)
{
	std::string code = "";

	code += "var element = document.getElementById('" + id + "');\n";
	code += "element.addEventListener('" + type + "', function(event)\n";
	code += "{\n";
	code += "	Minko.sendCallback";
	code += "	";
	code += "	";
	code += "});\n";
}*/

void
Cef::executeJavascript(std::string code)
{
	_impl->cefBrowser.get()->GetMainFrame()->ExecuteJavaScript(code, "", 0);
}

void
Cef::enterFrame()
{
	CefDoMessageLoopWork();
	_impl->cefRenderHandler->uploadTexture();
}