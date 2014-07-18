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
#include "minko/input/leap/Pointable.hpp"

namespace Leap
{
    class Bone;
    class Finger;
}

namespace minko
{
    namespace input
    {
        namespace leap
        {
            class Finger : public Pointable
            {
                friend class Hand;
                friend class Frame;

            public:
                enum class Type
                {
                    Thumb   = 0,
                    Index   = 1,
                    Middle  = 2,
                    Ring    = 3,
                    Pinky   = 4
                };

                class Bone
                {
                    friend class Finger;

                public:
                    enum class Type
                    {
                        Metacarpal      = 0,
                        Proximal        = 1,
                        Intermediate    = 2,
                        Distal          = 3
                    };

                private:
                    typedef std::shared_ptr<Leap::Bone> LeapBonePtr;

                private:
                    LeapBonePtr _leapBone;

                public:
                    bool
                    isValid() const;

                    std::shared_ptr<minko::math::Matrix4x4>
                    basis() const;

                    std::shared_ptr<minko::math::Vector3>
                    center() const;

                    std::shared_ptr<minko::math::Vector3>
                    direction() const;

                    std::shared_ptr<minko::math::Vector3>
                    nextJoint() const;

                    std::shared_ptr<minko::math::Vector3>
                    previousJoint() const;

                    float
                    length() const;

                    float
                    width() const;

                    Type
                    type() const;

                private:
                    explicit
                    Bone(const Leap::Bone& bone);
                };

            private:
                typedef std::shared_ptr<Bone> BonePtr;

            public:
                ~Finger()
                {
                }

                BonePtr
                bone(Bone::Type type) const;

                float
                length() const;

                Type
                type() const;

            private:
                explicit
                Finger(const Leap::Finger& finger);
            };
        }
    }
}