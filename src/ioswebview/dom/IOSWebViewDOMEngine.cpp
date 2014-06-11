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
#include "ioswebview/dom/IOSWebViewDOMEvent.hpp"

#include "minko/MinkoSDL.hpp"

#include "SDL.h"
#include "SDL_syswm.h"

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
    _isReady(false),
    _webViewWidth(0),
    _fingers()
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
        
        //[_webView setUserInteractionEnabled:NO];
        
        _window.rootViewController.view.autoresizesSubviews = YES;
        
        // Add the web view to the current window
        [_window.rootViewController.view addSubview:_webView];
        
        // Save the web view width
        _webViewWidth = _webView.frame.size.width;
        
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
                   //NSLog(@"Received message from javascript: %@", data);
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
        _webViewWidth = w;
        
        updateWebViewWidth();
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
    //std::cout << "Processing the current message: [type: " << type << "; value: " << value << "]" << std::endl;
    
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
        raiseDomEvent(type, value);
    }
    else if (type == "touchend")
    {
        raiseDomEvent(type, value);
    }
    else if (type == "touchmove")
    {
        raiseDomEvent(type, value);
    }
}

void
IOSWebViewDOMEngine::raiseDomEvent(const std::string& type, const std::string& value)
{
    std::size_t separatorIndex = value.find("|");
    if (separatorIndex != std::string::npos)
    {
        std::string accessor = value.substr(0, separatorIndex);
        std::string eventIndex = value.substr(separatorIndex + 1);

        auto element = IOSWebViewDOMElement::getDOMElement(accessor, shared_from_this());
        
        std::string eventName = accessor + ".minkoEvents[" + eventIndex + "]";
        IOSWebViewDOMEvent::Ptr event = IOSWebViewDOMEvent::create(eventName, shared_from_this());
        
        if (type == "touchstart")
        {
            element->ontouchdown()->execute(event);
            element->onmousedown()->execute(event);
        }
        else if (type == "touchend")
        {
            element->ontouchup()->execute(event);
            element->onclick()->execute(event);
            element->onmouseup()->execute(event);
        }
        else if (type == "touchmove")
        {
            element->ontouchmotion()->execute(event);
            element->onmousemove()->execute(event);
        }
    }
}

