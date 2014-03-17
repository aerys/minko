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

#include "minko/file/File.hpp"

#if defined(_MSC_VER)
# include <windows.h>
#elif defined(__APPLE__)
# include "CoreFoundation/CoreFoundation.h"
#elif defined(LINUX) || defined(__unix__) // Linux
# include <unistd.h>
# include <linux/limits.h>
#endif

using namespace minko::file;

std::string
File::getCurrentWorkingDirectory()
{
#if defined(_MSC_VER) // WINDOWS
	TCHAR buffer[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, buffer);

	return sanitizeFilename(std::string((char*)buffer));
#elif defined(TARGET_IPHONE_SIMULATOR) or defined(TARGET_OS_IPHONE) // iOS
	return getBinaryDirectory();
#elif defined(EMSCRIPTEN) // HTML5
	return getBinaryDirectory();
#elif defined(LINUX) || defined(__unix__) // Linux
    char temp[PATH_MAX];

	return sanitizeFilename((getcwd(temp, MAXPATHLEN) ? std::string(temp) : std::string("")));
#else
	return ".";
#endif
}

std::string
File::getBinaryDirectory()
{
#if defined(_MSC_VER) // WINDOWS
	TCHAR buffer[MAX_PATH];

	GetModuleFileName(NULL, buffer, MAX_PATH);

	auto path = sanitizeFilename(std::string((char*)buffer));
	auto pos = path.find_last_of("/");

	return path.substr(0, pos);
#elif defined(TARGET_IPHONE_SIMULATOR) or defined(TARGET_OS_IPHONE) // iOS
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
	char path[PATH_MAX];
	if (!CFURLGetFileSystemRepresentation(resourcesURL, true, (UInt8*)path, PATH_MAX))
		throw std::runtime_error("cannot find .app path");
	CFRelease(resourcesURL);

	return sanitizeFilename(path);
#elif defined(EMSCRIPTEN) // HTML5
	std::string eval = "(document.location.href)";
	char* buffer = emscripten_run_script_string(eval.c_str());
	auto path = sanitizeFilename(std::string(buffer));
	auto pos = path.find_last_of("/");

	return path.substr(0, pos);
#elif defined(LINUX) || defined(__unix__) // Linux
    char buffer[PATH_MAX];

    readlink("/proc/self/exe", buffer, PATH_MAX);

	return sanitizeFilename(std::string((char*)buffer));
#else
	return ".";
#endif
}

std::string
File::sanitizeFilename(const std::string& filename)
{
	auto f = filename;
	auto a = '\\';

	for (auto pos = f.find_first_of(a);
		pos != std::string::npos;
		pos = f.find_first_of(a))
	{
		f = f.replace(pos, 1, 1, '/');
	}

	return f;
}

