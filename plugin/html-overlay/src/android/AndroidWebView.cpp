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

#include "android/AndroidWebView.hpp"
#include <android/log.h>
#include "SDL.h"

#define LOG_TAG "MINKO"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

void WebViewInit(JNIEnv* oldEnv, jobject obj)
{
    LOGI("Get a new JNIEnv");
    // Retrieve the JNI environment from SDL 
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    LOGI("Init WebView");

    jclass cls = env->GetObjectClass(obj);

    LOGI("Get layout");
    jmethodID getLayoutMethod = env->GetMethodID(cls, "getLayout", "()Landroid/view/ViewGroup;");
    jobject layout = env->CallObjectMethod(obj, getLayoutMethod);

 	LOGI("Get getContext method ID");
    //jmethodID getContext = env->GetMethodID(cls, "getContext", "()Landroid/content/Context;");
    jmethodID getContext = env->GetStaticMethodID(cls, "getContext","()Landroid/content/Context;");
	LOGI("Get context");
    jobject context = env->CallObjectMethod(obj, getContext);

    LOGI("Get WebView class");
    jclass webViewClass = env->FindClass("android/webkit/WebView");
    LOGI("Get WebView constructor method");
    jmethodID webViewCtr = env->GetMethodID(webViewClass, "<init>", "(Landroid/content/Context;)V");

    LOGI("Instanciate WebView");
    jobject webView = env->NewObject(webViewClass, webViewCtr, context);

    LOGI("Get class from layout obj");
    jclass layoutClass = env->GetObjectClass(layout);
    LOGI("Get addView method ID");
    jmethodID addViewMethod = env->GetMethodID(layoutClass, "addView", "(Landroid/view/View;)V");
    LOGI("Add WebView on layout");
    env->CallVoidMethod(layout, addViewMethod, webView);

	LOGI("Get loadUrl method");
    jmethodID loadUrlMethod = env->GetMethodID(webViewClass, "loadUrl", "(Ljava/lang/String;)V");
    LOGI("Load an URL");
    env->CallVoidMethod(webView, loadUrlMethod, env->NewStringUTF("file:///android_asset/index.html"));
}