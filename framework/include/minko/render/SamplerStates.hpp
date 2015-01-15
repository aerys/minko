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

#include "minko/render/WrapMode.hpp"
#include "minko/render/TextureFilter.hpp"
#include "minko/render/MipFilter.hpp"


namespace minko
{
    namespace render
    {
	    class SamplerStates
	    {
        public:
            static const std::string                        PROPERTY_WRAP_MODE;
            static const std::string                        PROPERTY_TEXTURE_FILTER;
            static const std::string                        PROPERTY_MIP_FILTER;

            static const std::array<std::string, 3>         PROPERTY_NAMES;

            static const WrapMode		                    DEFAULT_WRAP_MODE;
            static const TextureFilter	                    DEFAULT_TEXTURE_FILTER;
            static const MipFilter		                    DEFAULT_MIP_FILTER;
            
            static
            inline
            std::string uniformNameToSamplerStateName(const std::string& uniformName, const std::string& sampleState)
            {
                return uniformName + "/" + sampleState;
            }

            static
            inline
            std::string uniformNameToSamplerStateBindingName(const std::string& uniformName, const std::string& samplerState)
            {
                auto samplerStateCapitalized = std::string(samplerState);
                samplerStateCapitalized[0] = toupper(samplerStateCapitalized[0]);
                    
                return uniformName + samplerStateCapitalized;
            }

            static
            inline
            WrapMode stringToWrapMode(const std::string& value)
            {
                return value == "repeat" ? WrapMode::REPEAT : WrapMode::CLAMP;
            }

            static
            inline
            TextureFilter stringToTextureFilter(const std::string& value)
            {
                return value == "linear" ? TextureFilter::LINEAR : TextureFilter::NEAREST;
            }

            static
            inline
            MipFilter stringToMipFilter(const std::string& value)
            {
                return value == "linear" ? MipFilter::LINEAR : (value == "nearest" ? MipFilter::NEAREST : MipFilter::NONE);
            }

        public:
            WrapMode* wrapMode;
            TextureFilter* textureFilter;
            MipFilter* mipFilter;

            SamplerStates(WrapMode* wm, TextureFilter* tf, MipFilter* mf) :
                wrapMode(wm),
                textureFilter(tf),
                mipFilter(mf)
            {
            }
        };
	}
}
