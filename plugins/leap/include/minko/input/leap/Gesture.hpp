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

#include "minko/Common.hpp"
#include "minko/Signal.hpp"

namespace Leap
{
	class Gesture;
}

namespace minko
{
	namespace input
	{
		namespace leap
		{
			class SwipeGesture;
			class CircleGesture;
			class ScreenTapGesture;
			class KeyTapGesture;

			class Gesture : public std::enable_shared_from_this<Gesture>
			{
				friend class Frame; // Only a Frame can instanciate a Gesture

			public:
				enum class State
				{
					Invalid		= -1,
					Start		= 1,
					Update		= 2,
					Stop		= 3
				};

				enum class Type
				{
					Invalid		= -1, 
					Swipe		= 1, 
					Circle		= 4, 
					ScreenTap	= 5, 
					KeyTap		= 6
				};

			public:	
				typedef std::shared_ptr<Gesture>	Ptr;

			protected:
				std::shared_ptr<Leap::Gesture>		_leapGesture;

			public:
				
				int32_t
				id() const;

				bool
				isValid() const;

				int64_t
				durationMicroseconds() const; 

				State
				state() const;

				Type
				type() const;

				void
				handIDs(std::unordered_set<int32_t>&) const;

				void
				pointablesIDs(std::unordered_set<int32_t>&) const;

				std::shared_ptr<SwipeGesture>
				toSwipeGesture() const;

				std::shared_ptr<CircleGesture>
				toCircleGesture() const;

				std::shared_ptr<ScreenTapGesture>
				toScreenTapGesture() const;

				std::shared_ptr<KeyTapGesture>
				toKeyTapGesture() const;

			protected:
				Gesture(); // no implementation!

				explicit
				Gesture(const Leap::Gesture&);
			};
		}
	}
}