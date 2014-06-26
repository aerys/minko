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
	class Pointable;
}

namespace minko
{
	namespace input
	{
		namespace leap
		{
			class Pointable : public std::enable_shared_from_this<Pointable>
			{
				friend class Frame; // Only a Frame can instanciate a Pointable

			public:
				enum class Zone
				{
					None		= 0,
					Hovering	= 1,
					Touching	= 2
				};

			public:	
				typedef std::shared_ptr<Pointable>		Ptr;

				typedef std::shared_ptr<math::Vector3>	Vector3Ptr;

			protected:
				std::shared_ptr<Leap::Pointable>		_leapPointable;

			public:
                virtual
                ~Pointable()
                {

                }

				int32_t
				id() const;

				bool
				isValid() const;

				bool
				isFinger() const;

				bool
				isTool() const;

				uint64_t
				frameID() const;

				uint32_t
				handID() const;

				Vector3Ptr
				direction(Vector3Ptr output = nullptr) const;

				Vector3Ptr
				stabilizedTipPosition(Vector3Ptr output = nullptr) const;

				Vector3Ptr
				tipPosition(Vector3Ptr output = nullptr) const;

				Vector3Ptr
				tipVelocity(Vector3Ptr output = nullptr) const;
				
				float
				lengthMillimeters() const;

				float
				widthMillimeters() const;	
				
				float
				timeVisible() const;
				
				float
				touchDistance() const;
				
				Zone
				touchZone() const;

				// (In)Equality tests
				bool
				operator!=(const Pointable&) const;

				bool
				operator==(const Pointable&) const;

            protected:
                explicit
                Pointable(const Leap::Pointable&);

			private:
				Pointable(); // no implementation!
			};
		}
	}
}
