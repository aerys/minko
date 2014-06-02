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

#include "minko/file/Options.hpp"

#include "minko/material/Material.hpp"
#include "minko/file/AbstractProtocol.hpp"
#include "minko/file/AssetLibrary.hpp"

#ifdef __APPLE__
# include "CoreFoundation/CoreFoundation.h"
#endif

using namespace minko;
using namespace minko::file;

Options::ProtocolFunction
Options::_defaultProtocolFunction = 0;

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
    _effect(nullptr)
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
#if defined(_WIN32) || defined(_WIN64)
    _platforms.push_back("windows");
    _platforms.push_back("desktop");
#endif
#ifdef TARGET_OS_IPHONE
    _platforms.push_back("iphone");
    _platforms.push_back("mobile");
#endif
#ifdef TARGET_OS_MAC
    _platforms.push_back("macosx");
    _platforms.push_back("desktop");
#endif
#ifdef __ANDROID_API__
    _platforms.push_back("android");
    _platforms.push_back("mobile");
#endif
#ifdef EMSCRIPTEN
    _platforms.push_back("web");
#endif
#if defined(LINUX) || defined(__unix__)
    _platforms.push_back("linux");
    _platforms.push_back("desktop");
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

std::shared_ptr<AbstractProtocol>
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
}
