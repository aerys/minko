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
#include "osxwebview/dom/OSXWebViewDOMEngine.hpp"
#include "osxwebview/dom/OSXWebViewDOMEvent.hpp"

#include "minko/MinkoSDL.hpp"

#include "SDL.h"
#include "SDL_syswm.h"
// #include <UIKit/NSWindow.h>

using namespace minko;
using namespace minko::component;
using namespace minko::dom;
using namespace osxwebview;
using namespace osxwebview::dom;

int
OSXWebViewDOMEngine::_domUid = 0;

std::function<void(std::string&, std::string&)>
OSXWebViewDOMEngine::handleJavascriptMessageWrapper;

// Slots
Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot onmousemoveSlot;
Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot onmousedownSlot;
Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot onmouseupSlot;

Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot ontouchdownSlot;
Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot ontouchupSlot;
Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot ontouchmotionSlot;

OSXWebViewDOMEngine::OSXWebViewDOMEngine() :
	_loadedPreviousFrameState(0),
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_visible(true),
    _waitingForLoad(true),
    _isReady(false),
    _webViewWidth(0)
{
}

void
OSXWebViewDOMEngine::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
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
        _webView = [[OSXWebView alloc] initWithFrame:_window.frame];
        CIColor* clearColor = [CIColor colorWithRed:255/255.0f green:1/255.0f blue:0/255.0f alpha:0.f];
        // [_webView setBackgroundColor:clearColor];
        // [_webView setOpaque:NO];
        // [_webView.scrollView setDelaysContentTouches:NO];
        
        // Disable web view scroll
        // _webView.scrollView.scrollEnabled = NO;
        // _webView.scrollView.bounces = NO;
        
        // Resize the web view according to device dimension and orientation
        // _webView.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin |
        //                             UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin |
        //                             UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        // _webView.scalesPageToFit = YES;
        
        //[_webView setUserInteractionEnabled:NO];
        
        _window.contentViewController.view.autoresizesSubviews = YES;
        
        // Add the web view to the current window
        [_window.contentViewController.view addSubview:_webView];
        
        // Save the web view width
        _webViewWidth = _webView.frame.size.width;
        
        // Load iframe containing bridge JS callback handler
        NSURL *url = [NSURL fileURLWithPath:@"asset/html/iframe.html"];
        NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
        [_webView.mainFrame loadRequest:requestObj];
        
        // Create a C++ handler to process the message received by the Javascript bridge
        handleJavascriptMessageWrapper = std::bind(
                       &OSXWebViewDOMEngine::handleJavascriptMessage,
                       this,
                       std::placeholders::_1,
                       std::placeholders::_2
        );
        
        // Create the bridge
        _bridge = [WebViewJavascriptBridge bridgeForWebView:_webView handler:
                   ^(id data, WVJBResponseCallback responseCallback) {
                   NSLog(@"Received message from javascript: %@", data);
                   responseCallback(@"Right back atcha");

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
                   
                        OSXWebViewDOMEngine::handleJavascriptMessageWrapper(type, value);
                   }
                   else if ([data isKindOfClass:[NSString class]])
                   {
                        NSString* dataString = (NSString *)data;
                        std::string value([dataString UTF8String]);
                        std::string type = "log";
                   
                        OSXWebViewDOMEngine::handleJavascriptMessageWrapper(type, value);
                   }
         }];
        
#if DEBUG
        // Enable bridge logging
        //[WebViewJavascriptBridge enableLogging];
#endif
    }

    visible(_visible);

	_canvasResizedSlot = _canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        _webViewWidth = w;
        
        updateWebViewWidth();
    });

	_enterFrameSlot = _sceneManager->frameBegin()->connect([&](std::shared_ptr<component::SceneManager>, float, float)
	{
		enterFrame();
	});
}

void
OSXWebViewDOMEngine::loadScript(std::string filename)
{
    auto options = file::Options::create(_sceneManager->assets()->loader()->options());
    options->loadAsynchronously(false);

    file::AbstractProtocol::Ptr loader = file::FileProtocol::create();

    auto loaderComplete = loader->complete()->connect([&](std::shared_ptr<file::AbstractProtocol> loader)
    {
    	std::string js;
    	js.assign(loader->file()->data().begin(), loader->file()->data().end());

		eval(js);
    });

	loader->load(filename, options);
}

void
OSXWebViewDOMEngine::createNewDom()
{
	std::string domName = "Minko.dom" + std::to_string(_domUid++);

	_currentDOM = OSXWebViewDOM::create(domName, shared_from_this());
}


minko::dom::AbstractDOM::Ptr
OSXWebViewDOMEngine::mainDOM()
{
	return _currentDOM;
}

