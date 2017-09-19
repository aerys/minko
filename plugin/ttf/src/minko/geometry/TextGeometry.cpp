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

#ifdef MINKO_PLUGIN_TTF_FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/geometry/TextGeometry.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/RectangleTexture.hpp"
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
    render::AbstractTexture::Ptr    atlas;
    std::vector<FontCharacter>      characters;
    math::uvec2                     atlasSize;
    math::uvec2                     atlasCharOffset;
};

static
const std::vector<char>&
getCharacterSet();

#ifdef MINKO_PLUGIN_TTF_FREETYPE
static
bool
loadFont(render::AbstractContext::Ptr context, const std::string& fontFilename, Font& font);

static
bool
buildAtlas(Font& font, render::AbstractContext::Ptr context, const std::string& fontFilename, float scale, unsigned int fontCharacterStride);

static
bool
getCharacterBox(const Font&         font,
                const std::string&  text,
                float               scale,
                math::ivec2&        boxMin,
                math::ivec2&        boxMax,
                int                 characterIndex = -1);
#endif

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

#ifdef MINKO_PLUGIN_TTF_FREETYPE
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

        buildAtlas(font, _context, fontFilename, scale, 16u);
        font.atlas->upload();
    }

	_atlasTexture = font.atlas;
    _textSize = getTextSize(font, text, scale);

    buildGeometry(_context, shared_from_this(), text, scale, font, centerOrigin);

    return std::static_pointer_cast<TextGeometry>(shared_from_this());
}
#endif

TextGeometry::Ptr
TextGeometry::setText(std::shared_ptr<render::AbstractTexture>    atlasTexture,
                      const std::string&                          text,
                      float                                       scale,
                      bool                                        centerOrigin,
                      int                                         stride)
{
    if (!atlasTexture)
        return std::static_pointer_cast<TextGeometry>(shared_from_this());

    auto font = Font();

    for (auto c : getCharacterSet())
        font.characters.emplace_back(FontCharacter{c});

    font.atlasSize = math::uvec2(atlasTexture->originalWidth(), atlasTexture->originalHeight());
    font.atlasCharOffset = math::uvec2(font.atlasSize) / static_cast<unsigned int>(stride);
    font.atlas = atlasTexture;

    _atlasTexture = font.atlas;
    _textSize = getTextSize(font, text, scale);

    buildGeometry(_context, shared_from_this(), text, scale, font, centerOrigin);

    return std::static_pointer_cast<TextGeometry>(shared_from_this());
}

#ifdef MINKO_PLUGIN_TTF_FREETYPE
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

        // See https://learnopengl.com/code_viewer.php?code=in-practice/text_rendering
        const auto advance = static_cast<unsigned int>(face->glyph->advance.x) >> 6;
        const auto bearing = math::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);

        font.characters.emplace_back(FontCharacter{
            c,
            textureData,
            math::ivec2(w, h),
            bearing,
            advance
        });
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return true;
}

