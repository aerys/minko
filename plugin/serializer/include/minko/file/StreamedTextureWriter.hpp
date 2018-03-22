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
#include "minko/file/AbstractWriter.hpp"
#include "minko/file/TextureWriter.hpp"

namespace minko
{
    namespace file
    {
        class StreamedTextureWriter :
            public AbstractWriter<std::shared_ptr<render::AbstractTexture>>
        {
        public:
            typedef std::shared_ptr<StreamedTextureWriter>                              Ptr;

            typedef std::function<bool(std::shared_ptr<render::AbstractTexture>,
                                       const std::string&,
                                       std::shared_ptr<WriterOptions>,
                                       msgpack::sbuffer&,
                                       std::vector<msgpack::type::tuple<int, int>>&)>   FormatWriterFunction;

        private:
            static std::unordered_map<render::TextureFormat, FormatWriterFunction, Hash<render::TextureFormat>>  _formatWriterFunctions;

            std::string                         _textureType;

            std::shared_ptr<LinkedAsset>        _linkedAsset;
            int                                 _linkedAssetId;

        public:
            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new StreamedTextureWriter());

                return instance;
            }

            inline
            void
            textureType(const std::string& value)
            {
                _textureType = value;
            }

            inline
            void
            linkedAsset(std::shared_ptr<LinkedAsset> linkedAsset, int linkedAssetId)
            {
                _linkedAsset = linkedAsset;

                _linkedAssetId = linkedAssetId;
            }

            std::string
            embed(std::shared_ptr<AssetLibrary>     assetLibrary,
                  std::shared_ptr<Options>          options,
                  std::shared_ptr<Dependency>       dependency,
                  std::shared_ptr<WriterOptions>    writerOptions,
                  std::vector<unsigned char>&       embeddedHeaderData);

        private:
            StreamedTextureWriter();

            void
            ensureTextureSizeIsValid(std::shared_ptr<render::AbstractTexture>   texture,
                                     std::shared_ptr<WriterOptions>             writerOptions,
                                     const std::string&                         textureType);

            static
            bool
            writeMipLevels(render::TextureFormat                          textureFormat,
                           int                                            textureWidth,
                           int                                            textureHeight,
                           const std::vector<unsigned char>&              data,
                           std::vector<msgpack::type::tuple<int, int>>&   mipLevels,
                           msgpack::sbuffer&                              blob);

            static
            bool
            writeRGBATexture(std::shared_ptr<render::AbstractTexture>       texture,
                             const std::string&                             textureType,
                             std::shared_ptr<WriterOptions>                 writerOptions,
                             msgpack::sbuffer&                              blob,
                             std::vector<msgpack::type::tuple<int, int>>&   mipLevels);

            static
            bool
            writePvrCompressedTexture(render::TextureFormat                         textureFormat,
                                      std::shared_ptr<render::AbstractTexture>      texture,
                                      const std::string&                            textureType,
                                      std::shared_ptr<WriterOptions>                writerOptions,
                                      msgpack::sbuffer&                             blob,
                                      std::vector<msgpack::type::tuple<int, int>>&  mipLevels);

            static
            bool
            writeQCompressedTexture(render::TextureFormat                           textureFormat,
                                    std::shared_ptr<render::AbstractTexture>        texture,
                                    const std::string&                              textureType,
                                    std::shared_ptr<WriterOptions>                  writerOptions,
                                    msgpack::sbuffer&                               blob,
                                    std::vector<msgpack::type::tuple<int, int>>&    mipLevels);

            static
            bool
            writeCRNCompressedTexture(render::TextureFormat                         textureFormat,
                                      std::shared_ptr<render::AbstractTexture>      abstractTexture,
                                      const std::string&                            textureType,
                                      std::shared_ptr<WriterOptions>                writerOptions,
                                      msgpack::sbuffer&                             blob,
                                      std::vector<msgpack::type::tuple<int, int>>&  mipLevels);
        };
    }
}
