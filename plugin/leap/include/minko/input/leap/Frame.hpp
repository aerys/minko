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
	class Frame;
}

namespace minko
{
	namespace input
	{
		namespace leap
		{
			class Hand;
			class Pointable;
            class Finger;

			class Frame : public std::enable_shared_from_this<Frame>
			{
				friend class Controller; // Only a Controller can instanciate a Frame

			public:
				typedef std::shared_ptr<Frame>		            Ptr;

				typedef std::shared_ptr<Hand>		            HandPtr;
				typedef std::shared_ptr<Gesture>	            GesturePtr;
				typedef std::shared_ptr<Pointable>	            PointablePtr;

                typedef std::shared_ptr<Finger>                 FingerPtr;
                typedef std::vector<FingerPtr>                  FingerList;

                typedef std::shared_ptr<math::Vector3>	        Vector3Ptr;
                typedef std::shared_ptr<math::Matrix4x4>        Matrix4x4Ptr;

			private:
				std::shared_ptr<Leap::Frame>	_leapFrame;

			public:

				int64_t
				id() const;

				bool
				isValid() const;

				// Hands 
				HandPtr
				handByID(int32_t) const;

				uint
				numHands() const;

				HandPtr
				handByIndex(int) const;

				HandPtr
				frontmostHand() const;

				HandPtr
				leftmostHand() const;

				HandPtr
				rightmostHand() const;

				// Gestures
				GesturePtr
				gestureByID(int32_t) const;

				uint
				numGestures() const;

				GesturePtr
				gestureByIndex(int) const;

				// Pointables
				PointablePtr
				pointableByID(int32_t) const;

				uint
				numPointables() const;

				PointablePtr
				pointableByIndex(int) const;

                FingerList
                fingers() const;

                Vector3Ptr
                translation(int32_t handId, Ptr sinceFrame) const;

                Matrix4x4Ptr
                rotationMatrix(int32_t handId, Ptr sinceFrame) const;

				// (In)Equality tests
				bool
				operator!=(const Frame&) const;

				bool
				operator==(const Frame&) const;

			private:
				Frame(); // no implementation!
				
				explicit
				Frame(const Leap::Frame&);
			};
		}
	}
}
