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

#include <ft2build.h>
#include FT_FREETYPE_H

#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/geometry/TextGeometry.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/VertexBuffer.hpp"

using namespace minko;
using namespace minko::geometry;

struct FontCharacter
{
    char                        c;
    std::vector<unsigned char>  textureData;
    math::ivec2                 size;
    math::ivec2                 bearing;
    unsigned int                advance;
};

struct Font
{
    render::Texture::Ptr            atlas;
    std::vector<FontCharacter>      characters;
    math::uvec2                     atlasSize;
    math::uvec2                     atlasCharOffset;
};

static
const std::vector<char>&
getCharacterSet();

static
bool
loadFont(render::AbstractContext::Ptr context, const std::string& fontFilename, Font& font);

static
bool
buildAtlas(Font& font);

static
math::vec2
getTextSize(const Font& font, const std::string& text, float scale);

static
bool
buildGeometry(render::AbstractContext::Ptr  context,
              Geometry::Ptr                 geometry,
              const std::string&            text,
              float                         fontSize,
              const Font&                   font,
		      bool							centerOrigin);

TextGeometry::TextGeometry() :
    Geometry("text"),
    _context(),
    _atlasTexture(),
    _textSize(0.f)
{
}

TextGeometry::Ptr
TextGeometry::setText(const std::string& fontFilename, const std::string& text, float scale, bool centerOrigin)
{
    static auto fontCache = std::unordered_map<std::string, Font>();

    auto it = fontCache.emplace(fontFilename, Font());
    auto& font = it.first->second;

    if (it.second)
    {
        if (!loadFont(_context, fontFilename, it.first->second))
        {
            const auto error = std::string("Failed to load font: " + fontFilename);

            LOG_ERROR(error);

            throw std::runtime_error(error.c_str());
        }

        font.atlasSize = math::uvec2(1024);
        font.atlasCharOffset = math::uvec2(64);
        font.atlas = render::Texture::create(
            _context,
            font.atlasSize.x, font.atlasSize.y,
            false, false, false,
            render::TextureFormat::RGBA,
            fontFilename
        );

        buildAtlas(font);
        font.atlas->upload();
    }

	_atlasTexture = font.atlas;
    _textSize = getTextSize(font, text, scale);

    buildGeometry(_context, shared_from_this(), text, scale, font, centerOrigin);

    return std::static_pointer_cast<TextGeometry>(shared_from_this());
}

const std::vector<char>&
getCharacterSet()
{
    static auto characters = std::vector<char>();

    if (characters.empty())
    {
        characters.reserve(256);
        for (int i = std::numeric_limits<char>::min(); i <= std::numeric_limits<char>::max(); ++i)
            characters.push_back(static_cast<char>(i));
    }

    return characters;
}

bool
loadFont(render::AbstractContext::Ptr context, const std::string& fontFilename, Font& font)
{
    auto fontLoaded = false;
    auto loader = file::Loader::create();
    loader->options()->assetLibrary(file::AssetLibrary::create(context));
    loader->options()
        ->loadAsynchronously(false)
        ->storeDataIfNotParsed(true);

    auto errorSlot = loader->error()->connect([](file::Loader::Ptr loader, const file::Error& error)
        {
            LOG_ERROR(error.type() << ": " << error.what());
        }
    );

    auto completeSlot = loader->complete()->connect([&fontLoaded](file::Loader::Ptr loader)
        {
            fontLoaded = true;
        }
    );

    loader
        ->queue(fontFilename)
        ->load();

    if (!fontLoaded)
        return false;

    const auto& fontData = loader->options()->assetLibrary()->blob(fontFilename);

    static FT_Library ft = nullptr;

    if (FT_Init_FreeType(&ft))
        return false;

    FT_Face face;
    if (FT_New_Memory_Face(ft, fontData.data(), fontData.size(), 0, &face))
        return false;

    FT_Set_Pixel_Sizes(face, 0, 48);

    for (auto c : getCharacterSet())
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            continue;

        const auto w = face->glyph->bitmap.width;
        const auto h = face->glyph->bitmap.rows;
        const auto textureData = std::vector<unsigned char>(face->glyph->bitmap.buffer, face->glyph->bitmap.buffer + w * h);

        font.characters.emplace_back(FontCharacter{
            c,
            textureData,
            math::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            math::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        });
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return true;
}

