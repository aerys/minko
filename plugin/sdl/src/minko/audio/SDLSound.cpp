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

#include "minko/audio/SDLSound.hpp"
#include "minko/audio/SDLSoundChannel.hpp"
#include "minko/audio/SoundTransform.hpp"
#include "minko/log/Logger.hpp"

using namespace minko;
using namespace minko::audio;

SDLSound::SDLSound() :
    _chunk(nullptr)
{
}

SDLSound::~SDLSound()
{
}

std::shared_ptr<SoundChannel>
SDLSound::play(int count)
{
#if SDL_AUDIO_ENABLED
    if (count < 0)
        throw std::invalid_argument("count cannot be less than zero");

    auto channel = std::shared_ptr<SDLSoundChannel>(new SDLSoundChannel(shared_from_this()));

    channel->channel(Mix_PlayChannel(-1, _chunk, count == 0 ? -1 : (count == 1 ? 0 : count)));

    if (channel->_channel < 0)
    {
        LOG_ERROR("Fail playing sound: " << Mix_GetError());
        return nullptr;
    }

    return channel;
#else
    LOG_ERROR("Playing audio is not supported on this platform.");
    return nullptr;
#endif
}
