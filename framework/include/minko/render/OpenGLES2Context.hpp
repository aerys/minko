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
#include "minko/Hash.hpp"

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
            typedef std::unordered_map<uint, uint>							BlendFactorsMap;
			typedef std::unordered_map<CompareMode, uint>					CompareFuncsMap;
			typedef std::unordered_map<StencilOperation, uint>				StencilOperationMap;
            typedef std::unordered_map<uint, uint>							TextureToBufferMap;
			typedef std::pair<uint, uint>									TextureSize;
			typedef Hash<TextureFormat>								TextFormatHash;
			typedef std::unordered_map<TextureFormat, uint, TextFormatHash>	TextureFormats;

		protected:
	        static BlendFactorsMap					_blendingFactors;
			static CompareFuncsMap					_compareFuncs;
			static StencilOperationMap				_stencilOps;

            static TextureFormats  					_availableTextureFormats;

			bool									_errorsEnabled;

			std::list<uint>							_textures;
            std::unordered_map<uint, TextureSize>	_textureSizes;
            std::unordered_map<uint, bool>          _textureHasMipmaps;
			std::unordered_map<uint, TextureType>   _textureTypes;

            std::string                             _driverInfo;
			int										_oglMajorVersion;
			int										_oglMinorVersion;

			std::list<uint>	                		_vertexBuffers;
			std::list<uint>	                		_indexBuffers;
			std::list<uint>                 		_programs;
			std::list<uint>                 		_vertexShaders;
			std::list<uint>                 		_fragmentShaders;

            TextureToBufferMap                      _frameBuffers;
            TextureToBufferMap                      _renderBuffers;
			bool									_scissorTest;
			math::ivec4								_scissorBox;

			uint			                		_viewportX;
			uint			                		_viewportY;
			uint			                		_viewportWidth;
			uint			                		_viewportHeight;
            uint                              		_oldViewportX;
            uint                              		_oldViewportY;
            uint                              		_oldViewportWidth;
            uint                              		_oldViewportHeight;

            uint                            		_currentTarget;
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
			Blending::Mode			                _currentBlendingMode;
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

            std::vector<bool>                       _vertexAttributeEnabled;

			int										_stencilBits;

		public:
			~OpenGLES2Context();

			static
			Ptr
			create()
			{
				return std::shared_ptr<OpenGLES2Context>(new OpenGLES2Context());
			}

            static
            const std::unordered_map<TextureFormat, uint, Hash<TextureFormat>>&
            availableTextureFormats();

			inline
			bool
			errorsEnabled() override
			{
				return _errorsEnabled;
			}

			inline
			void
			errorsEnabled(bool errorsEnabled) override
			{
				_errorsEnabled = errorsEnabled;
			}

            inline
            const std::string&
            driverInfo() override
            {
                return _driverInfo;
            }

            inline
            uint
            renderTarget() override
            {
            	return _currentTarget;
            }

            inline
            uint
            viewportWidth() override
            {
            	return _viewportWidth;
            }

            inline
            uint
            viewportHeight() override
            {
            	return _viewportHeight;
            }

			inline
			uint
			currentProgram() override
			{
				return _currentProgram;
			}

			void
			configureViewport(const uint x,
							  const uint y,
							  const uint with,
							  const uint height) override;

			void
			clear(float red 	= 0.f,
				  float green	= 0.f,
				  float blue	= 0.f,
				  float alpha	= 0.f,
				  float depth	= 1.f,
				  uint stencil	= 0,
				  uint mask		= 0xffffffff) override;

			void
			present() override;

			void
			drawTriangles(const uint indexBuffer, const uint firstIndex, const int numTriangles) override;

			void
			drawTriangles(const uint firstIndex, const int numTriangles) override;

			const uint
			createVertexBuffer(const uint size) override;

			void
			setVertexBufferAt(const uint	position,
							  const uint	vertexBuffer,
							  const uint	size,
							  const uint	stride,
							  const uint	offset) override;
			void
			uploadVertexBufferData(const uint 	vertexBuffer,
								   const uint 	offset,
								   const uint 	size,
								   void* 		data) override;

			void
			deleteVertexBuffer(const uint vertexBuffer) override;

			const uint
			createIndexBuffer(const uint size) override;

			void
			uploaderIndexBufferData(const uint 	indexBuffer,
									const uint 	offset,
									const uint 	size,
									void*		data) override;

			void
			deleteIndexBuffer(const uint indexBuffer) override;

			uint
			createTexture(TextureType	type,
						  uint  width,
						  uint  height,
						  bool		    mipMapping,
                          bool          optimizeForRenderToTexture = false) override;

            uint
            createCompressedTexture(TextureType     type,
                                    TextureFormat   format,
                                    uint    		width,
                                    uint    		height,
                                    bool            mipMapping) override;


            uint
            createRectangleTexture(TextureType  type,
                                   unsigned int width,
                                   unsigned int height) override;

			void
			uploadTexture2dData(uint	texture,
							    uint 	width,
							    uint 	height,
							    uint 	mipLevel,
							    void*	data) override;

			void
			uploadCubeTextureData(uint				texture,
								  CubeTexture::Face face,
							      uint 				width,
							      uint 				height,
							      uint 				mipLevel,
							      void*				data) override;

			void
            uploadCompressedTexture2dData(uint          texture,
                                          TextureFormat format,
                                          uint  		width,
                                          uint  		height,
                                          uint  		size,
                                          uint  		mipLevel,
                                          void*         data) override;

            void
            uploadCompressedCubeTextureData(uint                texture,
                                            CubeTexture::Face   face,
                                            TextureFormat       format,
                                            uint        		width,
                                            uint        		height,
                                            uint        		mipLevel,
                                            void*               data) override;

            void
            activateMipMapping(uint texture) override;

            void
			deleteTexture(uint texture) override;

			void
			setTextureAt(uint	position,
						 int	texture		= 0,
						 int	location	= -1) override;

            void
            setSamplerStateAt(uint				position,
                              WrapMode          wrapping,
                              TextureFilter     filtering,
                              MipFilter         mipFiltering) override;

			const uint
			createProgram() override;

			void
			attachShader(const uint program, const uint shader) override;

			void
			linkProgram(const uint program) override;

			void
			deleteProgram(const uint program) override;

			void
			compileShader(const uint shader) override;

			void
			setProgram(const uint program) override;

			virtual
			void
			setShaderSource(const uint shader, const std::string& source) override;

			const uint
			createVertexShader() override;

			void
			deleteVertexShader(const uint vertexShader) override;

			const uint
			createFragmentShader() override;

			void
			deleteFragmentShader(const uint fragmentShader) override;

			ProgramInputs
			getProgramInputs(const uint program) override;

			std::string
			getShaderCompilationLogs(const uint shader);

			std::string
			getProgramInfoLogs(const uint program);

            void
            setBlendingMode(Blending::Source source, Blending::Destination destination) override;

            void
            setBlendingMode(Blending::Mode blendingMode) override;

			void
			setDepthTest(bool depthMask, CompareMode depthFunc) override;

			void
			setColorMask(bool) override;

			void
			setStencilTest(CompareMode		stencilFunc,
						   int				stencilRef,
						   uint				stencilMask,
						   StencilOperation	stencilFailOp,
						   StencilOperation	stencilZFailOp,
						   StencilOperation	stencilZPassOp) override;

			void
			setScissorTest(bool	scissorTest, const math::ivec4& scissorBox) override;

			void
			readPixels(uint x, uint y, uint width, uint height, unsigned char* pixels) override;

			void
			readPixels(unsigned char* pixels) override;

            void
            setTriangleCulling(TriangleCulling triangleCulling) override;

            void
            setRenderToBackBuffer() override;

            void
            setRenderToTexture(uint texture, bool enableDepthAndStencil = false) override;

            void
            generateMipmaps(uint texture) override;

            bool
            supportsExtension(const std::string& extensionNameString);

            int
            createVertexAttributeArray() override;

            void
            setVertexAttributeArray(const uint vertexArray) override;

            void
            deleteVertexAttributeArray(const uint vertexArray);

		protected:
			OpenGLES2Context();

            void
            initializeExtFunctions();

			virtual
			std::vector<ProgramInputs::UniformInput>
			getUniformInputs(const uint program);

			virtual
            std::vector<ProgramInputs::AttributeInput>
            getAttributeInputs(const uint program);

			static
			ProgramInputs::Type
			convertInputType(uint);

            static
            BlendFactorsMap
            initializeBlendFactorsMap();

			static
            CompareFuncsMap
            initializeDepthFuncsMap();

			static
			StencilOperationMap
			initializeStencilOperationsMap();

            uint
            createTexture(TextureType             type,
                          unsigned int            width,
                          unsigned int            height,
                          bool                    mipMapping,
                          bool                    optimizeForRenderToTexture,
                          bool                    assertPowerOfTwoSized);

            void
            createRTTBuffers(TextureType	type,
							 uint			texture,
							 uint	        width,
							 uint           height);

			void
            getShaderSource(uint            shader,
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

            uint
            getError();

			TextureType
			getTextureType(uint textureId) const;

            void
            setUniformFloat(uint location, uint count, const float* v) override;

            void
            setUniformFloat2(uint location, uint count, const float* v) override;

            void
            setUniformFloat3(uint location, uint count, const float* v) override;

            void
            setUniformFloat4(uint location, uint count, const float* v) override;

            void
            setUniformMatrix4x4(uint location, uint count, const float* v) override;

            void
            setUniformInt(uint location, uint count, const int* v) override;

            void
            setUniformInt2(uint location, uint count, const int* v) override;

            void
            setUniformInt3(uint location, uint count, const int* v) override;

            void
            setUniformInt4(uint location, uint count, const int* v) override;
		};
	}
}
