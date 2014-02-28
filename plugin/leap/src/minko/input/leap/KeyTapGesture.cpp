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
#include "minko/input/leap/KeyTapGesture.hpp"

using namespace minko;
using namespace minko::input;
using namespace minko::input::leap;

KeyTapGesture::KeyTapGesture(const Gesture& gesture):
	Gesture(gesture),
	_leapKeyTap(nullptr)
{
	if (_leapGesture->type() != Leap::Gesture::Type::TYPE_KEY_TAP)
		throw std::logic_error("Provided Gesture is not a KeyType Gesture.");

	Leap::KeyTapGesture leapKeyTap	= *_leapGesture;
	_leapKeyTap						= std::make_shared<Leap::KeyTapGesture>(leapKeyTap);

	if (_leapKeyTap == nullptr)
		throw std::invalid_argument("gesture");
}

math::Vector3::Ptr
KeyTapGesture::direction(math::Vector3::Ptr output) const
{
	return convert(_leapKeyTap->direction(), output);
}

math::Vector3::Ptr
KeyTapGesture::position(math::Vector3::Ptr output) const
{
	return convert(_leapKeyTap->position(), output);
}

float
KeyTapGesture::progress() const
{
	return _leapKeyTap->progress();
}

uint32_t
KeyTapGesture::pointableID() const
{
	return _leapKeyTap->pointable().id();
}