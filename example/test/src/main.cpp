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

const uint WINDOW_WIDTH = 800;
const uint WINDOW_HEIGHT = 600;

#define LOG_TAG "MINKO"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace minko;
using namespace minko::math;
using namespace minko::component;

static bool webViewInitialized = false;
static JNIEnv* env = nullptr;
static jobject sdlActivity = nullptr;
static jmethodID runOnUiThreadMethod = nullptr;
static jclass initWebViewTaskClass = nullptr;
static jobject initWebViewTask = nullptr;

static Surface::Ptr cubeSurface = nullptr;

#ifdef __cplusplus
extern "C" {
#endif

/* Start up the Minko app */
void Java_minko_plugin_htmloverlay_InitWebViewTask_webViewInitialized(JNIEnv* env, jobject obj)
{
    webViewInitialized = true;

    LOGI("WEBVIEW INITIALIZED (FROM C++)");
}

void Java_minko_plugin_htmloverlay_WebViewJSInterface_minkoNativeOnMessage(JNIEnv* env, jobject obj, jstring message)
{
    LOGI("RECEIVED A MESSAGE FROM JS: ");
    
    const char *nativeString = env->GetStringUTFChars(message, 0);

    auto r = ((double)rand() / (RAND_MAX));
    auto g = ((double)rand() / (RAND_MAX));
    auto b = ((double)rand() / (RAND_MAX));

    cubeSurface->material()->set("diffuseColor", Vector4::create(r, g, b, 1.f));

    std::string rs = std::to_string(r);
    char const *rchar = rs.c_str();

    std::string gs = std::to_string(g);
    char const *gchar = gs.c_str();

    std::string bs = std::to_string(b);
    char const *bchar = bs.c_str();

    LOGI(rchar);
    LOGI(gchar);
    LOGI(bchar);

    LOGI(nativeString);
}

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

int
main(int argc, char** argv)
{
    srand((int)time(NULL));

    auto canvas = Canvas::create("Minko Test!", WINDOW_WIDTH, WINDOW_HEIGHT);
    auto sceneManager = component::SceneManager::create(canvas->context());

    sceneManager->assets()->loader()->queue("effect/Basic.effect");

    auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
        cubeSurface = Surface::create(
            geometry::CubeGeometry::create(canvas->context()),
            material::BasicMaterial::create()->diffuseColor(Vector4::create(0.f, 0.f, 1.f, 1.f)),
            sceneManager->assets()->effect("effect/Basic.effect")
        );

        auto root = scene::Node::create("root")
            ->addComponent(sceneManager);

        auto camera = scene::Node::create("camera")
            ->addComponent(Renderer::create(0x7f7f7fff))
            ->addComponent(PerspectiveCamera::create(
            (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, float(M_PI) * 0.25f, .1f, 1000.f)
            );

        root->addChild(camera);

        auto cube = scene::Node::create("cube")
            ->addComponent(Transform::create(Matrix4x4::create()->translation(0.f, 0.f, -5.f)))
            ->addComponent(cubeSurface);

        root->addChild(cube);

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

            cube->component<Transform>()->matrix()->prependRotationY(.01f);
            sceneManager->nextFrame(time, deltaTime);
        });

        canvas->run();

    });

    sceneManager->assets()->loader()->load();
    

    return 0;
}
