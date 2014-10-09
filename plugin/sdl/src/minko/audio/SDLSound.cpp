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
#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    _chunk(nullptr)
#else
    _buffer(nullptr),
    _length(0),
    _pos(0)
#endif
{
}

SDLSound::~SDLSound()
{
#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5
    SDL_FreeWAV(_buffer);
#endif
}

std::shared_ptr<SoundChannel>
SDLSound::play(int count)
{
    auto channel = std::shared_ptr<SDLSoundChannel>(new SDLSoundChannel(shared_from_this()));

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    channel->_channel = Mix_PlayChannel(-1, _chunk, -1);

    if (channel->_channel < 0)
    {
        LOG_ERROR("Fail playing sound: " << Mix_GetError());
        return nullptr;
    }
#else
    SDL_AudioSpec want;
    SDL_AudioSpec have;

    SDL_zero(want);
    SDL_zero(have);
    want.freq = 48000;
    want.format = AUDIO_F32;
    want.channels = 2;
    want.samples = 4096;
    want.userdata = channel.get();
    want.callback = &SDLSound::fillBuffer;

    channel->_device = SDL_OpenAudioDevice(nullptr, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

    if (!channel->_device)
    {
        std::cerr << "Sound::play(): failed to open audio (" << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_PauseAudioDevice(channel->_device, 0);
#endif

    return channel;
}

void
SDLSound::fillBuffer(void* that, unsigned char* stream, int length)
{
#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5
    SDLSoundChannel* channel = static_cast<SDLSoundChannel*>(that);
    SDLSound* sound = static_cast<SDLSound*>(channel->sound().get());
    std::memset(stream, 0, length);
    std::memcpy(stream, sound->_buffer + sound->_pos, std::min(length, int(sound->_length - sound->_pos)));

    SoundTransform::Ptr transform = channel->transform();

    if (!!transform)
        for (int i = 0; i < length; ++i)
        {
            if (i % 2 == 0)
                stream[i] *= transform->left() * transform->volume();
            else
                stream[i] *= transform->right() * transform->volume();
        }
#endif
}
