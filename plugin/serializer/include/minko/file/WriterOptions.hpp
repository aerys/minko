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
#include "minko/Flyweight.hpp"
#include "minko/SerializerCommon.hpp"
#include "minko/Types.hpp"
#include "minko/render/TextureFormat.hpp"
#include "minko/render/TextureFormatInfo.hpp"

namespace minko
{
    namespace file
    {
        class WriterOptions:
            public std::enable_shared_from_this<WriterOptions>
        {
        public:
            typedef std::shared_ptr<WriterOptions>                              Ptr;

            struct EmbedMode
            {
                static const unsigned int None;
                static const unsigned int Geometry;
                static const unsigned int Material;
                static const unsigned int Texture;
                static const unsigned int All;
            };

        private:
            typedef std::function<const std::string(const std::string&)>        NameFunction;
            typedef std::function<const std::string(const std::string&)>        UriFunction;

            typedef std::function<
                std::shared_ptr<geometry::Geometry>(
                    const std::string&,
                    std::shared_ptr<geometry::Geometry>
                )
            > GeometryFunction;

            typedef std::function<
                std::shared_ptr<material::Material>(
                    const std::string&,
                    std::shared_ptr<material::Material>
                )
            > MaterialFunction;

            typedef std::function<
                std::shared_ptr<render::AbstractTexture>(
                    const std::string&,
                    std::shared_ptr<render::AbstractTexture>
                )
            > TextureFunction;

            struct TextureOptions
            {
                serialize::ImageFormat  imageFormat;
                float                   jpegImageQualityFactor;
                bool                    compressTexture;
                float                   compressedTextureQualityFactor;
                bool                    preserveMipMaps;
                bool                    generateMipMaps;
                bool                    useTextureSRGBSpace;
                bool                    upscaleTextureWhenProcessedForMipMapping;
                math::vec2              textureScale;
                math::ivec2             textureMaxSize;
                render::TextureFilter   textureFilter;
                render::MipFilter       mipFilter;
            };

        private:
            bool                                _addBoundingBoxes;

            unsigned int                        _embedMode;

            NameFunction                        _geometryNameFunction;
            NameFunction                        _materialNameFunction;
            NameFunction                        _textureNameFunction;

            UriFunction                         _geometryUriFunction;
            UriFunction                         _materialUriFunction;
            UriFunction                         _textureUriFunction;

            GeometryFunction                    _geometryFunction;
            MaterialFunction                    _materialFunction;
            TextureFunction                     _textureFunction;

            std::vector<render::TextureFormat>  _textureFormats;

            std::vector<std::string>            _compressedTextureExceptions;

            std::unordered_map<
                Flyweight<std::string>,
                TextureOptions
            >                                   _textureOptions;

            bool                                _writeAnimations;

            std::set<std::string>               _nullAssetUuids;

        public:
            inline
            static
			Ptr
			create()
			{
                auto writerOptions = Ptr(new WriterOptions());

                return writerOptions;
			}

            inline
            static
            Ptr
            create(WriterOptions::Ptr other)
            {
                auto instance = WriterOptions::create();

                instance->_addBoundingBoxes = other->_addBoundingBoxes;
                instance->_embedMode = other->_embedMode;
                instance->_geometryNameFunction = other->_geometryNameFunction;
                instance->_materialNameFunction = other->_materialNameFunction;
                instance->_textureNameFunction = other->_textureNameFunction;
                instance->_geometryUriFunction = other->_geometryUriFunction;
                instance->_materialUriFunction = other->_materialUriFunction;
                instance->_textureUriFunction = other->_textureUriFunction;
                instance->_geometryFunction = other->_geometryFunction;
                instance->_materialFunction = other->_materialFunction;
                instance->_textureFunction = other->_textureFunction;
                instance->_textureFormats = other->_textureFormats;
                instance->_compressedTextureExceptions = other->_compressedTextureExceptions;
                instance->_textureOptions = other->_textureOptions;
                instance->_writeAnimations = other->_writeAnimations;
                instance->_nullAssetUuids = other->_nullAssetUuids;

                return instance;
            }

            inline
			bool
			addBoundingBoxes() const
			{
				return _addBoundingBoxes;
			}

			inline
			Ptr
			addBoundingBoxes(bool value)
			{
				_addBoundingBoxes = value;

				return shared_from_this();
			}

            inline
			unsigned int
			embedMode() const
			{
				return _embedMode;
			}

			inline
			Ptr
			embedMode(unsigned int value)
			{
				_embedMode = value;

				return shared_from_this();
			}

            inline
            const NameFunction&
            geometryNameFunction() const
            {
                return _geometryNameFunction;
            }

            inline
            Ptr
            geometryNameFunction(const NameFunction& func)
            {
                _geometryNameFunction = func;

                return shared_from_this();
            }

            inline
            const NameFunction&
            materialNameFunction() const
            {
                return _materialNameFunction;
            }

            inline
            Ptr
            materialNameFunction(const NameFunction& func)
            {
                _materialNameFunction = func;

                return shared_from_this();
            }

            inline
            const NameFunction&
            textureNameFunction() const
            {
                return _textureNameFunction;
            }

            inline
            Ptr
            textureNameFunction(const NameFunction& func)
            {
                _textureNameFunction = func;

                return shared_from_this();
            }

            inline
            const UriFunction&
            geometryUriFunction() const
            {
                return _geometryUriFunction;
            }

            inline
            Ptr
            geometryUriFunction(const UriFunction& func)
            {
                _geometryUriFunction = func;

                return shared_from_this();
            }

            inline
            const UriFunction&
            materialUriFunction() const
            {
                return _materialUriFunction;
            }

            inline
            Ptr
            materialUriFunction(const UriFunction& func)
            {
                _materialUriFunction = func;

                return shared_from_this();
            }

            inline
            const UriFunction&
            textureUriFunction() const
            {
                return _textureUriFunction;
            }

            inline
            Ptr
            textureUriFunction(const UriFunction& func)
            {
                _textureUriFunction = func;

                return shared_from_this();
            }

            inline
            const GeometryFunction&
            geometryFunction() const
            {
                return _geometryFunction;
            }

            inline
            Ptr
            geometryFunction(const GeometryFunction& func)
            {
                _geometryFunction = func;

                return shared_from_this();
            }

            inline
            const MaterialFunction&
            materialFunction() const
            {
                return _materialFunction;
            }

            inline
            Ptr
            materialFunction(const MaterialFunction& func)
            {
                _materialFunction = func;

                return shared_from_this();
            }

            inline
            const TextureFunction&
            textureFunction() const
            {
                return _textureFunction;
            }

            inline
            Ptr
            textureFunction(const TextureFunction& func)
            {
                _textureFunction = func;

                return shared_from_this();
            }

            inline
            std::vector<render::TextureFormat>
            textureFormats(const std::string& textureType, const std::string& filename)
            {
                const auto compressTexture =
                    this->compressTexture(textureType) &&
                    this->compressTextureByFilename(filename);

                if (compressTexture)
                    return _textureFormats;

                auto textureFormats = std::vector<render::TextureFormat>();

                for (const auto textureFormat : _textureFormats)
                    if (!render::TextureFormatInfo::isCompressed(textureFormat))
                        textureFormats.push_back(textureFormat);

                if (textureFormats.empty())
                    textureFormats.push_back(render::TextureFormat::RGBA);

                return textureFormats;
            }

            inline
            const std::vector<render::TextureFormat>&
            textureFormats() const
            {
                return _textureFormats;
            }

            inline
            Ptr
            registerTextureFormat(render::TextureFormat textureFormat)
            {
                _textureFormats.push_back(textureFormat);

                return shared_from_this();
            }

            inline
            bool
            compressTextureByFilename(const std::string& filename)
            {
                return std::find(
                    _compressedTextureExceptions.begin(),
                    _compressedTextureExceptions.end(),
                    filename
                ) == _compressedTextureExceptions.end();
            }

            inline
            Ptr
            registerCompressedTextureException(const std::string& filename)
            {
                _compressedTextureExceptions.push_back(filename);

                return shared_from_this();
            }

            inline
            serialize::ImageFormat
            imageFormat(const std::string& textureType) const
            {
                return textureOptions(textureType).imageFormat;
            }

            inline
            Ptr
            imageFormat(const std::string& textureType, serialize::ImageFormat value)
            {
                if (textureType.empty())
                {
                    for (auto& textureOption : _textureOptions)
                        textureOption.second.imageFormat = value;
                }
                else
                {
                    _textureOptions.at(textureType).imageFormat = value;
                }

                return shared_from_this();
            }

            inline
            float
            jpegImageQualityFactor(const std::string& textureType) const
            {
                return textureOptions(textureType).jpegImageQualityFactor;
            }

            inline
            Ptr
            jpegImageQualityFactor(const std::string& textureType, float value)
            {
                if (textureType.empty())
                {
                    for (auto& textureOption : _textureOptions)
                        textureOption.second.jpegImageQualityFactor = math::clamp(value, 0.f, 1.f);
                }
                else
                {
                    _textureOptions.at(textureType).jpegImageQualityFactor = math::clamp(value, 0.f, 1.f);
                }

                return shared_from_this();
            }

            inline
            float
            compressedTextureQualityFactor(const std::string& textureType) const
            {
                return textureOptions(textureType).compressedTextureQualityFactor;
            }

            inline
            Ptr
            compressedTextureQualityFactor(const std::string& textureType, float value)
            {
                if (textureType.empty())
                {
                    for (auto& textureOption : _textureOptions)
                        textureOption.second.compressedTextureQualityFactor = math::clamp(value, 0.f, 1.f);
                }
                else
                {
                    _textureOptions.at(textureType).compressedTextureQualityFactor = math::clamp(value, 0.f, 1.f);
                }

                return shared_from_this();
            }

            inline
            bool
            compressTexture(const std::string& textureType) const
            {
                return textureOptions(textureType).compressTexture;
            }

            inline
            Ptr
            compressTexture(const std::string& textureType, bool value)
            {
                if (textureType.empty())
                {
                    for (auto& textureOption : _textureOptions)
                        textureOption.second.compressTexture = value;
                }
                else
                {
                    _textureOptions.at(textureType).compressTexture = value;
                }

                return shared_from_this();
            }

            inline
            bool
            preserveMipMaps(const std::string& textureType) const
            {
                return textureOptions(textureType).preserveMipMaps;
            }

            inline
            Ptr
            preserveMipMaps(const std::string& textureType, bool value)
            {
                if (textureType.empty())
                {
                    for (auto& textureOption : _textureOptions)
                        textureOption.second.preserveMipMaps = value;
                }
                else
                {
                    _textureOptions.at(textureType).preserveMipMaps = value;
                }

                return shared_from_this();
            }

            inline
            bool
            generateMipMaps(const std::string& textureType) const
            {
                return textureOptions(textureType).generateMipMaps;
            }

            inline
            Ptr
            generateMipMaps(const std::string& textureType, bool value)
            {
                if (textureType.empty())
                {
                    for (auto& textureOption : _textureOptions)
                        textureOption.second.generateMipMaps = value;
                }
                else
                {
                    _textureOptions.at(textureType).generateMipMaps = value;
                }

                return shared_from_this();
            }

            inline
            bool
            useTextureSRGBSpace(const std::string& textureType) const
            {
                return textureOptions(textureType).useTextureSRGBSpace;
            }

            inline
            Ptr
            useTextureSRGBSpace(const std::string& textureType, bool value)
            {
                if (textureType.empty())
                {
                    for (auto& textureOption : _textureOptions)
                        textureOption.second.useTextureSRGBSpace = value;
                }
                else
                {
                    _textureOptions.at(textureType).useTextureSRGBSpace = value;
                }

                return shared_from_this();
            }

            inline
            bool
            upscaleTextureWhenProcessedForMipMapping(const std::string& textureType) const
            {
                return textureOptions(textureType).upscaleTextureWhenProcessedForMipMapping;
            }

            inline
            Ptr
            upscaleTextureWhenProcessedForMipMapping(const std::string& textureType, bool value)
            {
                if (textureType.empty())
                {
                    for (auto& textureOption : _textureOptions)
                        textureOption.second.upscaleTextureWhenProcessedForMipMapping = value;
                }
                else
                {
                    _textureOptions.at(textureType).upscaleTextureWhenProcessedForMipMapping = value;
                }

                return shared_from_this();
            }

            inline
            const math::ivec2&
            textureMaxSize(const std::string& textureType) const
            {
                return textureOptions(textureType).textureMaxSize;
            }

            inline
            Ptr
            textureMaxSize(const std::string& textureType, const math::ivec2& value)
            {
                if (textureType.empty())
                {
                    for (auto& textureOption : _textureOptions)
                        textureOption.second.textureMaxSize = value;
                }
                else
                {
                    _textureOptions.at(textureType).textureMaxSize = value;
                }

                return shared_from_this();
            }

            inline
            const math::vec2&
            textureScale(const std::string& textureType) const
            {
                return textureOptions(textureType).textureScale;
            }

            inline
            Ptr
            textureScale(const std::string& textureType, const math::vec2& value)
            {
                if (textureType.empty())
                {
                    for (auto& textureOption : _textureOptions)
                        textureOption.second.textureScale = value;
                }
                else
                {
                    _textureOptions.at(textureType).textureScale = value;
                }

                return shared_from_this();
            }

            inline
            render::TextureFilter
            textureFilter(const std::string& textureType) const
            {
                return textureOptions(textureType).textureFilter;
            }

            inline
            Ptr
            textureFilter(const std::string& textureType, render::TextureFilter value)
            {
                if (textureType.empty())
                {
                    for (auto& textureOption : _textureOptions)
                        textureOption.second.textureFilter = value;
                }
                else
                {
                    _textureOptions.at(textureType).textureFilter = value;
                }

                return shared_from_this();
            }

            inline
            render::MipFilter
            mipFilter(const std::string& textureType) const
            {
                return textureOptions(textureType).mipFilter;
            }

            inline
            Ptr
            mipFilter(const std::string& textureType, render::MipFilter value)
            {
                if (textureType.empty())
                {
                    for (auto& textureOption : _textureOptions)
                        textureOption.second.mipFilter = value;
                }
                else
                {
                    _textureOptions.at(textureType).mipFilter = value;
                }

                return shared_from_this();
            }

            inline
            bool
            writeAnimations() const
            {
                return _writeAnimations;
            }

            inline
            Ptr
            writeAnimations(bool value)
            {
                _writeAnimations = value;

                return shared_from_this();
            }

            inline
            std::set<std::string>&
            nullAssetUuids()
            {
                return _nullAssetUuids;
            }

            inline
            bool
            assetIsNull(const std::string& uuid) const
            {
                return _nullAssetUuids.find(uuid) != _nullAssetUuids.end();
            }

        private:
            WriterOptions();

            inline
            TextureOptions&
            textureOptions(const std::string& textureType)
            {
                auto textureOptionsIt = _textureOptions.find(textureType);

                if (textureOptionsIt != _textureOptions.end())
                    return textureOptionsIt->second;

                return _textureOptions.at("");
            }

            inline
            const TextureOptions&
            textureOptions(const std::string& textureType) const
            {
                auto textureOptionsIt = _textureOptions.find(textureType);

                if (textureOptionsIt != _textureOptions.end())
                    return textureOptionsIt->second;

                return _textureOptions.at("");
            }
        };
    }
}
