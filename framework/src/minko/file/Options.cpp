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
#include "minko/file/FileLoader.hpp"

using namespace minko;
using namespace minko::file;

Options::Options(std::shared_ptr<render::AbstractContext> context) :
	_context(context),
	_includePaths(),
	_platforms(),
	_userFlags(),
    _generateMipMaps(false),
	_resizeSmoothly(false),
	_isCubeTexture(false),
	_skinningFramerate(30),
	_skinningMethod(component::SkinningMethod::HARDWARE),
	_material(material::Material::create())
{
#if defined(DEBUG)
	includePaths().push_back("../../../asset");
	includePaths().push_back("asset");
# if defined(_WIN32)
	includePaths().push_back("bin/windows32/debug/asset");
# elif defined(_WIN64)
	includePaths().push_back("bin/windows64/debug/asset");
# elif defined(TARGET_OS_MAC)
	includePaths().push_back("bin/osx64/debug/asset");
# elif defined(EMSCRIPTEN)
	includePaths().push_back("bin/html5/debug/asset");
# elif defined(LINUX) || defined(__unix__)
#  if defined(__x86_64__)
	includePaths().push_back("bin/linux64/debug/asset");
#  else
	includePaths().push_back("bin/linux32/debug/asset");
#  endif
# endif
#else // release
# if defined(_WIN32)
	includePaths().push_back("bin/windows32/release/asset");
# elif defined(_WIN64)
	includePaths().push_back("bin/windows64/release/asset");
# elif defined(TARGET_OS_MAC)
	includePaths().push_back("bin/osx64/release/asset");
# elif defined(EMSCRIPTEN)
	includePaths().push_back("bin/html5/release/asset");
# elif defined(LINUX) || defined(__unix__)
#  if defined(__x86_64__)
	includePaths().push_back("bin/linux64/release/asset");
#  else
	includePaths().push_back("bin/linux32/release/asset");
#  endif 
# endif
#endif

	_materialFunction = [](const std::string&, material::Material::Ptr material) -> material::Material::Ptr
	{ 
		return material;
	};

	_geometryFunction = [](const std::string&, GeomPtr geom) -> GeomPtr
	{
		return geom;
	};

	_loaderFunction = [](const std::string&) -> std::shared_ptr<AbstractLoader>
	{
		return FileLoader::create();
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
