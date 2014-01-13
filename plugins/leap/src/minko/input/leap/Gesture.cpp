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
#include "minko/input/leap/Gesture.hpp"
#include "minko/input/leap/SwipeGesture.hpp"

using namespace minko;
using namespace minko::input;
using namespace minko::input::leap;

Gesture::Gesture(const Leap::Gesture& leapGesture):
	_leapGesture(std::make_shared<Leap::Gesture>(leapGesture))
{
	if (_leapGesture == nullptr)
		throw std::invalid_argument("leapGesture");
}

int32_t
Gesture::id() const
{
	return _leapGesture->id();
}

bool
Gesture::isValid() const
{
	return _leapGesture->isValid();
}

int64_t
Gesture::durationMicroseconds() const
{
	return _leapGesture->duration();
}

Gesture::State
Gesture::state() const
{
	return convert(_leapGesture->state());
}

Gesture::Type
Gesture::type() const
{
	return convert(_leapGesture->type());
}

void
Gesture::handIDs(std::unordered_set<int32_t>& ids) const
{
	const Leap::HandList& hands = _leapGesture->hands();

	ids.clear();
	for (auto& hand : hands)
		ids.insert(hand.id());
}

void
Gesture::pointablesIDs(std::unordered_set<int32_t>& ids) const
{
	const Leap::PointableList& pointables = _leapGesture->pointables();

	ids.clear();
	for (auto& pointable : pointables)
		ids.insert(pointable.id());
}

std::shared_ptr<SwipeGesture>
Gesture::toSwipeGesture() const
{
	return _leapGesture->type() == Leap::Gesture::Type::TYPE_SWIPE
	? std::shared_ptr<SwipeGesture>(new SwipeGesture(*this))
	: nullptr;
}
