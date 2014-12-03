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

#include "apple/dom/AppleWebViewDOMEngine.hpp"
#include "apple/dom/AppleWebViewDOMMouseEvent.hpp"

#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
#elif TARGET_OS_MAC // OSX
# include "apple/dom/OSXWebUIDelegate.h"
#endif

using namespace minko;
using namespace minko::component;
using namespace minko::dom;
using namespace apple;
using namespace apple::dom;

int
AppleWebViewDOMEngine::_domUid = 0;

std::function<void(std::string&, std::string&)>
AppleWebViewDOMEngine::handleJavascriptMessageWrapper;

AppleWebViewDOMEngine::AppleWebViewDOMEngine() :
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_visible(true),
    _waitingForLoad(true),
    _isReady(false),
    _updateNextFrame(false),
    _pollRate(-1),
    _lastUpdateTime(0.0)
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
    , _webViewWidth(0)
#endif
{
}

void
AppleWebViewDOMEngine::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;

    // URL of the local file that contains JS callback handler
    std::string uri = "asset/html/iframe.html";

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
        _webView.scrollView.decelerationRate = UIScrollViewDecelerationRateNormal;

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
        // [_webView loadRequest:request];

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
#endif

        // Create a C++ handler to process the message received by the Javascript bridge
        handleJavascriptMessageWrapper = std::bind(
                       &AppleWebViewDOMEngine::handleJavascriptMessage,
                       this,
                       std::placeholders::_1,
                       std::placeholders::_2
        );

        // Create the bridge
        _bridge = [WebViewJavascriptBridge bridgeForWebView:_webView handler:
                   ^(id data, WVJBResponseCallback responseCallback)
        {

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

                AppleWebViewDOMEngine::handleJavascriptMessageWrapper(type, value);
            }
            else if ([data isKindOfClass:[NSString class]])
            {
                NSString* dataString = (NSString *)data;
                std::string value([dataString UTF8String]);
                std::string type = "log";

                AppleWebViewDOMEngine::handleJavascriptMessageWrapper(type, value);
            }
         }];
    }

    visible(_visible);

	_enterFrameSlot = _sceneManager->frameBegin()->connect([&](std::shared_ptr<component::SceneManager>, float t, float dt)
	{
		enterFrame(t);
	});
}

void
AppleWebViewDOMEngine::createNewDom()
{
	std::string domName = "Minko.dom" + std::to_string(_domUid++);

	_currentDOM = AppleWebViewDOM::create(domName, shared_from_this());
}

minko::dom::AbstractDOM::Ptr
AppleWebViewDOMEngine::mainDOM()
{
	return _currentDOM;
}

