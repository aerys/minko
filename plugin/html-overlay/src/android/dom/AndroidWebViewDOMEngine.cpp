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
#include "minko/MinkoSDL.hpp"

#include "minko/dom/AbstractDOMTouchEvent.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/JSON.hpp"
#include "minko/file/FileProtocol.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/input/Touch.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/SharedTexture.hpp"

#include "SDL.h"

#include "android/dom/AndroidWebViewDOMEngine.hpp"
#include "android/dom/AndroidWebViewDOMMouseEvent.hpp"
#include "android/dom/AndroidWebViewDOMTouchEvent.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::dom;
using namespace minko::file;
using namespace minko::render;
using namespace android;
using namespace android::dom;

int AndroidWebViewDOMEngine::_domUid = 0;

std::vector<std::string> AndroidWebViewDOMEngine::messages;
std::vector<minko::dom::AbstractDOMEvent::Ptr> AndroidWebViewDOMEngine::events;

Signal<>::Ptr AndroidWebViewDOMEngine::onWebViewInitialized = Signal<>::create();
Signal<>::Ptr AndroidWebViewDOMEngine::onWebViewPageLoaded = Signal<>::create();

std::mutex AndroidWebViewDOMEngine::eventMutex;
std::mutex AndroidWebViewDOMEngine::messageMutex;

AndroidWebViewDOMEngine::Ptr AndroidWebViewDOMEngine::currentEngine;

int AndroidWebViewDOMEngine::numTouches = 0;
int AndroidWebViewDOMEngine::firstIdentifier = -1;

bool AndroidWebViewDOMEngine::shouldUpdateWebViewTexture = false;

