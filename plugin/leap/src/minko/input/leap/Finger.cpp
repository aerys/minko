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

#include "LeapPimpl.hpp" // voluntarily not pointed to by the includedirs
#include "minko/input/leap/Finger.hpp"

using namespace minko;
using namespace minko::input;
using namespace minko::input::leap;
using namespace minko::math;

leap::Finger::Bone::Bone(const Leap::Bone& bone) :
    _leapBone(std::make_shared<Leap::Bone>(bone))
{
}

bool
leap::Finger::Bone::isValid() const
{
    return _leapBone->isValid();
}

Matrix4x4::Ptr
leap::Finger::Bone::basis() const
{
    return convert(_leapBone->basis());
}

Vector3::Ptr
leap::Finger::Bone::center() const
{
    return convert(_leapBone->center());
}

Vector3::Ptr
leap::Finger::Bone::direction() const
{
    return convert(_leapBone->direction());
}

leap::Finger::Bone::Type
leap::Finger::Bone::type() const
{
    return convert(_leapBone->type());
}

Vector3::Ptr
leap::Finger::Bone::nextJoint() const
{
    return convert(_leapBone->nextJoint());
}

Vector3::Ptr
leap::Finger::Bone::previousJoint() const
{
    return convert(_leapBone->prevJoint());
}

float
leap::Finger::Bone::length() const
{
    return _leapBone->length();
}

float
leap::Finger::Bone::width() const
{
    return _leapBone->width();
}

leap::Finger::Finger(const Leap::Finger& finger) :
    Pointable(finger)
{
}

leap::Finger::BonePtr
leap::Finger::bone(Bone::Type type) const
{
    auto finger = std::make_shared<Leap::Finger>(*_leapPointable);

    return std::shared_ptr<Bone>(new Bone(finger->bone(convert(type))));
}

leap::Finger::Type
leap::Finger::type() const
{
    auto finger = std::make_shared<Leap::Finger>(*_leapPointable);

    return convert(finger->type());
}

float
leap::Finger::length() const
{
    return _leapPointable->length();
}