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
#include "minko/input/leap/ScreenTapGesture.hpp"

using namespace minko;
using namespace minko::input;
using namespace minko::input::leap;

ScreenTapGesture::ScreenTapGesture(const Gesture& gesture):
    Gesture(gesture),
    _leapScreenTap(nullptr)
{
    if (_leapGesture->type() != Leap::Gesture::Type::TYPE_SCREEN_TAP)
        throw std::logic_error("Provided Gesture is not a ScreenTap Gesture.");

    Leap::ScreenTapGesture leapScreenTap    = *_leapGesture;
    _leapScreenTap                            = std::make_shared<Leap::ScreenTapGesture>(leapScreenTap);

    if (_leapScreenTap == nullptr)
        throw std::invalid_argument("gesture");
}

math::Vector3::Ptr
ScreenTapGesture::direction(math::Vector3::Ptr output) const
{
    return convert(_leapScreenTap->direction(), output);
}

math::Vector3::Ptr
ScreenTapGesture::position(math::Vector3::Ptr output) const
{
    return convert(_leapScreenTap->position(), output);
}

float
ScreenTapGesture::progress() const
{
    return _leapScreenTap->progress();
}

uint32_t
ScreenTapGesture::pointableID() const
{
    return _leapScreenTap->pointable().id();
}