#ifdef __cplusplus
extern "C" {
#endif

// JNI native functions

void Java_minko_plugin_htmloverlay_InitWebViewTask_webViewInitialized(JNIEnv* env, jobject obj)
{
    AndroidWebViewDOMEngine::onWebViewInitialized->execute();
}

void Java_minko_plugin_htmloverlay_MinkoWebViewClient_webViewPageLoaded(JNIEnv* env, jobject obj)
{
    AndroidWebViewDOMEngine::onWebViewPageLoaded->execute();
}

void Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnMessage(JNIEnv* env, jobject obj, jstring message)
{
    jboolean isCopy;
    const char* rawNativeMessage = env->GetStringUTFChars(message, &isCopy);
    const auto nativeMessage = std::string(rawNativeMessage);

    if (isCopy)
        env->ReleaseStringUTFChars(message, rawNativeMessage);

    AndroidWebViewDOMEngine::messageMutex.lock();
    AndroidWebViewDOMEngine::messages.push_back(nativeMessage);
    AndroidWebViewDOMEngine::messageMutex.unlock();
}

void Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnEvent(JNIEnv* env, jobject obj, jstring accessor, jstring eventData)
{
    AndroidWebViewDOMEngine::eventMutex.lock();

    jboolean nativeAccessorIsCopy;
    const char* rawNativeAccessor = env->GetStringUTFChars(accessor, &nativeAccessorIsCopy);
    const auto nativeAccessor = std::string(rawNativeAccessor);

    jboolean nativeEventIsCopy;
    const char* rawNativeEvent = env->GetStringUTFChars(eventData, &nativeEventIsCopy);
    const auto nativeEvent = std::string(rawNativeEvent);

    if (nativeAccessorIsCopy)
        env->ReleaseStringUTFChars(accessor, rawNativeAccessor);

    if (nativeEventIsCopy)
        env->ReleaseStringUTFChars(eventData, rawNativeEvent);

    JSON::Json json;

    try
    {
        json = JSON::json::parse(nativeEvent.data());
    }
    catch (JSON::Json::parse_error& e)
    {
        LOG_ERROR(e.what());
        return;
    }

    auto type = JSON::as_string(JSON::get(json, "type"));
    auto target = AndroidWebViewDOMElement::getDOMElement(nativeAccessor, AndroidWebViewDOMEngine::currentEngine);

    if (type == "input" || type == "change")
    {
        auto event = AndroidWebViewDOMEvent::create(type, target);
        AndroidWebViewDOMEngine::events.push_back(event);
    }
    else if (type.substr(0, 5) == "mouse" || type == "click")
    {
        auto mouseEvent = AndroidWebViewDOMMouseEvent::create(type, target);

        mouseEvent->clientX(JSON::as_int(JSON::get(json, "clientX")));
        mouseEvent->clientY(JSON::as_int(JSON::get(json, "clientY")));
        mouseEvent->pageX(JSON::as_int(JSON::get(json, "pageX")));
        mouseEvent->pageY(JSON::as_int(JSON::get(json, "pageY")));
        mouseEvent->screenX(JSON::as_int(JSON::get(json, "screenX")));
        mouseEvent->screenY(JSON::as_int(JSON::get(json, "screenY")));

        AndroidWebViewDOMEngine::events.push_back(mouseEvent);
    }
    else if (type.substr(0, 5) == "touch")
    {
        // Parse touches
        auto touches = JSON::get(json,"changedTouches");

        if (touches.is_array())
        {
            for (auto touch : touches)
            {
                // Touch event
                auto touchEvent = AndroidWebViewDOMTouchEvent::create(type, target);
    
                touchEvent->clientX(JSON::as_int(JSON::get(touch, "clientX")));
                touchEvent->clientY(JSON::as_int(JSON::get(touch, "clientY")));
                touchEvent->identifier(JSON::as_int(JSON::get(touch, "identifier")));

                AndroidWebViewDOMEngine::events.push_back(touchEvent);

                // Mouse event
                auto mouseEvent = AndroidWebViewDOMMouseEvent::create("mousedown", target);

                mouseEvent->clientX(touchEvent->clientX());
                mouseEvent->clientY(touchEvent->clientY());
                mouseEvent->pageX(JSON::as_int(JSON::get(json, "pageX")));
                mouseEvent->pageY(JSON::as_int(JSON::get(json, "pageY")));
                mouseEvent->screenX(JSON::as_int(JSON::get(json, "screenX")));
                mouseEvent->screenY(JSON::as_int(JSON::get(json, "screenY")));

                if (type == "touchstart")
                {
                    AndroidWebViewDOMEngine::numTouches++;
                    if (AndroidWebViewDOMEngine::numTouches == 1)
                    {
                        AndroidWebViewDOMEngine::firstIdentifier = touchEvent->identifier();

                        AndroidWebViewDOMEngine::events.push_back(mouseEvent);
                    }
                }
                else if (type == "touchend")
                {
                    AndroidWebViewDOMEngine::numTouches--;

                    if (AndroidWebViewDOMEngine::numTouches == 0)
                    {
                        AndroidWebViewDOMEngine::firstIdentifier = -1;

                        mouseEvent->type("mouseup");

                        auto mouseClickEvent = AndroidWebViewDOMMouseEvent::create(mouseEvent);
                        mouseClickEvent->type("click");

                        AndroidWebViewDOMEngine::events.push_back(mouseEvent);
                        AndroidWebViewDOMEngine::events.push_back(mouseClickEvent);
                    }
                }
                else if (type == "touchmove")
                {
                    if (AndroidWebViewDOMEngine::firstIdentifier == touchEvent->identifier())
                    {
                        mouseEvent->type("mousemove");

                        AndroidWebViewDOMEngine::events.push_back(mouseEvent);
                    }
                }
            }
        }
    }

    AndroidWebViewDOMEngine::eventMutex.unlock();
}

void Java_minko_plugin_htmloverlay_MinkoWebView_minkoNativeOnWebViewUpdate(JNIEnv* env)
{
    AndroidWebViewDOMEngine::shouldUpdateWebViewTexture = true;
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
    _onWebViewInitializedSlot = onWebViewInitialized->connect([&]()
    {
        _webViewInitialized = true;
        updateWebViewResolution(_canvas->width(), _canvas->height());
    });

    _onWebViewPageLoadedSlot = onWebViewPageLoaded->connect([&]()
    {
        _webViewPageLoaded = true;
    });
}

void
AndroidWebViewDOMEngine::initialize(AbstractCanvas::Ptr canvas, SceneManager::Ptr sceneManager)
{
	_canvas = canvas;
	_sceneManager = sceneManager;

    // JNI

    // Retrieve the JNI environment from SDL
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    // Retrieve the Java instance of the SDLActivity
    jobject sdlActivity = (jobject)SDL_AndroidGetActivity();
    // Get SDLActivity java class
    jclass sdlActivityClass = env->GetObjectClass(sdlActivity);

    // Get initWebViewTask class
    jclass initWebViewTaskClass = env->FindClass("minko/plugin/htmloverlay/InitWebViewTask");
    // Get initWebViewTask constructor method
    jmethodID initWebViewTaskCtor = env->GetMethodID(initWebViewTaskClass, "<init>", "(Landroid/app/Activity;)V");
    // Instanciate a initWebViewTask
    _initWebViewTask = env->NewGlobalRef(env->NewObject(initWebViewTaskClass, initWebViewTaskCtor, sdlActivity));

    // Get runOnUiThread method from sdlActivity
    jmethodID runOnUiThreadMethod = env->GetMethodID(sdlActivityClass, "runOnUiThread", "(Ljava/lang/Runnable;)V");
    // Call runOnUiThread with initWebViewTask

    // Init the WebView
    env->CallVoidMethod(sdlActivity, runOnUiThreadMethod, _initWebViewTask);

    // Get JNI methods

    // Get eval method
    _evalJSMethod = env->GetMethodID(initWebViewTaskClass, "evalJS", "(Ljava/lang/String;)Ljava/lang/String;");
    // Get loadUrl method
    _loadUrlMethod = env->GetMethodID(initWebViewTaskClass, "loadUrl", "(Ljava/lang/String;)V");
    // Get changeResolution method
    _changeResolutionMethod = env->GetMethodID(initWebViewTaskClass, "changeResolution", "(II)V");
    // Get hide method
    _hideMethod = env->GetMethodID(initWebViewTaskClass, "hide", "(Z)V");

    // To set the rendering surface 
    _setWebViewRendererSurfaceMethod = env->GetMethodID(initWebViewTaskClass, "setWebViewRendererSurface", "(Landroid/view/Surface;)V");

    // SurfaceTexture

    _surfaceTextureClass = env->FindClass("android/graphics/SurfaceTexture");
    // Find the constructor that takes an int (texture name)
    _surfaceTextureConstructor = env->GetMethodID(_surfaceTextureClass, "<init>", "(I)V");
    // Retrieve SurfaceTexture methods we will use
    _updateTexImageMethodId = env->GetMethodID(_surfaceTextureClass, "updateTexImage", "()V");
    _setDefaultBufferSizeMethodId = env->GetMethodID(_surfaceTextureClass, "setDefaultBufferSize", "(II)V");

    // Surface

    // Create a Surface from the SurfaceTexture using JNI
    _surfaceClass = env->FindClass("android/view/Surface");
    _surfaceConstructor = env->GetMethodID(_surfaceClass, "<init>", "(Landroid/graphics/SurfaceTexture;)V");

    visible(_visible);

    _canvasResizedSlot = _canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        updateWebViewResolution(w, h);
    });

	_enterFrameSlot = _sceneManager->frameBegin()->connect([&](std::shared_ptr<component::SceneManager>, float t, float dt)
	{
		enterFrame(t);
	}, -100.0f);
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

    updateEvents();

    if (_currentDOM->initialized() && _isReady)
    {
        std::vector<std::string> messagesToProcess;

        messageMutex.lock();
        messagesToProcess.swap(AndroidWebViewDOMEngine::messages);
        messageMutex.unlock();

        auto l = messagesToProcess.size();

        if (l > 0)
        {
            for(int i = 0; i < l; ++i)
            {
                auto message = messagesToProcess[i];

                _currentDOM->onmessage()->execute(_currentDOM, message);
                _onmessage->execute(_currentDOM, message);
            }
        }
    }


    if (_pollRate == -1 || _updateNextFrame || (_pollRate != 0 && (time - _lastUpdateTime) > (1000.0 / (float)(_pollRate))))
    {
        _updateNextFrame = false;
        _lastUpdateTime = time;
    }

    // If we are rendering the WebView into a texture, 
    // don't forget to update the surface texture
    if (_jniSurfaceTexture != nullptr && AndroidWebViewDOMEngine::shouldUpdateWebViewTexture)
    {
        AndroidWebViewDOMEngine::shouldUpdateWebViewTexture = false;

         auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();
         env->CallVoidMethod(_jniSurfaceTexture, _updateTexImageMethodId);
    }
}

