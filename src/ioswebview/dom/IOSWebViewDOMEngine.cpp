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
#include "ioswebview/dom/IOSWebViewDOMElement.hpp"

#include "minko/MinkoSDL.hpp"

#include "../../plugin/sdl/lib/sdl/include/SDL2/SDL.h"
#include "../../plugin/sdl/lib/sdl/include/SDL2/SDL_syswm.h"

using namespace minko;
using namespace minko::component;
using namespace minko::dom;
using namespace ioswebview;
using namespace ioswebview::dom;

int
IOSWebViewDOMEngine::_domUid = 0;

std::function<void(std::string&)>
IOSWebViewDOMEngine::handleJavascriptMessageWrapper;

IOSWebViewDOMEngine::IOSWebViewDOMEngine() :
	_loadedPreviousFrameState(0),
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_visible(true),
    _waitingForLoad(true)
{
}

void
IOSWebViewDOMEngine::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;

	//loadScript("script/overlay.js");
    
    // Create a web view
    NSLog(@"COUCOU");
    
    // Get window from canvas
    auto newCanvas = std::static_pointer_cast<Canvas>(canvas);
    SDL_Window* sdlWindow = newCanvas->window();
    SDL_SysWMinfo info;
    
    SDL_VERSION(&info.version);
    
    if (SDL_GetWindowWMInfo(sdlWindow, &info))
    {
        _window = (IOSTapDetectingWindow*)(info.info.uikit.window);
        
        // Create the web view
        _webView = [[IOSWebView alloc] initWithFrame:_window.bounds];
        //UIColor * clearColor = [UIColor colorWithRed:255/255.0f green:1/255.0f blue:0/255.0f alpha:1.0f];
        [_webView setBackgroundColor: [UIColor clearColor]];
        [_webView setOpaque:NO];

        _webView.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin |
                                    UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin |
                                    UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        
        _webView.scrollView.scrollEnabled = NO;
        _webView.scrollView.bounces = NO;
        
        IOSWebViewController *iOSWebViewDelegate = [[IOSWebViewController alloc] init];
        [_webView setDelegate: iOSWebViewDelegate];
        _webView.delegate = iOSWebViewDelegate;
        
        //[_webView stringByEvaluatingJavaScriptFromString:@"var e = document.createEvent('Events'); e.initEvent('orientationchange', true, false); document.dispatchEvent(e);"];
        
        _window.autoresizesSubviews = YES;
        
        // Add the web view to the current window
        [_window addSubview:_webView];
        
        // Load iframe with bridge JS callback handler
        NSURL *url = [NSURL fileURLWithPath:@"asset/html/iframe.html"];
        
        NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
        [_webView loadRequest:requestObj];
        
        
        handleJavascriptMessageWrapper = std::bind(
                       &IOSWebViewDOMEngine::handleJavascriptMessage,
                       this,
                       std::placeholders::_1
        );
        
        [WebViewJavascriptBridge enableLogging];
        _bridge = [WebViewJavascriptBridge bridgeForWebView:_webView handler:
                   //[=](id data, WVJBResponseCallback responseCallback){
                   ^(id data, WVJBResponseCallback responseCallback) {
                   NSLog(@"Received message from javascript: %@", data);
                   responseCallback(@"Right back atcha");

                   if ([data isKindOfClass:[NSString class]])
                   {
                        NSString *dataString = (NSString *)data;
                        
                        std::string cppString([dataString UTF8String]);
                   
                        IOSWebViewDOMEngine::handleJavascriptMessageWrapper(cppString);
                   }
         }];
        
        //[_webView stringByEvaluatingJavaScriptFromString:@"alert('CECI EST UN COUCOU DE TEST');"];
        
        //window.viewToObserve = _webView;
        //window.controllerThatObserves = nil;
        
        [_bridge send:@"alert('COUCOU');"];
        
        
    }
    
    visible(_visible);

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
    
    //[_bridge send: eval];
	//emscripten_run_script(eval.c_str());
	
	_currentDOM = IOSWebViewDOM::create(domName, shared_from_this());
}


