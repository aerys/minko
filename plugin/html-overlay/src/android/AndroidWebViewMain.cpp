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

#ifdef __ANDROID__

/*******************************************************************************
                 Functions called by JNI
*******************************************************************************/
#include <jni.h>


/* Called before SDL_main() to initialize JNI bindings in SDL library */
extern void WebViewInit(JNIEnv* env, jobject obj);

#ifdef __cplusplus
extern "C" {
#endif

/* Start up the SDL app */
void Java_minko_example_cube_MinkoActivity_minkoNativeInit(JNIEnv* env, jobject obj)
{
    /* This interface could expand with ABI negotiation, calbacks, etc. */
    WebViewInit(env, obj);
}

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* __ANDROID__ */
