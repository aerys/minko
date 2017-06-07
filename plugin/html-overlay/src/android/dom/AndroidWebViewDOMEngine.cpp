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

    JSON::Value root;
    JSON::Reader reader;

    if (!reader.parse(nativeEvent.data(), root, false))
    {
        LOG_ERROR(reader.getFormattedErrorMessages().c_str());

        return;
    }

    auto type = root.get("type", "unknown").asString();
    auto target = AndroidWebViewDOMElement::getDOMElement(nativeAccessor, AndroidWebViewDOMEngine::currentEngine);

    if (type == "input" || type == "change")
    {
        auto event = AndroidWebViewDOMEvent::create(type, target);
        AndroidWebViewDOMEngine::events.push_back(event);
    }
    else if (type.substr(0, 5) == "mouse" || type == "click")
    {
        auto mouseEvent = AndroidWebViewDOMMouseEvent::create(type, target);

        mouseEvent->clientX(root.get("clientX", 0).asInt());
        mouseEvent->clientY(root.get("clientY", 0).asInt());
        mouseEvent->pageX(root.get("pageX", 0).asInt());
        mouseEvent->pageY(root.get("pageY", 0).asInt());
        mouseEvent->screenX(root.get("screenX", 0).asInt());
        mouseEvent->screenY(root.get("screenY", 0).asInt());

        AndroidWebViewDOMEngine::events.push_back(mouseEvent);
    }
    else if (type.substr(0, 5) == "touch")
    {
        // Parse touches
        auto touches = root.get("changedTouches", 0);

        if (touches.isArray())
        {
            for (auto touch : touches)
            {
                // Touch event
                auto touchEvent = AndroidWebViewDOMTouchEvent::create(type, target);

                touchEvent->clientX(touch.get("clientX", 0).asInt());
                touchEvent->clientY(touch.get("clientY", 0).asInt());
                touchEvent->identifier(touch.get("identifier", 0).asInt());

                AndroidWebViewDOMEngine::events.push_back(touchEvent);

                // Mouse event
                auto mouseEvent = AndroidWebViewDOMMouseEvent::create("mousedown", target);

                mouseEvent->clientX(touchEvent->clientX());
                mouseEvent->clientY(touchEvent->clientY());
                mouseEvent->pageX(root.get("pageX", 0).asInt());
                mouseEvent->pageY(root.get("pageY", 0).asInt());
                mouseEvent->screenX(root.get("screenX", 0).asInt());
                mouseEvent->screenY(root.get("screenY", 0).asInt());

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
        messageMutex.lock();

        auto l = AndroidWebViewDOMEngine::messages.size();

        if (l > 0)
        {
            for(int i = 0; i < l; ++i)
            {
                auto message = AndroidWebViewDOMEngine::messages[i];

                _currentDOM->onmessage()->execute(_currentDOM, message);
                _onmessage->execute(_currentDOM, message);
            }

            // Don't forget to remove the message from the list
            AndroidWebViewDOMEngine::messages.clear();
            AndroidWebViewDOMEngine::messages.shrink_to_fit();
        }

        messageMutex.unlock();
    }


    if (_pollRate == -1 || _updateNextFrame || (_pollRate != 0 && (time - _lastUpdateTime) > (1000.0 / (float)(_pollRate))))
    {
        _updateNextFrame = false;
        _lastUpdateTime = time;
    }

    // If we are rendering the WebView into a texture, 
    // don't forget to update the surface texture
    if (_jniSurfaceTexture != nullptr)
    {
         auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();
         env->CallVoidMethod(_jniSurfaceTexture, _updateTexImageMethodId);
    }
}

void
AndroidWebViewDOMEngine::updateEvents()
{
    if (_isReady)
    {
        eventMutex.lock();

        for (int i = 0; i < events.size(); ++i)
        {
            auto event = events[i];
            auto type = event->type();
            auto target = event->target();

            if (type == "change")
                target->onchange()->execute(event);
            else if (type == "input")
                target->oninput()->execute(event);
            else if (type == "click")
                target->onclick()->execute(std::dynamic_pointer_cast<AbstractDOMMouseEvent>(event));
            else if (type == "mousedown")
                target->onmousedown()->execute(std::dynamic_pointer_cast<AbstractDOMMouseEvent>(event));
            else if (type == "mouseup")
                target->onmouseup()->execute(std::dynamic_pointer_cast<AbstractDOMMouseEvent>(event));
            else if (type == "mousemove")
                target->onmousemove()->execute(std::dynamic_pointer_cast<AbstractDOMMouseEvent>(event));
            else if (type == "mouseover")
                target->onmouseover()->execute(std::dynamic_pointer_cast<AbstractDOMMouseEvent>(event));
            else if (type == "mouseout")
                target->onmouseout()->execute(std::dynamic_pointer_cast<AbstractDOMMouseEvent>(event));
            else if (type == "touchstart")
                target->ontouchstart()->execute(std::dynamic_pointer_cast<AbstractDOMTouchEvent>(event));
            else if (type == "touchend")
                target->ontouchend()->execute(std::dynamic_pointer_cast<AbstractDOMTouchEvent>(event));
            else if (type == "touchmove")
                target->ontouchmove()->execute(std::dynamic_pointer_cast<AbstractDOMTouchEvent>(event));
        }

        events.clear();
        events.shrink_to_fit();

        eventMutex.unlock();
    }
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

    _ontouchstartSlot = std::static_pointer_cast<AndroidWebViewDOMElement>(_currentDOM->document())->ontouchstart()->connect([&](AbstractDOMTouchEvent::Ptr event)
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

    _ontouchendSlot = std::static_pointer_cast<AndroidWebViewDOMElement>(_currentDOM->document())->ontouchend()->connect([&](AbstractDOMTouchEvent::Ptr event)
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

    _ontouchmoveSlot = std::static_pointer_cast<AndroidWebViewDOMElement>(_currentDOM->document())->ontouchmove()->connect([&](AbstractDOMTouchEvent::Ptr event)
    {
        int identifier = event->identifier();
        auto identifiers = _canvas->touch()->identifiers();

        if(std::find(identifiers.begin(), identifiers.end(), identifier) == identifiers.end())
            return;

        float x = event->clientX();
        float y = event->clientY();

        float oldX = _canvas->touch()->touch(identifier).x;
        float oldY = _canvas->touch()->touch(identifier).y;

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
AndroidWebViewDOMEngine::enableRenderToTexture(std::shared_ptr<SharedTexture> texture)
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
    LOG_INFO("DisableRenderToTexture");
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    LOG_INFO("Delete global refs");
    env->DeleteGlobalRef(_jniSurfaceTexture);
    env->DeleteGlobalRef(_jniSurface);

    LOG_INFO("Nullptr");
    _jniSurfaceTexture = nullptr;
    _jniSurface = nullptr;

    LOG_INFO("Set WebView Surface to nullptr");
    env->CallVoidMethod(_initWebViewTask, _setWebViewRendererSurfaceMethod, _jniSurface);
}