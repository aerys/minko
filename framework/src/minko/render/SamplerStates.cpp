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

#include "minko/render/SamplerStates.hpp"

using namespace minko::render;

const std::string SamplerStates::PROPERTY_WRAP_MODE = "wrapMode";
const std::string SamplerStates::PROPERTY_TEXTURE_FILTER = "textureFilter";
const std::string SamplerStates::PROPERTY_MIP_FILTER = "mipFilter";

const WrapMode SamplerStates::DEFAULT_WRAP_MODE = WrapMode::CLAMP;
const TextureFilter SamplerStates::DEFAULT_TEXTURE_FILTER = TextureFilter::NEAREST;
const MipFilter SamplerStates::DEFAULT_MIP_FILTER = MipFilter::NONE;

const std::array<std::string, 3> SamplerStates::PROPERTY_NAMES = {
    PROPERTY_WRAP_MODE,
    PROPERTY_TEXTURE_FILTER,
    PROPERTY_MIP_FILTER
};