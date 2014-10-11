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

#include "minko/audio/SDLSoundChannel.hpp"
#include "minko/audio/SoundTransform.hpp"

using namespace minko;
using namespace minko::audio;

SDLSoundChannel::SDLSoundChannel(std::shared_ptr<Sound> sound) :
    SoundChannel(sound),
    _channel(0)
{
}

SDLSoundChannel::~SDLSoundChannel()
{
    Mix_HaltChannel(_channel);
}

void
SDLSoundChannel::stop()
{
    Mix_Pause(_channel);
}

SoundChannel::Ptr
SDLSoundChannel::transform(SoundTransform::Ptr value)
{
    if (!!value)
    {
        Mix_SetPanning(_channel, uint(value->left() * value->volume() * 255), uint(value->right() * value->volume() * 255));
    }

    return SoundChannel::transform(value);
}

SoundTransform::Ptr
SDLSoundChannel::transform() const
{
    return SoundChannel::transform();
}
