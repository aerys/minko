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

#pragma once

#include "minko/Common.hpp"

namespace minko
{
	namespace render
	{
		class AbstractContext
		{
		public:
			typedef std::shared_ptr<AbstractContext>	Ptr;

		public:
			~AbstractContext()
			{
			}

			virtual
			void
			configureViewport(const unsigned int x,
								const unsigned int y,
								const unsigned int with,
								const unsigned int height) = 0;

			virtual
			void
			clear(float red 			= 0.f,
					float green			= 0.f,
					float blue			= 0.f,
					float alpha			= 0.f,
					float depth			= 1.f,
					unsigned int stencil	= 0,
					unsigned int mask		= 0xffffffff) = 0;

			virtual
			void
			present() = 0;

			virtual
			void
			drawTriangles(const unsigned int indexBuffer, const int numTriangles) = 0;

			virtual
			const unsigned int
			createVertexBuffer(const unsigned int size) = 0;

			virtual
			void
			setVertexBufferAt(const unsigned int	position,
								const unsigned int	vertexBuffer,
								const unsigned int	size,
								const unsigned int	stride,
								const unsigned int	offset) = 0;

			virtual
			void
			uploadVertexBufferData(const unsigned int 	vertexBuffer,
									const unsigned int 	offset,
									const unsigned int 	size,
									void* 				data) = 0;

			virtual	
			void
			deleteVertexBuffer(const unsigned int vertexBuffer) = 0;
				
			virtual
			const unsigned int
			createIndexBuffer(const unsigned int size) = 0;

			virtual
			void
			uploaderIndexBufferData(const unsigned int 	indexBuffer,
									const unsigned int 	offset,
									const unsigned int 	size,
									void*				data) = 0;

			virtual
			void
			deleteIndexBuffer(const unsigned int indexBuffer) = 0;

			virtual
			const unsigned int
			createTexture(unsigned int width,
						  unsigned int height,
						  bool		   mipMapping) = 0;

			virtual
			void
			uploadTextureData(const unsigned int texture,
							  unsigned int 		 width,
							  unsigned int 		 height,
							  unsigned int 		 mipLevel,
							  void*				 data) = 0;

			virtual
			void
			deleteTexture(const unsigned int texture) = 0;

			virtual
			void
			setTextureAt(const unsigned int	position,
						 const int			texture		= -1,
						 const int			location	= -1) = 0;

			virtual
			const unsigned int
			createProgram() = 0;

			virtual
			void
			attachShader(const unsigned int program, const unsigned int shader) = 0;

			virtual
			void
			linkProgram(const unsigned int program) = 0;

			virtual
			void
			deleteProgram(const unsigned int program) = 0;

			virtual void
			setProgram(const unsigned int program) = 0;

			virtual
			void
			compileShader(const unsigned int shader) = 0;

			virtual
			void
			setShaderSource(const unsigned int shader,
							const std::string& source) = 0;

			virtual
			const unsigned int
			createVertexShader() = 0;

			virtual
			void
			deleteVertexShader(const unsigned int vertexShader) = 0;

			virtual
			const unsigned int
			createFragmentShader() = 0;

			virtual
			void
			deleteFragmentShader(const unsigned int fragmentShader) = 0;

			virtual
			std::shared_ptr<ShaderProgramInputs>
			getProgramInputs(const unsigned int program) = 0;

			virtual
			void
			setUniform(unsigned int location, float value) = 0;

			virtual
			void
			setUniform(unsigned int location, float value1, float value2) = 0;

			virtual
			void
			setUniform(unsigned int location, float value1, float value2, float value3) = 0;

			virtual
			void
			setUniform(unsigned int location, float value1, float value2, float value3, float value4) = 0;

			virtual
			void
			setUniformMatrix4x4(unsigned int location, unsigned int size, bool transpose, const float* values) = 0;

		};
	}	
}
