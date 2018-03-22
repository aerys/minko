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
#include "minko/Types.hpp"
#include "minko/SerializerCommon.hpp"
#include "minko/file/AbstractWriter.hpp"
#include "minko/Hash.hpp"
#include "minko/render/TextureFormat.hpp"

namespace minko
{
    namespace file
    {
        class TextureWriter :
            public AbstractWriter<std::shared_ptr<render::AbstractTexture>>
        {
        public:
            typedef std::shared_ptr<TextureWriter>                                      Ptr;

            typedef std::function<bool(std::shared_ptr<render::AbstractTexture>,
                                       const std::string&,
                                       std::shared_ptr<WriterOptions>,
                                       std::stringstream& blob)>                        FormatWriterFunction;

        private:
            typedef std::shared_ptr<AssetLibrary>               AssetLibraryPtr;
            typedef std::shared_ptr<Options>                    OptionsPtr;
            typedef std::shared_ptr<Dependency>                 DependencyPtr;
            typedef std::shared_ptr<WriterOptions>              WriterOptionsPtr;
            typedef std::shared_ptr<render::AbstractTexture>    AbstractTexturePtr;
            typedef std::shared_ptr<render::Texture>            TexturePtr;

        private:
            static std::unordered_map<render::TextureFormat, FormatWriterFunction, Hash<render::TextureFormat>> _formatWriterFunctions;

            static const float _defaultGamma;

            std::string _textureType;

            int _headerSize;

        public:
            ~TextureWriter() = default;

            static
            Ptr
            create()
            {
                return std::shared_ptr<TextureWriter>(new TextureWriter());
            }

            static
            int
            numMipLevels(unsigned int textureWidth, unsigned int textureHeight);

            static
            float
            defaultGamma()
            {
                return _defaultGamma;
            }

            static
            void
            gammaEncode(const std::vector<unsigned char>&    src,
                        std::vector<unsigned char>&          dst,
                        float                                gamma);

            static
            void
            gammaDecode(const std::vector<unsigned char>&    src,
                        std::vector<unsigned char>&          dst,
                        float                                gamma);

            inline
            void
            textureType(const std::string& textureType)
            {
                _textureType = textureType;
            }

            inline
            int
            headerSize() const
            {
                return _headerSize;
            }

            std::string
            embed(AssetLibraryPtr               assetLibrary,
                  OptionsPtr                    options,
                  DependencyPtr                 dependency,
                  WriterOptionsPtr              writerOptions,
                  std::vector<unsigned char>&   embeddedHeaderData);

        protected:
            TextureWriter();

        private:
            void
            ensureTextureSizeIsValid(std::shared_ptr<render::AbstractTexture>   texture,
                                     std::shared_ptr<WriterOptions>             writerOptions,
                                     const std::string&                         textureType);

            static
            bool
            writeRGBATexture(AbstractTexturePtr abstractTexture,
                             const std::string& textureType,
                             WriterOptionsPtr   writerOptions,
                             std::stringstream& blob);

            static
            bool
            writePvrCompressedTexture(render::TextureFormat   textureFormat,
                                      AbstractTexturePtr      abstractTexture,
                                      const std::string&      textureType,
                                      WriterOptionsPtr        writerOptions,
                                      std::stringstream&      blob);

            static
            bool
            writeQCompressedTexture(render::TextureFormat   textureFormat,
                                    AbstractTexturePtr      abstractTexture,
                                    const std::string&      textureType,
                                    WriterOptionsPtr        writerOptions,
                                    std::stringstream&      blob);

            static
            bool
            writeCRNCompressedTexture(render::TextureFormat   textureFormat,
                                      AbstractTexturePtr      abstractTexture,
                                      const std::string&      textureType,
                                      WriterOptionsPtr        writerOptions,
                                      std::stringstream&      blob);
        };
    }
}
