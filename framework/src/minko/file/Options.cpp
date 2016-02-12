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

#include "minko/file/Options.hpp"

#include "minko/material/Material.hpp"
#include "minko/file/AbstractProtocol.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/log/Logger.hpp"

#ifdef __APPLE__
# include "CoreFoundation/CoreFoundation.h"
#endif

#if MINKO_PLATFORM & MINKO_PLATFORM_HTML5
# include "emscripten.h"
#endif

using namespace minko;
using namespace minko::file;

Options::ProtocolFunction Options::_defaultProtocolFunction = nullptr;
Options::MaterialPtr Options::_defaultMaterial = material::Material::create();

Options::Options() :
    _context(nullptr),
    _includePaths(),
    _platforms(),
    _userFlags(),
    _optimizeForRendering(true),
    _generateMipMaps(true),
    _parseMipMaps(false),
    _resizeSmoothly(false),
    _isCubeTexture(false),
    _isRectangleTexture(false),
    _generateSmoothNormals(false),
    _normalMaxSmoothingAngle(80.f),
    _includeAnimation(true),
    _startAnimation(true),
    _loadAsynchronously(false),
    _disposeIndexBufferAfterLoading(false),
    _disposeVertexBufferAfterLoading(false),
    _disposeTextureAfterLoading(false),
    _storeDataIfNotParsed(true),
    _preserveMaterials(true),
    _trackAssetDescriptor(false),
    _skinningFramerate(30),
    _skinningMethod(component::SkinningMethod::HARDWARE),
    _material(nullptr),
    _effect(nullptr),
    _seekingOffset(0),
    _seekedLength(0)
{
    auto binaryDir = File::getBinaryDirectory();

    includePaths().push_back(binaryDir + "/asset");

 #if defined(DEBUG) && !defined(EMSCRIPTEN)
     includePaths().push_back(binaryDir + "/../../../asset");
 #endif

    initializePlatforms();
    initializeUserFlags();
}

Options::Options(const Options& copy) :
    _context(copy._context),
    _assets(copy._assets),
    _includePaths(copy._includePaths),
    _platforms(copy._platforms),
    _userFlags(copy._userFlags),
    _optimizeForRendering(copy._optimizeForRendering),
    _parsers(copy._parsers),
    _protocols(copy._protocols),
    _generateMipMaps(copy._generateMipMaps),
    _parseMipMaps(copy._parseMipMaps),
    _resizeSmoothly(copy._resizeSmoothly),
    _isCubeTexture(copy._isCubeTexture),
	_isRectangleTexture(copy._isRectangleTexture),
	_generateSmoothNormals(copy._generateSmoothNormals),
	_normalMaxSmoothingAngle(copy._normalMaxSmoothingAngle),
    _includeAnimation(copy._includeAnimation),
    _startAnimation(copy._startAnimation),
    _disposeIndexBufferAfterLoading(copy._disposeIndexBufferAfterLoading),
    _disposeVertexBufferAfterLoading(copy._disposeVertexBufferAfterLoading),
    _disposeTextureAfterLoading(copy._disposeTextureAfterLoading),
    _storeDataIfNotParsed(copy._storeDataIfNotParsed),
    _preserveMaterials(copy._preserveMaterials),
    _trackAssetDescriptor(copy._trackAssetDescriptor),
    _skinningFramerate(copy._skinningFramerate),
    _skinningMethod(copy._skinningMethod),
    _effect(copy._effect),
    _textureFormats(copy._textureFormats),
    _material(copy._material),
    _materialFunction(copy._materialFunction),
    _textureFunction(copy._textureFunction),
    _geometryFunction(copy._geometryFunction),
    _protocolFunction(copy._protocolFunction),
    _parserFunction(copy._parserFunction),
    _uriFunction(copy._uriFunction),
    _nodeFunction(copy._nodeFunction),
    _effectFunction(copy._effectFunction),
    _textureFormatFunction(copy._textureFormatFunction),
    _attributeFunction(copy._attributeFunction),
    _fileStatusFunction(copy._fileStatusFunction),
    _loadAsynchronously(copy._loadAsynchronously),
    _seekingOffset(copy._seekingOffset),
    _seekedLength(copy._seekedLength)
{
}

