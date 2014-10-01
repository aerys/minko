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
#include <jni.h>
#include <android/log.h>

#include "minko/dom/AbstractDOMTouchEvent.hpp"

#include "android/dom/AndroidWebViewDOMEngine.hpp"
#include "android/dom/AndroidWebViewDOMMouseEvent.hpp"

#define LOG_TAG "MINKO"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace minko;
using namespace minko::component;
using namespace minko::dom;
using namespace android;
using namespace android::dom;

static bool webViewInitialized = false;
static bool webViewPageLoaded = false;
static jobject sdlActivity = nullptr;
static jmethodID runOnUiThreadMethod = nullptr;
static jclass initWebViewTaskClass = nullptr;
static jobject initWebViewTask = nullptr;
static std::string jsGlobalResult = "";
static jclass sdlActivityClass = nullptr;
static jmethodID runOnUiThreadWithReturnValueMethod = nullptr;
static jmethodID evalJSMethod = nullptr;
static jmethodID loadUrlMethod = nullptr;

int
AndroidWebViewDOMEngine::_domUid = 0;

std::function<void(std::string&, std::string&)>
AndroidWebViewDOMEngine::handleJavascriptMessageWrapper;

// Slots
Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot onmousemoveSlot;
Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot onmousedownSlot;
Signal<minko::dom::AbstractDOMMouseEvent::Ptr>::Slot onmouseupSlot;
Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Slot ontouchdownSlot;
Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Slot ontouchupSlot;
Signal<minko::dom::AbstractDOMTouchEvent::Ptr>::Slot ontouchmotionSlot;

#ifdef __cplusplus
extern "C" {
#endif

// JNI native functions

void Java_minko_plugin_htmloverlay_InitWebViewTask_webViewInitialized(JNIEnv* env, jobject obj)
{
    webViewInitialized = true;

    LOGI("WEBVIEW INITIALIZED (FROM C++)");
}

void Java_minko_plugin_htmloverlay_MinkoWebViewClient_webViewPageLoaded(JNIEnv* env, jobject obj)
{
    webViewPageLoaded = true;

    LOGI("WEBVIEW HAS FINISHED TO LOAD THE PAGE (FROM C++)");
}

void Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnMessage(JNIEnv* env, jobject obj, jstring message)
{
    LOGI("RECEIVED A MESSAGE FROM JS: ");
    
    const char *nativeString = env->GetStringUTFChars(message, 0);

    LOGI(nativeString);
}

void Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnJSResult(JNIEnv* env, jobject obj, jstring jsResult)
{
    const char *nativeString = env->GetStringUTFChars(jsResult, 0);

    env->ReleaseStringUTFChars(jsResult, 0);

    jsGlobalResult = std::string(nativeString);
    
    LOGI("RECEIVED A JS RESULT: ");
    LOGI(nativeString);
}

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

AndroidWebViewDOMEngine::AndroidWebViewDOMEngine() :
	_onload(Signal<AbstractDOM::Ptr, std::string>::create()),
	_onmessage(Signal<AbstractDOM::Ptr, std::string>::create()),
	_visible(true),
    _waitingForLoad(true),
    _isReady(false), 
    _webViewWidth(0)
{
}

void
AndroidWebViewDOMEngine::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;

    // JNI
    LOGI("Get the SDL JNIEnv");
    // Retrieve the JNI environment from SDL 
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    LOGI("Get the SDLActivity instance");
    // Retrieve the Java instance of the SDLActivity
    sdlActivity = (jobject)SDL_AndroidGetActivity();
    LOGI("Get sdlActivity class");
    sdlActivityClass = env->GetObjectClass(sdlActivity);

    LOGI("Get initWebViewTask class");
    initWebViewTaskClass = env->FindClass("minko/plugin/htmloverlay/InitWebViewTask");
    LOGI("Get initWebViewTask constructor method");
    jmethodID initWebViewTaskCtor = env->GetMethodID(initWebViewTaskClass, "<init>", "(Landroid/app/Activity;)V");
    LOGI("Instanciate a initWebViewTask");
    initWebViewTask = env->NewObject(initWebViewTaskClass, initWebViewTaskCtor, sdlActivity);

    LOGI("Get runOnUiThread method from sdlActivity");
    runOnUiThreadMethod = env->GetMethodID(sdlActivityClass, "runOnUiThread", "(Ljava/lang/Runnable;)V");
    LOGI("Call runOnUiThread with initWebViewTask");

    env->CallVoidMethod(sdlActivity, runOnUiThreadMethod, initWebViewTask);
    LOGI("Get eval function from Java WebView");
    evalJSMethod = env->GetMethodID(initWebViewTaskClass, "evalJS", "(Ljava/lang/String;)Ljava/lang/String;");

    // Get loadUrl method
    loadUrlMethod = env->GetMethodID(initWebViewTaskClass, "loadUrl", "(Ljava/lang/String;)V");

    visible(_visible);

    _canvasResizedSlot = _canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        /*
        // Change the width of the webview directly
        CGRect webViewFrame = _webView.frame;
        webViewFrame.size.width = w / 2;
        webViewFrame.size.height = h / 2;
        _webView.frame = webViewFrame;
        */
        _webViewWidth = w;
        // Useful on iOS to have the same coordinates on the web view as on the canvas
        updateWebViewWidth();
    });

	_enterFrameSlot = _sceneManager->frameBegin()->connect([&](std::shared_ptr<component::SceneManager>, float, float)
	{
		enterFrame();
	});
}

