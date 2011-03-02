package aerys.minko.render.state
{
	import aerys.common.IVersionnable;
	import aerys.minko.ns.minko;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.shader.Shader3D;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.stream.VertexStream3DList;
	import aerys.minko.type.vertex.format.Vertex3DComponent;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBlendFactor;
	import flash.display3D.Context3DProgramType;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.Context3DTriangleFace;
	import flash.display3D.Program3D;
	import flash.display3D.VertexBuffer3D;
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
		
		private static const CULLING_STR			: Vector.<String>	= Vector.<String>([Context3DTriangleFace.NONE,
																						   Context3DTriangleFace.BACK,
																						   Context3DTriangleFace.FRONT]);

		private static const RENDER_TARGET			: uint	= 1 << 0;
		private static const BLENDING				: uint	= 1 << 1;
		private static const SHADER					: uint	= 1 << 2;
		private static const COLOR_MASK				: uint	= 1 << 3;
		private static const TRIANGLE_CULLING		: uint	= 1 << 4;
		private static const TEXTURE1				: uint	= 1 << 5;
		private static const TEXTURE2				: uint	= 1 << 6;
		private static const TEXTURE3				: uint	= 1 << 7;
		private static const TEXTURE4				: uint	= 1 << 8;
		private static const TEXTURE5				: uint	= 1 << 9;
		private static const TEXTURE6				: uint	= 1 << 10;
		private static const TEXTURE7				: uint	= 1 << 11;
		private static const TEXTURE8				: uint	= 1 << 12;
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
		
		public static const TEXTURES				: uint	= TEXTURE1 | TEXTURE2 | TEXTURE3 | TEXTURE4
															  | TEXTURE5 | TEXTURE6 | TEXTURE7 | TEXTURE8;
		
		private var _version			: uint						= 0;
		private var _setFlags			: uint						= 0;
			
		private var _renderTarget		: RenderTarget				= null;
		private var _blending			: uint						= 0;
		private var _shader				: Shader3D					= null;
		private var _colorMask			: uint						= 0;
		private var _triangleCulling	: uint						= 0;
		private var _textures			: Vector.<TextureBase>		= new Vector.<TextureBase>(8, true);
	
		private var _vertexStreams		: Vector.<VertexStream3D>	= new Vector.<VertexStream3D>(8, true);
		private var _vertexOffsets		: Vector.<int>				= new Vector.<int>(8, true);
		private var _vertexFormats		: Vector.<String>			= new Vector.<String>(8, true);
		private var _indexStream		: IndexStream3D				= null;
		
		private var _vertexConstants	: Vector.<Number>			= new Vector.<Number>(NUM_VERTEX_CONSTS * 4);
		private var _fragmentConstants	: Vector.<Number>			= new Vector.<Number>(NUM_FRAGMENT_CONSTS * 4);
		private var _rectangle			: Rectangle					= null;
		
		public function get rectangle() : Rectangle
		{
			return _rectangle;
		}
		
		public function get version() : uint
		{
			return _version;
		}
		
		public function get renderTarget() : RenderTarget
		{
			return _renderTarget;
		}
		
		public function get colorMask() : uint
		{
			return _colorMask; 
		}
		
		public function get shader() : Shader3D
		{
			return _shader;
		}
		
		public function get blending() : uint
		{
			return _blending;
		}
		
		public function get triangleCulling() : uint
		{
			return _triangleCulling;
		}
		
		public function get indexStream()	: IndexStream3D 
		{
			return _indexStream;
		}
		
		public function set rectangle(value : Rectangle) : void
		{
			_rectangle = value;
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
		
		public function set shader(value : Shader3D) : void
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
		
		public function set indexStream(value : IndexStream3D) : void 
		{
			//if (value != _indexStream)
			{
				_indexStream = value;
				_setFlags |= INDEX_STREAM;
				++_version;
			}
		}
		
		public function setVertexStreamList(streamList	: VertexStream3DList) : void
		{
			var vertexInput	: Vector.<Vertex3DComponent> 	= _shader._vertexInput;
			var numInputs	: int							= vertexInput.length;
			
			for (var i : int = 0; i < numInputs; ++i)
			{
				var neededComponent:Vertex3DComponent = vertexInput[i];
				
				if (neededComponent)
				{
					var stream:VertexStream3D = streamList.getComponentStream(neededComponent);
					
					_vertexStreams[i] = stream;
					_vertexFormats[i] = neededComponent.nativeFormatString;
					_vertexOffsets[i] = stream.format.getOffsetForComponent(neededComponent);
					
					_setFlags |= VERTEX_STREAM_1 << i;
				}
			}
		}
		
		public function setTextureAt(index : int, texture : TextureBase) : void
		{
			var flag : uint = TEXTURE1 << index;
			
			if (!(_setFlags & flag) || _textures[index] != texture)
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
			value._matrix.copyRawDataTo(_fragmentConstants, register * 4, transposed);
			
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
			value._matrix.copyRawDataTo(_vertexConstants, register * 4, transposed);
			
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
				//prepareContextDelta(context, current);
				throw new Error();
			}
			else
			{
				_shader.prepare(context);
				
				if (_setFlags & VERTEX_CONSTS)
					context.setProgramConstantsFromVector(Context3DProgramType.VERTEX,
														  0,
														  _vertexConstants);

				if (_setFlags & FRAGMENT_CONSTS)
					context.setProgramConstantsFromVector(Context3DProgramType.FRAGMENT,
														  0,
														  _fragmentConstants);
				
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
											BLENDING_STR[int(_blending >> 16)]);
				
				for (var i : int = 0; i < 8; ++i)
				{
					// set texture
					context.setTextureAt(i, (_setFlags & (TEXTURE1 << i)) ? _textures[i] : null);
					
					// set vertex buffer
					if (_setFlags & (VERTEX_STREAM_1 << i))
					{
						var vertexBuffer : VertexBuffer3D	= _vertexStreams[i].getVertexBuffer3D(context);
						var vertexOffset : int				= _vertexOffsets[i];
						var vertexFormat : String			= _vertexFormats[i];
					
						context.setVertexBufferAt(i, vertexBuffer, vertexOffset, vertexFormat);
					}
					else 
					{
						context.setVertexBufferAt(i, null);
					}
				}
			}
		}
		
		private function prepareContextDelta(context : Context3D, current : RenderState) : void
		{
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
				var textureFlag	: uint			= TEXTURE1 << i;
				var texture		: TextureBase 	= (_setFlags & textureFlag)
											  	  ? _textures[i]
											  	  : null;
				
				if (texture != (current._setFlags & textureFlag ? current._textures[i] : null))
					context.setTextureAt(i, texture);
				
				// set vertex buffers
				var vertexFlag : uint = VERTEX_STREAM_1 << i;
				var vertexStream : VertexBuffer3D	= _setFlags & vertexFlag
													  ? _vertexStreams[i].getVertexBuffer3D(context)
													  : null;
				var vertexOffset : int				= _vertexOffsets[i];
				var vertexFormat : String			= _vertexFormats[i];
				
				if (vertexStream != (current._setFlags & vertexFlag ? current._vertexStreams[i] : null))
					context.setVertexBufferAt(i, vertexStream, vertexOffset, vertexFormat);
			}
		}
		
		public function copy(target : RenderState) : RenderState
		{
			/*target._setFlags = _setFlags;
			
			if (_setFlags & RENDER_TARGET)
				target._renderTarget = _renderTarget;
			if (_setFlags & SHADER)
				target._shader = _shader;
			if (_setFlags & VERTEX_CONSTS)
			{
				var numVertexConstants : int = _vertexConstants.length;
				
				for (var k : int = 0; k < numVertexConstants; ++k)
					target._vertexConstants[k] = _vertexConstants[k];
			}
			if (_setFlags & FRAGMENT_CONSTS)
			{
				var numFragmentConstants : int = _fragmentConstants.length;
				
				for (var j : int = 0; j < numFragmentConstants; ++j)	
					target._fragmentConstants[j] = _fragmentConstants[j];
			}
			if (_setFlags & TRIANGLE_CULLING)
				target._triangleCulling = _triangleCulling;
			if (_setFlags & COLOR_MASK)
				target._colorMask = _colorMask;
			if (_setFlags & BLENDING)
				target._blending = _blending;
			if (_setFlags & TEXTURES)
				for (var i : int = 0; i < 8; ++i)
					if (_setFlags & (TEXTURE1 << i))
						target._textures[i] = _textures[i];*/
			
			return target;
		}
		
		public function compare(state : RenderState) : Number
		{
			return 0.;
		}
		
	}
}