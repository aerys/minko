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
#include "minko/file/Options.hpp"
#include "minko/file/FileProtocol.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "macwebview/dom/MacWebViewDOMEngine.hpp"
#include "macwebview/dom/MacWebViewDOMMouseEvent.hpp"

#include "minko/MinkoSDL.hpp"

#include "SDL.h"
#include "SDL_syswm.h"

#import "macwebview/dom/MacWebUIDelegate.h"

using namespace minko;
using namespace minko::component;
using namespace minko::dom;
using namespace macwebview;
using namespace macwebview::dom;

int
MacWebViewDOMEngine::_domUid = 0;

std::function<void(std::string&, std::string&)>
MacWebViewDOMEngine::handleJavascriptMessageWrapper;

// Slots
Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot onmousemoveSlot;
Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot onmousedownSlot;
Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot onmouseupSlot;


MacWebViewDOMEngine::MacWebViewDOMEngine() :
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_visible(true),
    _waitingForLoad(true),
    _isReady(false)
{
}

void
MacWebViewDOMEngine::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;
    
    // Get window from canvas
    auto newCanvas = std::static_pointer_cast<Canvas>(canvas);
    SDL_Window* sdlWindow = newCanvas->window();
    SDL_SysWMinfo info;
    
    SDL_VERSION(&info.version);
    
    if (SDL_GetWindowWMInfo(sdlWindow, &info))
    {
        _window = info.info.cocoa.window;
        
        // Create the web view
        _webView = [[MacWebView alloc] initWithFrame:NSMakeRect(0, 0, _canvas->width(), _canvas->height())];

        // Display the webview
        [_webView setWantsLayer: YES];
        
        // Webview's background
        CGColorRef clearColor = CGColorCreateGenericRGB(255/255.0f, 1/255.0f, 0/255.0f, 0.f);
        _webView.layer.backgroundColor = clearColor;

        // Display the canvas behind the overlay
        [_webView setDrawsBackground:NO];
        
        // Disable web view scroll
        [[[_webView mainFrame] frameView] setAllowsScrolling:NO];
        _webView.mainFrame.frameView.documentView.enclosingScrollView.verticalScrollElasticity = NSScrollElasticityNone;
        
        // Set UIDelegate (used to enable JS alert and disable right click)
        [_webView setUIDelegate:[MacWebUIDelegate alloc]];

        // Resize the overlay according to the window's size
        [_window.contentView setAutoresizesSubviews:YES];
        [_webView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        
        // Add the web view to the current window
        [_window.contentView addSubview:_webView];
        
        // Load iframe containing bridge JS callback handler
        NSURL *url = [NSURL fileURLWithPath:@"asset/html/iframe.html"];
        NSURLRequest *request = [NSURLRequest requestWithURL:url];
        [[_webView mainFrame] loadRequest:request];
        
        // Create a C++ handler to process the message received by the Javascript bridge
        handleJavascriptMessageWrapper = std::bind(
                       &MacWebViewDOMEngine::handleJavascriptMessage,
                       this,
                       std::placeholders::_1,
                       std::placeholders::_2
        );
        
        // Create the bridge
        _bridge = [WebViewJavascriptBridge bridgeForWebView:_webView handler:
                   ^(id data, WVJBResponseCallback responseCallback) {

                   // If the message is a dictionary (of this form: {type: 'ready', value: 'true'})
                   if([data isKindOfClass:[NSDictionary class]])
                   {
                        std::string type;
                        std::string value;
                
                        for (NSString *key in [data allKeys])
                        {
                            id val = [data objectForKey:key];
                   
                            if ([key isEqualToString:@"type"])
                                type = [val UTF8String];
                            else if ([key isEqualToString:@"value"])
                                value = [val UTF8String];
                        }
                   
                        MacWebViewDOMEngine::handleJavascriptMessageWrapper(type, value);
                   }
                   else if ([data isKindOfClass:[NSString class]])
                   {
                        NSString* dataString = (NSString *)data;
                        std::string value([dataString UTF8String]);
                        std::string type = "log";
                   
                        MacWebViewDOMEngine::handleJavascriptMessageWrapper(type, value);
                   }
         }];
        
#if DEBUG
        // Enable bridge logging
//        [WebViewJavascriptBridge enableLogging];
#endif
    }

    visible(_visible);

	_enterFrameSlot = _sceneManager->frameBegin()->connect([&](std::shared_ptr<component::SceneManager>, float, float)
	{
		enterFrame();
	});
}

void
MacWebViewDOMEngine::createNewDom()
{
	std::string domName = "Minko.dom" + std::to_string(_domUid++);

	_currentDOM = MacWebViewDOM::create(domName, shared_from_this());
}


minko::dom::AbstractDOM::Ptr
MacWebViewDOMEngine::mainDOM()
{
	return _currentDOM;
}

