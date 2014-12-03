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
#include "minko/input/leap/CircleGesture.hpp"

using namespace minko;
using namespace minko::input;
using namespace minko::input::leap;

CircleGesture::CircleGesture(const Gesture& gesture):
    Gesture(gesture),
    _leapCircle(nullptr)
{
    if (_leapGesture->type() != Leap::Gesture::Type::TYPE_CIRCLE)
        throw std::logic_error("Provided Gesture is not a Circle Gesture.");

    Leap::CircleGesture leapCircle    = *_leapGesture;
    _leapCircle                        = std::make_shared<Leap::CircleGesture>(leapCircle);

    if (_leapCircle == nullptr)
        throw std::invalid_argument("gesture");
}

math::Vector3::Ptr
CircleGesture::center(math::Vector3::Ptr output) const
{
    return convert(_leapCircle->center(), output);
}

math::Vector3::Ptr
CircleGesture::normal(math::Vector3::Ptr output) const
{
    return convert(_leapCircle->normal(), output);
}

float
CircleGesture::progress() const
{
    return _leapCircle->progress();
}

float
CircleGesture::radius() const
{
    return _leapCircle->radius();
}

uint32_t
CircleGesture::pointableID() const
{
    return _leapCircle->pointable().id();
}