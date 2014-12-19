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
#include "minko/input/leap/Pointable.hpp"

using namespace minko;
using namespace minko::input;
using namespace minko::input::leap;

Pointable::Pointable(const Leap::Pointable& leapPointable):
    _leapPointable(std::make_shared<Leap::Pointable>(leapPointable))
{
    if (_leapPointable == nullptr)
        throw std::invalid_argument("leapPointable");
}

int32_t
Pointable::id() const
{
    return _leapPointable->id();
}

bool
Pointable::isValid() const
{
    return _leapPointable->isValid();
}

bool
Pointable::isFinger() const
{
    return _leapPointable->isFinger();
}

bool
Pointable::isTool() const
{
    return _leapPointable->isTool();
}

uint64_t
Pointable::frameID() const
{
    return _leapPointable->frame().id();
}

uint32_t
Pointable::handID() const
{
    return _leapPointable->hand().id();
}

math::Vector3::Ptr
Pointable::direction(math::Vector3::Ptr output) const
{
    return convert(_leapPointable->direction(), output);
}

math::Vector3::Ptr
Pointable::stabilizedTipPosition(math::Vector3::Ptr output) const
{
    return convert(_leapPointable->stabilizedTipPosition(), output);
}

math::Vector3::Ptr
Pointable::tipPosition(math::Vector3::Ptr output) const
{
    return convert(_leapPointable->tipPosition(), output);
}

math::Vector3::Ptr
Pointable::tipVelocity(math::Vector3::Ptr output) const
{
    return convert(_leapPointable->tipVelocity(), output);
}

float
Pointable::lengthMillimeters() const
{
    return _leapPointable->length();
}

float
Pointable::widthMillimeters() const
{
    return _leapPointable->width();
}

float
Pointable::timeVisible() const
{
    return _leapPointable->timeVisible();
}

float
Pointable::touchDistance() const
{
    return _leapPointable->touchDistance();
}

Pointable::Zone
Pointable::touchZone() const
{
    return convert(_leapPointable->touchZone());
}

bool
Pointable::operator!=(const Pointable& other) const
{
    return _leapPointable->operator!=(*other._leapPointable);
}

bool
Pointable::operator==(const Pointable& other) const
{
    return _leapPointable->operator==(*other._leapPointable);
}
