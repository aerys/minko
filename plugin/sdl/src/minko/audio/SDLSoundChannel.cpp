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

std::map<uint, std::shared_ptr<SDLSoundChannel>>
SDLSoundChannel::_activeChannels;

SDLSoundChannel::SDLSoundChannel(std::shared_ptr<Sound> sound) :
    SoundChannel(sound),
    _channel(0)
{
}

SDLSoundChannel::~SDLSoundChannel()
{
    stop();
}

void
SDLSoundChannel::stop()
{
#if SDL_AUDIO_ENABLED
    if (_channel >= 0)
    {
        Mix_HaltChannel(_channel);
    }
#endif
}

SoundChannel::Ptr
SDLSoundChannel::transform(SoundTransform::Ptr value)
{
#if SDL_AUDIO_ENABLED
    if (!!value && _channel >= 0)
    {
        Mix_SetPanning(_channel, uint(value->left() * value->volume() * 255), uint(value->right() * value->volume() * 255));
    }
#endif

    return SoundChannel::transform(value);
}

SoundTransform::Ptr
SDLSoundChannel::transform() const
{
    return SoundChannel::transform();
}

void
SDLSoundChannel::channel(int c)
{
    _channel = c;
    _activeChannels[c] = std::static_pointer_cast<SDLSoundChannel>(shared_from_this());
}

void
SDLSoundChannel::channelComplete(int c)
{
    if (_activeChannels.size() <= uint(c))
        return;

    std::shared_ptr<SDLSoundChannel> channel = _activeChannels[c];
    _activeChannels.erase(c);
    channel->_channel = -1;

    channel->complete()->execute(channel);
}

bool
SDLSoundChannel::playing() const
{
    return _channel != -1;
}