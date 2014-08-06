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

#include "minko/render/Program.hpp"

#include "minko/render/AbstractContext.hpp"
#include "minko/render/Shader.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"

using namespace minko;
using namespace minko::render;
using namespace minko::math;

Program::Program(Program::AbstractContextPtr context) :
    AbstractResource(context),
    _uniformFloat2(),
    _textures(),
    _vertexBuffers(),
    _indexBuffer(nullptr)
{
}

void
Program::upload()
{
    _id = context()->createProgram();
    _context->attachShader(_id, _vertexShader->id());
    _context->attachShader(_id, _fragmentShader->id());
    _context->linkProgram(_id);

    _inputs = _context->getProgramInputs(_id);
}

void
Program::dispose()
{
    _context->deleteProgram(_id);
    _id = -1;

    _vertexShader = nullptr;
    _fragmentShader = nullptr;


    _uniformFloat2.clear();
    _textures.clear();
    _vertexBuffers.clear();
    _indexBuffer = nullptr;
}

void
Program::setUniform(const std::string& name, float v1)
{
    if (!_inputs->hasName(name))
        return;

    _uniformFloat[_inputs->location(name)] = v1;
}

void
Program::setUniform(const std::string& name, float v1, float v2)
{
    if (!_inputs->hasName(name))
        return;

    _uniformFloat2[_inputs->location(name)] = Vector2::create(v1, v2);
}

void
Program::setUniform(const std::string& name, float v1, float v2, float v3)
{
    if (!_inputs->hasName(name))
        return;

    _uniformFloat3[_inputs->location(name)] = Vector3::create(v1, v2, v3);
}

void
Program::setUniform(const std::string& name, float v1, float v2, float v3, float v4)
{
    if (!_inputs->hasName(name))
        return;

    _uniformFloat4[_inputs->location(name)] = Vector4::create(v1, v2, v3, v4);
}

void
Program::setUniform(const std::string& name, AbstractTexture::Ptr texture)
{
    if (!_inputs->hasName(name))
        return;

    _textures[_inputs->location(name)] = texture;
}

void
Program::setUniform(const std::string& name, Texture::Ptr texture)
{
    setUniform(name, std::static_pointer_cast<AbstractTexture>(texture));
}

void
Program::setUniform(const std::string& name, CubeTexture::Ptr texture)
{
    setUniform(name, std::static_pointer_cast<AbstractTexture>(texture));
}

void
Program::setVertexAttribute(const std::string& name, unsigned int attributeSize, const std::vector<float>& data)
{
    if (!_inputs->hasName(name))
        return;

    auto vertexBuffer = VertexBuffer::create(_context, data);
    vertexBuffer->addAttribute(name, attributeSize, 0);

    _vertexBuffers[_inputs->location(name)] = vertexBuffer;
}

void
Program::setIndexBuffer(const std::vector<unsigned short>& indices)
{
    if (indices.empty())
    {
        _indexBuffer = nullptr;

        return;
    }

    _indexBuffer = IndexBuffer::create(_context, indices);
}