void
AppleWebViewDOMEngine::enterFrame(float time)
{
    if (_waitingForLoad)
    {
        std::string jsEval = "try{ var loaded = Minko.loaded; ('true')} catch(e) {('false')}";

        std::string res = eval(jsEval);

        if (res == "true")
        {
            _waitingForLoad = false;
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

    if (_pollRate == -1 || _updateNextFrame || (_pollRate != 0 && (time - _lastUpdateTime) > (1000.0 / (float)(_pollRate))))
    {
        for(auto element : AppleWebViewDOMElement::domElements)
        {
            element->update();
        }

        if (_currentDOM->initialized() && _isReady)
        {
            std::string jsEval = "(Minko.messagesToSend.length);";
            std::string evalResult = eval(jsEval);
            int l = atoi(evalResult.c_str());

            if (l > 0)
            {
                std::cout << "Messages found!" << std::endl;
                for(int i = 0; i < l; ++i)
                {
                    jsEval = "(Minko.messagesToSend[" + std::to_string(i) + "])";

                    std::string message = eval(jsEval);

                    std::cout << "Message: " << message << std::endl;

                    _currentDOM->onmessage()->execute(_currentDOM, message);
                    _onmessage->execute(_currentDOM, message);
                }

                jsEval = "Minko.messagesToSend = [];";
                eval(jsEval);
            }
        }

        _updateNextFrame = false;
        _lastUpdateTime = time;
    }

}

AppleWebViewDOMEngine::Ptr
AppleWebViewDOMEngine::create()
{
	AppleWebViewDOMEngine::Ptr engine(new AppleWebViewDOMEngine);
	return engine;
}

AbstractDOM::Ptr
AppleWebViewDOMEngine::load(std::string uri)
{
    if (_currentDOM == nullptr || _currentDOM->initialized())
        createNewDom();

    bool isHttp		= uri.substr(0, 7) == "http://";
    bool isHttps	= uri.substr(0, 8) == "https://";

    if (!isHttp && !isHttps)
    {
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
        uri = "file://" + file::File::getBinaryDirectory() + "/asset/" + uri;
#elif TARGET_OS_MAC // OSX
        std::string path = file::File::getBinaryDirectory();
# if DEBUG
        uri = path + "/../../../asset/" + uri;
# else
        uri = path + "/asset/" + uri;
# endif
#endif
    }

    std::cout << "AppleWebViewDOMEngine::load(): " << uri << std::endl;

    const char *cURI = uri.c_str();
    NSString *nsURI = [NSString stringWithCString:cURI encoding:[NSString defaultCStringEncoding]];

    NSURL *url = [NSURL URLWithString:nsURI];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];

#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
    [_webView loadRequest:request];
#elif TARGET_OS_MAC // OSX
    [[_webView mainFrame] loadRequest:request];
#endif

    _waitingForLoad = true;

    return _currentDOM;
}

void
AppleWebViewDOMEngine::clear()
{
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
AppleWebViewDOMEngine::onload()
{
	return _onload;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
AppleWebViewDOMEngine::onmessage()
{
	return _onmessage;
}

bool
AppleWebViewDOMEngine::visible()
{
    return _visible;
}

void
AppleWebViewDOMEngine::visible(bool value)
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

void AppleWebViewDOMEngine::handleJavascriptMessage(std::string type, std::string value)
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
AppleWebViewDOMEngine::registerDomEvents()
{
    _onmousedownSlot = _currentDOM->document()->onmousedown()->connect([&](AbstractDOMMouseEvent::Ptr event)
    {
        int x = event->clientX();
        int y = event->clientY();
        
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);

        _canvas->mouse()->leftButtonDown()->execute(_canvas->mouse());
    });

    _onmouseupSlot = _currentDOM->document()->onmouseup()->connect([&](AbstractDOMMouseEvent::Ptr event)
    {
        int x = event->clientX();
        int y = event->clientY();
        
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);
        
        _canvas->mouse()->leftButtonUp()->execute(_canvas->mouse());
    });

    _onmousemoveSlot = _currentDOM->document()->onmousemove()->connect([&](AbstractDOMMouseEvent::Ptr event)
    {
        int x = event->clientX();
        int y = event->clientY();
        
        auto oldX = _canvas->mouse()->x();
        auto oldY = _canvas->mouse()->y();

        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);

        _canvas->mouse()->move()->execute(_canvas->mouse(), x - oldX, y - oldY);
    });
    
    _ontouchstartSlot = _currentDOM->document()->ontouchstart()->connect([&](AbstractDOMTouchEvent::Ptr event)
    {
        int identifier = event->identifier();
        float x = event->clientX();
        float y = event->clientY();

        SDL_Event sdlEvent;
        sdlEvent.type = SDL_FINGERDOWN;
        sdlEvent.tfinger.fingerId = identifier;
        sdlEvent.tfinger.x =  x / _canvas->width();
        sdlEvent.tfinger.y = y / _canvas->height();

        SDL_PushEvent(&sdlEvent);
    });
    
    _ontouchendSlot = _currentDOM->document()->ontouchend()->connect([&](AbstractDOMTouchEvent::Ptr event)
    {
        int identifier = event->identifier();
        float x = event->clientX();
        float y = event->clientY();
        
        SDL_Event sdlEvent;
        sdlEvent.type = SDL_FINGERUP;
        sdlEvent.tfinger.fingerId = identifier;
        sdlEvent.tfinger.x = x / _canvas->width();
        sdlEvent.tfinger.y = y / _canvas->height();

        SDL_PushEvent(&sdlEvent);
    });
    
    _ontouchmoveSlot = _currentDOM->document()->ontouchmove()->connect([&](AbstractDOMTouchEvent::Ptr event)
    {
        int identifier = event->identifier();
        auto identifiers = _canvas->touch()->identifiers();

        if (std::find(identifiers.begin(), identifiers.end(), identifier) == identifiers.end())
            return;

        float x = event->clientX();
        float y = event->clientY();
        
        float oldX = _canvas->touch()->touch(identifier)->x();
        float oldY = _canvas->touch()->touch(identifier)->y();

        SDL_Event sdlEvent;
        sdlEvent.type = SDL_FINGERMOTION;
        sdlEvent.tfinger.fingerId = identifier;
        sdlEvent.tfinger.x = x / _canvas->width();
        sdlEvent.tfinger.y = y / _canvas->height();
        sdlEvent.tfinger.dx = (x - oldX) / _canvas->width();
        sdlEvent.tfinger.dy = (y - oldY) / _canvas->height();
    
        SDL_PushEvent(&sdlEvent);
    });
}

#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
void
AppleWebViewDOMEngine::updateWebViewWidth()
{
    std::string jsEval = "Minko.changeViewportWidth(" + std::to_string(_webViewWidth) + ");";

    eval(jsEval);
}
#endif

std::string
AppleWebViewDOMEngine::eval(std::string data)
{
    const char *dataChar = data.c_str();
    NSString *nsString = [NSString stringWithCString:dataChar encoding:[NSString defaultCStringEncoding]];
    NSString *result = [_webView stringByEvaluatingJavaScriptFromString: nsString];
    std::string resultString([result UTF8String]);

    return resultString;
}