void
OSXWebViewDOMEngine::enterFrame()
{
    if (_waitingForLoad)
    {
        std::string jsEval = "try{ var loaded = Minko.loaded; ('true')} catch(e) {('false')}";
        
        std::string res = eval(jsEval);
        
        if (res == "true")
        {
            _waitingForLoad = false;
            load(_uriToLoad);
            updateWebViewWidth();
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

    /*
	for(auto element : OSXWebViewDOMEvent::domElements)
	{
		element->update();
	}
    */

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

OSXWebViewDOMEngine::Ptr
OSXWebViewDOMEngine::create()
{
	OSXWebViewDOMEngine::Ptr engine(new OSXWebViewDOMEngine);
	return engine;
}

AbstractDOM::Ptr
OSXWebViewDOMEngine::load(std::string uri)
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
            uri = "../../asset/" + uri;
        }
        
        std::string jsEval = "Minko.loadUrl('" + uri + "')";
        
        eval(jsEval);
    }

	return _currentDOM;
}

void
OSXWebViewDOMEngine::clear()
{
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
OSXWebViewDOMEngine::onload()
{
	return _onload;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
OSXWebViewDOMEngine::onmessage()
{
	return _onmessage;
}

bool
OSXWebViewDOMEngine::visible()
{
    return _visible;
}

void
OSXWebViewDOMEngine::visible(bool value)
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

void OSXWebViewDOMEngine::handleJavascriptMessage(std::string type, std::string value)
{
    std::cout << "Processing the current message: [type: " << type << "; value: " << value << "]" << std::endl;
    
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
    // JS event
    if (type == "touchstart")
    {
        auto element = OSXWebViewDOMElement::getDOMElement(value, shared_from_this());
        element->update();
    }
    else if (type == "touchend")
    {
        auto element = OSXWebViewDOMElement::getDOMElement(value, shared_from_this());
        element->update();
    }
    else if (type == "touchmove")
    {
        auto element = OSXWebViewDOMElement::getDOMElement(value, shared_from_this());
        element->update();
    }
}

void
OSXWebViewDOMEngine::registerDomEvents()
{
    onmousemoveSlot = _currentDOM->document()->onmousemove()->connect([&](AbstractDOMEvent::Ptr event)
    {
        std::cout << "Event info: " << std::endl
        << "screen: " << event->screenX() << ", " << event->screenY() << std::endl
        << "layer: " << event->layerX() << ", " << event->layerY() << std::endl
        << "page: " << event->pageX() << ", " << event->pageY() << std::endl
        << "client: " << event->clientX() << ", " << event->clientY() << std::endl;
      
        auto oldX = _canvas->mouse()->x();
        auto oldY = _canvas->mouse()->y();
      
        int x = event->clientX();
        int y = event->clientY();
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);
      
        _canvas->mouse()->move()->execute(_canvas->mouse(), event->clientX() - oldX, event->clientY() - oldY);
    });

    onmouseupSlot = _currentDOM->document()->onmouseup()->connect([&](AbstractDOMEvent::Ptr event)
    {
        int x = event->layerX();
        int y = event->layerY();
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);
                                                          
        _canvas->mouse()->leftButtonUp()->execute(_canvas->mouse());
    });

    onmousedownSlot = _currentDOM->document()->onmousedown()->connect([&](AbstractDOMEvent::Ptr event)
    {
        int x = event->clientX();
        int y = event->clientY();
     
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);
        
        std::cout << "Mouse position (webview): (" << _canvas->mouse()->x() << "," << _canvas->mouse()->y() << ")" << std::endl;
        
        _canvas->mouse()->leftButtonDown()->execute(_canvas->mouse());
    });
    
    ontouchdownSlot = _currentDOM->document()->ontouchdown()->connect([&](AbstractDOMEvent::Ptr event)
    {
        _canvas->finger()->fingerDown()->execute(_canvas->finger(), event->layerX(), event->layerY());
    });
    
    ontouchupSlot = _currentDOM->document()->ontouchup()->connect([&](AbstractDOMEvent::Ptr event)
    {
        _canvas->finger()->fingerUp()->execute(_canvas->finger(), event->layerX(), event->layerY());
    });
    
    ontouchmotionSlot = _currentDOM->document()->ontouchmotion()->connect([&](AbstractDOMEvent::Ptr event)
    {
        _canvas->finger()->fingerMotion()->execute(_canvas->finger(), event->layerX(), event->layerY());
    });
}

void
OSXWebViewDOMEngine::updateWebViewWidth()
{
    std::string jsEval = "Minko.changeViewportWidth(" + std::to_string(_webViewWidth) + ");";
    
    eval(jsEval);
}

std::string
OSXWebViewDOMEngine::eval(std::string data)
{
    const char *dataChar = data.c_str();
    NSString *nsString = [NSString stringWithCString:dataChar encoding:[NSString defaultCStringEncoding]];
    NSString *result = [_webView stringByEvaluatingJavaScriptFromString: nsString];
    std::string resultString([result UTF8String]);
    
    return resultString;
}
