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

#include "GLSLParser.hpp"

#include "minko/AssetsLibrary.hpp"

#include "glsl_optimizer.h"

using namespace minko::file;

void
GLSLParser::parse(const std::string&				filename,
				  const std::string&                resolvedFilename,
                  std::shared_ptr<Options>          options,
				  const std::vector<unsigned char>&	data,
				  std::shared_ptr<AssetsLibrary>	assetsLibrary)
{
    std::string src = "#version 100\n" + std::string((const char*)(&data[0]), data.size());
	const char* sourceString = src.c_str();
    auto optimizer = glslopt_initialize(true);
    auto type = resolvedFilename.find_last_of(".fragment.glsl") >= 0
        ? kGlslOptShaderFragment
        : kGlslOptShaderVertex;

    auto optimizedShader = glslopt_optimize(optimizer, type, sourceString, kGlslOptionSkipPreprocessor);
    if (glslopt_get_status(optimizedShader))
    {
        auto optimizedSource = std::string(glslopt_get_output(optimizedShader));

        assetsLibrary->blob(filename, (const std::vector<unsigned char>&)optimizedSource);
    }
    else
    {
        std::cerr << glslopt_get_log(optimizedShader) << std::endl;
        throw std::invalid_argument("source");
    }
    glslopt_shader_delete(optimizedShader);

    delete optimizer;
}
