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

#if defined(_MSC_VER) // Windows
# include <windows.h>
#elif defined(__APPLE__) // iOS
# include "CoreFoundation/CoreFoundation.h"
#elif defined(EMSCRIPTEN) // HTML5
# include "emscripten.h"
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

    return sanitizeFilename((getcwd(temp, PATH_MAX) ? std::string(temp) : std::string("")));
#else
    return ".";
#endif
}

std::string
File::getBinaryDirectory()
{
#if defined(_MSC_VER) // Windows
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
#elif MINKO_PLATFORM == MINKO_PLATFORM_HTML5 // HTML5
    return ".";
#elif MINKO_PLATFORM == MINKO_PLATFORM_LINUX // Linux
    char buffer[PATH_MAX];
    size_t l = readlink("/proc/self/exe", buffer, PATH_MAX);
    auto path = sanitizeFilename(std::string((char*)buffer, l));
    auto pos = path.find_last_of("/");

    return path.substr(0, pos);
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

std::string
File::canonizeFilename(const std::string& filename)
{
    // Split input string on '/'
    std::vector<std::string> segments;
    std::stringstream ss(filename);
    std::string item;

    while (std::getline(ss, item, '/'))
        segments.push_back(item);

    // Moving path into a stack (but using deque for later iterative access).
    std::deque<std::string> path;

    for (auto current : segments)
    {
        if (current.empty() || current == ".")
            continue;

        if (current != "..")
            path.push_back(current);
        else if (path.size() > 0 && path.back() != "..")
            path.pop_back();
        else
            path.push_back(current);
    }

    // Keep leading '/' if absolute and reset stream.
    ss.str(filename.size() && filename.at(0) == '/' ? "/" : "");
    ss.clear();

    // Recompose path.
    std::copy(path.begin(), path.end(), std::ostream_iterator<std::string>(ss, "/"));

    std::string output = ss.str();

    // Remove trailing '/' inserted by ostream_iterator.
    if (path.size())
        output.erase(output.size() - 1);

    // Relative to nothing means relative to current directory.
    if (output.size() == 0)
        output = ".";

    return output;
}

std::string
File::removePrefixPathFromFilename(const std::string& filename)
{
    const auto cleanFilename = sanitizeFilename(filename);

    auto filenameWithoutPrefixPath = cleanFilename;

    const auto lastSeparatorPosition = filenameWithoutPrefixPath.find_last_of("/");

    if (lastSeparatorPosition != std::string::npos)
        filenameWithoutPrefixPath = filenameWithoutPrefixPath.substr(lastSeparatorPosition + 1);

    return filenameWithoutPrefixPath;
}

std::string
File::extractPrefixPathFromFilename(const std::string& filename)
{
    const auto cleanFilename = sanitizeFilename(filename);

    auto prefixPath = cleanFilename;

    const auto lastSeparatorPosition = prefixPath.find_last_of("/");

    if (lastSeparatorPosition != std::string::npos)
        prefixPath = prefixPath.substr(0, lastSeparatorPosition);

    return prefixPath;
}

std::string
File::getExtension(const std::string& filename)
{
    auto extension = std::string();

    const auto lastDotPosition = filename.find_last_of(".");

    if (lastDotPosition != std::string::npos)
    {
        extension = filename.substr(lastDotPosition + 1);

        std::transform(
            extension.begin(),
            extension.end(),
            extension.begin(),
            ::tolower
        );
    }

    return extension;
}

std::string
File::replaceExtension(const std::string& filename, const std::string& extension)
{
    auto transformedFilename = filename;

    const auto lastDotPosition = transformedFilename.find_last_of(".");

    if (lastDotPosition != std::string::npos)
    {
        auto previousExtension = transformedFilename.substr(lastDotPosition + 1);

        transformedFilename = transformedFilename.substr(
            0,
            transformedFilename.size() - (previousExtension.size() + 1)
        );
    }

    transformedFilename += std::string(".") + extension;

    return transformedFilename;
}
