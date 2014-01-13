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

#include "minko/input/leap/Gesture.hpp"

namespace Leap
{
	class KeyTapGesture;
}

namespace minko
{
	namespace input
	{
		namespace leap
		{
			class KeyTapGesture : public Gesture
			{
				friend class Gesture; // Only a Gesture can instanciate a KeyTapGesture

			public:
				typedef std::shared_ptr<KeyTapGesture>	Ptr;

			private:
				typedef std::shared_ptr<math::Vector3>	Vector3Ptr;

			private:
				std::shared_ptr<Leap::KeyTapGesture>	_leapKeyTap;

			public:
				
				Vector3Ptr
				direction(Vector3Ptr output = nullptr) const;

				Vector3Ptr
				position(Vector3Ptr output = nullptr) const;

				float
				progress() const;

				uint32_t
				pointableID() const;

			private:
				KeyTapGesture(); // no implementation

				explicit
				KeyTapGesture(const Gesture&);
			};
		}
	}
}