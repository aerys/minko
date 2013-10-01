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

#include "ParticleVertexBuffer.hpp"

#include "minko/render/AbstractContext.hpp"

using namespace minko;
using namespace minko::render;


ParticleVertexBuffer::ParticleVertexBuffer(std::shared_ptr<AbstractContext> context) :
	VertexBuffer(context)
{
}

void
ParticleVertexBuffer::initialize()
{
	addAttribute("offset", 2, 0);
	addAttribute("position", 3, 2);
}

// void 
// ParticleVertexBuffer::update(unsigned int nParticles, unsigned int vertexSize)
// {	
// 	unsigned int size = nParticles * vertexSize * 4;
	
// 	_context->uploadVertexBufferData(_id, 0, size, &data()[0]);
// }

void 
ParticleVertexBuffer::resize(unsigned int nParticles, unsigned int vertexSize)
{	
	std::vector<float>& vsData = data();
	unsigned int oldSize = vsData.size();
	unsigned int size = nParticles * vertexSize * 4;

	if (oldSize != size)
	{
		auto attrs = attributes();
		
		dispose();
		/*
		for (auto& attr : attrs)
		{
			const std::string&	attrName	= std::get<0>(*attr);
			unsigned int		attrSize	= std::get<1>(*attr);
			unsigned int		attrOffset	= std::get<2>(*attr);

			addAttribute(attrName, attrSize, attrOffset);
		}
		*/
	}

	vsData.resize(size);
	for (unsigned int i = 0; i < nParticles; ++i)
	{
		vsData[i * vertexSize * 4] = -0.5;
		vsData[i * vertexSize * 4 + 1] = -0.5;

		vsData[vertexSize + i * vertexSize * 4] = 0.5;
		vsData[vertexSize + i * vertexSize * 4 + 1] = -0.5;

		vsData[2 * vertexSize + i * vertexSize * 4] = -0.5;
		vsData[2 * vertexSize + i * vertexSize * 4 + 1] = 0.5;

		vsData[3 * vertexSize + i * vertexSize * 4] = 0.5;
		vsData[3 * vertexSize + i * vertexSize * 4 + 1] = 0.5;
	}

	upload();
}