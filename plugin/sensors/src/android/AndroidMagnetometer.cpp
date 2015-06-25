/*
Copyright (c) 2015 Aerys

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
#include "minko/Signal.hpp"
#include "android/AndroidMagnetometer.hpp"

#include "SDL.h"

#include <jni.h>

using namespace minko;
using namespace android::sensors;

Signal<float, float, float>::Ptr AndroidMagnetometer::sensorChanged = Signal<float, float, float>::create();
math::vec3 AndroidMagnetometer::sensorValue;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_minko_plugin_sensors_AndroidMagnetometer_minkoNativeOnMagnetometerEvent(JNIEnv* env, jobject obj, jfloatArray magnetometer)
{
    jfloat* magnetometerFloat = env->GetFloatArrayElements(magnetometer, 0);

    AndroidMagnetometer::sensorChanged->execute(magnetometerFloat[0], magnetometerFloat[1], magnetometerFloat[2]);
    AndroidMagnetometer::sensorValue = math::vec3(magnetometerFloat[0], magnetometerFloat[1], magnetometerFloat[2]);

    env->ReleaseFloatArrayElements(magnetometer, magnetometerFloat, 0);
}

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

AndroidMagnetometer::AndroidMagnetometer()
{
}

void AndroidMagnetometer::initialize()
{
    // Retrieve the JNI environment from SDL 
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    // Retrieve the Java instance of the SDLActivity
    jobject sdlActivity = (jobject)SDL_AndroidGetActivity();
    // Get SDLActivity java class
    jclass sdlActivityClass = env->GetObjectClass(sdlActivity);

    // Get AndroidMagnetometer class
    jclass AndroidMagnetometerClass = env->FindClass("minko/plugin/sensors/AndroidMagnetometer");
    // Get AndroidMagnetometer constructor method
    jmethodID AndroidMagnetometerCtor = env->GetMethodID(AndroidMagnetometerClass, "<init>", "(Landroid/app/Activity;)V");
    // Instanciate an AndroidMagnetometer
    _magnetometer = env->NewGlobalRef(env->NewObject(AndroidMagnetometerClass, AndroidMagnetometerCtor, sdlActivity));

    // Get JNI methods 
    _startTrackingMethod = env->GetMethodID(AndroidMagnetometerClass, "startTracking", "()V");
    _stopTrackingMethod = env->GetMethodID(AndroidMagnetometerClass, "stopTracking", "()V");
    _isSupportedMethod = env->GetMethodID(AndroidMagnetometerClass, "isSupported", "()Z");
}

void
AndroidMagnetometer::startTracking()
{
    LOG_INFO("Start tracking");

    // Retrieve the JNI environment from SDL 
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    env->CallVoidMethod(_magnetometer, _startTrackingMethod);
}

void
AndroidMagnetometer::stopTracking()
{
    LOG_INFO("Stop tracking");

    // Retrieve the JNI environment from SDL 
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    env->CallVoidMethod(_magnetometer, _stopTrackingMethod);
}

bool
AndroidMagnetometer::isSupported()
{
    // Retrieve the JNI environment from SDL 
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    
    auto isSupported = (bool)env->CallObjectMethod(_magnetometer, _isSupportedMethod);
    
    return isSupported;
}