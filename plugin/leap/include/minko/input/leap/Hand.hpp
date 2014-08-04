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

namespace minko
{
    namespace input
    {
        namespace leap
        {
            class Finger;
            class Frame;
        }
    }
}

namespace Leap
{
    class Hand;
}

namespace minko
{
    namespace input
    {
        namespace leap
        {
            class Hand : public std::enable_shared_from_this<Hand>
            {
                friend class Frame; // Only a Frame can instanciate a Hand

            public:
                typedef std::shared_ptr<Hand>                    Ptr;

                typedef std::shared_ptr<Frame>                  FramePtr;
                typedef std::shared_ptr<Finger>                 FingerPtr;
                typedef std::vector<FingerPtr>                  FingerList;

                typedef std::shared_ptr<math::Vector3>            Vector3Ptr;
                typedef std::shared_ptr<math::Matrix4x4>        Matrix4x4Ptr;

            private:
                std::shared_ptr<Leap::Hand>                _leapHand;

            public:
                int32_t
                id() const;

                bool
                isValid() const;

                bool
                isRight() const;

                uint64_t
                frameID() const;

                Vector3Ptr
                palmPosition(Vector3Ptr output = nullptr) const;

                Vector3Ptr
                direction(Vector3Ptr output = nullptr) const;

                Vector3Ptr
                palmNormal(Vector3Ptr output = nullptr) const;

                Vector3Ptr
                palmVelocity(Vector3Ptr output = nullptr) const;

                float
                palmWidth() const;

                float
                pinchStrength() const;

                float
                grabStrength() const;

                Matrix4x4Ptr
                basis() const;

                FingerList
                fingers() const;

                float
                confidence() const;

                // (In)Equality tests
                bool
                operator!=(const Hand&) const;

                bool
                operator==(const Hand&) const;

            private:
                Hand(); // no implementation!

                explicit
                Hand(const Leap::Hand&);
            };
        }
    }
}