Options::Ptr
Options::clone()
{
    auto copy = Ptr(new Options(*this));

    copy->initialize();

    return copy;
}

void
Options::initialize()
{
    resetNotInheritedValues();
    initializeDefaultFunctions();

    if (_parsers.find("effect") == _parsers.end())
        registerParser<file::EffectParser>("effect");

    if (_protocols.find("file") == _protocols.end())
        registerProtocol<FileProtocol>("file");
}

void
Options::initializePlatforms()
{
#if MINKO_PLATFORM & MINKO_PLATFORM_WINDOWS
    _platforms.push_back("windows");
#elif MINKO_PLATFORM & MINKO_PLATFORM_OSX
    _platforms.push_back("osx");
#elif MINKO_PLATFORM & MINKO_PLATFORM_LINUX
    _platforms.push_back("linux");
#elif MINKO_PLATFORM & MINKO_PLATFORM_IOS
    _platforms.push_back("ios");
#elif MINKO_PLATFORM & MINKO_PLATFORM_ANDROID
    _platforms.push_back("android");
#elif MINKO_PLATFORM & MINKO_PLATFORM_HTML5
    _platforms.push_back("html5");
    if (testUserAgentPlatform("Windows"))
        _platforms.push_back("windows");
    if (testUserAgentPlatform("Macintosh"))
    {
        _platforms.push_back("osx");
        if (testUserAgentPlatform("Safari"))
            _platforms.push_back("safari");
    }
    if (testUserAgentPlatform("Linux"))
        _platforms.push_back("linux");
    if (testUserAgentPlatform("iPad"))
        _platforms.push_back("ios");
    if (testUserAgentPlatform("iPhone"))
        _platforms.push_back("ios");
    if (testUserAgentPlatform("iPod"))
        _platforms.push_back("ios");
    if (testUserAgentPlatform("Android"))
        _platforms.push_back("android");
    if (testUserAgentPlatform("Firefox"))
        _platforms.push_back("firefox");
    if (testUserAgentPlatform("Chrome"))
        _platforms.push_back("chrome");
    if (testUserAgentPlatform("Opera"))
        _platforms.push_back("opera");
    if (testUserAgentPlatform("MSIE") || testUserAgentPlatform("Trident"))
        _platforms.push_back("msie");
#endif
}

void
Options::initializeUserFlags()
{
}

AbstractParser::Ptr
Options::getParser(const std::string& extension)
{
    if (_parserFunction)
        return _parserFunction(extension);

    return _parsers.count(extension) == 0 ? nullptr : _parsers[extension]();
}

Options::AbsProtocolPtr
Options::getProtocol(const std::string& protocol)
{
    auto p = _protocols.count(protocol) == 0 ? nullptr : _protocols[protocol]();

    if (p)
        p->options(p->options()->clone());

    return p;
}

void
Options::defaultProtocolFunction(const std::string& filename, const ProtocolFunction& func)
{
    _defaultProtocolFunction = func;
}

