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

#if defined(TARGET_IPHONE_SIMULATOR) or defined(TARGET_OS_IPHONE) // iOS

#include "minko/Common.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/FileProtocol.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "ioswebview/dom/IOSWebViewDOMEngine.hpp"

#include "minko/MinkoSDL.hpp"

#import "WebViewJavascriptBridge.h"

#include "../../plugin/sdl/lib/sdl/include/SDL2/SDL.h"
#include "../../plugin/sdl/lib/sdl/include/SDL2/SDL_syswm.h"

using namespace minko;
using namespace minko::component;
using namespace minko::dom;
using namespace ioswebview;
using namespace ioswebview::dom;

int
IOSWebViewDOMEngine::_domUid = 0;

IOSWebViewDOMEngine::IOSWebViewDOMEngine() :
	_loadedPreviousFrameState(0),
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_visible(true)
{
}

void
IOSWebViewDOMEngine::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;

	//loadScript("script/overlay.js");

	visible(_visible);
    
    // Create a web view
    NSLog(@"COUCOU");
    
    // Get window from canvas
    auto newCanvas = std::static_pointer_cast<Canvas>(canvas);
    SDL_Window* sdlWindow = newCanvas->window();
    UIWindow *window;
    SDL_SysWMinfo info;
    
    SDL_VERSION(&info.version);
    
    if (SDL_GetWindowWMInfo(sdlWindow, &info))
    {
        window = (UIWindow*)(info.info.uikit.window);
        
        // Create the web view
        _webView = [[UIWebView alloc] initWithFrame:window.bounds];
        //UIColor * clearColor = [UIColor colorWithRed:255/255.0f green:1/255.0f blue:0/255.0f alpha:1.0f];
        [_webView setBackgroundColor: [UIColor clearColor]];
        [_webView setOpaque:NO];
        _webView.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin |
                                    UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin |
                                    UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        
        //window.autoresizesSubviews = YES;
        [window addSubview:_webView];
        
        /*
        WebViewJavascriptBridge* bridge = [WebViewJavascriptBridge bridgeForWebView:_webView handler:^(id data, WVJBResponseCallback responseCallback) {
                                           NSLog(@"Received message from javascript: %@", data);
                                           responseCallback(@"Right back atcha");
                                           }];
        
        [bridge send:@"Well hello there"];
        */
    }

	_canvasResizedSlot = _canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		std::string eval = "";

		eval += "Minko.iframeElement.style.width = '" + std::to_string(w) + "px';\n";
		eval += "Minko.iframeElement.style.height = '" + std::to_string(h) + "px';\n";

		//emscripten_run_script(eval.c_str());
	});

	_enterFrameSlot = _sceneManager->frameBegin()->connect([&](std::shared_ptr<component::SceneManager>, float, float)
	{
		enterFrame();
	});
}

void
IOSWebViewDOMEngine::loadScript(std::string filename)
{
    auto options = file::Options::create(_sceneManager->assets()->loader()->options());
    options->loadAsynchronously(false);

    file::AbstractProtocol::Ptr loader = file::FileProtocol::create();

    auto loaderComplete = loader->complete()->connect([](std::shared_ptr<file::AbstractProtocol> loader)
    {
    	std::string eval;
    	eval.assign(loader->file()->data().begin(), loader->file()->data().end());

		//emscripten_run_script(eval.c_str());
    });

	loader->load(filename, options);
}

void
IOSWebViewDOMEngine::createNewDom()
{
	std::string domName = "Minko.dom" + std::to_string(_domUid++);

	std::string eval = domName + " = {};";
	//emscripten_run_script(eval.c_str());
	
	_currentDOM = IOSWebViewDOM::create(domName);
}


minko::dom::AbstractDOM::Ptr
IOSWebViewDOMEngine::mainDOM()
{
	return _currentDOM;
}

void
IOSWebViewDOMEngine::enterFrame()
{
	std::string eval = "(Minko.loaded)";
    
    /*
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
    */
}

IOSWebViewDOMEngine::Ptr
IOSWebViewDOMEngine::create()
{
	IOSWebViewDOMEngine::Ptr engine(new IOSWebViewDOMEngine);
	return engine;
}

AbstractDOM::Ptr
IOSWebViewDOMEngine::load(std::string uri)
{
	bool isHttp		= uri.substr(0, 7) == "http://";
	bool isHttps	= uri.substr(0, 8) == "https://";

    NSURL *url;
    
	if (!isHttp && !isHttps)
    {
		uri = "asset/" + uri;
        NSString *fullUrl = [NSString stringWithCString:uri.c_str() encoding:[NSString defaultCStringEncoding]];
        url = [NSURL fileURLWithPath:fullUrl];
    }
    else
    {
        NSString *fullUrl = [NSString stringWithCString:uri.c_str() encoding:[NSString defaultCStringEncoding]];
        url = [NSURL URLWithString:fullUrl];
    }
    
    NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
    [_webView loadRequest:requestObj];
    
	createNewDom();

	return _currentDOM;
}

void
IOSWebViewDOMEngine::clear()
{
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
IOSWebViewDOMEngine::onload()
{
	return _onload;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
IOSWebViewDOMEngine::onmessage()
{
	return _onmessage;
}

void
IOSWebViewDOMEngine::visible(bool value)
{
	_visible = value;
}
#endif