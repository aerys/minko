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

#include "minko/render/AbstractContext.hpp"
#include "minko/render/ProgramInputs.hpp"
#include "minko/render/Blending.hpp"

namespace minko
{
	namespace render
	{
		class OpenGLES2Context :
			public AbstractContext,
			public std::enable_shared_from_this<OpenGLES2Context>
		{
		public:
			typedef std::shared_ptr<OpenGLES2Context> Ptr;

        private:
            typedef std::unordered_map<unsigned int, unsigned int>		BlendFactorsMap;
			typedef std::unordered_map<CompareMode, unsigned int>		CompareFuncsMap;
			typedef std::unordered_map<StencilOperation, unsigned int>	StencilOperationMap;
            typedef std::unordered_map<unsigned int, unsigned int>		TextureToBufferMap;
			typedef std::pair<uint, uint>								TextureSize;

		protected:
	        static BlendFactorsMap					_blendingFactors;
			static CompareFuncsMap					_compareFuncs;
			static StencilOperationMap				_stencilOps;

            static std::unordered_map<TextureFormat,
                                      unsigned int>   _availableTextureFormats;

			bool									_errorsEnabled;

			std::list<uint>							_textures;
            std::unordered_map<uint, TextureSize>	_textureSizes;
            std::unordered_map<uint, bool>          _textureHasMipmaps;
			std::unordered_map<uint, TextureType>   _textureTypes;

            std::string                             _driverInfo;

			std::list<unsigned int>	                _vertexBuffers;
			std::list<unsigned int>	                _indexBuffers;
			std::list<unsigned int>                 _programs;
			std::list<unsigned int>                 _vertexShaders;
			std::list<unsigned int>                 _fragmentShaders;

            TextureToBufferMap                      _frameBuffers;
            TextureToBufferMap                      _renderBuffers;

			unsigned int			                _viewportX;
			unsigned int			                _viewportY;
			unsigned int			                _viewportWidth;
			unsigned int			                _viewportHeight;
            unsigned int                              _oldViewportX;
            unsigned int                              _oldViewportY;
            unsigned int                              _oldViewportWidth;
            unsigned int                              _oldViewportHeight;

            unsigned int                            _currentTarget;
			int						                _currentIndexBuffer;
			std::vector<int>		                _currentVertexBuffer;
			std::vector<int>		                _currentVertexSize;
			std::vector<int>		                _currentVertexStride;
			std::vector<int>		                _currentVertexOffset;
			uint									_currentBoundTexture;
			std::vector<int>		                _currentTexture;
            std::unordered_map<uint, WrapMode>      _currentWrapMode;
            std::unordered_map<uint, TextureFilter> _currentTextureFilter;
            std::unordered_map<uint, MipFilter>     _currentMipFilter;
			int						                _currentProgram;
			Blending::Mode			                _currentBlendMode;
			bool									_currentColorMask;
			bool					                _currentDepthMask;
			CompareMode				                _currentDepthFunc;
            TriangleCulling                         _currentTriangleCulling;
			CompareMode								_currentStencilFunc;
			int										_currentStencilRef;
			uint									_currentStencilMask;
			StencilOperation						_currentStencilFailOp;
			StencilOperation						_currentStencilZFailOp;
			StencilOperation						_currentStencilZPassOp;

		public:
			~OpenGLES2Context();

			static
			Ptr
			create()
			{
				return std::shared_ptr<OpenGLES2Context>(new OpenGLES2Context());
			}

            static
            const std::unordered_map<TextureFormat, unsigned int>&
            availableTextureFormats();

			inline
			bool
			errorsEnabled()
			{
				return _errorsEnabled;
			}

			inline
			void
			errorsEnabled(bool errorsEnabled)
			{
				_errorsEnabled = errorsEnabled;
			}

            inline
            const std::string&
            driverInfo()
            {
                return _driverInfo;
            }

            inline
            uint
            renderTarget()
            {
            	return _currentTarget;
            }

            inline
            uint
            viewportWidth()
            {
            	return _viewportWidth;
            }

            inline
            uint
            viewportHeight()
            {
            	return _viewportHeight;
            }

			inline
			uint
			currentProgram()
			{
				return _currentProgram;
			}

			void
			configureViewport(const uint x,
							  const uint y,
							  const uint with,
							  const uint height);

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
			drawTriangles(const uint indexBuffer, const int numTriangles);

			const uint
			createVertexBuffer(const uint size);

			void
			setVertexBufferAt(const uint	position,
							  const uint	vertexBuffer,
							  const uint	size,
							  const uint	stride,
							  const uint	offset);
			void
			uploadVertexBufferData(const uint 	vertexBuffer,
								   const uint 	offset,
								   const uint 	size,
								   void* 				data);

			void
			deleteVertexBuffer(const uint vertexBuffer);

			const uint
			createIndexBuffer(const uint size);

			void
			uploaderIndexBufferData(const uint 	indexBuffer,
									const uint 	offset,
									const uint 	size,
									void*				data);

			void
			deleteIndexBuffer(const uint indexBuffer);

			uint
			createTexture(TextureType	type,
						  unsigned int  width,
						  unsigned int  height,
						  bool		    mipMapping,
                          bool          optimizeForRenderToTexture = false);

            uint
            createCompressedTexture(TextureType     type,
                                    TextureFormat   format,
                                    unsigned int    width,
                                    unsigned int    height,
                                    bool            mipMapping);


			void
			uploadTexture2dData(uint			texture,
							    unsigned int 	width,
							    unsigned int 	height,
							    unsigned int 	mipLevel,
							    void*			data);

			void
			uploadCubeTextureData(uint				texture,
								  CubeTexture::Face face,
							      unsigned int 		width,
							      unsigned int 		height,
							      unsigned int 		mipLevel,
							      void*				data);

			void
            uploadCompressedTexture2dData(uint          texture,
                                          TextureFormat format,
                                          unsigned int  width,
                                          unsigned int  height,
                                          unsigned int  size,
                                          unsigned int  mipLevel,
                                          void*         data);

            void
            uploadCompressedCubeTextureData(uint                texture,
                                            CubeTexture::Face   face,
                                            TextureFormat       format,
                                            unsigned int        width,
                                            unsigned int        height,
                                            unsigned int        mipLevel,
                                            void*               data);

            void
            activateMipMapping(uint texture);

            void
			deleteTexture(uint texture);

			void
			setTextureAt(uint			position,
						 int			texture		= 0,
						 int			location	= -1);

            void
            setSamplerStateAt(uint				position,
                              WrapMode          wrapping,
                              TextureFilter     filtering,
                              MipFilter         mipFiltering);

			const uint
			createProgram();

			void
			attachShader(const uint program, const uint shader);

			void
			linkProgram(const uint program);

			void
			deleteProgram(const uint program);

			void
			compileShader(const uint shader);

			void
			setProgram(const uint program);

			virtual
			void
			setShaderSource(const uint shader, const std::string& source);

			const uint
			createVertexShader();

			void
			deleteVertexShader(const uint vertexShader);

			const uint
			createFragmentShader();

			void
			deleteFragmentShader(const uint fragmentShader);

			ProgramInputs
			getProgramInputs(const uint program);

			std::string
			getShaderCompilationLogs(const uint shader);

			std::string
			getProgramInfoLogs(const uint program);

            void
            setBlendMode(Blending::Source source, Blending::Destination destination);

            void
            setBlendMode(Blending::Mode blendMode);

			void
			setDepthTest(bool depthMask, CompareMode depthFunc);

			void
			setColorMask(bool);

			void
			setStencilTest(CompareMode		stencilFunc,
						   int				stencilRef,
						   uint				stencilMask,
						   StencilOperation	stencilFailOp,
						   StencilOperation	stencilZFailOp,
						   StencilOperation	stencilZPassOp);

			void
			setScissorTest(bool	scissorTest, const math::ivec4& scissorBox);

			void
			readPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char* pixels);