void
IOSWebViewDOMEngine::registerDomEvents()
{
    onmousedownSlot = _currentDOM->document()->onmousedown()->connect([&](AbstractDOMEvent::Ptr event)
    {
        int x = event->clientX();
        int y = event->clientY();
        
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);

//        SDL_Event sdlEvent;
//        sdlEvent.type = SDL_MOUSEBUTTONDOWN;
//        sdlEvent.button.button = SDL_BUTTON_LEFT;
//
//        SDL_PushEvent(&sdlEvent);
        
        _canvas->mouse()->leftButtonDown()->execute(_canvas->mouse());
    });

    onmouseupSlot = _currentDOM->document()->onmouseup()->connect([&](AbstractDOMEvent::Ptr event)
    {
        int x = event->layerX();
        int y = event->layerY();
        
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);

//        SDL_Event sdlEvent;
//        sdlEvent.type = SDL_MOUSEBUTTONUP;
//        sdlEvent.button.button = SDL_BUTTON_LEFT;
//        
//        SDL_PushEvent(&sdlEvent);
        
        _canvas->mouse()->leftButtonUp()->execute(_canvas->mouse());
    });

    onmousemoveSlot = _currentDOM->document()->onmousemove()->connect([&](AbstractDOMEvent::Ptr event)
    {
        int x = event->clientX();
        int y = event->clientY();
                                                                          
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);
        
//        SDL_Event sdlEvent;
//        sdlEvent.type = SDL_MOUSEMOTION;
//        sdlEvent.motion.x = x;
//        sdlEvent.motion.y = y;
//
//        SDL_PushEvent(&sdlEvent);
        
        auto oldX = _canvas->mouse()->x();
        auto oldY = _canvas->mouse()->y();

        _canvas->mouse()->move()->execute(_canvas->mouse(), event->clientX() - oldX, event->clientY() - oldY);
    });
    
    ontouchdownSlot = _currentDOM->document()->ontouchdown()->connect([&](AbstractDOMEvent::Ptr event)
    {
        // Get number of finger
        std::string js = event->accessor() + ".changedTouches.length";
        int fingerNumber = atoi(eval(js).c_str());
        
        for (auto i = 0; i < fingerNumber; i++)
        {
            int fingerId = event->identifier(i);
            int x = event->clientX(i);
            int y = event->clientY(i);
            
            SDL_Event sdlEvent;
            sdlEvent.type = SDL_FINGERDOWN;
            sdlEvent.tfinger.fingerId = fingerId;
            sdlEvent.tfinger.x =  x / _canvas->width();
            sdlEvent.tfinger.y = y / _canvas->height();
            
            //std::cout << "[DOM] Finger down id: " << sdlEvent.tfinger.fingerId << " (for i = " << i << ")" << std::endl;
            
            //std::cout << "[DOM] Finger down at: " << sdlEvent.tfinger.x << ", " << sdlEvent.tfinger.y << std::endl;
            
            SDL_PushEvent(&sdlEvent);
            
            // Add finger to list
            auto finger = minko::SDLFinger::create(std::static_pointer_cast<Canvas>(_canvas));
            finger->x(x);
            finger->y(y);

            _fingers.insert(std::pair<int, std::shared_ptr<minko::SDLFinger>>(sdlEvent.tfinger.fingerId, finger));
        }
        
        //_canvas->finger()->fingerDown()->execute(_canvas->finger(), event->layerX(), event->layerY());
    });
    
    ontouchupSlot = _currentDOM->document()->ontouchup()->connect([&](AbstractDOMEvent::Ptr event)
    {
//        SDL_Event sdlEvent;
//        sdlEvent.type = SDL_FINGERUP;
//        sdlEvent.tfinger.fingerId = event->identifier();
//        sdlEvent.tfinger.x = event->layerX();
//        sdlEvent.tfinger.y = event->layerY();
//        
//        SDL_PushEvent(&sdlEvent);

        // Get number of finger
        std::string js = event->accessor() + ".changedTouches.length";
        int fingerNumber = atoi(eval(js).c_str());
        
        for (auto i = 0; i < fingerNumber; i++)
        {
            int x = event->clientX(i);
            int y = event->clientY(i);
            
            SDL_Event sdlEvent;
            sdlEvent.type = SDL_FINGERUP;
            sdlEvent.tfinger.fingerId = event->identifier(i);
            sdlEvent.tfinger.x = x / _canvas->width();
            sdlEvent.tfinger.y = y / _canvas->height();
            
            //std::cout << "[DOM] Finger up id: " << sdlEvent.tfinger.fingerId << " (for i = " << i << ")" << std::endl;
            
            //std::cout << "[DOM] Finger up at: " << sdlEvent.tfinger.x << ", " << sdlEvent.tfinger.y << "(for i = " << i << ")" << std::endl;
            
            SDL_PushEvent(&sdlEvent);
            
            // Remove finger from list
            _fingers.erase(sdlEvent.tfinger.fingerId);
        }
        
        //_canvas->finger()->fingerUp()->execute(_canvas->finger(), event->layerX(), event->layerY());
    });
    
    ontouchmotionSlot = _currentDOM->document()->ontouchmotion()->connect([&](AbstractDOMEvent::Ptr event)
    {
//        SDL_Event sdlEvent;
//        sdlEvent.type = SDL_FINGERMOTION;
//        sdlEvent.tfinger.x = event->layerX();
//        sdlEvent.tfinger.y = event->layerY();
//        
//        SDL_PushEvent(&sdlEvent);
        
        // Get number of finger
        std::string js = event->accessor() + ".changedTouches.length";
        int fingerNumber = atoi(eval(js).c_str());
        
        for (auto i = 0; i < fingerNumber; i++)
        {
            int fingerId = event->identifier(i);
            float oldX = _fingers.at(fingerId)->minko::input::Finger::x();
            float oldY = _fingers.at(fingerId)->minko::input::Finger::y();
            float x = event->clientX(i);
            float y = event->clientY(i);
            
            SDL_Event sdlEvent;
            sdlEvent.type = SDL_FINGERMOTION;
            sdlEvent.tfinger.fingerId = fingerId;
            sdlEvent.tfinger.x = x / _canvas->width();
            sdlEvent.tfinger.y = y / _canvas->height();
            sdlEvent.tfinger.dx = (x - oldX) / _canvas->width();
            sdlEvent.tfinger.dy = (y - oldY) / _canvas->height();
            
//            std::cout << "[DOM] Canvas size: " << _canvas->width() << "x" << _canvas->height() << std::endl;
//            
//            std::cout << "[DOM] Finger motion => " << std::endl
//            << "x: " << x << ", y = " << y << std::endl
//            << "oldX: " << oldX << ", oldY: " << oldY << std::endl
//            << "dx: " << ((x - oldX) / _canvas->width()) << "(" << sdlEvent.tfinger.dx
//            << "), dy: " << ((y - oldY) / _canvas->height()) << "(" << sdlEvent.tfinger.dy << ")" << std::endl;
            
            SDL_PushEvent(&sdlEvent);
            
            // Store finger information
            _fingers.at(fingerId)->x(x);
            _fingers.at(fingerId)->y(y);
        }
        
//        _canvas->finger()->fingerMotion()->execute(_canvas->finger(), event->layerX(), event->layerY());
    });
}

void
IOSWebViewDOMEngine::updateWebViewWidth()
{
    std::string jsEval = "Minko.changeViewportWidth(" + std::to_string(_webViewWidth) + ");";
    
    eval(jsEval);
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