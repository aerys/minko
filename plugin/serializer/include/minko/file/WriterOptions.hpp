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
#include "minko/SerializerCommon.hpp"
#include "minko/Types.hpp"

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

        private:
            bool                                _addBoundingBoxes;

            unsigned int                        _embedMode;

            UriFunction                         _geometryUriFunction;
            UriFunction                         _materialUriFunction;
            UriFunction                         _textureUriFunction;

            GeometryFunction                    _geometryFunction;
            MaterialFunction                    _materialFunction;
            TextureFunction                     _textureFunction;

            serialize::ImageFormat              _imageFormat;
            std::list<render::TextureFormat>    _textureFormats;

            bool                                _compressTexture;
            bool                                _generateMipmaps;
            bool                                _upscaleTextureWhenProcessedForMipmapping;
            math::ivec2                         _textureMaxResolution;
            render::MipFilter                   _mipFilter;
            bool                                _optimizeForNormalMapping;

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
                instance->_geometryUriFunction = other->_geometryUriFunction;
                instance->_materialUriFunction = other->_materialUriFunction;
                instance->_textureUriFunction = other->_textureUriFunction;
                instance->_geometryFunction = other->_geometryFunction;
                instance->_materialFunction = other->_materialFunction;
                instance->_textureFunction = other->_textureFunction;
                instance->_imageFormat = other->_imageFormat;
                instance->_textureFormats = other->_textureFormats;
                instance->_compressTexture = other->_compressTexture;
                instance->_generateMipmaps = other->_generateMipmaps;
                instance->_upscaleTextureWhenProcessedForMipmapping = other->_upscaleTextureWhenProcessedForMipmapping;
                instance->_textureMaxResolution = other->_textureMaxResolution;
                instance->_mipFilter = other->_mipFilter;
                instance->_optimizeForNormalMapping = other->_optimizeForNormalMapping;
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
			serialize::ImageFormat
			imageFormat() const
			{
				return _imageFormat;
			}

			inline
			Ptr
			imageFormat(serialize::ImageFormat value)
			{
				_imageFormat = value;

				return shared_from_this();
			}

            inline
            const std::list<render::TextureFormat>&
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
            compressTexture() const
            {
                return _compressTexture;
            }

            inline
            Ptr
            compressTexture(bool value)
            {
                _compressTexture = value;

                return shared_from_this();
            }

            inline
            bool
            generateMipmaps() const
            {
                return _generateMipmaps;
            }

            inline
            Ptr
            generateMipmaps(bool value)
            {
                _generateMipmaps = value;

                return shared_from_this();
            }

            inline
            bool
            upscaleTextureWhenProcessedForMipmapping() const
            {
                return _upscaleTextureWhenProcessedForMipmapping;
            }

            inline
            Ptr
            upscaleTextureWhenProcessedForMipmapping(bool value)
            {
                _upscaleTextureWhenProcessedForMipmapping = value;

                return shared_from_this();
            }

            inline
            const math::ivec2&
            textureMaxResolution() const
            {
                return _textureMaxResolution;
            }

            inline
            Ptr
            textureMaxResolution(const math::ivec2& value)
            {
                _textureMaxResolution = value;

                return shared_from_this();
            }

            inline
            render::MipFilter
            mipFilter() const
            {
                return _mipFilter;
            }

            inline
            Ptr
            mipFilter(render::MipFilter value)
            {
                _mipFilter = value;

                return shared_from_this();
            }

            inline
            bool
            optimizeForNormalMapping() const
            {
                return _optimizeForNormalMapping;
            }

            inline
            Ptr
            optimizeForNormalMapping(bool value)
            {
                _optimizeForNormalMapping = value;

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
        };
    }
}
