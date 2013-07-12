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

#include "GLSLPreProcessor.hpp"

#include <regex>

using namespace minko::render;

void
GLSLPreProcessor::process(const std::string& glslCode)
{
	std::istringstream iss(glslCode);
	std::string line;

	while (std::getline(iss, line))
	{
		line = reduce(line);

		if (line != "")
		{
			std::regex r("#define ([a-zA-Z_]+[a-zA-Z0-9_]*)\\s?(.*)");
			std::smatch matches;

			if (std::regex_match(line, matches, r))
			{
				auto constantName	= matches[1];
				//auto constantValue	= matches[2];

				_constants.push_back(constantName);
			}
		}
	}
}

std::string
GLSLPreProcessor::trim(const std::string& str, const std::string& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::string
GLSLPreProcessor::reduce(const std::string& str, const std::string& fill, const std::string& whitespace)
{
    // trim first
    auto result = trim(str, whitespace);

    // replace sub ranges
    auto beginSpace = result.find_first_of(whitespace);
    while (beginSpace != std::string::npos)
    {
        const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
        const auto range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        const auto newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}