bool
buildAtlas(Font& font, render::AbstractContext::Ptr context, const std::string& fontFilename, float scale, unsigned int fontCharacterStride)
{
    const auto& fontCharacters = font.characters;
    const auto referenceChar = 'A';

    auto fontCharacterMaxSize = math::ivec2(std::numeric_limits<int>::min());
    auto yBounds = math::ivec2(std::numeric_limits<int>::max(), std::numeric_limits<int>::min());
    auto referenceCharBoxMin = math::ivec2();
    auto referenceCharBoxMax = math::ivec2();

    for (auto i = 0; i < fontCharacters.size(); ++i)
    {
        auto fontCharacterBoxMin = math::ivec2();
        auto fontCharacterBoxMax = math::ivec2();
        if (!getCharacterBox(font, std::string{ fontCharacters.at(i).c }, 1.f, fontCharacterBoxMin, fontCharacterBoxMax))
            continue;

        if (fontCharacters.at(i).c == referenceChar)
        {
            referenceCharBoxMin = fontCharacterBoxMin;
            referenceCharBoxMax = fontCharacterBoxMax;
        }

        const auto fontCharacterSize = fontCharacterBoxMax - fontCharacterBoxMin;

        fontCharacterMaxSize.x = math::max(fontCharacterMaxSize.x, fontCharacterSize.x);
        fontCharacterMaxSize.y = math::max(fontCharacterMaxSize.y, fontCharacterSize.y);

        yBounds.x = math::min(yBounds.x, fontCharacterBoxMin.y);
        yBounds.y = math::max(yBounds.y, fontCharacterBoxMax.y);
    }

    // Characters are centered base on a reference 'A' character
    // They all share the same box and are offsetted according
    // to the reference char baseline
    const auto referenceCharOffset = -(referenceCharBoxMin.y + referenceCharBoxMax.y) / 2;

    font.atlasCharOffset = math::ivec2(fontCharacterMaxSize.x, (yBounds.y - yBounds.x) + referenceCharOffset);
    font.atlasSize = fontCharacterStride * font.atlasCharOffset;
    font.atlas = render::RectangleTexture::create(
        context,
        font.atlasSize.x, font.atlasSize.y,
        render::TextureFormat::RGBA,
        fontFilename
    );

    const auto atlasCharOffset = font.atlasCharOffset;
    const auto atlasSize = font.atlasSize;

    auto atlasTextureData = std::vector<unsigned char>(atlasSize.x * atlasSize.y * 4);

    for (auto i = 0; i < fontCharacters.size(); ++i)
    {
        const auto& fontCharacter = fontCharacters.at(i);
        const auto w = fontCharacter.size.x;
        const auto h = fontCharacter.size.y;

        auto fontCharacterBoxMin = math::ivec2();
        auto fontCharacterBoxMax = math::ivec2();
        if (!getCharacterBox(font, std::string{ fontCharacter.c }, 1.f, fontCharacterBoxMin, fontCharacterBoxMax))
            continue;
        const auto fontCharacterSize = fontCharacterBoxMax - fontCharacterBoxMin;
        assert(fontCharacterSize == math::ivec2(w, h) && "Inconsistent computed character box");

        const auto offset = i * atlasCharOffset.x * atlasCharOffset.y * 4;

        auto fontCharacterData = std::vector<unsigned char>(w * h * 4);

        for (auto y = 0; y < h; ++y)
        {
            for (auto x = 0; x < w; ++x)
            {
                const auto dstOffset = (y * w + x) * 4;
                const auto srcOffset = y * w + x;
                const auto srcValue = fontCharacter.textureData.at(srcOffset);

                fontCharacterData[dstOffset]     = srcValue;
                fontCharacterData[dstOffset + 1] = srcValue;
                fontCharacterData[dstOffset + 2] = srcValue;
                fontCharacterData[dstOffset + 3] = srcValue;
            }
        }

        const auto uDivisions = atlasSize.x / atlasCharOffset.x;

        for (auto y = 0; y < fontCharacter.size.y; ++y)
        {
            const auto dstOffsetY = atlasCharOffset.y - referenceCharOffset + fontCharacterBoxMin.y + y + (i / uDivisions) * atlasCharOffset.y;
            const auto dstOffsetX = fontCharacterBoxMin.x + (i % uDivisions) * atlasCharOffset.x;

            const auto dstOffset = (dstOffsetY * atlasSize.x + dstOffsetX) * 4;
            const auto srcOffset = (y * fontCharacter.size.x) * 4;
            const auto srcLength = fontCharacter.size.x * 4;

            std::copy(
                fontCharacterData.begin() + srcOffset,
                fontCharacterData.begin() + srcOffset + srcLength,
                atlasTextureData.begin() + dstOffset
            );
        }
    }

    auto texture = std::dynamic_pointer_cast<render::RectangleTexture>(font.atlas);

    if (!texture)
    {
        LOG_ERROR("Atlas texture must be an instance of RectangleTexture");
        return false;
    }

    texture->data(atlasTextureData.data(), font.atlasSize.x, font.atlasSize.y);

    return true;
}

bool
getCharacterBox(const Font&         font,
                const std::string&  text,
                float               scale,
                math::ivec2&        boxMin,
                math::ivec2&        boxMax,
                int                 characterIndex)
{
    boxMin = math::vec2(0.f, -std::numeric_limits<float>::max());

    if (characterIndex < 0)
        characterIndex = text.size();

    if (characterIndex > text.size())
    {
        LOG_WARNING("Character index is out of bounds");
        return false;
    }

    for (auto i = 0; i < characterIndex; ++i)
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

        if (i == 0 && characterIndex > 1)
            boxMin.x += (fontCharacter.advance - fontCharacter.bearing.x) * scale;
        else if (i < characterIndex - 1)
            boxMin.x += fontCharacter.advance * scale;
        else
        {
            boxMin.x += fontCharacter.bearing.x * scale;
            boxMax.x = boxMin.x + fontCharacter.size.x * scale;

            boxMin.y = -fontCharacter.bearing.y * scale;
            boxMax.y = boxMin.y + fontCharacter.size.y * scale;
        }
    }

    return true;
}
#endif

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

math::vec2
getTextSize(const Font& font, const std::string& text, float scale)
{
    return math::vec2(
        font.atlasCharOffset.x * static_cast<float>(text.size()),
        font.atlasCharOffset.y
    ) * scale;
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
		? -math::vec2(textSize.x, textSize.y) / 2.f
		: math::vec2();

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

        const auto uDivisions = atlasSize.x / atlasCharOffset.x;
        const auto vDivisions = atlasSize.y / atlasCharOffset.y;

        const auto uvOffset = math::vec2(
            (indexOfCharacter % uDivisions) / static_cast<float>(uDivisions),
            (indexOfCharacter / uDivisions) / static_cast<float>(vDivisions)
        );
        const auto nextUvOffset = uvOffset + math::vec2(
            1.f / static_cast<float>(uDivisions),
            1.f / static_cast<float>(vDivisions)
        );

        const auto min = positionOffset;
        const auto max = min + math::vec2(atlasCharOffset.x, atlasCharOffset.y) * scale;
        positionOffset += math::vec2(atlasCharOffset.x, 0.f) * scale;

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
