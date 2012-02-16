package aerys.minko.render
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.resource.IndexBuffer3DResource;
	import aerys.minko.render.resource.VertexBuffer3DResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.binding.IBinder;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.ColorMask;
	import aerys.minko.type.enum.TriangleCulling;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBlendFactor;
	import flash.display3D.Context3DCompareMode;
	import flash.display3D.Context3DProgramType;
	import flash.display3D.Context3DTriangleFace;
	import flash.utils.Dictionary;
	
	public final class DrawCall
	{
		use namespace minko_render;
		
		private var _cpuConstants		: Dictionary						= new Dictionary();
		private var _bindings			: Object							= null;
		
		// states
		private var _indexBuffer		: IndexBuffer3DResource				= null;
		private var _firstIndex			: int								= 0;
		private var _numTriangles		: int								= 0;
		
		private var _vertexBuffers		: Vector.<VertexBuffer3DResource>	= new Vector.<VertexBuffer3DResource>(8, true);
		private var _numVertexBuffers	: int								= 0;
		private var _vsConstants		: Vector.<Number>					= null;
		private var _offsets			: Vector.<int>						= new Vector.<int>(8, true);
		private var _formats			: Vector.<String>					= new Vector.<String>(8, true);
		private var _vsInputComponents	: Vector.<VertexComponent>			= null;
		private var _vsInputIndices		: Vector.<uint>						= null;
		
		private var _fsConstants		: Vector.<Number>					= null;
		private var _fsTextures			: Vector.<ITextureResource>			= new Vector.<ITextureResource>(8, true);
		
		private var _blendingSource		: String							= null;
		private var _blendingDest		: String							= null;
		private var _triangleCulling	: String							= null;
		private var _colorMaskR			: Boolean							= true;
		private var _colorMaskG			: Boolean							= true;
		private var _colorMaskB			: Boolean							= true;
		private var _colorMaskA			: Boolean							= true;
		
		public function get vertexComponents() : Vector.<VertexComponent>
		{
			return _vsInputComponents;
		}
		
		public function set blending(v : uint) : void
		{
			_blendingSource = Blending.STRINGS[int(v & 0xffff)];
			_blendingDest	= Blending.STRINGS[int(v >>> 16)]
		}
		
		public function set triangleCulling(v : uint) : void
		{
			_triangleCulling = TriangleCulling.STRINGS[v];
		}
		
		public function set colorMask(v : uint) : void
		{
			_colorMaskR = (v & ColorMask.RED) != 0;
			_colorMaskG = (v & ColorMask.GREEN) != 0;
			_colorMaskB = (v & ColorMask.BLUE) != 0;
			_colorMaskA = (v & ColorMask.ALPHA) != 0;
		}
		
		public function DrawCall(vsConstants 			: Vector.<Number>,
								 fsConstants 			: Vector.<Number>,
								 fsTextures				: Vector.<ITextureResource>,
								 vertexInputComponents	: Vector.<VertexComponent>,
								 vertexInputIndices		: Vector.<uint>,
								 bindings				: Object)
		{
			_vsConstants		= vsConstants.concat();
			_fsConstants		= fsConstants.concat();
			_fsTextures			= fsTextures.concat();
			_vsInputComponents	= vertexInputComponents;
			_vsInputIndices		= vertexInputIndices;
			_bindings			= bindings;
			
			initialize();
		}
		
		private function initialize() : void
		{
			triangleCulling	= TriangleCulling.FRONT;
			blending		= Blending.NORMAL;
			colorMask		= ColorMask.RGBA;
		}
		
		public function clone() : DrawCall
		{
			var clone : DrawCall	= new DrawCall(
				_vsConstants,
				_fsConstants,
				_fsTextures,
				_vsInputComponents,
				_vsInputIndices,
				_bindings
			);
			
			clone._triangleCulling	= _triangleCulling;
			clone._blendingSource	= _blendingSource;
			clone._blendingDest		= _blendingDest;
			clone._colorMaskR		= _colorMaskR;
			clone._colorMaskG		= _colorMaskG;
			clone._colorMaskB		= _colorMaskB;
			clone._colorMaskA		= _colorMaskA;
			
			return clone;
		}
		
		public function setStreams(vertexStreams 	: Vector.<IVertexStream>,
								   indexStream		: IndexStream) : void
		{
			_numVertexBuffers	= _vsInputComponents.length;
			_indexBuffer		= indexStream.resource;
			
			for (var i : int = 0; i < _numVertexBuffers; ++i)
			{
				var component	: VertexComponent	= _vsInputComponents[i];
				var index		: uint				= _vsInputIndices[i];
				
				if (component)
				{
					var stream 	: VertexStream	= vertexStreams[index].getStreamByComponent(component);
					if (stream == null)
						throw new Error('Missing vertex component: ' + component.toString() + '.');
					
					var format 	: VertexFormat 	= stream.format;
					
					_vertexBuffers[i] = stream.resource;
					_formats[i] = component.nativeFormatString;
					_offsets[i] = format.getOffsetForComponent(component);
				}
			}
		}
		
		public function apply(context 	: Context3D,
							  previous	: DrawCall) : uint
		{
			context.setColorMask(_colorMaskR, _colorMaskG, _colorMaskB, _colorMaskA);
			context.setBlendFactors(_blendingSource, _blendingDest);
			context.setCulling(_triangleCulling);
			
			context.setProgramConstantsFromVector(
				Context3DProgramType.VERTEX,
				0,
				_vsConstants
			);
			
			context.setProgramConstantsFromVector(
				Context3DProgramType.FRAGMENT,
				0,
				_fsConstants
			);
			
			var numTextures	: int	= _fsTextures.length;
			var maxTextures	: int	= previous ? previous._fsTextures.length : 8;
			var maxBuffers	: int	= previous ? previous._numVertexBuffers : 8;
			var i 			: int 	= 0;
			
			for (i = 0; i < numTextures; ++i)
				context.setTextureAt(i, _fsTextures[i].getNativeTexture(context));
			
			while (i < maxTextures)
				context.setTextureAt(i++, null);
			
			for (i = 0; i < _numVertexBuffers; ++i)
				context.setVertexBufferAt(
					i,
					_vertexBuffers[i].getVertexBuffer3D(context),
					_offsets[i],
					_formats[i]
				);
			
			while (i < maxBuffers)
				context.setVertexBufferAt(i++, null);
			
			context.drawTriangles(_indexBuffer.getIndexBuffer3D(context));
			
			return _indexBuffer.numIndices / 3;
		}
		
		public function setParameter(name 	: String,
									 value 	: Object) : void
		{
			var binding : IBinder = _bindings[name] as IBinder;
			
			if (binding)
				binding.set(_cpuConstants, _vsConstants, _fsConstants, _fsTextures, value);
		}
		
		public function hasParameter(name : String) : Boolean
		{
			return _bindings[name] != null;
		}
	}
}