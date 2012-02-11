package aerys.minko.render
{
	import aerys.minko.render.resource.IndexBuffer3DResource;
	import aerys.minko.render.resource.VertexBuffer3DResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.binding.IBinder;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DProgramType;
	import flash.utils.Dictionary;
	
	public final class DrawCall
	{
		private var _vsConstants		: Vector.<Number>					= null;
		private var _fsConstants		: Vector.<Number>					= null;
		private var _fsTextures			: Vector.<ITextureResource>			= new Vector.<ITextureResource>(8, true);
		
		private var _bindings			: Object							= null;
		private var _dataStore			: Dictionary						= new Dictionary();
		
		private var _vertexBuffers		: Vector.<VertexBuffer3DResource>	= new Vector.<VertexBuffer3DResource>(8, true);
		private var _numVertexBuffers	: int								= 0;
		private var _offsets			: Vector.<int>						= new Vector.<int>(8, true);
		private var _formats			: Vector.<String>					= new Vector.<String>(8, true);
		private var _inputComponents	: Vector.<VertexComponent>			= null;
		private var _inputIndices		: Vector.<uint>						= null;
		
		private var _indexBuffer		: IndexBuffer3DResource				= null;
		private var _firstIndex			: int								= 0;
		private var _numTriangles		: int								= 0;
		
		public function get vertexComponents() : Vector.<VertexComponent>
		{
			return _inputComponents;
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
			_inputComponents	= vertexInputComponents;
			_inputIndices		= vertexInputIndices;
			_bindings			= bindings;
		}
		
		public function setStreams(vertexStreams 	: Vector.<IVertexStream>,
								   indexStream		: IndexStream) : void
		{
			_numVertexBuffers	= _inputComponents.length;
			_indexBuffer		= indexStream.resource;
			
			for (var i : int = 0; i < _numVertexBuffers; ++i)
			{
				var component	: VertexComponent	= _inputComponents[i];
				var index		: uint				= _inputIndices[i];
				
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
			{
				context.setVertexBufferAt(
					i,
					_vertexBuffers[i].getVertexBuffer3D(context),
					_offsets[i],
					_formats[i]
				);
			}
			
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
				binding.set(_vsConstants, _fsConstants, _fsTextures, value, _dataStore);
		}
		
		public function hasParameter(name : String) : Boolean
		{
			return _bindings[name] != null;
		}
	}
}