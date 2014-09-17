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

#include "minko/MinkoSDL.hpp"

#include "SDL.h"
#include "SDL_syswm.h"

#include "minko/dom/AbstractDOMTouchEvent.hpp"

#include "macwebview/dom/MacWebViewDOMEngine.hpp"
#include "macwebview/dom/MacWebViewDOMMouseEvent.hpp"

#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
#elif TARGET_OS_MAC // OSX
# include "macwebview/dom/OSXWebUIDelegate.h"
#endif

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

#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Slot ontouchdownSlot;
Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Slot ontouchupSlot;
Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Slot ontouchmotionSlot;
#endif

MacWebViewDOMEngine::MacWebViewDOMEngine() :
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_visible(true),
    _waitingForLoad(true),
    _isReady(false)
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
    , _webViewWidth(0)
#endif
{
}

void
MacWebViewDOMEngine::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;

    // URL of the local file that contains JS callback handler
    std::string uri = "asset/html/iframe.html";

#if TARGET_OS_MAC // OSX
    uri = "file://" + file::File::getBinaryDirectory() + "/" + uri;
#endif

    const char *cURI = uri.c_str();
    NSString *nsURI = [NSString stringWithCString:cURI encoding:[NSString defaultCStringEncoding]];

    // FIXME: Replate by a proper call to Loader.
    NSURL *url = [NSURL URLWithString:nsURI];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    
    // Get window from canvas
    auto newCanvas = std::static_pointer_cast<Canvas>(canvas);
    SDL_Window* sdlWindow = newCanvas->window();
    SDL_SysWMinfo info;
    
    SDL_VERSION(&info.version);
    
    if (SDL_GetWindowWMInfo(sdlWindow, &info))
    {
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
        // Get the UIKit window from SDL
        _window = info.info.uikit.window;
        
        // Create the web view
        _webView = [[IOSWebView alloc] initWithFrame:_window.bounds];
        
        // Change web view's background properties
        UIColor * clearColor = [UIColor colorWithRed:255/255.0f green:1/255.0f blue:0/255.0f alpha:0.f];
        [_webView setBackgroundColor: clearColor];
        [_webView setOpaque: NO];
        [_webView.scrollView setDelaysContentTouches: NO];
        
        // Disable web view scroll
        _webView.scrollView.scrollEnabled = NO;
        _webView.scrollView.bounces = NO;
        
        // Resize the web view according to device dimension and orientation
        _webView.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin |
        UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleBottomMargin |
        UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        _webView.scalesPageToFit = YES;
        
        NSHTTPCookieStorage *cookieStorage = [NSHTTPCookieStorage sharedHTTPCookieStorage];
        [cookieStorage setCookieAcceptPolicy:NSHTTPCookieAcceptPolicyAlways];

        // Disable web view interaction
        //[_webView setUserInteractionEnabled:NO];
        
        _window.rootViewController.view.autoresizesSubviews = YES;
        
        // Add the web view to the current window
        [_window.rootViewController.view addSubview:_webView];
        
        // Save the web view width
        _webViewWidth = _webView.frame.size.width;
        
        // Load iframe containing bridge JS callback handler
        [_webView loadRequest:request];
        
        _canvasResizedSlot = _canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
        {
            // Change the width of the webview directly
            CGRect webViewFrame = _webView.frame;
            webViewFrame.size.width = w / 2;
            webViewFrame.size.height = h / 2;
            _webView.frame = webViewFrame;
            
            _webViewWidth = w;
            // Useful on iOS to have the same coordinates on the web view as on the canvas
            updateWebViewWidth();
        });

