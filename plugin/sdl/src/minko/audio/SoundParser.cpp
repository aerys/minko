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

#include "minko/audio/SoundParser.hpp"
#include "minko/audio/SDLSound.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/log/Logger.hpp"

using namespace minko;
using namespace minko::audio;

SoundParser::SoundParser()
{
}

void
SoundParser::parse(const std::string&                   filename,
                   const std::string&                   resolvedFilename,
                   std::shared_ptr<file::Options>       options,
                   const std::vector<unsigned char>&    data,
                   std::shared_ptr<file::AssetLibrary>  assets)
{
#if SDL_AUDIO_ENABLED
    std::shared_ptr<SDLSound> sound(new SDLSound);

    SDL_RWops* ops = SDL_RWFromConstMem(&*data.begin(), data.size());

    sound->_chunk = Mix_LoadWAV_RW(ops, 0);

    if (!sound->_chunk)
    {
        error()->execute(shared_from_this(), file::Error(SDL_GetError()));
        return;
    }

    SDL_FreeRW(ops);

    assets->sound(filename, sound);

    complete()->execute(shared_from_this());
#else
    LOG_ERROR("Audio file parsing is not supported on this platform.");
#endif
}