minko::dom::AbstractDOM::Ptr
IOSWebViewDOMEngine::mainDOM()
{
	return _currentDOM;
}

void
IOSWebViewDOMEngine::enterFrame()
{
    if (_waitingForLoad)
    {
        std::string jsEval = "try{ var loaded = Minko.loaded; ('true')} catch(e) {('false')}";
        
        std::string res = eval(jsEval);
        
        if (res == "true")
        {
            _waitingForLoad = false;
            load(_uriToLoad);
        }
        
        return;
    }
	std::string jsEval = "(Minko.loaded + '')";

    std::string res = eval(jsEval);
	int loadedState = atoi(res.c_str());

	if (loadedState == 1)
	{
		if (_currentDOM->initialized())
			createNewDom();

		_currentDOM->initialized(true);

		_currentDOM->onload()->execute(_currentDOM, _currentDOM->fullUrl());
		_onload->execute(_currentDOM, _currentDOM->fullUrl());

		jsEval = "Minko.loaded = 0";
		eval(jsEval);
	}

//	for(auto element : IOSWebViewDOMElement::domElements)
//	{
//		element->update();
//	}

	if (_currentDOM->initialized())
	{
		std::string jsEval = "(Minko.iframeElement.contentWindow.Minko.messagesToSend.length);";
		int l = atoi(eval(jsEval).c_str());

		if (l > 0)
		{
			for(int i = 0; i < l; ++i)
			{
                jsEval = "(Minko.iframeElement.contentWindow.Minko.messagesToSend[" + std::to_string(i) + "])";
                
				std::string message = eval(jsEval);

				_currentDOM->onmessage()->execute(_currentDOM, message);
				_onmessage->execute(_currentDOM, message);
			}

			jsEval = "Minko.iframeElement.contentWindow.Minko.messagesToSend = [];";
			eval(jsEval);
		}
	}
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
	createNewDom();
    
    if (_waitingForLoad)
    {
        _uriToLoad = uri;
    }
    else
    {
        bool isHttp		= uri.substr(0, 7) == "http://";
        bool isHttps	= uri.substr(0, 8) == "https://";
        
        NSURL *url;
        
        /*
         if (!isHttp && !isHttps)
         {
         uri = "asset/" + uri;
         NSString *fullUrl = [NSString st	ringWithCString:uri.c_str() encoding:[NSString defaultCStringEncoding]];
         url = [NSURL fileURLWithPath:fullUrl];
         }
         else
         {
         NSString *fullUrl = [NSString stringWithCString:uri.c_str() encoding:[NSString defaultCStringEncoding]];
         url = [NSURL URLWithString:fullUrl];
         }
         
         NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
         [_webView loadRequest:requestObj];
         */
        
        
        std::string jsEval = "Minko.loadUrl('" + uri + "')";
        
        
        eval(jsEval);
    }

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

bool
IOSWebViewDOMEngine::visible()
{
    return _visible;
}

void
IOSWebViewDOMEngine::visible(bool value)
{
    if (_canvas != nullptr)
	{
        //[_webView setHidden:value];
        //_webView.hidden = value;
        
        if (value != _visible)
        {
            if (value)
                [_window addSubview:_webView];
            else
                [_webView removeFromSuperview];
        }
	}
    
	_visible = value;
}

void IOSWebViewDOMEngine::handleJavascriptMessage(std::string message)
{
    if (message == "ready")
    {
        std::cout << "READY" << std::endl;
    }
}

std::string
IOSWebViewDOMEngine::eval(std::string data)
{
    NSString *result = [_webView stringByEvaluatingJavaScriptFromString: [NSString stringWithCString:data.c_str() encoding:[NSString defaultCStringEncoding]]];
    std::string resultString([result UTF8String]);
    
    return resultString;
}

#endif