void
Options::initializeDefaultFunctions()
{
    auto options = this;

    if (!_materialFunction)
        _materialFunction = [](const std::string&, material::Material::Ptr material) -> material::Material::Ptr
        {
            return material;
        };

    if (!_textureFunction)
        _textureFunction = [](const std::string&, AbstractTexturePtr texture) -> AbstractTexturePtr
        {
            return texture;
        };

    if (!_geometryFunction)
        _geometryFunction = [](const std::string&, GeomPtr geom) -> GeomPtr
        {
            return geom;
        };

    if (!_uriFunction)
        _uriFunction = [](const std::string& uri) -> const std::string
        {
            return uri;
        };

    if (!_nodeFunction)
        _nodeFunction = [](NodePtr node) -> NodePtr
        {
            return node;
        };

    if (!_effectFunction)
        _effectFunction = [](EffectPtr effect) -> EffectPtr
        {
            return effect;
        };

    _textureFormatFunction = [=](const TextureFormatSet& availableTextureFormats) ->render::TextureFormat
    {
        static const auto defaultTextureFormats = std::list<render::TextureFormat>
        {
            render::TextureFormat::RGBA_PVRTC2_2BPP,
            render::TextureFormat::RGBA_PVRTC2_4BPP,

            render::TextureFormat::RGBA_PVRTC1_2BPP,
            render::TextureFormat::RGBA_PVRTC1_4BPP,

            render::TextureFormat::RGB_PVRTC1_2BPP,
            render::TextureFormat::RGB_PVRTC1_4BPP,

            render::TextureFormat::RGBA_DXT5,
            render::TextureFormat::RGBA_DXT3,

            render::TextureFormat::RGBA_ATITC,
            render::TextureFormat::RGB_ATITC,

            render::TextureFormat::RGBA_ETC1,
            render::TextureFormat::RGB_ETC1,

            render::TextureFormat::RGBA_DXT1,
            render::TextureFormat::RGB_DXT1,

            render::TextureFormat::RGBA,
            render::TextureFormat::RGB
        };

        auto& textureFormats = options->_textureFormats.empty() ? defaultTextureFormats : options->_textureFormats;

        auto textureFormatIt = std::find_if(textureFormats.begin(), textureFormats.end(),
                            [&](render::TextureFormat textureFormat) -> bool
        {
            return availableTextureFormats.find(textureFormat) != availableTextureFormats.end();
        });

        if (textureFormatIt != textureFormats.end())
            return *textureFormatIt;

        if (std::find(textureFormats.begin(),
                      textureFormats.end(),
                      render::TextureFormat::RGB) != textureFormats.end() &&
            availableTextureFormats.find(render::TextureFormat::RGBA) != availableTextureFormats.end())
            return render::TextureFormat::RGBA;

        if (std::find(textureFormats.begin(),
                      textureFormats.end(),
                      render::TextureFormat::RGBA) != textureFormats.end() &&
            availableTextureFormats.find(render::TextureFormat::RGB) != availableTextureFormats.end())
            return render::TextureFormat::RGB;

        static const auto errorMessage = "No desired texture format available";

        LOG_ERROR(errorMessage);

        throw std::runtime_error(errorMessage);
    };

    if (_material == nullptr)
        _material = _defaultMaterial;

    if (!_attributeFunction)
        _attributeFunction = [](NodePtr node, const std::string& key, const std::string& value) -> void
        {
        };

    if (!_defaultProtocolFunction)
        _defaultProtocolFunction = [=](const std::string& filename) -> std::shared_ptr<AbstractProtocol>
        {
            auto defaultProtocol = options->getProtocol("file"); // "file" might be overriden (by APKProtocol for instance)

            defaultProtocol->options(options->clone());

            return defaultProtocol;
        };

    _protocolFunction = [=](const std::string& filename) -> std::shared_ptr<AbstractProtocol>
    {
        std::string protocol = "";

        uint i;

        for (i = 0; i < filename.length(); ++i)
        {
            if (i < filename.length() - 2 && filename.at(i) == ':' && filename.at(i + 1) == '/' && filename.at(i + 2) == '/')
                break;

            protocol += filename.at(i);
        }

        if (i != filename.length())
        {
            auto loader = options->getProtocol(protocol);

            if (loader)
                return loader;
        }

        return _defaultProtocolFunction(filename);
    };

    _parserFunction = nullptr;
}

void
Options::resetNotInheritedValues()
{
    seekingOffset(0);
    seekedLength(0);
}

#if MINKO_PLATFORM & MINKO_PLATFORM_HTML5
bool
Options::testUserAgentPlatform(const std::string& platform)
{
    std::string script = "navigator.userAgent.indexOf(\"" + platform + "\") < 0 ? 0 : 1";

    return emscripten_run_script_int(script.c_str()) == 1;
}
#endif
