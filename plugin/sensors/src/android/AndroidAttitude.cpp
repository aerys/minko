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
#include "android/AndroidAttitude.hpp"

#include "SDL.h"

using namespace minko;
using namespace android::sensors;

#ifdef __cplusplus
extern "C" {
#endif

void Java_minko_plugin_sensors_AndroidHeadTracker_minkoNativeOnHeadTrackerEvent(JNIEnv* env, jobject obj, jstring message)
{
    const char *nativeMessage = env->GetStringUTFChars(message, 0);

    // Don't forget to release jstring!
    env->ReleaseStringUTFChars(message, 0);

    std::string eventType;
    float timestamp;
    math::vec3 orientation;
    std::stringstream ssRotation(nativeMessage);

    ssRotation >> eventType;
    ssRotation >> orientation.x;
    ssRotation >> orientation.y;
    ssRotation >> orientation.z;
    ssRotation >> timestamp;

    LOG_INFO("on Head Tracking event: " << nativeMessage);
}

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

AndroidAttitude::AndroidAttitude()
{
}

void AndroidAttitude::initialize()
{
    // JNI

    // Retrieve the JNI environment from SDL 
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    // Retrieve the Java instance of the SDLActivity
    jobject sdlActivity = (jobject)SDL_AndroidGetActivity();
    // Get SDLActivity java class
    jclass sdlActivityClass = env->GetObjectClass(sdlActivity);

    // Get AndroidAttitude class
    jclass androidAttitudeClass = env->FindClass("minko/plugin/sensors/AndroidAttitude");
    // Get AndroidAttitude constructor method
    jmethodID androidAttitudeCtor = env->GetMethodID(androidAttitudeClass, "<init>", "(Landroid/app/Activity;)V");
    // Instanciate an AndroidAttitude
    _attitude = env->NewGlobalRef(env->NewObject(androidAttitudeClass, androidAttitudeCtor, sdlActivity));

    // Get JNI methods 

    _startTrackingMethod = env->GetMethodID(androidAttitudeClass, "startTracking", "()V");
    _stopTrackingMethod = env->GetMethodID(androidAttitudeClass, "stopTracking", "()V");
}

void
AndroidAttitude::startTracking()
{
    LOG_INFO("Start tracking");

    // Retrieve the JNI environment from SDL 
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    env->CallVoidMethod(_attitude, _startTrackingMethod);
}

void
AndroidAttitude::stopTracking()
{
    LOG_INFO("Stop tracking");

    // Retrieve the JNI environment from SDL 
    auto env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    env->CallVoidMethod(_attitude, _stopTrackingMethod);
}

const math::mat4&
AndroidAttitude::rotationMatrix()
{
    // TODO
    return math::mat4();
}

const math::quat&
AndroidAttitude::quaternion()
{
    // TODO
    return math::quat();
}