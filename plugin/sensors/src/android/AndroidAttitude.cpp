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

#include "SDL.h"
#include "jni.h"

#include "minko/Common.hpp"
#include "minko/MinkoSDL.hpp"
#include "android/AndroidAttitude.hpp"

using namespace minko;
using namespace android::sensors;

// Static fields
math::mat4 AndroidAttitude::rotationMatrixValue;
math::quat AndroidAttitude::quaternionValue;
std::mutex AndroidAttitude::rotationMatrixMutex;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_minko_plugin_sensors_AndroidAttitude_minkoNativeOnAttitudeEvent(JNIEnv* env, jobject obj, jfloatArray rotationMatrix, jfloatArray quaternion)
{
    jfloat* rotationMatrixFloat = env->GetFloatArrayElements(rotationMatrix, 0);
    jfloat* quaternionFloat = env->GetFloatArrayElements(quaternion, 0);
    
    // Set the quaternion
    AndroidAttitude::quaternionValue = math::quat();
    AndroidAttitude::quaternionValue.x = quaternionFloat[0];
    AndroidAttitude::quaternionValue.y = quaternionFloat[1];
    AndroidAttitude::quaternionValue.z = quaternionFloat[2];
    AndroidAttitude::quaternionValue.w = quaternionFloat[3];

    // Set the rotation matrix
    AndroidAttitude::rotationMatrixMutex.lock();
    auto c = 0;

    for (auto i = 0; i < 4; i++)
    {
        for (auto j = 0; j < 4; j++)
        {
            AndroidAttitude::rotationMatrixValue[i][j] = rotationMatrixFloat[c];
            c++;
        }
    }
    AndroidAttitude::rotationMatrixMutex.unlock();

    env->ReleaseFloatArrayElements(rotationMatrix, rotationMatrixFloat, 0);
    env->ReleaseFloatArrayElements(quaternion, quaternionFloat, 0);
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
    // The inertial reference frame has z up and x forward, while the world has z out and x right
    _worldToInertialReferenceFrame = getRotateEulerMatrix(-90.f, 0.f, 0.f);
    
    // This assumes the device is landscape with the home button on the right
    _deviceToDisplay = getRotateEulerMatrix(0.f, 0.f, 0.f);

    // Defaut
    AndroidAttitude::rotationMatrixValue = math::mat4();

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
    auto worldToDevice = rotationMatrixValue * _worldToInertialReferenceFrame;
    auto worldRotationMatrix = _deviceToDisplay * worldToDevice;

    return worldRotationMatrix;
}

const math::quat&
AndroidAttitude::quaternion()
{
    return quaternionValue;
}

math::mat4
AndroidAttitude::getRotateEulerMatrix(float x, float y, float z)
{
    x *= (float)(M_PI / 180.0f);
    y *= (float)(M_PI / 180.0f);
    z *= (float)(M_PI / 180.0f);
    
    float cx = (float) cos(x);
    float sx = (float) sin(x);
    float cy = (float) cos(y);
    float sy = (float) sin(y);
    float cz = (float) cos(z);
    float sz = (float) sin(z);
    float cxsy = cx * sy;
    float sxsy = sx * sy;
    
    math::mat4 matrix;
    
    matrix[0][0] = cy * cz;
    matrix[0][1] = -cy * sz;
    matrix[0][2] = sy;
    matrix[0][3] = 0.0f;
    matrix[1][0] = cxsy * cz + cx * sz;
    matrix[1][1] = -cxsy * sz + cx * cz;
    matrix[1][2] = -sx * cy;
    matrix[1][3] = 0.0f;
    matrix[2][0] = -sxsy * cz + sx * sz;
    matrix[2][1] = sxsy * sz + sx * cz;
    matrix[2][2] = cx * cy;
    matrix[2][3] = 0.0f;
    matrix[3][0] = 0.0f;
    matrix[3][1] = 0.0f;
    matrix[3][2] = 0.0f;
    matrix[3][3] = 1.0f;
    
    return matrix;
}