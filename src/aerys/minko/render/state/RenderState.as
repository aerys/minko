package aerys.minko.render.state
{
	import aerys.common.IVersionnable;
	import aerys.minko.ns.minko;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.ressource.TextureRessource;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBlendFactor;
	import flash.display3D.Context3DCompareMode;
	import flash.display3D.Context3DProgramType;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.Context3DTriangleFace;
	import flash.display3D.Program3D;
	import flash.display3D.VertexBuffer3D;
	import flash.display3D.textures.Texture;
	import flash.display3D.textures.TextureBase;
	import flash.geom.Matrix3D;
	import flash.geom.Rectangle;
	
	public final class RenderState implements IVersionnable
	{
		use namespace minko;
		
		private static const NUM_VERTEX_CONSTS		: int				= 128;
		private static const NUM_FRAGMENT_CONSTS	: int				= 28;
		
		private static const TC_FRONT				: int				= TriangleCulling.FRONT;
		private static const TC_BACK				: int				= TriangleCulling.BACK;
		
		private static const PT_VERTEX				: String			= Context3DProgramType.VERTEX;
		private static const PT_FRAGMENT			: String			= Context3DProgramType.FRAGMENT;
		
		private static const TMP_VECTOR				: Vector.<Number>	= new Vector.<Number>();

		private static const BLENDING_STR			: Vector.<String>	= Vector.<String>([Context3DBlendFactor.DESTINATION_ALPHA,
																						   Context3DBlendFactor.DESTINATION_COLOR,
																						   Context3DBlendFactor.ONE,
																						   Context3DBlendFactor.ONE_MINUS_DESTINATION_ALPHA,
																						   Context3DBlendFactor.ONE_MINUS_DESTINATION_COLOR,
																						   Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA,
																						   Context3DBlendFactor.SOURCE_ALPHA,
																						   Context3DBlendFactor.SOURCE_COLOR,
																						   Context3DBlendFactor.ZERO]);
		
		private static const COMPARE_STR			: Vector.<String>	= Vector.<String>([Context3DCompareMode.NEVER,
																						   Context3DCompareMode.GREATER,
																						   Context3DCompareMode.GREATER_EQUAL,
																						   Context3DCompareMode.EQUAL,
																						   Context3DCompareMode.LESS_EQUAL,
																						   Context3DCompareMode.LESS,
																						   Context3DCompareMode.NOT_EQUAL,
																						   Context3DCompareMode.ALWAYS]);
		
		private static const COMPARE_FLAGS			: Vector.<uint>		= Vector.<uint>([CompareMode.NEVER,
																						 CompareMode.GREATER,
																						 CompareMode.GREATER | CompareMode.EQUAL,
																						 CompareMode.EQUAL,
																						 CompareMode.LESS | CompareMode.EQUAL,
																						 CompareMode.LESS,
																						 CompareMode.NOT_EQUAL,
																						 CompareMode.ALWAYS]);
		
		private static const CULLING_STR			: Vector.<String>	= Vector.<String>([Context3DTriangleFace.NONE,
																						   Context3DTriangleFace.BACK,
																						   Context3DTriangleFace.FRONT]);

		private static const RENDER_TARGET			: uint	= 1 << 0;
		private static const BLENDING				: uint	= 1 << 1;
		private static const SHADER					: uint	= 1 << 2;
		private static const COLOR_MASK				: uint	= 1 << 3;
		private static const TRIANGLE_CULLING		: uint	= 1 << 4;
		private static const TEXTURE_1				: uint	= 1 << 5;
		private static const TEXTURE_2				: uint	= 1 << 6;
		private static const TEXTURE_3				: uint	= 1 << 7;
		private static const TEXTURE_4				: uint	= 1 << 8;
		private static const TEXTURE_5				: uint	= 1 << 9;
		private static const TEXTURE_6				: uint	= 1 << 10;
		private static const TEXTURE_7				: uint	= 1 << 11;
		private static const TEXTURE_8				: uint	= 1 << 12;
		private static const INDEX_STREAM			: uint	= 1 << 13;
		private static const VERTEX_CONSTS			: uint	= 1 << 14;
		private static const FRAGMENT_CONSTS		: uint	= 1 << 15;
		private static const SCISSOR_RECTANGLE		: uint	= 1 << 16;
		private static const VERTEX_STREAM_1		: uint	= 1 << 17;
		private static const VERTEX_STREAM_2		: uint	= 1 << 18;
		private static const VERTEX_STREAM_3		: uint	= 1 << 19;
		private static const VERTEX_STREAM_4		: uint	= 1 << 20;
		private static const VERTEX_STREAM_5		: uint	= 1 << 21;
		private static const VERTEX_STREAM_6		: uint	= 1 << 22;
		private static const VERTEX_STREAM_7		: uint	= 1 << 23;
		private static const VERTEX_STREAM_8		: uint	= 1 << 24;
		private static const DEPTH_MASK				: uint	= 1 << 25;
		private static const PRIORITY				: uint	= 1 << 26;
		
		public static const TEXTURES				: uint	= TEXTURE_1 | TEXTURE_2 | TEXTURE_3 | TEXTURE_4
															  | TEXTURE_5 | TEXTURE_6 | TEXTURE_7 | TEXTURE_8;
		
		private var _version			: uint						= 0;
		private var _setFlags			: uint						= 0;
			
		private var _renderTarget		: RenderTarget				= null;
		private var _blending			: uint						= 0;
		private var _shader				: Shader					= null;
		private var _colorMask			: uint						= 0;
		private var _triangleCulling	: uint						= 0;
		private var _textures			: Vector.<TextureRessource>	= new Vector.<TextureRessource>(8, true);
	
		private var _vertexStreams		: Vector.<VertexStream>	= new Vector.<VertexStream>(8, true);
		private var _vertexOffsets		: Vector.<int>				= new Vector.<int>(8, true);
		private var _vertexFormats		: Vector.<String>			= new Vector.<String>(8, true);
		private var _indexStream		: IndexStream				= null;
		
		private var _vertexConstants	: Vector.<Number>			= new Vector.<Number>(NUM_VERTEX_CONSTS * 4);
		private var _fragmentConstants	: Vector.<Number>			= new Vector.<Number>(NUM_FRAGMENT_CONSTS * 4);
		private var _rectangle			: Rectangle					= null;
		
		private var _depthMask			: uint						= 0;
		
		private var _priority			: Number					= 0.;
		
		public function get priority() : Number
		{
			return _priority;
		}

		minko function get indexStream() : IndexStream
		{
			return _indexStream;
		}
		
		public function get rectangle() : Rectangle
		{
			return _setFlags & SCISSOR_RECTANGLE ? _rectangle : null;
		}
		
		public function get version() : uint
		{
			return _version;
		}
		
		public function get renderTarget() : RenderTarget
		{
			return _setFlags & RENDER_TARGET ? _renderTarget : null;
		}
		
		public function get colorMask() : uint
		{
			return _setFlags & COLOR_MASK ? _colorMask : null; 
		}
		
		public function get shader() : Shader
		{
			return _setFlags & SHADER ? _shader : null;
		}
		
		public function get blending() : uint
		{
			return _setFlags & BLENDING ? _blending : 0;
		}
		
		public function get triangleCulling() : uint
		{
			return _setFlags & TRIANGLE_CULLING ? _triangleCulling : 0;
		}
		
		public function get depthMask() : uint
		{
			return _setFlags & DEPTH_MASK ? _depthMask : 0;
		}
		
		public function set priority(value : Number) : void
		{
			_priority = value;
			_setFlags |= PRIORITY;
		}
		
		public function set rectangle(value : Rectangle) : void
		{
			_rectangle = value;
			_setFlags |= SCISSOR_RECTANGLE;
		}
		
		public function set renderTarget(value : RenderTarget) : void
		{
			//if (value != _renderTarget)
			{
				_renderTarget = value;
				_setFlags |= RENDER_TARGET;
				++_version;
			}
		}
		
		public function set shader(value : Shader) : void
		{
			//if (_shader != value)
			{
				_shader = value;
				_setFlags |= SHADER;
				++_version;
			}
		}
		
		public function set blending(value : uint) : void
		{
			//if (_blending != value)
			{
				_blending = value;
				_setFlags |= BLENDING;
				++_version;
			}
		}
		
		public function set colorMask(value : uint) : void
		{
			//if (value != _colorMask)
			{
				_colorMask = value;
				_setFlags |= COLOR_MASK;
				++_version;
			}
		}
		
		public function set triangleCulling(value : uint) : void
		{
			//if (value != _triangleCulling)
			{
				_triangleCulling = value;
				_setFlags |= TRIANGLE_CULLING;
				++_version;
			}
		}
		
		public function set depthMask(value : uint) : void
		{
			_depthMask = value;
			_setFlags |= DEPTH_MASK;
			++_version;
		}
		
		public function setInputStreams(vertexStream	: VertexStreamList,
										indexStream		: IndexStream) : void
		{
			var vertexInput	: Vector.<VertexComponent> 	= shader._vertexInput;
			var numInputs	: int							= vertexInput.length;
			
			_indexStream = indexStream;
			_setFlags |= INDEX_STREAM;
			
			for (var i : int = 0; i < numInputs; ++i)
			{
				var neededComponent:VertexComponent = vertexInput[i];
				
				if (neededComponent)
				{
					var stream : VertexStream = vertexStream.getStreamByComponent(neededComponent);
					
					if (!stream)
						throw new Error("Missing vertex components: " + neededComponent.implodedFields);
					
					_vertexStreams[i] = stream;
					_vertexFormats[i] = neededComponent.nativeFormatString;
					_vertexOffsets[i] = stream.format.getOffsetForComponent(neededComponent);
					
					_setFlags |= VERTEX_STREAM_1 << i;
					++_version;
				}
			}
		}
		
		public function setTextureAt(index : int, texture : TextureRessource) : void
		{
			var flag : uint = TEXTURE_1 << index;
			
			//if (!(_setFlags & flag) || _textures[index] != texture)
			{
				_textures[index] = texture;
				_setFlags |= flag;
				++_version;
			}
		}
		
		public function setFragmentConstant(register	: int,
											x			: Number,
											y			: Number = 0.,
											z			: Number = 0.,
											w			: Number = 0.) : void
		{
			var offset : int = register * 4;
			
			_fragmentConstants[offset] = x;
			_fragmentConstants[int(offset + 1)] = y;
			_fragmentConstants[int(offset + 2)] = z;
			_fragmentConstants[int(offset + 3)] = w;
			
			_setFlags |= FRAGMENT_CONSTS;
			++_version;
		}
		
		public function setFragmentConstants(register	: int,
											 data		: Vector.<Number>) : void
		{
			var offset : int = register * 4;
			var numData : int = data.length;
			
			for (var i : int = 0; i < numData; ++i)
				_fragmentConstants[int(offset + i)] = data[i];
			
			_setFlags |= FRAGMENT_CONSTS;
			++_version;
		}
		
		public function setFragmentConstantMatrix(register 		: int,
												  value 		: Matrix4x4,
												  transposed	: Boolean = true) : void
		{
			value.getRawData(_fragmentConstants, register * 4, transposed);
			
			_setFlags |= FRAGMENT_CONSTS;
			++_version;
		}
		
		public function setVertexConstant(register	: int,
										  x			: Number,
										  y			: Number = 0.,
										  z			: Number = 0.,
										  w			: Number = 0.) : void
		{
			var offset : int = register * 4;
			
			_vertexConstants[offset] = x;
			_vertexConstants[int(offset + 1)] = y;
			_vertexConstants[int(offset + 2)] = z;
			_vertexConstants[int(offset + 3)] = w;
			
			_setFlags |= VERTEX_CONSTS;
			++_version;
		}
		
		public function setVertexConstants(register	: int,
										   data		: Vector.<Number>) : void
		{
			var offset : int = register * 4;
			var numData : int = data.length;
			
			for (var i : int = 0; i < numData; ++i)
				_vertexConstants[int(offset + i)] = data[i];
			
			_setFlags |= VERTEX_CONSTS;
			++_version;
		}
		
		public function setVertexConstantMatrix(register	: int,
												value		: Matrix4x4,
												transposed	: Boolean = true) : void
		{
			value.getRawData(_vertexConstants, register * 4, transposed);
			
			_setFlags |= VERTEX_CONSTS;
			++_version;
		}
		
		public function clear() : void
		{
			_setFlags = 0;
			_version = 0;
		}
		
		public function prepareContext(context : Context3D, current : RenderState = null) : void
		{
			if (current)
			{
				prepareContextDelta(context, current);
			}
			else
			{
				if (_shader)
					_shader.prepare(context);
				
				if (_setFlags & VERTEX_CONSTS)
					context.setProgramConstantsFromVector(PT_VERTEX, 0, _vertexConstants);

				if (_setFlags & FRAGMENT_CONSTS)
					context.setProgramConstantsFromVector(PT_FRAGMENT, 0, _fragmentConstants);
				
				if (_setFlags & TRIANGLE_CULLING)
					context.setCulling((_triangleCulling & TC_FRONT) && (_triangleCulling & TC_BACK)
									   ? Context3DTriangleFace.FRONT_AND_BACK
									   : CULLING_STR[_triangleCulling]);
				
				if (_setFlags & COLOR_MASK)
					context.setColorMask((_colorMask & WriteMask.COLOR_RED) != 0,
										 (_colorMask & WriteMask.COLOR_GREEN) != 0,
										 (_colorMask & WriteMask.COLOR_BLUE) != 0,
										 (_colorMask & WriteMask.COLOR_ALPHA) != 0);
				
				if (_setFlags & BLENDING)
					context.setBlendFactors(BLENDING_STR[int(_blending & 0xffff)],
											BLENDING_STR[int(_blending >>> 16)]);
				
				for (var i : int = 0; i < 8; ++i)
				{
					// set texture
					if (_setFlags & (TEXTURE_1 << i))
					{
						var texture : Texture = _textures[i].getNativeTexture(context);
						context.setTextureAt(i, texture);
					}
					else
					{
						context.setTextureAt(i, null);
					}
					
					// set vertex buffer
					if (_setFlags & (VERTEX_STREAM_1 << i))
					{
						var vertexBuffer : VertexBuffer3D	= _vertexStreams[i].getNativeVertexBuffer3D(context);
						var vertexOffset : int				= _vertexOffsets[i];
						var vertexFormat : String			= _vertexFormats[i];
					
						context.setVertexBufferAt(i, vertexBuffer, vertexOffset, vertexFormat);
					}
					else 
					{
						context.setVertexBufferAt(i, null);
					}
				}
				
				if (_setFlags & DEPTH_MASK)
				{
					for (var j : int = 0; j < 8; ++j)
					{
						if (_depthMask == COMPARE_FLAGS[j])
						{
							context.setDepthTest(true, COMPARE_STR[j]);
							break ;
						}
					}
				}
				
				if (_setFlags & RENDER_TARGET)
				{
					if (_renderTarget == null)
					{
						context.setRenderToBackBuffer();
						context.clear();
					}
					else
					{
						var renderTexture		: Texture	= _renderTarget.textureRessource.getNativeTexture(context);
						var useDepthAndStencil	: Boolean	= _renderTarget.useDepthAndStencil;
						var antialiasing		: int		= _renderTarget.antiAliasing;
						
						context.setRenderToTexture(renderTexture, useDepthAndStencil, antialiasing);
						context.clear(0);
					}
				}
			}
		}
		
		private function prepareContextDelta(context : Context3D, current : RenderState) : void
		{
			if (_setFlags & SHADER && _shader != current._shader)
				_shader.prepare(context);
			
			if (_setFlags & VERTEX_CONSTS)
				context.setProgramConstantsFromVector(PT_VERTEX, 0, _vertexConstants);
											
			if (_setFlags & FRAGMENT_CONSTS)
				context.setProgramConstantsFromVector(PT_FRAGMENT, 0, _fragmentConstants);
			
			if (_setFlags & TRIANGLE_CULLING
				&& (!(current._setFlags & TRIANGLE_CULLING) || current._triangleCulling != _triangleCulling))
			{
				if ((_triangleCulling & TC_FRONT) && (_triangleCulling & TC_BACK))
					context.setCulling(Context3DTriangleFace.FRONT_AND_BACK);
				else
					context.setCulling(CULLING_STR[_triangleCulling]);
			}
			
			if (_setFlags & COLOR_MASK
				&& (!(current._setFlags & COLOR_MASK) || current._colorMask != _colorMask))
			{
				context.setColorMask((_colorMask & WriteMask.COLOR_RED) != 0,
									 (_colorMask & WriteMask.COLOR_GREEN) != 0,
									 (_colorMask & WriteMask.COLOR_BLUE) != 0,
									 (_colorMask & WriteMask.COLOR_ALPHA) != 0);
			}
			
			if (_setFlags & BLENDING
				&& (!(current._setFlags & BLENDING) || current._blending != _blending))
			{
				context.setBlendFactors(BLENDING_STR[int(_blending & 0xffff)],
										BLENDING_STR[int(_blending >> 16)]);
			}
			
			for (var i : int = 0; i < 8; ++i)
			{
				// set textures
				var textureFlag			: uint			= TEXTURE_1 << i;
				var textureRessource	: TextureRessource = _textures[i];
				var texture				: TextureBase 	= (_setFlags & textureFlag)
											  	  ? textureRessource.getNativeTexture(context)
											  	  : null;
				
				if (texture != ((current._setFlags & textureFlag) ? current._textures[i] : null))
					context.setTextureAt(i, texture);
				
				// set vertex buffers
				var vertexFlag : uint = VERTEX_STREAM_1 << i;
				var vertexStream : VertexStream	= _setFlags & vertexFlag
													  ? _vertexStreams[i]
													  : null;
				var vertexOffset : int				= _vertexOffsets[i];
				var vertexFormat : String			= _vertexFormats[i];
				
				if (!vertexStream != (current._setFlags & vertexFlag ? current._vertexStreams[i] : null))
					context.setVertexBufferAt(i,
											  vertexStream ? vertexStream.getNativeVertexBuffer3D(context) : null,
											  vertexOffset,
											  vertexFormat);
			}
			
			if (_setFlags & DEPTH_MASK
				&& (!(current._setFlags & DEPTH_MASK) || _depthMask != current._depthMask))
			{
				for (var j : int = 0; j < 8; ++j)
				{
					if (_depthMask == COMPARE_FLAGS[j])
					{
						context.setDepthTest(true, COMPARE_STR[j]);
						break ;
					}
				}
			}
			
			if (_setFlags & RENDER_TARGET
				&& (!(current._setFlags & RENDER_TARGET) || _renderTarget != current._renderTarget))
			{
				if (_renderTarget == null)
				{
					context.setRenderToBackBuffer();
					context.clear();
				}
				else
				{
					var renderTexture		: Texture	= _renderTarget.textureRessource.getNativeTexture(context);
					var useDepthAndStencil	: Boolean	= _renderTarget.useDepthAndStencil;
					var antialiasing		: int		= _renderTarget.antiAliasing;
					
					context.setRenderToTexture(renderTexture, useDepthAndStencil, antialiasing);
					context.clear();
				}
			}
		}
		
		public function copyTo(target : RenderState) : RenderState
		{
			target._setFlags = _setFlags;
			
			if (_setFlags & SHADER)
				target._shader = _shader;
			
			if (_setFlags & TRIANGLE_CULLING)
				target._triangleCulling = _triangleCulling;
			
			if (_setFlags & COLOR_MASK)
				target._colorMask = _colorMask;
			
			if (_setFlags & BLENDING)
				target._blending = _blending;
			
			for (var i : int = 0; i < 8; ++i)
			{
				if (_setFlags & (TEXTURE_1 << i))
					target._textures[i] = _textures[i];
				
				if (_setFlags & (VERTEX_STREAM_1 << i))
				{
					target._vertexStreams[i] = _vertexStreams[i];
					target._vertexOffsets[i] = _vertexOffsets[i];
					target._vertexFormats[i] = _vertexFormats[i];
				}
			}
			
			if (_setFlags & INDEX_STREAM)
				target._indexStream = _indexStream;
			
			return target;
		}
		
		public function compare(state : RenderState) : Number
		{
			return 0.;
		}
		
	}
}