bool
buildAtlas(Font& font)
{
    const auto atlasSize = font.atlasSize;
    const auto atlasCharOffset = font.atlasCharOffset;
    const auto& fontCharacters = font.characters;

    auto atlasTextureData = std::vector<unsigned char>(atlasSize.x * atlasSize.y * 4);

    for (auto i = 0; i < fontCharacters.size(); ++i)
    {
        const auto& fontCharacter = fontCharacters.at(i);
        const auto w = fontCharacter.size.x;
        const auto h = fontCharacter.size.y;

        const auto offset = i * atlasCharOffset.x * atlasCharOffset.y * 4;

        auto fontCharacterData = std::vector<unsigned char>(atlasCharOffset.x * atlasCharOffset.y * 4);

        for (auto y = 0; y < h; ++y)
        {
            for (auto x = 0; x < w; ++x)
            {
                const auto dstOffset = (y * atlasCharOffset.y + x) * 4;
                const auto srcOffset = y * w + x;
                const auto srcValue = fontCharacter.textureData.at(srcOffset);

                fontCharacterData[dstOffset]     = srcValue;
                fontCharacterData[dstOffset + 1] = srcValue;
                fontCharacterData[dstOffset + 2] = srcValue;
                fontCharacterData[dstOffset + 3] = srcValue;
            }
        }

        const auto uDivisions = atlasSize.x / atlasCharOffset.x;

        for (auto y = 0; y < atlasCharOffset.y; ++y)
        {
            const auto dstOffsetY = (y + (i / uDivisions) * atlasCharOffset.y);
            const auto dstOffsetX = (i % uDivisions) * atlasCharOffset.x;

            const auto dstOffset = (dstOffsetY * atlasSize.x + dstOffsetX) * 4;
            const auto srcOffset = (y * atlasCharOffset.x) * 4;
            const auto srcLength = atlasCharOffset.x * 4;

            std::copy(
                fontCharacterData.begin() + srcOffset,
                fontCharacterData.begin() + srcOffset + srcLength,
                atlasTextureData.begin() + dstOffset
            );
        }
    }

    font.atlas->data(atlasTextureData.data());

    return true;
}

math::vec2
getTextSize(const Font& font, const std::string& text, float scale)
{
    auto size = math::vec2(0.f, -std::numeric_limits<float>::max());

    for (auto i = 0; i < text.size(); ++i)
    {
        const auto c = text.at(i);

        const auto it = std::find_if(
            font.characters.begin(),
            font.characters.end(),
            [&c](const FontCharacter& fontCharacter) { return fontCharacter.c == c; });

        if (it == font.characters.end())
        {
            LOG_WARNING("Unhandled character " << c);
            continue;
        }

        const auto& fontCharacter = *it;

        const auto w = fontCharacter.size.x * scale;
        const auto h = fontCharacter.size.y * scale;
        const auto horizontalBearing = i == 0 ? 0.f : fontCharacter.bearing.x;
        const auto min = math::vec3(
            horizontalBearing * scale,
            -(fontCharacter.size.y - fontCharacter.bearing.y) * scale,
            0.f
        );
        const auto max = min + math::vec3(w, h, 0.f);

        if (i == 0)
            size.x += ((fontCharacter.advance >> 6) - fontCharacter.bearing.x) * scale;
        else if (i < text.size() - 1)
            size.x += (fontCharacter.advance >> 6) * scale;
        else
            size.x += (fontCharacter.bearing.x + fontCharacter.size.x) * scale;

        size.y = math::max(size.y, (max - min).y);
    }

    return size;
}

