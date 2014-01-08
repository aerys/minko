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
#include "minko/input/leap/Hand.hpp"

using namespace minko;
using namespace minko::input;
using namespace minko::input::leap;

Hand::Hand(const Leap::Hand& leapHand):
	_leapHand(std::make_shared<Leap::Hand>(leapHand))
{
	if (_leapHand == nullptr)
		throw std::invalid_argument("leapHand");
}

int32_t
Hand::id() const
{
	return _leapHand->id();
}

bool
Hand::isValid() const
{
	return _leapHand->isValid();
}

uint64_t
Hand::frameID() const
{
	return _leapHand->frame().id();
}

math::Vector3::Ptr
Hand::palmPosition(math::Vector3::Ptr output) const
{
	return convert(_leapHand->palmPosition(), output);
}

math::Vector3::Ptr
Hand::palmNormal(math::Vector3::Ptr output) const
{
	return convert(_leapHand->palmNormal(), output);
}

math::Vector3::Ptr
Hand::palmVelocity(math::Vector3::Ptr output) const
{
	return convert(_leapHand->palmVelocity(), output);
}

bool
Hand::operator!=(const Hand& other) const
{
	return _leapHand->operator!=(*other._leapHand);
}

bool
Hand::operator==(const Hand& other) const
{
	return _leapHand->operator==(*other._leapHand);
}
