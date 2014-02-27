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

#pragma once

#include "Leap/Leap.h"

#include "minko/math/Vector3.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/input/leap/Gesture.hpp"
#include "minko/input/leap/Pointable.hpp"

/*
* Below are defined a couple of methods that are used to convert basic objects from the
* Leap API to their Minko counterparts (and vice versa). 
*
* This file is not intended to be accessible to
* users and is meant to be included in the *.cpp files of the Minko Leap plugin ONLY.
*/
namespace minko
{
	namespace input
	{
		namespace leap
		{
			static
			minko::math::Vector3::Ptr
			convert(const Leap::Vector&, minko::math::Vector3::Ptr output = nullptr);

			static
			minko::math::Matrix4x4::Ptr
			convert(const Leap::Matrix&, minko::math::Matrix4x4::Ptr output = nullptr);

			static 
			Leap::Gesture::State 
			convert(Gesture::State);

			static 
			Gesture::Type 
			convert(Leap::Gesture::Type);

			static 
			Gesture::State 
			convert(Leap::Gesture::State);

			static 
			Leap::Gesture::Type 
			convert(Gesture::Type);

			static
			Pointable::Zone
			convert(Leap::Pointable::Zone);
		}
	}
}

minko::math::Vector3::Ptr
minko::input::leap::convert(const Leap::Vector& value, 
							minko::math::Vector3::Ptr output)
{
	if (output == nullptr)
		output = minko::math::Vector3::create(value.x, value.y, value.z);
	else
		output->setTo(value.x, value.y, value.z);

	return output;
}

minko::math::Matrix4x4::Ptr
minko::input::leap::convert(const Leap::Matrix& value, 
							minko::math::Matrix4x4::Ptr output)
{
	if (output == nullptr)
		output = minko::math::Matrix4x4::create();

	value.toArray4x4(&(output->data()[0]));

	return output->transpose();
}

Leap::Gesture::State 
minko::input::leap::convert(minko::input::leap::Gesture::State value)
{
	switch (value)
	{
	case minko::input::leap::Gesture::State::Start:
		return Leap::Gesture::State::STATE_START;
		
	case minko::input::leap::Gesture::State::Update:
		return Leap::Gesture::State::STATE_UPDATE;
		
	case minko::input::leap::Gesture::State::Stop:
		return Leap::Gesture::State::STATE_STOP;

	default:
		return Leap::Gesture::State::STATE_INVALID;
	}
}

Leap::Gesture::Type 
minko::input::leap::convert(minko::input::leap::Gesture::Type value)
{
	switch (value)
	{
	case minko::input::leap::Gesture::Type::Circle:
		return Leap::Gesture::Type::TYPE_CIRCLE;
		
	case minko::input::leap::Gesture::Type::KeyTap:
		return Leap::Gesture::Type::TYPE_KEY_TAP;
		
	case minko::input::leap::Gesture::Type::ScreenTap:
		return Leap::Gesture::Type::TYPE_SCREEN_TAP;

	case minko::input::leap::Gesture::Type::Swipe:
		return Leap::Gesture::Type::TYPE_SWIPE;

	default:
		return Leap::Gesture::Type::TYPE_INVALID;
	}
}

minko::input::leap::Gesture::State 
minko::input::leap::convert(Leap::Gesture::State value)
{
	switch (value)
	{
	case Leap::Gesture::State::STATE_START:
		return minko::input::leap::Gesture::State::Start;
		
	case Leap::Gesture::State::STATE_UPDATE:
		return minko::input::leap::Gesture::State::Update;
		
	case Leap::Gesture::State::STATE_STOP:
		return minko::input::leap::Gesture::State::Stop;

	default:
		return minko::input::leap::Gesture::State::Invalid;
	}
}

minko::input::leap::Gesture::Type 
minko::input::leap::convert(Leap::Gesture::Type value)
{
	switch (value)
	{
	case Leap::Gesture::Type::TYPE_SWIPE:
		return minko::input::leap::Gesture::Type::Swipe;
		
	case Leap::Gesture::Type::TYPE_CIRCLE:
		return minko::input::leap::Gesture::Type::Circle;
		
	case Leap::Gesture::Type::TYPE_SCREEN_TAP:
		return minko::input::leap::Gesture::Type::ScreenTap;

	case Leap::Gesture::Type::TYPE_KEY_TAP:
		return minko::input::leap::Gesture::Type::KeyTap;

	default:
		return minko::input::leap::Gesture::Type::Invalid;
	}
}

minko::input::leap::Pointable::Zone
minko::input::leap::convert(Leap::Pointable::Zone value)
{
	switch (value)
	{
	case Leap::Pointable::ZONE_HOVERING:
		return minko::input::leap::Pointable::Zone::Hovering;
		
	case Leap::Pointable::ZONE_TOUCHING:
		return minko::input::leap::Pointable::Zone::Touching;

	default:
		return minko::input::leap::Pointable::Zone::None;
	}
}