package aerys.minko.render.renderer
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.resource.Program3DResource;
	import aerys.minko.render.resource.Texture3DResource;
	import aerys.minko.type.Factory;
	import aerys.minko.type.enum.ColorMask;
	import aerys.minko.type.enum.CompareMode;
	import aerys.minko.type.enum.TriangleCulling;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBlendFactor;
	import flash.display3D.Context3DCompareMode;
	import flash.display3D.Context3DProgramType;
	import flash.display3D.Context3DTriangleFace;
	import flash.display3D.IndexBuffer3D;
	import flash.display3D.VertexBuffer3D;
	import flash.display3D.textures.TextureBase;
	import flash.geom.Rectangle;
	
	public final class RendererState
	{
		use namespace minko_render;
		
		private static const FACTORY				: Factory			= Factory.getFactory(RendererState);
		
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
		private static const VERTEX_STREAM			: uint	= 1 << 24;
		private static const DEPTH_MASK				: uint	= 1 << 25;
		private static const PRIORITY				: uint	= 1 << 26;
		
		private static const TEXTURES				: uint	= TEXTURE_1 | TEXTURE_2 | TEXTURE_3 | TEXTURE_4
															  | TEXTURE_5 | TEXTURE_6 | TEXTURE_7 | TEXTURE_8;
		
		private var _setFlags			: uint						= 0;
		
		private var _renderTarget		: RenderTarget				= null;
		private var _blending			: uint						= 0;
		private var _program			: Program3DResource			= null;
		private var _colorMask			: uint						= 0;
		private var _triangleCulling	: uint						= 0;
		private var _textures			: Vector.<Texture3DResource>	= new Vector.<Texture3DResource>(8, true);
	
		private var _vertexStreams		: Vector.<IVertexStream>	= new Vector.<IVertexStream>();
		private var _indexStream		: IndexStream				= null;
		
		private var _vertexConstants	: Vector.<Number>			= new Vector.<Number>();
		private var _fragmentConstants	: Vector.<Number>			= new Vector.<Number>();
		private var _rectangle			: Rectangle					= null;
		
		private var _depthTest			: uint						= 0;
		
		private var _priority			: Number					= 0.;

		private var _drawCalls			: Vector.<int>				= new Vector.<int>();
		private var _numDrawCalls		: int						= 0;
		
		public function get priority() : Number
		{
			return _setFlags & PRIORITY ? _priority : 0.;
		}

		public function get rectangle() : Rectangle
		{
			return _setFlags & SCISSOR_RECTANGLE ? _rectangle : null;
		}
		
		public function get renderTarget() : RenderTarget
		{
			return _setFlags & RENDER_TARGET ? _renderTarget : null;
		}
		
		public function get colorMask() : uint
		{
			return _setFlags & COLOR_MASK ? _colorMask : null; 
		}
		
		public function get program() : Program3DResource
		{
			return _setFlags & SHADER ? _program : null;
		}
		
		public function get blending() : uint
		{
			return _setFlags & BLENDING ? _blending : 0;
		}
		
		public function get triangleCulling() : uint
		{
			return _setFlags & TRIANGLE_CULLING ? _triangleCulling : 0;
		}
		
		public function get depthTest() : uint
		{
			return _setFlags & DEPTH_MASK ? _depthTest : 0;
		}
		
		public function get indexStream() : IndexStream
		{
			return _setFlags & INDEX_STREAM ? _indexStream : null;
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
			_renderTarget = value;
			_setFlags |= RENDER_TARGET;
		}
		
		public function set program(value : Program3DResource) : void
		{
			_program = value;
			_setFlags |= SHADER;
		}
		
		public function set blending(value : uint) : void
		{
			_blending = value;
			_setFlags |= BLENDING;
		}
		
		public function set colorMask(value : uint) : void
		{
			_colorMask = value;
			_setFlags |= COLOR_MASK;
		}
		
		public function set triangleCulling(value : uint) : void
		{
			_triangleCulling = value;
			_setFlags |= TRIANGLE_CULLING;
		}
		
		public function set depthTest(value : uint) : void
		{
			_depthTest = value;
			_setFlags |= DEPTH_MASK;
		}
		
		public function setVertexStreamAt(vertexStream 	: IVertexStream,
										  index			: int) : void
		{
			_vertexStreams[index] = vertexStream;
			
			_setFlags |= VERTEX_STREAM;
		}
		
		public function set indexStream(value : IndexStream) : void
		{
			_indexStream = value;
			_setFlags |= INDEX_STREAM;
		}
		
		public function setTextureAt(index : int, texture : Texture3DResource) : void
		{
			var flag : uint = TEXTURE_1 << index;
			
			if (!texture)
				throw new Error("Argument 'texture' can not be null.");
			
			_textures[index] = texture;
			_setFlags |= flag;
		}
		
		public function setFragmentConstants(data : Vector.<Number>) : void
		{
			var numData : int = data.length;
			
			for (var i : int = 0; i < numData; ++i)
				_fragmentConstants[i] = data[i];
			
			_setFlags |= FRAGMENT_CONSTS;
		}
		
		public function setVertexConstants(data	: Vector.<Number>) : void
		{
			var numData : int = data.length;
			
			for (var i : int = 0; i < numData; ++i)
				_vertexConstants[i] = data[i];
			
			_setFlags |= VERTEX_CONSTS;
		}
		
		public function apply(context : Context3D, previous : RendererState) : uint
		{
			if (previous)
				prepareContextDelta(context, previous);
			else
				prepareContext(context);
			
			if (_numDrawCalls != 0)
			{
				var indexBuffer 	: IndexBuffer3D = _indexStream.resource.getIndexBuffer3D(context);
				var numTriangles	: int			= 0;
				
				for (var i : int = 0; i < _numDrawCalls; ++i)
				{
					var count : int = 0;
					
					numTriangles += count == -1 ? _indexStream.length / 3 : count;
					
					context.drawTriangles(indexBuffer, _drawCalls[int(i * 2)], _drawCalls[int(i * 2 + 1)]);
				}
			}
			
			return numTriangles;
		}
		
		private function prepareContext(context : Context3D) : void
		{
			if (_setFlags & SHADER)
				context.setProgram(_program.getProgram3D(context));
			
			if (_setFlags & VERTEX_CONSTS)
				context.setProgramConstantsFromVector(PT_VERTEX, 0, _vertexConstants);

			if (_setFlags & FRAGMENT_CONSTS)
				context.setProgramConstantsFromVector(PT_FRAGMENT, 0, _fragmentConstants);
			
			if (_setFlags & TRIANGLE_CULLING)
				context.setCulling((_triangleCulling & TC_FRONT) && (_triangleCulling & TC_BACK)
								   ? Context3DTriangleFace.FRONT_AND_BACK
								   : CULLING_STR[_triangleCulling]);
			
			if (_setFlags & COLOR_MASK)
				context.setColorMask((_colorMask & ColorMask.COLOR_RED) != 0,
									 (_colorMask & ColorMask.COLOR_GREEN) != 0,
									 (_colorMask & ColorMask.COLOR_BLUE) != 0,
									 (_colorMask & ColorMask.COLOR_ALPHA) != 0);
			
			if (_setFlags & BLENDING)
				context.setBlendFactors(BLENDING_STR[int(_blending & 0xffff)],
										BLENDING_STR[int(_blending >>> 16)]);
			
			context.setScissorRectangle(_setFlags & SCISSOR_RECTANGLE ? _rectangle : null);
			
			for (var i : int = 0; i < 8; ++i)
			{
				// set texture
				if (_setFlags & (TEXTURE_1 << i))
					context.setTextureAt(i, _textures[i].getNativeTexture(context));
				else
					context.setTextureAt(i, null);
			}
		
			if ((_setFlags & VERTEX_STREAM) != 0 || (_setFlags & SHADER) != 0)
			{
				var vertexInputComponents	: Vector.<VertexComponent> 	= _program._vertexComponents;
				var vertexInputIndices		: Vector.<uint>				= _program._vertexIndices;
				var numInputs				: int						= vertexInputComponents.length;
				
				for (i = 0; i < numInputs; ++i)
				{
					var component	: VertexComponent	= vertexInputComponents[i];
					var index		: uint				= vertexInputIndices[i];
					
					if (component)
					{
						var stream 			: VertexStream 		= _vertexStreams[index].getSubStreamByComponent(component);
						
						if (!stream)
							throw new Error("Missing vertex components: " + component.toString());
						
						var vertexBuffer 	: VertexBuffer3D	= stream.resource.getVertexBuffer3D(context);
						var vertexOffset 	: int				= stream.format.getOffsetForComponent(component);
						
						context.setVertexBufferAt(i, vertexBuffer, vertexOffset, component.nativeFormatString);
					}
				}
				
				while (i < 8)
					context.setVertexBufferAt(i++, null);
			}
			
			if (_setFlags & DEPTH_MASK)
			{
				for (var j : int = 0; j < 8; ++j)
				{
					if (_depthTest == COMPARE_FLAGS[j])
					{
						context.setDepthTest(true, COMPARE_STR[j]);
						
						break ;
					}
				}
			}
			
			if (_setFlags & RENDER_TARGET)
			{
				if (!_renderTarget)
				{
					throw new Error('RenderTarget cannot be undefined');
				}
				else if (_renderTarget.type == RenderTarget.BACKBUFFER)
				{
					context.setRenderToBackBuffer();
				}
				else
				{
					context.setRenderToTexture(_renderTarget.textureResource.getNativeTexture(context),
											   _renderTarget.useDepthAndStencil,
											   _renderTarget.antiAliasing);
				}
				
				var color : uint = _renderTarget.backgroundColor;
				
				context.clear(((color >> 16) & 0xff) / 255.,
							  ((color >> 8) & 0xff) / 255.,
							  (color & 0xff) / 255.,
							  ((color >> 24) & 0xff) / 255.);
			}
		}
		
		private function prepareContextDelta(context : Context3D, current : RendererState) : void
		{
			if (_setFlags & SHADER && _program != current._program)
				context.setProgram(_program.getProgram3D(context));
			
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
				context.setColorMask((_colorMask & ColorMask.COLOR_RED) != 0,
									 (_colorMask & ColorMask.COLOR_GREEN) != 0,
									 (_colorMask & ColorMask.COLOR_BLUE) != 0,
									 (_colorMask & ColorMask.COLOR_ALPHA) != 0);
			}
			
			if (_setFlags & BLENDING
				&& (!(current._setFlags & BLENDING) || current._blending != _blending))
			{
				context.setBlendFactors(BLENDING_STR[int(_blending & 0xffff)],
										BLENDING_STR[int(_blending >> 16)]);
			}
			
			if ((current._setFlags & SCISSOR_RECTANGLE) && _rectangle != current._rectangle)
				context.setScissorRectangle(_rectangle);
			
			for (var i : int = 0; i < 8; ++i)
			{
				// set textures
				var textureFlag			: uint				= TEXTURE_1 << i;
				var textureResource		: Texture3DResource = _textures[i];
				var texture				: TextureBase 		= (_setFlags & textureFlag)
											  	  			  ? textureResource.getNativeTexture(context)
											  	  			  : null;
				
				if (texture != ((current._setFlags & textureFlag) ? current._textures[i] : null))
					context.setTextureAt(i, texture);
			}
			
			if ((_setFlags & VERTEX_STREAM) != 0)
			{
				var invalidStreams	: Boolean	= false;
				var numStreams		: int		= _vertexStreams.length;
				
				invalidStreams ||= ((_setFlags & SHADER) != 0) && ((current._setFlags & SHADER) == 0 || current._program != _program);
				invalidStreams ||= ((current._setFlags & VERTEX_STREAM) == 0) && (current._vertexStreams.length != numStreams);

				for (i = 0; i < numStreams && !invalidStreams; ++i)
					invalidStreams ||= current._vertexStreams[i] != _vertexStreams[i];
			
				if (invalidStreams)
				{
					var vertexInputComponents	: Vector.<VertexComponent> 	= _program._vertexComponents;
					var vertexInputIndices		: Vector.<uint>				= _program._vertexIndices;
					var numInputs				: int						= vertexInputComponents.length;
					
					for (i = 0; i < numInputs; ++i)
					{
						var component	: VertexComponent	= vertexInputComponents[i];
						var index		: uint				= vertexInputIndices[i];
						
						if (component)
						{
							var stream 			: VertexStream 		= _vertexStreams[index].getSubStreamByComponent(component);
							
							if (!stream)
								throw new Error("Missing vertex component: " + component.toString());
							
							var vertexBuffer 	: VertexBuffer3D	= stream.resource.getVertexBuffer3D(context);
							var vertexOffset 	: int				= stream.format.getOffsetForComponent(component);
							
							context.setVertexBufferAt(i, vertexBuffer, vertexOffset, component.nativeFormatString);
						}
					}
					
					while (i < 8)
						context.setVertexBufferAt(i++, null);
				}
			}
			
			if (_setFlags & DEPTH_MASK
				&& (!(current._setFlags & DEPTH_MASK) || _depthTest != current._depthTest))
			{
				for (var j : int = 0; j < 8; ++j)
				{
					if (_depthTest == COMPARE_FLAGS[j])
					{
						context.setDepthTest(true, COMPARE_STR[j]);
						
						break ;
					}
				}
			}
			
			if (_setFlags & RENDER_TARGET
				&& (!(current._setFlags & RENDER_TARGET) || _renderTarget != current._renderTarget))
			{
				if (!_renderTarget)
				{
					throw new Error('RenderTarget cannot be undefined');
				}
				else if (_renderTarget.type == RenderTarget.BACKBUFFER)
				{
					context.setRenderToBackBuffer();
				}
				else
				{
					context.setRenderToTexture(_renderTarget.textureResource.getNativeTexture(context),
											   _renderTarget.useDepthAndStencil,
											   _renderTarget.antiAliasing);
				}
				
				var color : uint = _renderTarget.backgroundColor;
				
				context.clear(((color >> 16) & 0xff) / 255.,
							  ((color >> 8) & 0xff) / 255.,
							  (color & 0xff) / 255.,
							  ((color >> 24) & 0xff) / 255.);
			}
		}
		
		public function drawTriangles(offset : int = 0, numTriangles : int = -1) : void
		{
			_drawCalls[int(_numDrawCalls * 2)] = offset;
			_drawCalls[int(_numDrawCalls * 2 + 1)] = numTriangles;
			_numDrawCalls++;
		}
		
		public static function sort(states : Vector.<RendererState>, numStates : int) : void
		{
			var n 	: int 				= numStates;//states.length;
			var a	: Vector.<Number> 	= new Vector.<Number>(n);
			var i	: int 				= 0;
			var j	: int 				= 0;
			var k	: int 				= 0;
			var t	: int				= 0;
			
			for (i = 0; i < n; ++i)
				a[i] = -states[i]._priority;
			
			var m		: int 			= Math.ceil(n * .125);
			var l		: Vector.<int> 	= new Vector.<int>(m);
			var anmin	: Number 		= a[0];
			var nmax	: int  			= 0;
			var nmove	: int 			= 0;
			
			for (i = 1; i < n; ++i)
			{
				if (a[i] < anmin)
					anmin = a[i];
				if (a[i] > a[nmax])
					nmax = i;
			}
			
			if (anmin == a[nmax])
				return ;
			
			var c1	: Number = (m - 1) / (a[nmax] - anmin);
			
			for (i = 0; i < n; ++i)
			{
				k = int(c1 * (a[i] - anmin));
				++l[k];
			}
			
			for (k = 1; k < m; ++k)
				l[k] += l[int(k-1)];
			
			var hold		: Number 		= a[nmax];
			var holdState 	: RendererState = states[nmax];
			
			a[nmax] = a[0];
			a[0] = hold;
			states[nmax] = states[0];
			states[0] = holdState;
			
			var flash		: Number		= 0.;
			var flashState	: RendererState	= null;
			
			j = 0;
			k = int(m - 1);
			i = int(n - 1);
			
			while (nmove < i)
			{
				while (j > (l[k]-1))
					k = int(c1 * (a[int(++j)] - anmin));
				
				flash = a[j];
				flashState = states[j];
				
				while (!(j == l[k]))
				{
					k = int(c1 * (flash - anmin));
					
					hold = a[ (t = int(l[k]-1)) ];
					holdState = states[t];
					
					a[t] = flash;
					states[t] = flashState;
					
					flash = hold;
					flashState = holdState;
					
					--l[k];
					++nmove;
				}
			}
			
			for (j = 1; j < n; ++j)
			{
				hold = a[j];
				holdState = states[j];
				
				i = int(j - 1);
				while (i >= 0 && a[i] > hold)
				{
					// not trivial
					a[int(i+1)] = a[i];
					states[int(i+1)] = states[i];
					
					--i;
				}
				
				a[int(i+1)] = hold;
				states[int(i+1)] = holdState;
			}
		}
		
		public static function create(temporary : Boolean = true) : RendererState
		{
			var state : RendererState	= FACTORY.create(temporary) as RendererState;
			
			state._setFlags = 0;
			state._numDrawCalls = 0;
			
			return state;
		}
	}
}