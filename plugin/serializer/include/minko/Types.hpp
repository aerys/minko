/*
Copyright (c) 2013 Aerys

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
			TRANSFORM			= 100,
			PROJECTION_CAMERA	= 101,
			AMBIENT_LIGHT		= 102,
			DIRECTIONAL_LIGHT	= 103,
			POINT_LIGHT			= 104,
			SPOT_LIGHT			= 105,
			SURFACE				= 106,
			RENDERER			= 107,
			BOUNDINGBOX			= 108,
			ANIMATION			= 109,
			SKINNING			= 110,
            PARTICLES           = 60
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
			ENVMAPTYPE		= 10
		};

		enum AssetType
		{
			GEOMETRY_ASSET              = 0,
			EMBED_GEOMETRY_ASSET        = 10,
			MATERIAL_ASSET              = 1,
			EMBED_MATERIAL_ASSET        = 11,
			TEXTURE_ASSET               = 2,
            EMBED_TEXTURE_ASSET         = 12,
			EFFECT_ASSET                = 3,
			EMBED_EFFECT_ASSET          = 13
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
            static const auto imageFormatToExtensionMap = std::unordered_map<ImageFormat, std::string>
            {
                { ImageFormat::PNG, "png" },
                { ImageFormat::JPEG, "jpg" },
                { ImageFormat::TGA, "tga" },
            };
        }

        inline
        std::string
        extensionFromImageFormat(ImageFormat format)
        {
            return imageFormatToExtensionMap.at(format);
        }

        inline
        ImageFormat
        imageFormatFromExtension(const std::string& extension)
        {
            auto imageFormatToExtensionPairIt = std::find_if(imageFormatToExtensionMap.begin(),
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
