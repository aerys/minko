/*
Copyright (c) 2015 Aerys

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
#include "minko/render/TextureFormat.hpp"

namespace minko
{
    namespace file
    {
        class MipMapChainParser
        {
        public:
            MipMapChainParser();

            ~MipMapChainParser();

            std::shared_ptr<render::Texture>
			parseTexture(std::shared_ptr<render::AbstractContext>	context,
                         uint             							width,
                         uint             							height,
        				 const unsigned char* 						data,
                         bool                                       parseMipMaps,
                         bool                  					    mipMapping	= false,
                         bool                  					    smooth      = true,
                         render::TextureFormat 					    format      = render::TextureFormat::RGBA,
                         const std::string&    					    filename    = "");

            std::shared_ptr<render::CubeTexture>
            parseCubeTexture(std::shared_ptr<render::AbstractContext>   context,
                             uint             		                    width,
                             uint             		                    height,
                             const unsigned char* 	                    data,
                             bool                                       parseMipMaps,
                             bool                                       mipMapping  = false,
                             bool                                       smooth      = true,
                             render::TextureFormat                      format      = render::TextureFormat::RGBA,
                             const std::string&                         filename    = "");

            void
            parseMipMap(unsigned char*       out,
                        const unsigned char* data,
                        uint                 width,
                        uint                 height,
                        math::ivec2          offset,
                        uint                 mipMapWidth,
                        uint                 mipMapHeight,
                        uint                 bytesPerPixel);
        };
    }
}
