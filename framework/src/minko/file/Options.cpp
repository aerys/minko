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

using namespace minko;
using namespace minko::file;

Options::ProtocolFunction Options::_defaultProtocolFunction = nullptr;

Options::Options() :
    _context(nullptr),
    _includePaths(),
    _platforms(),
    _userFlags(),
    _generateMipMaps(false),
    _resizeSmoothly(false),
    _isCubeTexture(false),
    _startAnimation(true),
    _loadAsynchronously(false),
    _disposeIndexBufferAfterLoading(false),
    _disposeVertexBufferAfterLoading(false),
    _disposeTextureAfterLoading(false),
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
#endif
}

void
Options::initializeUserFlags()
{
#ifdef MINKO_NO_GLSL_STRUCT
    _userFlags.push_back("no-glsl-struct");
#endif // MINKO_NO_GLSL_STRUCT
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
        p->options(Options::create(p->options()));

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
    auto options = shared_from_this();

    _materialFunction = [](const std::string&, material::Material::Ptr material) -> material::Material::Ptr
    {
        return material;
    };

    _geometryFunction = [](const std::string&, GeomPtr geom) -> GeomPtr
    {
        return geom;
    };

    _uriFunction = [](const std::string& uri) -> const std::string
    {
        return uri;
    };

    _nodeFunction = [](NodePtr node) -> NodePtr
    {
        return node;
    };

    _effectFunction = [](EffectPtr effect) -> EffectPtr
    {
        return effect;
    };

    _textureFormatFunction = [this](const std::unordered_set<render::TextureFormat>& availableTextureFormats)
                                ->render::TextureFormat
    {
        auto textureFormatIt = std::find_if(_textureFormats.begin(), _textureFormats.end(),
                            [&](render::TextureFormat textureFormat) -> bool
        {
            return availableTextureFormats.find(textureFormat) != availableTextureFormats.end();
        });

        if (textureFormatIt != _textureFormats.end())
            return *textureFormatIt;

        if (std::find(_textureFormats.begin(),
                      _textureFormats.end(),
                      render::TextureFormat::RGB) != _textureFormats.end() &&
            availableTextureFormats.find(render::TextureFormat::RGBA) != availableTextureFormats.end())
            return render::TextureFormat::RGBA;

        if (std::find(_textureFormats.begin(),
                      _textureFormats.end(),
                      render::TextureFormat::RGBA) != _textureFormats.end() &&
            availableTextureFormats.find(render::TextureFormat::RGB) != availableTextureFormats.end())
            return render::TextureFormat::RGB;

        static const auto errorMessage = "No desired texture format available";

        LOG_DEBUG(errorMessage);

        throw std::runtime_error(errorMessage);
    };

    if (!_defaultProtocolFunction)
        _defaultProtocolFunction = [=](const std::string& filename) -> std::shared_ptr<AbstractProtocol>
    {
        auto defaultProtocol = options->getProtocol("file"); // "file" might be overriden (by APKProtocol for instance)

        defaultProtocol->options(Options::create(options));

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

}
