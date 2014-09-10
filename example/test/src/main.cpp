/*
Copyright (c) 2014 Aerys

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

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoAndroid.hpp"

#include "android/AndroidWebView.hpp"
#include <android/log.h>
#include "SDL.h"
#include <jni.h>

#define LOG_TAG "MINKO"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const std::string TEXTURE_FILENAME = "texture/box.png";

static bool webViewInitialized = false;
static JNIEnv* env = nullptr;
static jobject sdlActivity = nullptr;
static jmethodID runOnUiThreadMethod = nullptr;
static jclass initWebViewTaskClass = nullptr;
static jobject initWebViewTask = nullptr;

#ifdef __cplusplus
extern "C" {
#endif

/* Start up the Minko app */
void Java_minko_plugin_htmloverlay_InitWebViewTask_webViewInitialized(JNIEnv* env, jobject obj)
{
    webViewInitialized = true;

    LOGI("WEBVIEW INITIALIZED (FROM C++)");
}

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif


int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Cube", 800, 600);

    auto sceneManager = SceneManager::create(canvas->context());

    // setup assets
    sceneManager->assets()->loader()->options()
        ->resizeSmoothly(true)
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png");

    sceneManager->assets()->loader()
        ->queue(TEXTURE_FILENAME)
        ->queue("effect/Basic.effect");

    sceneManager->assets()->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto mesh = scene::Node::create("mesh")
        ->addComponent(Transform::create());

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
            Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f))
        ))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

    root->addChild(camera);

    auto _ = sceneManager->assets()->loader()->complete()->connect([ = ](file::Loader::Ptr loader)
    {
        mesh->addComponent(Surface::create(
            sceneManager->assets()->geometry("cube"),
            material::BasicMaterial::create()->diffuseMap(
                sceneManager->assets()->texture(TEXTURE_FILENAME)
            ),
            sceneManager->assets()->effect("effect/Basic.effect")
        ));

        root->addChild(mesh);

        // JNI

        LOGI("Get the SDL JNIEnv");
        // Retrieve the JNI environment from SDL 
        env = (JNIEnv*)SDL_AndroidGetJNIEnv();
        LOGI("Get the SDLActivity instance");
        // Retrieve the Java instance of the SDLActivity
        sdlActivity = (jobject)SDL_AndroidGetActivity();
        LOGI("Get sdlActivity class");
        jclass sdlActivityClass = env->GetObjectClass(sdlActivity);

        LOGI("Get initWebViewTask class");
        initWebViewTaskClass = env->FindClass("minko/plugin/htmloverlay/InitWebViewTask");
        LOGI("Get initWebViewTask constructor method");
        jmethodID initWebViewTaskCtor = env->GetMethodID(initWebViewTaskClass, "<init>", "(Landroid/app/Activity;Ljava/lang/String;)V");
        LOGI("Instanciate a initWebViewTask");
        initWebViewTask = env->NewObject(initWebViewTaskClass, initWebViewTaskCtor, sdlActivity, env->NewStringUTF("file:///android_asset/html/interface.html"));
        
        LOGI("Get runOnUiThread method from sdlActivity");
        runOnUiThreadMethod = env->GetMethodID(sdlActivityClass, "runOnUiThread", "(Ljava/lang/Runnable;)V");
        LOGI("Call runOnUiThread with initWebViewTask");
        env->CallVoidMethod(sdlActivity, runOnUiThreadMethod, initWebViewTask);
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
    {
        if (webViewInitialized)
        {
            // Get the WebView instance
            jmethodID getWebViewMethod = env->GetMethodID(initWebViewTaskClass, "getWebView", "()Landroid/webkit/WebView;");
            jobject webView = env->CallObjectMethod(initWebViewTask, getWebViewMethod);
            
            // Call JS eval
            jclass evalJSTaskClass = env->FindClass("minko/plugin/htmloverlay/EvalJSTask");
            jmethodID evalJSTaskCtor = env->GetMethodID(evalJSTaskClass, "<init>", "(Landroid/webkit/WebView;Ljava/lang/String;)V");
            jobject evalJSTask = env->NewObject(evalJSTaskClass, evalJSTaskCtor, webView, env->NewStringUTF("alert('COUCOU from C++')"));
            LOGI("Call runOnUiThread with evalJSTask");
            env->CallVoidMethod(sdlActivity, runOnUiThreadMethod, evalJSTask);

            webViewInitialized = false;
        }

        mesh->component<Transform>()->matrix()->appendRotationY(0.001f * deltaTime);

        sceneManager->nextFrame(time, deltaTime);
    });

    sceneManager->assets()->loader()->load();
    canvas->run();

    return 0;
}
