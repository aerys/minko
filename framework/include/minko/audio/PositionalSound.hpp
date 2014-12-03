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
#include "minko/component/AbstractScript.hpp"

namespace minko
{
    namespace audio
    {
        class SoundChannel;

        class PositionalSound :
            public component::AbstractScript
        {
        public:
            typedef std::shared_ptr<PositionalSound> Ptr;

            void
            update(std::shared_ptr<scene::Node> target) override;

            void
            stop(std::shared_ptr<scene::Node> target) override;

            std::function<float (float)>
            audibilityCurve() const
            {
                return _audibilityCurve;
            }

            Ptr
            audibilityCurve(std::function<float (float)> value)
            {
                _audibilityCurve = value;
                return std::static_pointer_cast<PositionalSound>(shared_from_this());
            }

            inline
            static
            Ptr
            create(std::shared_ptr<SoundChannel> channel,
                   std::shared_ptr<scene::Node> camera)
            {
                auto p = Ptr(new PositionalSound(channel, camera));

                return p;
            }

            virtual
            ~PositionalSound()
            {
            }

        protected:
            PositionalSound(std::shared_ptr<SoundChannel> channel,
                            std::shared_ptr<scene::Node> camera) :
                _channel(channel),
                _camera(camera),
                _audibilityCurve(&PositionalSound::defaultAudibilityCurve)
            {
            }

        private:
            static
            float
            defaultAudibilityCurve(float distance)
            {
                return float(10.f / (4.f * M_PI * distance));
            }

            std::shared_ptr<SoundChannel>   _channel;
            std::shared_ptr<scene::Node>    _camera;
            std::function<float (float)>    _audibilityCurve;
        };
    }
}
