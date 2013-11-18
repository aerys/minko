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

#include "Options.hpp"

#include "minko/data/Provider.hpp"
#include "minko/file/Loader.hpp"

using namespace minko;
using namespace minko::file;

Options::Options(std::shared_ptr<render::AbstractContext> context) :
	_context(context),
	_includePaths(),
	_platforms(),
	_userFlags(),
    _generateMipMaps(false),
	_material(data::Provider::create())
{
#ifdef DEBUG
	includePaths().insert("bin/debug");
#else
	includePaths().insert("bin/release");
#endif

	_materialFunction = [](const std::string&, data::Provider::Ptr material) -> data::Provider::Ptr
	{ 
		return material;
	};

	_loaderFunction = [](const std::string&) -> std::shared_ptr<AbstractLoader>
	{
		return Loader::create();
	};

	_uriFunction = [](const std::string& uri) -> const std::string
	{
		return uri;
	};

	initializePlatforms();
	initializeUserFlags();
}

void
Options::initializePlatforms()
{
#if defined(_WIN32) || defined(_WIN64)
	_platforms.push_back("windows");
#endif
#ifdef TARGET_OS_IPHONE
	_platforms.push_back("iphone");
#endif
#ifdef TARGET_OS_MAC
	_platforms.push_back("macosx");
#endif
#ifdef __ANDROID_API__
	_platforms.push_back("android");
#endif
#ifdef EMSCRIPTEN
	_platforms.push_back("web");
#endif
#if defined(LINUX) || defined(__unix__)
	_platforms.push_back("linux");
#endif
}

void
Options::initializeUserFlags()
{
#ifdef MINKO_NO_GLSL_STRUCT
	_userFlags.push_back("no-glsl-struct");
#endif // MINKO_NO_GLSL_STRUCT
}
