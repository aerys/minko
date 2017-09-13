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

namespace minko
{
	namespace serialize
	{
		enum ComponentId
		{
            IMAGE_BASED_LIGHT           = 30,
            ASCII_TEXT                  = 31,
            _COMPONENT_ID_RESERVED_1    = 32,
            _COMPONENT_ID_RESERVED_2    = 33,
            _COMPONENT_ID_RESERVED_3    = 34,
            _COMPONENT_ID_RESERVED_4    = 35,
            _COMPONENT_ID_RESERVED_5    = 36,
            _COMPONENT_ID_RESERVED_6    = 37,
            _COMPONENT_ID_RESERVED_7    = 38,
            _COMPONENT_ID_RESERVED_8    = 39,
            _COMPONENT_ID_RESERVED_9    = 40,
            _COMPONENT_ID_RESERVED_10   = 41,
            _COMPONENT_ID_RESERVED_11   = 42,
            _COMPONENT_ID_RESERVED_12   = 43,
            _COMPONENT_ID_RESERVED_13   = 44,
            _COMPONENT_ID_RESERVED_14   = 45,
            _COMPONENT_ID_RESERVED_15   = 46,
            _COMPONENT_ID_RESERVED_16   = 47,
            _COMPONENT_ID_RESERVED_17   = 48,
            _COMPONENT_ID_RESERVED_18   = 49,
			TRANSFORM			        = 100,
			PROJECTION_CAMERA	        = 101,
			AMBIENT_LIGHT		        = 102,
			DIRECTIONAL_LIGHT	        = 103,
			POINT_LIGHT			        = 104,
			SPOT_LIGHT			        = 105,
			SURFACE				        = 106,
			RENDERER			        = 107,
			BOUNDINGBOX			        = 108,
			ANIMATION			        = 109,
			SKINNING			        = 110,
            COLLIDER                    = 50,
            PARTICLES                   = 60,
			METADATA  				    = 70,
            MASTER_ANIMATION            = 90,
            CAMERA                      = 91,
            COMPONENT_ID_EXTENSION      = 111
		};

		enum MinkoTypes
		{
			MATRIX4X4		= 0,
			VECTOR4			= 3,
			VECTOR3			= 1,
			VECTOR2			= 2,
			INT				= 4,
			TEXTURE			= 5,
			FLOAT			= 6,
			BOOL			= 7,
			BLENDING		= 8,
			TRIANGLECULLING = 9,
			ENVMAPTYPE		= 10,
            STRING          = 11
		};

		enum AssetType
		{
			GEOMETRY_ASSET              = 0,
			EMBED_GEOMETRY_ASSET        = 10,
			MATERIAL_ASSET              = 1,
			EMBED_MATERIAL_ASSET        = 11,
			TEXTURE_ASSET               = 2,
            EMBED_TEXTURE_ASSET         = 120,
			EFFECT_ASSET                = 3,
			EMBED_EFFECT_ASSET          = 13,
            TEXTURE_PACK_ASSET          = 4,
            EMBED_TEXTURE_PACK_ASSET    = 14,
            LINKED_ASSET                = 15
		};

        enum class ImageFormat
        {
            SOURCE  = 1,
            PNG     = 2,
            JPEG    = 3,
            TGA     = 4
        };

        namespace
        {
            static const auto imageFormatToExtensionMap = std::map<ImageFormat, std::string>
            {
                { ImageFormat::PNG, "png" },
                { ImageFormat::JPEG, "jpg" },
                { ImageFormat::TGA, "tga" }
            };

            static const auto defaultImageFormat = ImageFormat::PNG;
        }

        inline
        std::string
        extensionFromImageFormat(ImageFormat format)
        {
            auto imageFormatToExtensionPairIt = imageFormatToExtensionMap.find(format);

            if (imageFormatToExtensionPairIt == imageFormatToExtensionMap.end())
                return imageFormatToExtensionMap.at(defaultImageFormat);

            return imageFormatToExtensionPairIt->second;
        }

        inline
        ImageFormat
        imageFormatFromExtension(const std::string& extension)
        {
            auto imageFormatToExtensionPairIt = std::find_if(
                imageFormatToExtensionMap.begin(),
                                              imageFormatToExtensionMap.end(),
            [=](const std::pair<ImageFormat, std::string>& imageFormatToExtensionPair)
            {
                return imageFormatToExtensionPair.second == extension;
            });

            if (imageFormatToExtensionPairIt == imageFormatToExtensionMap.end())
                return ImageFormat::SOURCE;

            return imageFormatToExtensionPairIt->first;
        }
	}
}
