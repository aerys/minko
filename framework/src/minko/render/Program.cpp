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
#include "minko/render/VertexAttribute.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/render/IndexBuffer.hpp"

using namespace minko;
using namespace minko::render;
using namespace minko::math;

Program::Program(const std::string& name, Program::AbsContextPtr context) :
	AbstractResource(context),
	_name(name)
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
    if (_id != -1)
    {
	    _context->deleteProgram(_id);
	    _id = -1;
    }

	_vertexShader = nullptr;
	_fragmentShader = nullptr;
}

Program&
Program::setUniform(const std::string& name, AbstractTexture::Ptr texture)
{
    auto it = std::find_if(_inputs.uniforms().begin(), _inputs.uniforms().end(), [&](const ProgramInputs::UniformInput& u)
    {
        return u.name == name;
    });

    if (it != _inputs.uniforms().end())
    {
        auto oldProgram = _context->currentProgram();

        _context->setTextureAt(_setTextures.size(), texture->id(), it->location);
        _context->setProgram(oldProgram);

        _setTextures.insert(name);
		_setUniforms.insert(name);
    }

    return *this;
}

Program&
Program::setUniform(const std::string& name, Texture::Ptr texture)
{
	return setUniform(name, std::static_pointer_cast<AbstractTexture>(texture));
}

Program&
Program::setUniform(const std::string& name, CubeTexture::Ptr texture)
{
	return setUniform(name, std::static_pointer_cast<AbstractTexture>(texture));
}

Program&
Program::setAttribute(const std::string& name, const VertexAttribute& attribute, const std::string& attributeName)
{
    auto it = std::find_if(_inputs.attributes().begin(), _inputs.attributes().end(), [&](const ProgramInputs::AttributeInput& a)
    {
        return a.name == name;
    });

    if (it != _inputs.attributes().end())
    {
        auto oldProgram = _context->currentProgram();

        _context->setVertexBufferAt(it->location, *attribute.resourceId, attribute.size, *attribute.vertexSize, attribute.offset);
        _context->setProgram(oldProgram);

        _setAttributes.insert(name);
    }

    return *this;
}
