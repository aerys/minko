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

#include "json/json.h"

#include "SDL.h"
#include <android/log.h>

#include "minko/dom/AbstractDOMTouchEvent.hpp"

#include "android/dom/AndroidWebViewDOMEngine.hpp"
#include "android/dom/AndroidWebViewDOMMouseEvent.hpp"

#define LOG_TAG "MINKOCPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace minko;
using namespace minko::component;
using namespace minko::dom;
using namespace android;
using namespace android::dom;

int
AndroidWebViewDOMEngine::_domUid = 0;

// WebView Signals
static Signal<>::Ptr onWebViewInitialized = Signal<>::create();
static Signal<>::Ptr onWebViewPageLoaded = Signal<>::create();

// JS messages
std::vector<std::string> AndroidWebViewDOMEngine::messages;

// JS events
std::multimap<std::string, minko::dom::JSEventData> AndroidWebViewDOMEngine::events;

// WebView Slots
Signal<>::Slot onWebViewInitializedSlot;
Signal<>::Slot onWebViewPageLoadedSlot;

// Inputs Slots
Signal<minko::dom::JSEventData>::Slot onmousemoveSlot;
Signal<minko::dom::JSEventData>::Slot onmousedownSlot;
Signal<minko::dom::JSEventData>::Slot onmouseupSlot;
Signal<minko::dom::JSEventData, int>::Slot ontouchdownSlot;
Signal<minko::dom::JSEventData, int>::Slot ontouchupSlot;
Signal<minko::dom::JSEventData, int>::Slot ontouchmotionSlot;

#ifdef __cplusplus
extern "C" {
#endif

// JNI native functions

void Java_minko_plugin_htmloverlay_InitWebViewTask_webViewInitialized(JNIEnv* env, jobject obj)
{
    onWebViewInitialized->execute();
}

void Java_minko_plugin_htmloverlay_MinkoWebViewClient_webViewPageLoaded(JNIEnv* env, jobject obj)
{
    onWebViewPageLoaded->execute();
}

void Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnMessage(JNIEnv* env, jobject obj, jstring message)
{
    LOGI("RECEIVED A MESSAGE FROM JS: ");
 
    const char *nativeMessage = env->GetStringUTFChars(message, 0);

    // Don't forget to release jstring!
    env->ReleaseStringUTFChars(message, 0);

    LOGI("Native onMessage");
    LOGI(nativeMessage);

    AndroidWebViewDOMEngine::messages.push_back(std::string(nativeMessage));
}

void Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnEvent(JNIEnv* env, jobject obj, jstring accessor, jstring eventData)
{
    LOGI("RECEIVED AN EVENT FROM JS: ");
 
    const char *nativeAccessor = env->GetStringUTFChars(accessor, 0);
    const char *nativeEvent = env->GetStringUTFChars(eventData, 0);

    // Don't forget to release jstring!
    env->ReleaseStringUTFChars(accessor, 0);
    env->ReleaseStringUTFChars(eventData, 0);

    LOGI("Native onEvent");
    LOGI(nativeAccessor);
    LOGI(nativeEvent);

    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(nativeEvent, root, false))
        LOGI(reader.getFormattedErrorMessages().c_str());

    // Store values of this JS event
    JSEventData jsEventData;

    jsEventData.type = root.get("type", "unknown").asString();
    jsEventData.clientX = root.get("clientX", 0).asInt();
    jsEventData.clientY = root.get("clientY", 0).asInt();
    jsEventData.pageX = root.get("pageX", 0).asInt();
    jsEventData.pageY = root.get("pageY", 0).asInt();
    jsEventData.layerX = root.get("layerX", 0).asInt();
    jsEventData.layerY = root.get("layerY", 0).asInt();
    jsEventData.screenX = root.get("screenX", 0).asInt();
    jsEventData.screenY = root.get("screenY", 0).asInt();

    LOGI(jsEventData.type.c_str());

    /*
    LOGI(std::to_string(jsEventData.clientX).c_str());
    LOGI(std::to_string(jsEventData.clientY).c_str());
    LOGI(std::to_string(jsEventData.pageX).c_str());
    LOGI(std::to_string(jsEventData.pageY).c_str());
    LOGI(std::to_string(jsEventData.layerX).c_str());
    LOGI(std::to_string(jsEventData.layerY).c_str());
    LOGI(std::to_string(jsEventData.screenX).c_str());
    LOGI(std::to_string(jsEventData.screenY).c_str());
    */

    // Parse touches
    auto changedTouches = std::vector<JSTouchEventData>();
    auto touches = root.get("changedTouches", 0);

    if (touches.isArray())
    {
        for (auto touch : touches)
        {
            JSTouchEventData touchEventData;
            LOGI("ClientX");
            touchEventData.clientX = touch.get("clientX", 0).asInt();
            LOGI("ClientY");
            touchEventData.clientY = touch.get("clientY", 0).asInt();
            LOGI("Identifier");
            touchEventData.identifier = touch.get("identifier", 0).asInt();
            LOGI("push_back_begin");
            changedTouches.push_back(touchEventData);
            LOGI("push_back_end");
        }
    }

    LOGI("Affect changedTouches");
    jsEventData.changedTouches = changedTouches;
    LOGI("Make pair");
    auto pair = std::make_pair(std::string(nativeAccessor), jsEventData);
    LOGI("Insert");
    AndroidWebViewDOMEngine::events.insert(pair);
    LOGI("After insertion");
}

void Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnJSResult(JNIEnv* env, jobject obj, jstring jsResult)
{
    /*
    const char *nativeString = env->GetStringUTFChars(jsResult, 0);

    env->ReleaseStringUTFChars(jsResult, 0);

    jsGlobalResult = std::string(nativeString);
    
    LOGI("RECEIVED A JS RESULT: ");
    LOGI(nativeString);
    */
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
    _webViewInitialized(false),
    _webViewPageLoaded(false),
    _updateNextFrame(false),
    _pollRate(-1),
    _lastUpdateTime(0.0),
    // JNI
    _initWebViewTask(nullptr),
    _evalJSMethod(nullptr),
    _changeResolutionMethod(nullptr),
    _loadUrlMethod(nullptr)
{
    onWebViewInitializedSlot = onWebViewInitialized->connect([&]()
    {
        _webViewInitialized = true;
        updateWebViewResolution(_canvas->width(), _canvas->height());
        LOGI("WEBVIEW INITIALIZED (FROM C++)");
    });

    onWebViewPageLoadedSlot = onWebViewPageLoaded->connect([&]()
    {
        _webViewPageLoaded = true;
        LOGI("WEBVIEW HAS FINISHED TO LOAD THE PAGE (FROM C++)");
    });
}

void
AndroidWebViewDOMEngine::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;

    LOGI("Canvas size:");
    LOGI(std::to_string(_canvas->height()).c_str());
    LOGI(std::to_string(_canvas->width()).c_str());

    // JNI
    LOGI("Get the SDL JNIEnv");
    // Retrieve the JNI environment from SDL 
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    LOGI("Get the SDLActivity instance");
    // Retrieve the Java instance of the SDLActivity
    jobject sdlActivity = (jobject)SDL_AndroidGetActivity();
    LOGI("Get sdlActivity class");
    jclass sdlActivityClass = env->GetObjectClass(sdlActivity);

    LOGI("Get initWebViewTask class");
    jclass initWebViewTaskClass = env->FindClass("minko/plugin/htmloverlay/InitWebViewTask");
    LOGI("Get initWebViewTask constructor method");
    jmethodID initWebViewTaskCtor = env->GetMethodID(initWebViewTaskClass, "<init>", "(Landroid/app/Activity;)V");
    LOGI("Instanciate a initWebViewTask");
    _initWebViewTask = env->NewGlobalRef(env->NewObject(initWebViewTaskClass, initWebViewTaskCtor, sdlActivity));

    LOGI("Get runOnUiThread method from sdlActivity");
    jmethodID runOnUiThreadMethod = env->GetMethodID(sdlActivityClass, "runOnUiThread", "(Ljava/lang/Runnable;)V");
    LOGI("Call runOnUiThread with initWebViewTask");

    // Init the WebView
    env->CallVoidMethod(sdlActivity, runOnUiThreadMethod, _initWebViewTask);

    // Get eval method
    _evalJSMethod = env->GetMethodID(initWebViewTaskClass, "evalJS", "(Ljava/lang/String;)Ljava/lang/String;");
    // Get loadUrl method
    _loadUrlMethod = env->GetMethodID(initWebViewTaskClass, "loadUrl", "(Ljava/lang/String;)V");
    // Get changeResolution method
    _changeResolutionMethod = env->GetMethodID(initWebViewTaskClass, "changeResolution", "(II)V");

    visible(_visible);

    _canvasResizedSlot = _canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        updateWebViewResolution(w, h);
    });

	_enterFrameSlot = _sceneManager->frameBegin()->connect([&](std::shared_ptr<component::SceneManager>, float t, float dt)
	{
		enterFrame(t);
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
AndroidWebViewDOMEngine::enterFrame(float time)
{
    if (_waitingForLoad)
    {
        if (_webViewInitialized)
        {
            _waitingForLoad = false;
            load(_uriToLoad);
        }

        return;
    }

    if (!_isReady && _webViewPageLoaded)
    {
    	if (_currentDOM->initialized())
    		createNewDom();

    	_currentDOM->initialized(true);

    	_currentDOM->onload()->execute(_currentDOM, _currentDOM->fullUrl());
    	_onload->execute(_currentDOM, _currentDOM->fullUrl());
        
        registerDomEvents();

        _isReady = true;
    }

    if (_pollRate == -1 || _updateNextFrame || (_pollRate != 0 && (time - _lastUpdateTime) > (1000.0 / (float)(_pollRate))))
    {
        for(auto element : AndroidWebViewDOMElement::domElements)
        {
            element->update();
        }

    	if (_currentDOM->initialized() && _isReady)
    	{
            auto l = AndroidWebViewDOMEngine::messages.size();

            if (l > 0)
            {
                LOGI("Message found!");
                for(int i = 0; i < l; ++i)
                {
                    auto message = AndroidWebViewDOMEngine::messages[i];
                    LOGI(std::string("Message: " + message).c_str());
                    
                    _currentDOM->onmessage()->execute(_currentDOM, message);
                    _onmessage->execute(_currentDOM, message);
                }

                // Don't forget to remove the message from the list
                AndroidWebViewDOMEngine::messages.clear();
            }
    	}

        _updateNextFrame = false;
        _lastUpdateTime = time;
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
        env->CallVoidMethod(_initWebViewTask, _loadUrlMethod, env->NewStringUTF(uri.c_str()));
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
    onmousedownSlot = _currentDOM->document()->onmousedown()->connect([&](JSEventData event)
    {
        int x = event.clientX;
        int y = event.clientY;
        
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);
        
        LOGI(std::string("Mouse down (" + std::to_string(x) + ", " + std::to_string(y) + ")").c_str());

        _canvas->mouse()->leftButtonDown()->execute(_canvas->mouse());
    });

    onmouseupSlot = _currentDOM->document()->onmouseup()->connect([&](JSEventData event)
    {
        int x = event.clientX;
        int y = event.clientY;
        
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);
        
        _canvas->mouse()->leftButtonUp()->execute(_canvas->mouse());
    });

    onmousemoveSlot = _currentDOM->document()->onmousemove()->connect([&](JSEventData event)
    {
        int x = event.clientX;
        int y = event.clientY;
        
        _canvas->mouse()->x(x);
        _canvas->mouse()->y(y);
        
        auto oldX = _canvas->mouse()->x();
        auto oldY = _canvas->mouse()->y();
        
        LOGI(std::string("Mouse move (" + std::to_string(x) + ", " + std::to_string(y) + "|" + std::to_string(oldX) + ", " + std::to_string(oldY) + ")").c_str());


        _canvas->mouse()->move()->execute(_canvas->mouse(), x - oldX, y - oldY);
    });
    
    ontouchdownSlot = std::static_pointer_cast<AndroidWebViewDOMElement>(_currentDOM->document())->ontouchdown()->connect([&](JSEventData event, int touchId)
    {
        int fingerId = event.changedTouches[touchId].identifier;
        float x = event.changedTouches[touchId].clientX;
        float y = event.changedTouches[touchId].clientY;

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
    
    ontouchupSlot = std::static_pointer_cast<AndroidWebViewDOMElement>(_currentDOM->document())->ontouchup()->connect([&](JSEventData event, int touchId)
    {
        int fingerId = event.changedTouches[touchId].identifier;
        float x = event.changedTouches[touchId].clientX;
        float y = event.changedTouches[touchId].clientY;
        
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
    
    ontouchmotionSlot = std::static_pointer_cast<AndroidWebViewDOMElement>(_currentDOM->document())->ontouchmotion()->connect([&](JSEventData event, int touchId)
    {
        int fingerId = event.changedTouches[touchId].identifier;
        float x = event.changedTouches[touchId].clientX;
        float y = event.changedTouches[touchId].clientY;
        
        if (fingerId >= _touches.size())
        {
            LOGI(std::string("FingerId: " + std::to_string(fingerId)).c_str());
            return;
        }

        float oldX = _touches.at(fingerId)->minko::input::Touch::x();
        float oldY = _touches.at(fingerId)->minko::input::Touch::y();
        
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
AndroidWebViewDOMEngine::updateWebViewResolution(int width, int height)
{
    // Retrieve the JNI environment from SDL 
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    env->CallVoidMethod(_initWebViewTask, _changeResolutionMethod, width, height);
}

std::string
AndroidWebViewDOMEngine::eval(std::string data)
{
    // Retrieve the JNI environment from SDL 
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    // Get the evalJS instance
    jstring js = env->NewStringUTF(data.c_str());

    /*
    LOGI("EVAL JS: ");
    LOGI(data.c_str());
    */

    jstring evalJSResult = (jstring)env->CallObjectMethod(_initWebViewTask, _evalJSMethod, js);

    const char* evalJSResultString = env->GetStringUTFChars(evalJSResult, JNI_FALSE);

    auto result = std::string(evalJSResultString);

    // clean up the local references.
    env->ReleaseStringUTFChars(evalJSResult, evalJSResultString);
    env->DeleteLocalRef(js);
    env->DeleteLocalRef(evalJSResult);

    /*
    LOGI("EVAL FINAL VALUE: ");
    LOGI(result.c_str());
    */

    return result;
}