void
AndroidWebViewDOMEngine::updateEvents()
{
}

AndroidWebViewDOMEngine::Ptr
AndroidWebViewDOMEngine::create()
{
	AndroidWebViewDOMEngine::Ptr engine(new AndroidWebViewDOMEngine);

    currentEngine = engine;

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
            uri = "file:///android_asset/" + uri;

        // Retrieve the JNI environment from SDL
        auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        // Call URL loading method
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
    if (_canvas != nullptr && _webViewInitialized)
	{
        if (value != _visible)
        {
            // Retrieve the JNI environment from SDL
            auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();
            env->CallVoidMethod(_initWebViewTask, _hideMethod, value);
        }
	}

	_visible = value;
}

void
AndroidWebViewDOMEngine::registerDomEvents()
{
    _onmousedownSlot = _currentDOM->document()->onmousedown()->connect([&](AbstractDOMMouseEvent::Ptr event)
    {
    });

    _onmouseupSlot = _currentDOM->document()->onmouseup()->connect([&](AbstractDOMMouseEvent::Ptr event)
    {
    });

    _onmousemoveSlot = _currentDOM->document()->onmousemove()->connect([&](AbstractDOMMouseEvent::Ptr event)
    {
    });

    _ontouchstartSlot = std::static_pointer_cast<AndroidWebViewDOMElement>(_currentDOM->document())->ontouchstart()->connect([&](AbstractDOMTouchEvent::Ptr event)
    {
    });

    _ontouchendSlot = std::static_pointer_cast<AndroidWebViewDOMElement>(_currentDOM->document())->ontouchend()->connect([&](AbstractDOMTouchEvent::Ptr event)
    {
    });

    _ontouchmoveSlot = std::static_pointer_cast<AndroidWebViewDOMElement>(_currentDOM->document())->ontouchmove()->connect([&](AbstractDOMTouchEvent::Ptr event)
    {
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
AndroidWebViewDOMEngine::eval(const std::string& data)
{
    // Retrieve the JNI environment from SDL
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    // Convert string to jstring
    jstring js = env->NewStringUTF(data.c_str());

    // Call the WebView's function to evaluate javascript
    jstring evalJSResult = (jstring)env->CallObjectMethod(_initWebViewTask, _evalJSMethod, js);

    // Convert back jstring result into char*
    const char* evalJSResultString = env->GetStringUTFChars(evalJSResult, JNI_FALSE);

    auto result = std::string(evalJSResultString);

    // Clean up the local references
    env->ReleaseStringUTFChars(evalJSResult, evalJSResultString);
    env->DeleteLocalRef(js);
    env->DeleteLocalRef(evalJSResult);

    return result;
}


// Render to texture

void
AndroidWebViewDOMEngine::enableRenderToTexture(std::shared_ptr<AbstractTexture> texture)
{
    if (!_canvas || !_canvas->context())
    {
        LOG_ERROR("No OpenGL context yet");
        return;
    }

    // Send the new texture Id to Java
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    // Create a SurfaceTexture using JNI
    jobject surfaceTextureObject = env->NewObject(_surfaceTextureClass, _surfaceTextureConstructor, texture->id());
    _jniSurfaceTexture = env->NewGlobalRef(surfaceTextureObject);

    // Create a Surface using JNI
    jobject surfaceObject = env->NewObject(_surfaceClass, _surfaceConstructor, _jniSurfaceTexture);
    _jniSurface = env->NewGlobalRef(surfaceObject);

    // Now that we have a globalRef, we can free the localRef
    env->DeleteLocalRef(surfaceTextureObject);
    env->DeleteLocalRef(surfaceObject);

    // Set the Surface on the Java WebView
    env->CallVoidMethod(_initWebViewTask, _setWebViewRendererSurfaceMethod, _jniSurface);
    // Make sure to resize the SurfaceTexture to match the shared texture
    env->CallVoidMethod(_jniSurfaceTexture, _setDefaultBufferSizeMethodId, texture->width(), texture->height());
}

void
AndroidWebViewDOMEngine::disableRenderToTexture()
{
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    env->DeleteGlobalRef(_jniSurfaceTexture);
    env->DeleteGlobalRef(_jniSurface);

    _jniSurfaceTexture = nullptr;
    _jniSurface = nullptr;

    env->CallVoidMethod(_initWebViewTask, _setWebViewRendererSurfaceMethod, _jniSurface);
}