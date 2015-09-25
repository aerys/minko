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

#pragma once

#include "minko/Common.hpp"

#include "minko/render/Blending.hpp"
#include "minko/render/TextureFormat.hpp"
#include "minko/render/CubeTexture.hpp"

namespace minko
{
    namespace render
    {
        class AbstractContext
        {
        public:
            typedef std::shared_ptr<AbstractContext>    Ptr;

        public:
            virtual
            ~AbstractContext()
            {
            }

            virtual
            bool
            errorsEnabled() = 0;

            virtual
            void
            errorsEnabled(bool errorsEnabled) = 0;

            virtual
            const std::string&
            driverInfo() = 0;

            virtual
            uint
            renderTarget() = 0;

            virtual
            uint
            viewportWidth() = 0;

            virtual
            uint
            viewportHeight() = 0;

            virtual
            uint
            currentProgram() = 0;

            virtual
            void
            configureViewport(const uint x,
                              const uint y,
                              const uint with,
                              const uint height) = 0;

            virtual
            void
            clear(float red             = 0.f,
                  float green            = 0.f,
                  float blue            = 0.f,
                  float alpha            = 0.f,
                  float depth            = 1.f,
                  unsigned int stencil    = 0,
                  unsigned int mask        = 0xffffffff) = 0;

            virtual
            void
            present() = 0;

            virtual
            void
			drawTriangles(const uint indexBuffer, const uint firstIndex, const int numTriangles) = 0;

			virtual
			void
			drawTriangles(const uint firstIndex, const int numTriangles) = 0;

            virtual
            const uint
            createVertexBuffer(const uint size) = 0;

            virtual
            void
            setVertexBufferAt(const uint    position,
                              const uint    vertexBuffer,
                              const uint    size,
                              const uint    stride,
                              const uint    offset) = 0;

            virtual
            void
            uploadVertexBufferData(const uint     vertexBuffer,
                                   const uint     offset,
                                   const uint     size,
                                   void*                 data) = 0;

            virtual
            void
            deleteVertexBuffer(const uint vertexBuffer) = 0;

            virtual
            const uint
            createIndexBuffer(const uint size) = 0;

            virtual
            void
            uploaderIndexBufferData(const uint     indexBuffer,
                                    const uint     offset,
                                    const uint     size,
                                    void*                data) = 0;

            virtual
            void
            deleteIndexBuffer(const uint indexBuffer) = 0;

            virtual
            uint
            createTexture(TextureType   type,
                          unsigned int  width,
                          unsigned int  height,
                          bool          mipMapping,
                          bool          optimizeForRenderToTexture = false) = 0;

            virtual
            uint
            createRectangleTexture(TextureType  type,
                                   unsigned int width,
                                   unsigned int height) = 0;

            virtual
            uint
            createCompressedTexture(TextureType     type,
                                    TextureFormat   format,
                                    unsigned int    width,
                                    unsigned int    height,
                                    bool            mipMapping) = 0;

            virtual
            void
            uploadTexture2dData(uint            texture,
                                unsigned int    width,
                                unsigned int    height,
                                unsigned int    mipLevel,
                                void*           data) = 0;

            virtual
            void
            uploadCubeTextureData(uint                texture,
                                  CubeTexture::Face   face,
                                  unsigned int        width,
                                  unsigned int        height,
                                  unsigned int        mipLevel,
                                  void*               data) = 0;

            virtual
            void
            uploadCompressedTexture2dData(uint          texture,
                                          TextureFormat format,
                                          unsigned int  width,
                                          unsigned int  height,
                                          unsigned int  size,
                                          unsigned int  mipLevel,
                                          void*         data) = 0;

            virtual
            void
            uploadCompressedCubeTextureData(uint                texture,
                                            CubeTexture::Face   face,
                                            TextureFormat       format,
                                            unsigned int        width,
                                            unsigned int        height,
                                            unsigned int        mipLevel,
                                            void*               data) = 0;

            virtual
            void
            activateMipMapping(uint texture) = 0;

            virtual
            void
            deleteTexture(uint    texture) = 0;

            virtual
            void
            setTextureAt(uint    position,
                         int    texture        = 0,
                         int    location    = -1) = 0;

            virtual
            void
            setSamplerStateAt(uint            position,
                              WrapMode      wrapping,
                              TextureFilter filtering,
                              MipFilter     mipFiltering) = 0;

            virtual
            const uint
            createProgram() = 0;

            virtual
            void
            attachShader(const uint program, const uint shader) = 0;

            virtual
            void
            linkProgram(const uint program) = 0;

            virtual
            void
            deleteProgram(const uint program) = 0;

            virtual void
            setProgram(const uint program) = 0;

            virtual
            void
            compileShader(const uint shader) = 0;

            virtual
            void
            setShaderSource(const uint shader, const std::string& source) = 0;

            virtual
            const uint
            createVertexShader() = 0;

            virtual
            void
            deleteVertexShader(const uint vertexShader) = 0;

            virtual
            const uint
            createFragmentShader() = 0;

            virtual
            void
            deleteFragmentShader(const uint fragmentShader) = 0;

            virtual
			ProgramInputs
            getProgramInputs(const uint program) = 0;

            virtual
            void
            setBlendingMode(Blending::Source source, Blending::Destination destination) = 0;

            virtual
            void
            setBlendingMode(Blending::Mode blendMode) = 0;

            virtual
            void
			setColorMask(bool) = 0;

            virtual
            void
			setDepthTest(bool depthMask, CompareMode depthFunc) = 0;

            virtual
            void
			setStencilTest(CompareMode		stencilFunc,
						   int				stencilRef,
						   uint				stencilMask,
						   StencilOperation	stencilFailOp,
						   StencilOperation	stencilZFailOp,
						   StencilOperation	stencilZPassOp) = 0;

            virtual
            void
			setScissorTest(bool	scissorTest, const math::ivec4&) = 0;

            virtual
            void
			readPixels(unsigned char* pixels) = 0;

            virtual
            void
			readPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char* pixels) = 0;

            virtual
            void
            setTriangleCulling(TriangleCulling triangleCulling) = 0;

            virtual
            void
            setRenderToBackBuffer() = 0;

            virtual
            void
            setRenderToTexture(unsigned int texture, bool enableDepthAndStencil = false) = 0;

            virtual
            void
            generateMipmaps(unsigned int texture) = 0;

            virtual
            void
            setUniformFloat(uint location, uint count, const float* v) = 0;

            virtual
            void
            setUniformFloat2(uint location, uint count, const float* v) = 0;

            virtual
            void
            setUniformFloat3(uint location, uint count, const float* v) = 0;

            virtual
            void
            setUniformFloat4(uint location, uint count, const float* v) = 0;

            virtual
            void
            setUniformMatrix4x4(uint location, uint count, const float* v) = 0;

            virtual
            void
            setUniformInt(uint location, uint count, const int* v) = 0;

            virtual
            void
            setUniformInt2(uint location, uint count, const int* v) = 0;

            virtual
            void
            setUniformInt3(uint location, uint count, const int* v) = 0;

            virtual
            void
            setUniformInt4(uint location, uint count, const int* v) = 0;

            virtual
            int
            createVertexAttributeArray() = 0;

            virtual
            void
            setVertexAttributeArray(const uint vertexArray) = 0;
        };
    }
}
