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

std::function<void(std::string&, std::string&)>
IOSWebViewDOMEngine::handleJavascriptMessageWrapper;

// Slots
Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot onmousemoveSlot;
Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot onmousedownSlot;
Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot onmouseupSlot;

Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot ontouchdownSlot;
Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot ontouchupSlot;
Signal<minko::dom::AbstractDOMEvent::Ptr>::Slot ontouchmotionSlot;

IOSWebViewDOMEngine::IOSWebViewDOMEngine() :
	_loadedPreviousFrameState(0),
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_visible(true),
    _waitingForLoad(true),
    _isReady(false)
{
}

void
IOSWebViewDOMEngine::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
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
        _window = info.info.uikit.window;
        
        // Create the web view
        _webView = [[IOSWebView alloc] initWithFrame:_window.bounds];
        UIColor * clearColor = [UIColor colorWithRed:255/255.0f green:1/255.0f blue:0/255.0f alpha:0.f];
        [_webView setBackgroundColor: clearColor];
        [_webView setOpaque:NO];
        [_webView.scrollView setDelaysContentTouches:NO];
        
        // Disable web view scroll
        _webView.scrollView.scrollEnabled = NO;
        _webView.scrollView.bounces = NO;
        
        // Resize the web view according to device dimension and orientation
        _webView.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin |
                                    UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin |
                                    UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        _webView.scalesPageToFit = YES;
        _window.rootViewController.view.autoresizesSubviews = YES;
        
        // Add the web view to the current window
        [_window.rootViewController.view addSubview:_webView];
        
        // Load iframe containing bridge JS callback handler
        NSURL *url = [NSURL fileURLWithPath:@"asset/html/iframe.html"];
        NSURLRequest *requestObj = [NSURLRequest requestWithURL:url];
        [_webView loadRequest:requestObj];
        
        // Create a C++ handler to process the message received by the Javascript bridge
        handleJavascriptMessageWrapper = std::bind(
                       &IOSWebViewDOMEngine::handleJavascriptMessage,
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
                   
                        IOSWebViewDOMEngine::handleJavascriptMessageWrapper(type, value);
                   }
                   else if ([data isKindOfClass:[NSString class]])
                   {
                        NSString* dataString = (NSString *)data;
                        std::string value([dataString UTF8String]);
                        std::string type = "log";
                   
                        IOSWebViewDOMEngine::handleJavascriptMessageWrapper(type, value);
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
        CGRect frame = _webView.frame;
        frame.size.width = w / 2;
        frame.size.height = h / 2;
        _webView.frame = frame;
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

    auto loaderComplete = loader->complete()->connect([&](std::shared_ptr<file::AbstractProtocol> loader)
    {
    	std::string js;
    	js.assign(loader->file()->data().begin(), loader->file()->data().end());

		eval(js);
    });

	loader->load(filename, options);
}

void
IOSWebViewDOMEngine::createNewDom()
{
	std::string domName = "Minko.dom" + std::to_string(_domUid++);

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
        
        if (res == "true"/* && _isReady*/)
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

	for(auto element : IOSWebViewDOMElement::domElements)
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

IOSWebViewDOMEngine::Ptr
IOSWebViewDOMEngine::create()
{
	IOSWebViewDOMEngine::Ptr engine(new IOSWebViewDOMEngine);
	return engine;
}

AbstractDOM::Ptr
IOSWebViewDOMEngine::load(std::string uri)
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

void IOSWebViewDOMEngine::handleJavascriptMessage(std::string type, std::string value)
{
    std::cout << "Processing the current message: [type: " << type << "; value: " << value << "]" << std::endl;
    
    if (type == "ready")
    {
        _isReady = (value == "true") ? true : false;
        
        if (_isReady)
            std::cout << "Bridge is ready!" << std::endl;
    }
    else if (type == "mousemove")
    {
        auto element = IOSWebViewDOMElement::getDOMElement(value, shared_from_this());
        element->update();
    }
    else if (type == "mouseclick")
    {
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
IOSWebViewDOMEngine::registerDomEvents()
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
      
        _canvas->mouse()->x(event->clientX());
        _canvas->mouse()->y(event->clientY());
      
        _canvas->mouse()->move()->execute(_canvas->mouse(), event->clientX() - oldX, event->clientY() - oldY);
    });

    onmouseupSlot = _currentDOM->document()->onmouseup()->connect([&](AbstractDOMEvent::Ptr event)
    {
        _canvas->mouse()->x(event->layerX());
        _canvas->mouse()->y(event->layerY());
                                                          
        _canvas->mouse()->leftButtonUp()->execute(_canvas->mouse());
    });

    onmousedownSlot = _currentDOM->document()->onmousedown()->connect([&](AbstractDOMEvent::Ptr event)
    {
        _canvas->mouse()->x(event->clientX());
        _canvas->mouse()->y(event->clientY());
        
        _canvas->mouse()->leftButtonDown()->execute(_canvas->mouse());
    });
    
    ontouchdownSlot = _currentDOM->document()->ontouchdown()->connect([&](AbstractDOMEvent::Ptr event)
    {
        _canvas->finger()->fingerDown()->execute(_canvas->finger(), event->layerX(), event->layerY());
                                                                          
        std::cout << "Touch down" << std::endl;
    });
    
    ontouchupSlot = _currentDOM->document()->ontouchup()->connect([&](AbstractDOMEvent::Ptr event)
    {
        _canvas->finger()->fingerUp()->execute(_canvas->finger(), event->layerX(), event->layerY());
                                                                      
        std::cout << "Touch up" << std::endl;
    });
    
    ontouchmotionSlot = _currentDOM->document()->ontouchmotion()->connect([&](AbstractDOMEvent::Ptr event)
    {
        _canvas->finger()->fingerMotion()->execute(_canvas->finger(), event->layerX(), event->layerY());
                                                                              
        std::cout << "Touch motion" << std::endl;
    });
}

std::string
IOSWebViewDOMEngine::eval(std::string data)
{
    const char *dataChar = data.c_str();
    NSString *nsString = [NSString stringWithCString:dataChar encoding:[NSString defaultCStringEncoding]];
    NSString *result = [_webView stringByEvaluatingJavaScriptFromString: nsString];
    std::string resultString([result UTF8String]);
    
    return resultString;
}

#endif