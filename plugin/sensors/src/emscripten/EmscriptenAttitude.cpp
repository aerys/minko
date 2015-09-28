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
#include "emscripten/EmscriptenAttitude.hpp"

using namespace minko;
using namespace emscripten::sensors;

math::mat4 EmscriptenAttitude::rotationMatrixValue;
math::quat EmscriptenAttitude::quaternionValue;
std::mutex EmscriptenAttitude::rotationMatrixMutex;

EmscriptenAttitude::EmscriptenAttitude() :
    _alpha(0.f),
    _beta(0.f),
    _gamma(0.f)
{
}

//In portrait : 
//Alpha = left/right
//Gamma = up/down
//Beta = tilt

void EmscriptenAttitude::initialize()
{
    // The inertial reference frame has z up and x forward, while the world has z out and x right
    _worldToInertialReferenceFrame = getRotateEulerMatrix(-90.f, 0.f, 0.f);
    
    // This assumes the device is landscape with the home button on the right
    _deviceToDisplay = getRotateEulerMatrix(0.f, 0.f, 0.f);

    // Defaut
    EmscriptenAttitude::rotationMatrixValue = math::mat4();

    std::string eval = "window.MinkoEmscriptenAttitude = {alpha: 0.0, beta: 0.0, gamma: 0.0}; window.addEventListener('deviceorientation', function(e){console.log('deviceorientation');window.MinkoEmscriptenAttitude = {alpha: e.alpha || 0.0, gamma: e.gamma || 0.0, beta: e.beta || 0.0};})";

    emscripten_run_script(eval.c_str());
}

void
EmscriptenAttitude::startTracking()
{
    LOG_INFO("Start tracking");
    //TODO
}

void
EmscriptenAttitude::stopTracking()
{
    LOG_INFO("Stop tracking");
    //TODO
}

math::mat4
EmscriptenAttitude::rotationMatrix()
{
    auto orientation = emscripten_run_script_int("(function(){if(window.orientation !== undefined) return window.orientation; else return -90;}())");

    auto newAlpha = atof(emscripten_run_script_string("(window.MinkoEmscriptenAttitude.alpha.toString())"));
    auto newBeta = atof(emscripten_run_script_string("(window.MinkoEmscriptenAttitude.beta.toString())"));
    auto newGamma = atof(emscripten_run_script_string("(window.MinkoEmscriptenAttitude.gamma.toString())"));

    if (orientation == -90)
    {
        newBeta = -newBeta;
    }
    else if (orientation == 90)
    {
        newAlpha = newAlpha -180.f;
        newGamma = -newGamma;
    }

    if (newAlpha != _alpha || newBeta != _beta || newGamma != _gamma)
    {
        _alpha = newAlpha;
        _beta = newBeta;
        _gamma = newGamma;

        rotationMatrixValue = getRotationMatrix(_alpha, _beta, _gamma);
    }
    
    auto worldToDevice = rotationMatrixValue * _worldToInertialReferenceFrame;
    auto worldRotationMatrix = _deviceToDisplay * worldToDevice;

    return worldRotationMatrix;
}

const math::quat&
EmscriptenAttitude::quaternion()
{
    return quaternionValue;
}

math::mat4
EmscriptenAttitude::getRotationMatrix(float alpha, float beta, float gamma)
{
    alpha *= (float)(M_PI / 180.0f);
    beta *= (float)(M_PI / 180.0f);
    gamma *= (float)(M_PI / 180.0f);
    
    float ca = (float) cos(alpha);
    float sa = (float) sin(alpha);
    float cb = (float) cos(beta);
    float sb = (float) sin(beta);
    float cg = (float) cos(gamma);
    float sg = (float) sin(gamma);


    math::mat4 matrix;

    matrix[0][0] = ca * cb;
    matrix[0][1] = ca * sb * sg - sa * cg;
    matrix[0][2] = ca * sb * cg + sa * sg;
    matrix[0][3] = 0.0f;

    matrix[1][0] = sa * cb;
    matrix[1][1] = sa * sb * sg + ca * cg;
    matrix[1][2] = sa * sb * cg - ca * sg;
    matrix[1][3] = 0.0f;

    matrix[2][0] = -sb;
    matrix[2][1] = cb * sg;
    matrix[2][2] = cb * cg;
    matrix[2][3] = 0.0f;

    matrix[3][0] = 0.0f;
    matrix[3][1] = 0.0f;
    matrix[3][2] = 0.0f;
    matrix[3][3] = 1.0f;

    return matrix;
}

math::mat4
EmscriptenAttitude::getRotateEulerMatrix(float x, float y, float z)
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

bool
EmscriptenAttitude::isSupported()
{
    std::string eval = "(function(){if(window.DeviceOrientationEvent) return 1; else return 0;}())";

    auto result = emscripten_run_script_int(eval.c_str());

    if (result)
        return true;
    else
        return false;
}