#pragma once

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
# include <GLUT/glut.h>          /* Open GL Util    APPLE */
#else
# include <GL/glut.h>            /* Open GL Util    OpenGL*/
#endif

#include "minko/Common.hpp"
#include "minko/render/context/AbstractContext.hpp"

namespace minko
{
	namespace render
	{
		namespace context
		{
			class OpenGLESContext :
				public AbstractContext
			{
			public:
				typedef std::shared_ptr<OpenGLESContext> ptr;

			public:
				~OpenGLESContext();

				static
				ptr
				create()
				{
					return std::shared_ptr<OpenGLESContext>(new OpenGLESContext());
				}

				void
				clear(float red 			= 0.f,
					  float green			= 0.f,
					  float blue			= 0.f,
					  float alpha			= 0.f,
					  float depth			= 1.f,
					  unsigned int stencil	= 0,
					  unsigned int mask		= 0xffffffff);

				void
				present();

				void
				drawTriangles(const unsigned int indexBuffer, const int numTriangles);

				const unsigned int
				createVertexBuffer(const unsigned int size);

				void
				setVertexBufferAt(const unsigned int index, const unsigned int vertexBuffer);

				void
				uploadVertexBufferData(const unsigned int 	vertexBuffer,
									   const unsigned int 	offset,
									   const unsigned int 	size,
									   void* 				data);
				
				void
				disposeVertexBuffer(const unsigned int vertexBuffer);
				
				const unsigned int
				createIndexBuffer(const unsigned int size);

				void
				uploaderIndexBufferData(const unsigned int 	indexBuffer,
										const unsigned int 	offset,
										const unsigned int 	size,
										void*				data);

				void
				disposeIndexBuffer(const unsigned int indexBuffer);

				const unsigned int
				createTexture(unsigned int width,
							  unsigned int height,
							  bool		   mipMapping);

				void
				uploadTextureData(const unsigned int texture,
								  unsigned int 		 width,
								  unsigned int 		 height,
								  unsigned int 		 mipLevel,
								  void*				 data);

				void
				disposeTexture(const unsigned int texture);

			private:
				std::list<unsigned int>	_vertexBuffers;
				std::list<unsigned int>	_indexBuffers;
				std::list<unsigned int>	_textures;
			};
		}
	}
}