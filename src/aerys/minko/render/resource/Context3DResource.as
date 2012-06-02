package aerys.minko.render.resource
{
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.IndexBuffer3D;
	import flash.display3D.Program3D;
	import flash.display3D.VertexBuffer3D;
	import flash.display3D.textures.CubeTexture;
	import flash.display3D.textures.Texture;
	import flash.display3D.textures.TextureBase;
	import flash.geom.Rectangle;

	public final class Context3DResource
	{
		private var _context				: Context3D					= null;
		
		private var _enableErrorChecking	: Boolean					= false;
		
		private var _rttTarget				: TextureBase				= null;
		private var _rttDepthAndStencil		: Boolean					= false;
		private var _rttAntiAliasing		: int						= 0;
		private var _rttSurfaceSelector		: int						= 0;
		private var _rectangle				: Rectangle					= null;
		
		private var _depthMask				: Boolean					= false;
		private var _passCompareMode		: String					= null;
		
		private var _program				: Program3D					= null;
		
		private var _blendingSource			: String					= null;
		private var _blendingDestination	: String					= null;
		
		private var _triangleCulling		: String					= null;
		
		private var _vertexBuffers			: Vector.<VertexBuffer3D>	= new Vector.<VertexBuffer3D>(8, true);
		private var _vertexBuffersOffsets	: Vector.<int>				= new Vector.<int>(8, true);
		private var _vertexBuffersFormats	: Vector.<String>			= new Vector.<String>(8, true);
		private var _textures				: Vector.<TextureBase>		= new Vector.<TextureBase>(8, true);
		
		private var _colorMaskRed			: Boolean					= false;
		private var _colorMaskGreen			: Boolean					= false;
		private var _colorMaskBlue			: Boolean					= false;
		private var _colorMaskAlpha			: Boolean					= false;
		
		public function get enabledErrorChecking() : Boolean
		{
			return _enableErrorChecking;
		}
		public function set enableErrorChecking(value : Boolean) : void
		{
			if (value != _enableErrorChecking)
			{
				_enableErrorChecking = value;
				_context.enableErrorChecking = value;
			}
		}
		
		public function Context3DResource(context : Context3D)
		{
			_context = context;
		}
		
		public function clear(red		: Number	= 0.0,
							  green		: Number	= 0.0,
							  blue		: Number	= 0.0,
							  alpha		: Number	= 1.0,
							  depth		: Number	= 1.0,
							  stencil	: uint		= 0,
							  mask		: uint		= 0xffffffff) : void
		{
			_context.clear(red, green, blue, alpha, depth, stencil);
		}
		
		public function configureBackBuffer(width					: int,
											height					: int,
											antiAlias				: int,
											enabledDepthAndStencil	: Boolean) : Context3DResource
		{
			_context.configureBackBuffer(width, height, antiAlias, enabledDepthAndStencil);
			
			return this;
		}
		
		public function createCubeTexture(size							: int,
										  format						: String,
										  optimizeForRenderToTexture	: Boolean) : CubeTexture
		{
			return _context.createCubeTexture(size, format, optimizeForRenderToTexture);
		}
		
		public function createIndexBuffer(numIndices : int) : IndexBuffer3D
		{
			return _context.createIndexBuffer(numIndices);
		}
		
		public function createProgram() : Program3D
		{
			return _context.createProgram();
		}
		
		public function createTexture(width							: int,
									  height						: int,
									  format						: String,
									  optimizeForRenderToTexture	: Boolean) : Texture
		{
			return _context.createTexture(width, height, format, optimizeForRenderToTexture);
		}
		
		public function createVertexBuffer(numVertices : int, data32PerVertex : int) : VertexBuffer3D
		{
			return _context.createVertexBuffer(numVertices, data32PerVertex);
		}
		
		public function dispose() : void
		{
			_context.dispose();
		}
		
		public function drawToBitmapData(destination : BitmapData) : Context3DResource
		{
			_context.drawToBitmapData(destination);
			
			return this;
		}
		
		public function drawTriangles(indexBuffer	: IndexBuffer3D,
									  firstIndex	: int	= 0,
									  numTriangles	: int	= -1) : Context3DResource
		{
			_context.drawTriangles(indexBuffer, firstIndex, numTriangles);
			
			return this;
		}

		public function present() : Context3DResource
		{
			_context.present();
			
			return this;
		}
		
		public function setBlendFactors(source : String, destination : String) : Context3DResource
		{
			if (source != _blendingSource || destination != _blendingDestination)
			{
				_blendingSource = source;
				_blendingDestination = destination;
				_context.setBlendFactors(_blendingSource, _blendingDestination);
			}
			
			return this;
		}
		
		public function setColorMask(red 	: Boolean,
									 green 	: Boolean,
									 blue 	: Boolean,
									 alpha 	: Boolean) : Context3DResource
		{
			if (red != _colorMaskRed || green != _colorMaskGreen
				|| blue != _colorMaskBlue || alpha != _colorMaskAlpha)
			{
				_colorMaskRed = red;
				_colorMaskGreen = green;
				_colorMaskBlue = blue;
				_colorMaskAlpha = alpha;
				_context.setColorMask(
					_colorMaskRed,
					_colorMaskGreen,
					_colorMaskBlue,
					_colorMaskAlpha
				);
			}
			
			return this;
		}
		
		public function setCulling(triangleCulling : String) : Context3DResource
		{
			if (triangleCulling != _triangleCulling)
			{
				_triangleCulling = triangleCulling;
				_context.setCulling(triangleCulling);
			}
			
			return this;
		}
		
		public function setDepthTest(depthMask : Boolean, passCompareMode : String) : Context3DResource
		{
			if (depthMask != _depthMask || passCompareMode != _passCompareMode)
			{
				_depthMask = depthMask;
				_passCompareMode = passCompareMode;
				_context.setDepthTest(_depthMask, _passCompareMode);
			}
			
			return this;
		}
		
		public function setProgram(program : Program3D) : Context3DResource
		{
			if (_program != program)
			{
				_program = program;
				_context.setProgram(program);
			}
			
			return this;
		}
		
		public function setProgramConstantsFromVector(programeType	: String,
													  offset		: uint,
													  values		: Vector.<Number>,
													  numRegisters	: int	= -1) : Context3DResource
		{
			_context.setProgramConstantsFromVector(programeType, offset, values, numRegisters);
			
			return this;
		}
		
		public function setRenderToBackBuffer() : Context3DResource
		{
			if (_rttTarget != null)
			{
				_rttTarget = null;
				_context.setRenderToBackBuffer();
			}
			
			return this;
		}
		
		public function setRenderToTexture(texture					: TextureBase,
										   enableDepthAndStencil	: Boolean	= false,
										   antiAlias				: int		= 0,
										   surfaceSelector			: int		= 0) : Context3DResource
		{
			if (texture != _rttTarget || enableDepthAndStencil != _rttDepthAndStencil
				|| antiAlias != _rttAntiAliasing || surfaceSelector != _rttSurfaceSelector)
			{
				_rttTarget = texture;
				_rttDepthAndStencil = enableDepthAndStencil;
				_rttAntiAliasing = antiAlias;
				_rttSurfaceSelector = surfaceSelector;
				_context.setRenderToTexture(
					texture,
					enableDepthAndStencil,
					antiAlias,
					surfaceSelector
				);
			}
			
			return this;
		}
		
		public function setScissorRectangle(rectangle : Rectangle) : Context3DResource
		{
			if (_rectangle != rectangle)
			{
				_rectangle = rectangle;
				_context.setScissorRectangle(_rectangle);
			}
			
			return this;
		}
		
		public function setTextureAt(sampler	: uint,
									 texture	: TextureBase) : Context3DResource
		{
			if (_textures[sampler] != texture)
			{
				_textures[sampler] = texture;
				_context.setTextureAt(sampler, texture);
			}
			
			return this;
		}
		
		public function setVertexBufferAt(index			: int,
										  vertexBuffer	: VertexBuffer3D,
										  bufferOffset	: int		= 0,
										  format		: String	= 'float4') : Context3DResource
		{
			if (_vertexBuffers[index] != vertexBuffer
				|| _vertexBuffersOffsets[index] != bufferOffset
				|| _vertexBuffersFormats[index] != format)
			{
				_vertexBuffers[index] = vertexBuffer;
				_vertexBuffersOffsets[index] = bufferOffset;
				_vertexBuffersFormats[index] = format;
				
				_context.setVertexBufferAt(
					index,
					vertexBuffer,
					bufferOffset,
					format
				);
			}
			
			return this;
		}
		
		public function setStencilReferenceValue(refNum		: uint 	= 0,
												 readMask	: uint 	= 255,
												 writeMask	: uint 	= 255) : Context3DResource			
		{
			_context.setStencilReferenceValue(refNum, readMask, writeMask);
			
			return this;
		}
		
		public function setStencilActions(triangleFace					: String = 'frontAndBack', 
										  compareMode					: String = 'always', 
										  actionOnBothPass				: String = 'keep', 
										  actionOnDepthFail				: String = 'keep', 
										  actionOnDepthPassStencilFail	: String = 'keep') : Context3DResource
		{			
			_context.setStencilActions( 
				triangleFace,
				compareMode,
				actionOnBothPass,
				actionOnDepthFail,
				actionOnDepthPassStencilFail
			);			

			return this;
		}
		
	}
}