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

#include "minko/component/JobManager.hpp"
#include "minko/file/AbstractStreamedAssetParser.hpp"

namespace minko
{
    namespace file
    {
        class StreamedTextureParser :
            public AbstractStreamedAssetParser
        {
        public:
            typedef std::shared_ptr<StreamedTextureParser> Ptr;

        private:
            struct DataChunk
            {
                const unsigned char*    data;
                unsigned int            offset;
                unsigned int            size;

                DataChunk(const unsigned char* data, unsigned int offset, unsigned int size) :
                    data(data),
                    offset(offset),
                    size(size)
                {
                }
            };

        private:
            std::shared_ptr<render::AbstractTexture>                _texture;

            render::TextureType                                     _textureType;
            render::TextureFormat                                   _textureFormat;
            int                                                     _textureWidth;
            int                                                     _textureHeight;
            int                                                     _textureNumFaces;
            int                                                     _textureNumMipmaps;
            std::vector<std::tuple<int, int>>                       _mipLevelsInfo;

        public:
            inline
            static
            Ptr
            create()
            {
                return Ptr(new StreamedTextureParser());
            }

        protected:
            bool
            useDescriptor(const std::string&                filename,
                          std::shared_ptr<Options>          options,
                          const std::vector<unsigned char>& data,
                          std::shared_ptr<AssetLibrary>     assetLibrary);

            void
            parsed(const std::string&                filename,
                   const std::string&                resolvedFilename,
                   std::shared_ptr<Options>          options,
                   const std::vector<unsigned char>& data,
                   std::shared_ptr<AssetLibrary>     assetLibrary);

            void
            headerParsed(const std::vector<unsigned char>&   data,
                         std::shared_ptr<Options>            options,
                         unsigned int&                       linkedAssetId);

            void
            lodParsed(int                                previousLod,
                      int                                currentLod,
                      const std::vector<unsigned char>&  data,
                      std::shared_ptr<Options>           options);

            bool
            complete(int currentLod);

            void
            completed();

            void
            lodRangeRequestByteRange(int lowerLod, int upperLod, int& offset, int& size) const override;

            int
            lodLowerBound(int lod) const override;

            int
            maxLod() const override;

        private:
            StreamedTextureParser();

            std::shared_ptr<render::AbstractTexture>
            createTexture(std::shared_ptr<AssetLibrary> assetLibrary,
                          const std::string&            filename,
                          render::TextureType           textureType);

            render::TextureFormat
            matchingTextureFormat(std::shared_ptr<Options>                  options,
                                  const std::list<render::TextureFormat>&   availableFormats);

            int
            lodToMipLevel(int lod) const;

            bool
            extractLodData(render::TextureFormat                format,
                           const std::string&                   filename,
                           std::shared_ptr<Options>             options,
                           std::shared_ptr<AssetLibrary>        assetLibrary,
                           const DataChunk&                     lodData,
                           std::vector<unsigned char>&          extractedLodData);

            int
            lodRangeRequestSize(int lowerLod, int upperLod) const;
        };
    }
}
