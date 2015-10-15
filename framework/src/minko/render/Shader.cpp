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

#include "minko/render/Shader.hpp"

#include "minko/render/AbstractContext.hpp"
#include "minko/log/Logger.hpp"

#ifdef MINKO_GLSL_OPTIMIZER_ENABLED
# include "glsl_optimizer.h"
#endif

using namespace minko;
using namespace minko::render;

#ifdef MINKO_GLSL_OPTIMIZER_ENABLED
# if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID || MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_HTML5
const glslopt_ctx* Shader::_glslOptimizer = glslopt_initialize(glslopt_target::kGlslTargetOpenGLES20);
# else
const glslopt_ctx* Shader::_glslOptimizer = glslopt_initialize(glslopt_target::kGlslTargetOpenGL);
# endif
#endif

void
Shader::dispose()
{
    if (_type == Type::VERTEX_SHADER)
        _context->deleteVertexShader(_id);
    else if (_type == Type::FRAGMENT_SHADER)
        _context->deleteFragmentShader(_id);

    _id = -1;
}

void
Shader::upload()
{
    _id = _type == Type::VERTEX_SHADER ? _context->createVertexShader() : _context->createFragmentShader();

#if MINKO_PLATFORM & (MINKO_PLATFORM_ANDROID | MINKO_PLATFORM_IOS | MINKO_PLATFORM_HTML5)
    std::string source = "#version 100\n" + _source;
#else
    std::string source = "#version 120\n" + _source;
#endif

#ifdef MINKO_GLSL_OPTIMIZER_ENABLED
    glslopt_shader* optimizedShader = nullptr;

    if (_type == Type::VERTEX_SHADER)
        optimizedShader = glslopt_optimize(const_cast<glslopt_ctx*>(_glslOptimizer), kGlslOptShaderVertex, source.c_str(), 0);
    else
        optimizedShader = glslopt_optimize(const_cast<glslopt_ctx*>(_glslOptimizer), kGlslOptShaderFragment, source.c_str(), 0);

    if (glslopt_get_status(optimizedShader))
    {
        auto optimizedSource = glslopt_get_output(optimizedShader);
        _context->setShaderSource(_id, optimizedSource);
    }
    else
    {
#ifdef DEBUG
        auto line = std::string();
        std::istringstream stream(source);
        auto lineNumber = 0u;

        while (std::getline(stream, line))
        {
            LOG_DEBUG(lineNumber << ": " << line);

            ++lineNumber;
        }
#endif

        LOG_ERROR(glslopt_get_log(optimizedShader));
        throw std::invalid_argument("source");
    }
    glslopt_shader_delete(optimizedShader);

#else
    _context->setShaderSource(_id, source);
#endif

    _context->compileShader(_id);
}