void
MacWebViewDOMEngine::enterFrame()
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
		jsEval = "Minko.loaded = 0";
		eval(jsEval);
        
		if (_currentDOM->initialized())
			createNewDom();

		_currentDOM->initialized(true);

		_currentDOM->onload()->execute(_currentDOM, _currentDOM->fullUrl());
		_onload->execute(_currentDOM, _currentDOM->fullUrl());
        
        registerDomEvents();
	}
    
    for(auto element : MacWebViewDOMElement::domElements)
    {
        element->update();
    }

	if (_currentDOM->initialized() && _isReady)
	{
		std::string jsEval = "(Minko.iframeElement.contentWindow.Minko.messagesToSend.length);";
        std::string evalResult = eval(jsEval);
        int l = atoi(evalResult.c_str());

		if (l > 0)
		{
            std::cout << "Messages found!" << std::endl;
			for(int i = 0; i < l; ++i)
			{
                jsEval = "(Minko.iframeElement.contentWindow.Minko.messagesToSend[" + std::to_string(i) + "])";
                
				std::string message = eval(jsEval);
                
                std::cout << "Message: " << message << std::endl;
                
				_currentDOM->onmessage()->execute(_currentDOM, message);
				_onmessage->execute(_currentDOM, message);
			}

			jsEval = "Minko.iframeElement.contentWindow.Minko.messagesToSend = [];";
			eval(jsEval);
		}
	}
}

MacWebViewDOMEngine::Ptr
MacWebViewDOMEngine::create()
{
	MacWebViewDOMEngine::Ptr engine(new MacWebViewDOMEngine);
	return engine;
}

AbstractDOM::Ptr
MacWebViewDOMEngine::load(std::string uri)
{
    if (_currentDOM == nullptr || _currentDOM->initialized())
        createNewDom();
    
    if (_waitingForLoad)
    {
        _uriToLoad = uri;
    }
    else
    {
        bool isHttp		= uri.substr(0, 7) == "http://";
        bool isHttps	= uri.substr(0, 8) == "https://";
        
        
        if (!isHttp && !isHttps)
        {
            std::string path = file::File::getBinaryDirectory();
#if DEBUG
            uri = path + "/../../../asset/" + uri;
#else
            uri = path + "/asset/" + uri;
#endif
        }
        
        std::string jsEval = "Minko.loadUrl('" + uri + "')";
        
        eval(jsEval);
    }

	return _currentDOM;
}

void
MacWebViewDOMEngine::clear()
{
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
MacWebViewDOMEngine::onload()
{
	return _onload;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
MacWebViewDOMEngine::onmessage()
{
	return _onmessage;
}

bool
MacWebViewDOMEngine::visible()
{
    return _visible;
}

void
MacWebViewDOMEngine::visible(bool value)
{
    if (_canvas != nullptr)
	{
        if (value != _visible)
        {
            if (value)
                [_window.contentView addSubview:_webView];
            else
                [_webView removeFromSuperview];
        }
	}
    
	_visible = value;
}

void MacWebViewDOMEngine::handleJavascriptMessage(std::string type, std::string value)
{
    if (type == "ready")
    {
        _isReady = (value == "true") ? true : false;
        
        if (_isReady)
            std::cout << "Bridge is ready!" << std::endl;
    }
    else if (type == "alert")
    {
        eval("Received a message from JS: " + value);
    }
    else if (type == "log")
    {
        std::cout << "[Bridge] " << value << std::endl;
    }
}

void
MacWebViewDOMEngine::registerDomEvents()
{
    onmousedownSlot = _currentDOM->document()->onmousedown()->connect([&](AbstractDOMMouseEvent::Ptr event)
    {
        int x = event->clientX();
        int y = event->clientY();
        
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);
        
        _canvas->mouse()->leftButtonDown()->execute(_canvas->mouse());
    });

    onmouseupSlot = _currentDOM->document()->onmouseup()->connect([&](AbstractDOMMouseEvent::Ptr event)
    {
        int x = event->layerX();
        int y = event->layerY();
        
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);
        
        _canvas->mouse()->leftButtonUp()->execute(_canvas->mouse());
    });

    onmousemoveSlot = _currentDOM->document()->onmousemove()->connect([&](AbstractDOMMouseEvent::Ptr event)
    {
        int x = event->clientX();
        int y = event->clientY();
        
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);
        
        auto oldX = _canvas->mouse()->x();
        auto oldY = _canvas->mouse()->y();
        
        _canvas->mouse()->move()->execute(_canvas->mouse(), event->clientX() - oldX, event->clientY() - oldY);
    });
}

std::string
MacWebViewDOMEngine::eval(std::string data)
{
    const char *dataChar = data.c_str();
    NSString *nsString = [NSString stringWithCString:dataChar encoding:[NSString defaultCStringEncoding]];
    NSString *result = [_webView stringByEvaluatingJavaScriptFromString: nsString];
    std::string resultString([result UTF8String]);
    
    return resultString;
}