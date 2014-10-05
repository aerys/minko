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

#include "minko/Canvas.hpp"
#include "minko/SDLAudio.hpp"

#include <SDL/SDL_mixer.h>

using namespace minko;
using namespace minko::audio;

SDLAudio::SDLAudio(std::shared_ptr<Canvas> canvas)
{
    Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
    int ret = Mix_OpenAudio(0, 0, 0, 0); // we ignore all these..
    assert(ret == 0);
}

std::shared_ptr<SDLAudio>
SDLAudio::create(std::shared_ptr<Canvas> canvas)
{
    return std::shared_ptr<SDLAudio>(new SDLAudio(canvas));
}

std::shared_ptr<Sound>
SDLAudio::load(std::string filename)
{
    sound = Mix_LoadWAV("sound.ogg");
    assert(sound);

}
