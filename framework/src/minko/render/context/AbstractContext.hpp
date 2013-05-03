#pragma once

#include "minko/Common.hpp"

namespace minko
{
	namespace render
	{
		namespace context
		{
			class AbstractContext
			{
			public:
				typedef std::shared_ptr<AbstractContext>	ptr;

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
				setVertexBufferAt(const unsigned int index, const unsigned int vertexBuffer) = 0;

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
			};
		}	
	}
}
