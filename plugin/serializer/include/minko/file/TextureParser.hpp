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

#include "minko/file/AbstractSerializerParser.hpp"

namespace minko
{
    namespace file
    {
        class TextureParser :
            public AbstractSerializerParser
        {
        public:
            typedef std::shared_ptr<TextureParser> Ptr;

            typedef std::shared_ptr<Options> OptionsPtr;
            typedef std::shared_ptr<AssetLibrary> AssetLibraryPtr;

            typedef std::function<bool(const std::string&,
                                       OptionsPtr,
                                       const std::vector<unsigned char>&,
                                       AssetLibraryPtr)> FormatParserFunction;

        private:
            typedef std::shared_ptr<render::AbstractTexture> AbstractTexturePtr;

        private:
            static std::unordered_map<render::TextureFormat, FormatParserFunction> _formatParserFunctions;

            unsigned int _textureHeaderSize;

        public:
            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new TextureParser());

                return instance;
            }

            inline
            Ptr
            textureHeaderSize(unsigned int value)
            {
                _textureHeaderSize = value;

                return std::static_pointer_cast<TextureParser>(shared_from_this());
            }

            void
            parse(const std::string&                filename,
                  const std::string&                resolvedFilename,
                  std::shared_ptr<Options>          options,
                  const std::vector<unsigned char>& data,
                  std::shared_ptr<AssetLibrary>     assetLibrary);

        private:
            TextureParser();

            static
            bool
            parseRGBATexture(const std::string&                 fileName,
                             OptionsPtr                         options,
                             const std::vector<unsigned char>&  data,
                             AssetLibraryPtr                    assetLibrary);

            static
            bool
            parsePVRTexture(render::TextureFormat               format,
                            const std::string&                  fileName,
                            OptionsPtr                          options,
                            const std::vector<unsigned char>&   data,
                            AssetLibraryPtr                     assetLibrary);
        };
    }
}