void
AndroidWebViewDOMEngine::createNewDom()
{
	std::string domName = "Minko.dom" + std::to_string(_domUid++);

	_currentDOM = AndroidWebViewDOM::create(domName, shared_from_this());
}

minko::dom::AbstractDOM::Ptr
AndroidWebViewDOMEngine::mainDOM()
{
	return _currentDOM;
}

void
AndroidWebViewDOMEngine::enterFrame()
{
    if (_waitingForLoad)
    {
        if (webViewInitialized)
        {
            _waitingForLoad = false;
            load(_uriToLoad);
            updateWebViewWidth();
        }

        return;
    }

    if (!_isReady && webViewPageLoaded)
    {
    	if (_currentDOM->initialized())
    		createNewDom();

    	_currentDOM->initialized(true);

    	_currentDOM->onload()->execute(_currentDOM, _currentDOM->fullUrl());
    	_onload->execute(_currentDOM, _currentDOM->fullUrl());
        
        registerDomEvents();

        _isReady = true;
    }


    for(auto element : AndroidWebViewDOMElement::domElements)
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
}

AndroidWebViewDOMEngine::Ptr
AndroidWebViewDOMEngine::create()
{
	AndroidWebViewDOMEngine::Ptr engine(new AndroidWebViewDOMEngine);
	return engine;
}

AbstractDOM::Ptr
AndroidWebViewDOMEngine::load(std::string uri)
{
    if (_currentDOM == nullptr || _currentDOM->initialized())
        createNewDom();

    if (_waitingForLoad)
    {
        _uriToLoad = uri;
    }
    else
    {

        bool isHttp	= uri.substr(0, 7) == "http://";
        bool isHttps = uri.substr(0, 8) == "https://";
        
        if (!isHttp && !isHttps)
        {
#if __ANDROID__
    uri = "file:///android_asset/" + uri;
#else
# if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
            uri = "../../asset/" + uri;
# elif TARGET_OS_MAC // OSX
            std::string path = file::File::getBinaryDirectory();
#  if DEBUG
            uri = path + "/../../../asset/" + uri;
#  else
            uri = path + "/asset/" + uri;
#  endif
# endif
#endif
        }

        // Retrieve the JNI environment from SDL 
        auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        // Call URL loading method
        LOGI("Try to load this URL:");
        LOGI(uri.c_str());
        env->CallVoidMethod(initWebViewTask, loadUrlMethod, env->NewStringUTF(uri.c_str()));

        /*
        std::string jsEval = "Minko.loadUrl('" + uri + "')";
        eval(jsEval);
        */
    }

	return _currentDOM;
}

void
AndroidWebViewDOMEngine::clear()
{
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
AndroidWebViewDOMEngine::onload()
{
	return _onload;
}

Signal<AbstractDOM::Ptr, std::string>::Ptr
AndroidWebViewDOMEngine::onmessage()
{
	return _onmessage;
}

bool
AndroidWebViewDOMEngine::visible()
{
    return _visible;
}

void
AndroidWebViewDOMEngine::visible(bool value)
{
    if (_canvas != nullptr)
	{
        if (value != _visible)
        {
            if (value)
            {
/*
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE // iOS
                [_window addSubview:_webView];
#elif TARGET_OS_MAC // OSX
                [_window.contentView addSubview:_webView];
#endif
*/
            }
            /*
            else
                [_webView removeFromSuperview];
            */
        }
	}
    
	_visible = value;
}

void
AndroidWebViewDOMEngine::registerDomEvents()
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
    
    ontouchdownSlot = std::static_pointer_cast<AndroidWebViewDOMElement>(_currentDOM->document())->ontouchdown()->connect([&](AbstractDOMTouchEvent::Ptr event)
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
    
    ontouchupSlot = std::static_pointer_cast<AndroidWebViewDOMElement>(_currentDOM->document())->ontouchup()->connect([&](AbstractDOMTouchEvent::Ptr event)
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
    
    ontouchmotionSlot = std::static_pointer_cast<AndroidWebViewDOMElement>(_currentDOM->document())->ontouchmotion()->connect([&](AbstractDOMTouchEvent::Ptr event)
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
}

void
AndroidWebViewDOMEngine::updateWebViewWidth()
{
    /*
    std::string jsEval = "Minko.changeViewportWidth(" + std::to_string(_webViewWidth) + ");";
    
    eval(jsEval);
    */
}

std::string
AndroidWebViewDOMEngine::eval(std::string data)
{
    if (!webViewPageLoaded)
    {
        LOGI("Try to eval before WebView page is loaded!");
        LOGI(data.c_str());
        return "null";
    }

    // Retrieve the JNI environment from SDL 
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    // Get the evalJS instance
    jstring js = env->NewStringUTF(data.c_str());

    LOGI("EVAL JS: ");
    LOGI(data.c_str());
    jstring evalJSResult = (jstring)env->CallObjectMethod(initWebViewTask, evalJSMethod, js);

    const char* evalJSResultString = env->GetStringUTFChars(evalJSResult, JNI_FALSE);

    auto result = std::string(evalJSResultString);

    // clean up the local references.
    env->ReleaseStringUTFChars(evalJSResult, evalJSResultString);
    env->DeleteLocalRef(js);
    env->DeleteLocalRef(evalJSResult);
    
    LOGI("EVAL FINAL VALUE: ");
    LOGI(result.c_str());
    
    return result;
}