bool
buildGeometry(render::AbstractContext::Ptr          context,
              Geometry::Ptr                         geometry,
              const std::string&                    text,
              float                                 scale,
              const Font&                           font,
			  bool									centerOrigin)
{
    const auto atlasSize = font.atlasSize;
    const auto atlasCharOffset = font.atlasCharOffset;
    const auto& fontCharacters = font.characters;

    auto vertexData = std::vector<float>();
    auto indexData = std::vector<unsigned short>();

    const auto vertexSize = 5;

    auto indexOffset = 0;

    // Origin point is at text center
    const auto textSize = getTextSize(font, text, scale);
    auto positionOffset = centerOrigin
		? math::vec3(-math::vec2(textSize.x, textSize.y) / 2.f, 0.f)
		: math::vec3();

    for (auto i = 0; i < text.size(); ++i)
    {
        const auto c = text.at(i);

        const auto it = std::find_if(
            fontCharacters.begin(),
            fontCharacters.end(),
            [&c](const FontCharacter& fontCharacter) { return fontCharacter.c == c; });

        if (it == fontCharacters.end())
        {
            LOG_WARNING("Unhandled character " << c);
            continue;
        }

        const auto& fontCharacter = *it;
        const auto indexOfCharacter = static_cast<unsigned int>(it - fontCharacters.begin());

        const auto w = fontCharacter.size.x * scale;
        const auto h = fontCharacter.size.y * scale;

        const auto uDivisions = atlasSize.x / atlasCharOffset.x;
        const auto vDivisions = atlasSize.y / atlasCharOffset.y;

        const auto uvOffset = math::vec2(
            (indexOfCharacter % uDivisions) / static_cast<float>(uDivisions),
            (indexOfCharacter / uDivisions) / static_cast<float>(vDivisions)
        );
        const auto nextUvOffset = uvOffset + math::vec2(fontCharacter.size.x, fontCharacter.size.y) / math::vec2(atlasSize.x, atlasSize.y);

        // Ignore front character bearing
        // See https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
        const auto horizontalBearing = i == 0 ? 0.f : fontCharacter.bearing.x;
        const auto min = positionOffset + math::vec3(
            horizontalBearing * scale,
            -(fontCharacter.size.y - fontCharacter.bearing.y) * scale,
            0.f
        );
        const auto max = min + math::vec3(w, h, 0.f);

        // See https://learnopengl.com/code_viewer.php?code=in-practice/text_rendering.
        if (i == 0)
            positionOffset.x += ((fontCharacter.advance >> 6) - fontCharacter.bearing.x) * scale;
        else
            positionOffset.x += (fontCharacter.advance >> 6) * scale;

        vertexData.insert(vertexData.end(), {
            min.x, min.y, 0.f, uvOffset.x, nextUvOffset.y,
            min.x, max.y, 0.f, uvOffset.x, uvOffset.y,
            max.x, max.y, 0.f, nextUvOffset.x, uvOffset.y,
            max.x, min.y, 0.f, nextUvOffset.x, nextUvOffset.y
        });

        indexData.insert(indexData.end(), {
            static_cast<unsigned short>(indexOffset), static_cast<unsigned short>(indexOffset + 2), static_cast<unsigned short>(indexOffset + 1),
            static_cast<unsigned short>(indexOffset), static_cast<unsigned short>(indexOffset + 3), static_cast<unsigned short>(indexOffset + 2)
        });
        indexOffset += 4;
    }

    auto vb = render::VertexBuffer::create(context, vertexData);
    vb->addAttribute("position", 3, 0);
    vb->addAttribute("uv", 2, 3);

    auto ib = render::IndexBuffer::create(context, indexData);

    geometry->addVertexBuffer(vb);
    geometry->indices(ib);
    geometry->upload();

    return true;
}
