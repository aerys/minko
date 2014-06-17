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

#include "ParticleIndexBuffer.hpp"

#include "minko/render/AbstractContext.hpp"

using namespace minko;
using namespace minko::render;

// void 
// ParticleIndexBuffer::update(unsigned int nParticles)
// {	
// 	unsigned int size = nParticles * 6;
	
// 	_context->uploaderIndexBufferData(_id, 0, size, &data()[0]);
	
// 	if(size < data().size())
// 		_context->uploaderIndexBufferData(_id, size, data().size() - size, &_padding[0]);
// }

void 
ParticleIndexBuffer::resize(unsigned int nParticles)
{	
	std::vector<unsigned short>& isData	= data();
	unsigned int oldSize				= isData.size();
	unsigned int size					= nParticles * 6;

	if (oldSize != size)
	{
		if (nParticles == 0)
			dispose();
		else
		{
			isData.resize(size);
			_padding.resize(size, 0);
	
			if (oldSize < size)
			{
				for (unsigned int i = 0, j = 0, k = 0; i < nParticles; ++i)
				{
					isData[j++] = k;
					isData[j++] = k + 2;
					isData[j++] = k + 1;
					isData[j++] = k + 1;
					isData[j++] = k + 2; 
					isData[j++] = k + 3;
	
					k += 4;
				}
			}		
			upload();
		}
	}
}