#elif TARGET_OS_MAC // OSX
        // Get the Cocoa window from SDL
        _window = info.info.cocoa.window;
        
        // Create the web view
        _webView = [[OSXWebView alloc] initWithFrame:NSMakeRect(0, 0, _canvas->width(), _canvas->height())];

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
        [_webView setUIDelegate:[OSXWebUIDelegate alloc]];
        
        // Resize the overlay according to the window's size
        [_window.contentView setAutoresizesSubviews:YES];
        [_webView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        
        // Add the web view to the current window
        [_window.contentView addSubview:_webView];
        
        // Load iframe containing bridge JS callback handler
        [[_webView mainFrame] loadRequest:request];
#endif
        
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
        
        // Enable bridge logging
        // [WebViewJavascriptBridge enableLogging];
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
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
            updateWebViewWidth();
#endif
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
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
            uri = "../../asset/" + uri;
#elif TARGET_OS_MAC // OSX
            std::string path = file::File::getBinaryDirectory();
# if DEBUG
            uri = path + "/../../../asset/" + uri;
# else
            uri = path + "/asset/" + uri;
# endif
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
            {
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
                [_window addSubview:_webView];
#elif TARGET_OS_MAC // OSX
                [_window.contentView addSubview:_webView];
#endif
            }
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
        _isReady = (value == "true");
    }
    else if (type == "alert")
    {
        eval("[html-overlay] " + value);
    }
    else if (type == "log")
    {
        std::cout << "[html-overlay] " << value << std::endl;
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
        int x = event->clientX();
        int y = event->clientY();
        
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
    
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
    ontouchdownSlot = std::static_pointer_cast<MacWebViewDOMElement>(_currentDOM->document())->ontouchdown()->connect([&](AbstractDOMTouchEvent::Ptr event)
    {
            int fingerId = event->fingerId();
            float x = event->clientX();
            float y = event->clientY();
            
            SDL_Event sdlEvent;
            sdlEvent.type = SDL_FINGERDOWN;
            sdlEvent.tfinger.fingerId = fingerId;
            sdlEvent.tfinger.x =  x / _canvas->width();
            sdlEvent.tfinger.y = y / _canvas->height();

            SDL_PushEvent(&sdlEvent);
            
            // Create a new touch object and keep it with a list
            auto touch = minko::SDLTouch::create(std::static_pointer_cast<Canvas>(_canvas));
            touch->fingerId(fingerId);
            touch->x(x);
            touch->y(y);

            _touches.insert(std::pair<int, std::shared_ptr<minko::SDLTouch>>(fingerId, touch));
    });
    
    ontouchupSlot = std::static_pointer_cast<MacWebViewDOMElement>(_currentDOM->document())->ontouchup()->connect([&](AbstractDOMTouchEvent::Ptr event)
    {
        int fingerId = event->fingerId();
        float x = event->clientX();
        float y = event->clientY();
        
        SDL_Event sdlEvent;
        sdlEvent.type = SDL_FINGERUP;
        sdlEvent.tfinger.fingerId = fingerId;
        sdlEvent.tfinger.x = x / _canvas->width();
        sdlEvent.tfinger.y = y / _canvas->height();

        SDL_PushEvent(&sdlEvent);
        
        // We check that the finger is into the list before removing it
        if (_touches.find(fingerId) != _touches.end())
        {
            _touches.erase(fingerId);
        }
    });
    
    ontouchmotionSlot = std::static_pointer_cast<MacWebViewDOMElement>(_currentDOM->document())->ontouchmotion()->connect([&](AbstractDOMTouchEvent::Ptr event)
    {
            int fingerId = event->fingerId();
            float oldX = _touches.at(fingerId)->minko::input::Touch::x();
            float oldY = _touches.at(fingerId)->minko::input::Touch::y();
            float x = event->clientX();
            float y = event->clientY();
            
            SDL_Event sdlEvent;
            sdlEvent.type = SDL_FINGERMOTION;
            sdlEvent.tfinger.fingerId = fingerId;
            sdlEvent.tfinger.x = x / _canvas->width();
            sdlEvent.tfinger.y = y / _canvas->height();
            sdlEvent.tfinger.dx = (x - oldX) / _canvas->width();
            sdlEvent.tfinger.dy = (y - oldY) / _canvas->height();
        
            SDL_PushEvent(&sdlEvent);
            
            // Store finger information
            _touches.at(fingerId)->x(x);
            _touches.at(fingerId)->y(y);
    });
#endif
}

#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
void
MacWebViewDOMEngine::updateWebViewWidth()
{
    std::string jsEval = "Minko.changeViewportWidth(" + std::to_string(_webViewWidth) + ");";
    
    eval(jsEval);
}
#endif

std::string
MacWebViewDOMEngine::eval(std::string data)
{
    const char *dataChar = data.c_str();
    NSString *nsString = [NSString stringWithCString:dataChar encoding:[NSString defaultCStringEncoding]];
    NSString *result = [_webView stringByEvaluatingJavaScriptFromString: nsString];
    std::string resultString([result UTF8String]);
    
    return resultString;
}