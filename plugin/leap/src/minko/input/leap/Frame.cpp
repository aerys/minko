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
#include "minko/input/leap/Frame.hpp"
#include "minko/input/leap/Hand.hpp"
#include "minko/input/leap/Gesture.hpp"
#include "minko/input/leap/Pointable.hpp"
#include "minko/input/leap/Finger.hpp"

using namespace minko;
using namespace minko::input;
using namespace minko::input::leap;

Frame::Frame(const Leap::Frame& leapFrame):
	_leapFrame(std::make_shared<Leap::Frame>(leapFrame))
{
	if (_leapFrame == nullptr)
		throw std::invalid_argument("leapFrame");
}

int64_t
Frame::id() const
{
	return _leapFrame->id();
}

bool
Frame::isValid() const
{
	return _leapFrame->isValid();
}

Hand::Ptr
Frame::handByID(int32_t id) const
{
	return std::shared_ptr<Hand>(new Hand(_leapFrame->hand(int32_t(id))));
}

uint
Frame::numHands() const
{
	return (uint)_leapFrame->hands().count();
}

Hand::Ptr
Frame::handByIndex(int index) const
{
	const Leap::HandList& hands	= _leapFrame->hands();

	return index >=0 && index < (int)hands.count()
	? std::shared_ptr<Hand>(new Hand(hands[index]))
	: nullptr;
}

Hand::Ptr
Frame::frontmostHand() const
{
	return std::shared_ptr<Hand>(new Hand(_leapFrame->hands().frontmost()));
}

Hand::Ptr
Frame::leftmostHand() const
{
	return std::shared_ptr<Hand>(new Hand(_leapFrame->hands().leftmost()));
}

Hand::Ptr
Frame::rightmostHand() const
{
	return std::shared_ptr<Hand>(new Hand(_leapFrame->hands().rightmost()));
}

Gesture::Ptr
Frame::gestureByID(int32_t id) const
{
	return std::shared_ptr<Gesture>(new Gesture(_leapFrame->gesture(int32_t(id))));	
}

uint
Frame::numGestures() const
{
	return (uint)_leapFrame->gestures().count();
}

Gesture::Ptr
Frame::gestureByIndex(int index) const
{
	const Leap::GestureList& gestures	= _leapFrame->gestures();

	return index >=0 && index < (int)gestures.count()
	? std::shared_ptr<Gesture>(new Gesture(gestures[index]))
	: nullptr;
}

Pointable::Ptr
Frame::pointableByID(int32_t id) const
{
	return std::shared_ptr<Pointable>(new Pointable(_leapFrame->pointable(id)));
}

uint
Frame::numPointables() const
{
	return (uint)_leapFrame->pointables().count();
}

Pointable::Ptr
Frame::pointableByIndex(int index) const
{
	const Leap::PointableList& pointables = _leapFrame->pointables();

	return index >=0 && index < (int)pointables.count()
	? std::shared_ptr<Pointable>(new Pointable(pointables[index]))
	: nullptr;	
}

Frame::FingerList
Frame::fingers() const
{
    FingerList fingers;

    for (auto finger : _leapFrame->fingers())
    {
        fingers.push_back(std::shared_ptr<Finger>(new Finger(finger)));
    }

    return fingers;
}

math::Vector3::Ptr
Frame::translation(int32_t handId, Ptr sinceFrame) const
{
    const auto& leapFrame = *sinceFrame->_leapFrame;

    return convert(_leapFrame->hand(handId).translation(leapFrame));
}

math::Matrix4x4::Ptr
Frame::rotationMatrix(int32_t handId, Ptr sinceFrame) const
{
    const auto& leapFrame = *sinceFrame->_leapFrame;

    return convert(_leapFrame->hand(handId).rotationMatrix(leapFrame));
}

bool
Frame::operator!=(const Frame& other) const
{
	return _leapFrame->operator!=(*other._leapFrame);
}

bool
Frame::operator==(const Frame& other) const
{
	return _leapFrame->operator==(*other._leapFrame);
}