			void
			readPixels(unsigned char* pixels);

            void
            setTriangleCulling(TriangleCulling triangleCulling);

            void
            setRenderToBackBuffer();

            void
            setRenderToTexture(unsigned int texture, bool enableDepthAndStencil = false);

            void
            generateMipmaps(unsigned int texture);

            bool
            supportsExtension(const std::string& extensionNameString);

		protected:
			OpenGLES2Context();

			virtual
			std::vector<ProgramInputs::UniformInput>
			getUniformInputs(const uint program);

			virtual
            std::vector<ProgramInputs::AttributeInput>
            getAttributeInputs(const uint program);

			static
			ProgramInputs::Type
			convertInputType(unsigned int);

            static
            BlendFactorsMap
            initializeBlendFactorsMap();

			static
            CompareFuncsMap
            initializeDepthFuncsMap();

			static
			StencilOperationMap
			initializeStencilOperationsMap();

            void
            createRTTBuffers(TextureType	type,
							 uint			texture,
							 unsigned int	width,
							 unsigned int   height);

			void
            getShaderSource(unsigned int    shader,
                            std::string&    output);

            inline
            void
            checkForErrors()
            {
#ifdef DEBUG
				if (_errorsEnabled && getError() != 0)
				{
					std::cout << "error: OpenGLES2Context::checkForErrors()" << std::endl;
					throw;
				}
#endif
            }

            unsigned int
            getError();

			TextureType
			getTextureType(uint textureId) const;

            void
            setUniformFloat(uint location, uint count, const float* v);

            void
            setUniformFloat2(uint location, uint count, const float* v);

            void
            setUniformFloat3(uint location, uint count, const float* v);

            void
            setUniformFloat4(uint location, uint count, const float* v);

            void
            setUniformMatrix4x4(uint location, uint count, const float* v);

            void
            setUniformInt(uint location, uint count, const int* v);

            void
            setUniformInt2(uint location, uint count, const int* v);

            void
            setUniformInt3(uint location, uint count, const int* v);

            void
            setUniformInt4(uint location, uint count, const int* v);
		};
	}
}
