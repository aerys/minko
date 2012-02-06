package aerys.minko.render
{
	import aerys.minko.render.resource.IndexBuffer3DResource;
	import aerys.minko.render.resource.VertexBuffer3DResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.binding.IParameterBinding;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DProgramType;
	
	public final class DrawCall
	{
		private var _vertexConstants	: Vector.<Number>					= null;
		private var _fragmentConstants	: Vector.<Number>					= null;
		private var _textureResources	: Vector.<ITextureResource>			= new Vector.<ITextureResource>(8, true);
		private var _bindings			: Vector.<IParameterBinding>		= null;
		private var _nameToBinding		: Object							= {};
		
		private var _numVertexBuffers	: int								= 0;
		private var _vertexBuffers		: Vector.<VertexBuffer3DResource>	= new Vector.<VertexBuffer3DResource>(8, true);
		private var _offsets			: Vector.<int>						= new Vector.<int>(8, true);
		private var _formats			: Vector.<String>					= new Vector.<String>(8, true);
		private var _inputComponents	: Vector.<VertexComponent>			= null;
		private var _inputIndices		: Vector.<uint>						= null;
		
		private var _indexBuffer		: IndexBuffer3DResource				= null;
		private var _firstIndex			: int								= 0
		private var _numTriangles		: int								= 0;
		
		public function get vertexComponents() : Vector.<VertexComponent>
		{
			return _inputComponents;
		}
		
		public function setStreams(vertexStreams 	: Vector.<IVertexStream>,
								   indexStream		: IndexStream) : void
		{
			_numVertexBuffers = _inputComponents.length;
			_indexBuffer = indexStream.resource;
			
			for (var i : int = 0; i < _numVertexBuffers; ++i)
			{
				var component	: VertexComponent	= _inputComponents[i];
				var index		: uint				= _inputIndices[i];
				
				if (component)
				{
					var stream 	: VertexStream	= vertexStreams[index].getStreamByComponent(component);
					var format 	: VertexFormat 	= stream.format;
					
					_vertexBuffers[i] = stream.resource;
					_formats[i] = component.nativeFormatString;
					_offsets[i] = format.getOffsetForComponent(component);
				}
			}
		}
		
		public function DrawCall(vertexConstants 		: Vector.<Number>,
								 fragmentConstants 		: Vector.<Number>,
								 textureResources		: Vector.<ITextureResource>,
								 vertexInputComponents	: Vector.<VertexComponent>,
								 vertexInputIndices		: Vector.<uint>,
								 bindings				: Vector.<IParameterBinding>)
		{
			_vertexConstants = vertexConstants.concat();
			_fragmentConstants = fragmentConstants.concat();
			_textureResources = textureResources.concat();
			_inputComponents = vertexInputComponents;
			_inputIndices = vertexInputIndices;
			_bindings = bindings;
			
			initialize();
		}
		
		private function initialize() : void
		{
			var numBindings : int = _bindings.length;
			
			for (var bindingId : int = 0; bindingId < numBindings; ++bindingId)
			{
				var binding : IParameterBinding = _bindings[bindingId];
				
				_nameToBinding[binding.name] = binding;
			}
		}
		
		public function apply(context : Context3D) : uint
		{
			context.setProgramConstantsFromVector(
				Context3DProgramType.VERTEX,
				0,
				_vertexConstants
			);
			
			context.setProgramConstantsFromVector(
				Context3DProgramType.FRAGMENT,
				0,
				_fragmentConstants
			);
			
			context.setTextureAt(
				0,
				_textureResources[0] ? _textureResources[0].getNativeTexture(context) : null
			);
			context.setTextureAt(
				1,
				_textureResources[1] ? _textureResources[0].getNativeTexture(context) : null
			);
			context.setTextureAt(
				2,
				_textureResources[2] ? _textureResources[0].getNativeTexture(context) : null
			);
			context.setTextureAt(
				3,
				_textureResources[3] ? _textureResources[0].getNativeTexture(context) : null
			);
			context.setTextureAt(
				4,
				_textureResources[4] ? _textureResources[0].getNativeTexture(context) : null
			);
			context.setTextureAt(
				5,
				_textureResources[5] ? _textureResources[0].getNativeTexture(context) : null
			);
			context.setTextureAt(
				6,
				_textureResources[6] ? _textureResources[0].getNativeTexture(context) : null
			);
			context.setTextureAt(
				7,
				_textureResources[7] ? _textureResources[0].getNativeTexture(context) : null
			);
			
			context.setVertexBufferAt(
				0,
				_vertexBuffers[0] ? _vertexBuffers[0].getVertexBuffer3D(context) : null,
				_offsets[0],
				_formats[0]
			);
			context.setVertexBufferAt(
				1,
				_vertexBuffers[1] ? _vertexBuffers[1].getVertexBuffer3D(context) : null,
				_offsets[1],
				_formats[1]
			);
			context.setVertexBufferAt(
				2,
				_vertexBuffers[2] ? _vertexBuffers[2].getVertexBuffer3D(context) : null,
				_offsets[2],
				_formats[2]
			);
			context.setVertexBufferAt(
				3,
				_vertexBuffers[3] ? _vertexBuffers[3].getVertexBuffer3D(context) : null,
				_offsets[3],
				_formats[3]
			);
			context.setVertexBufferAt(
				4,
				_vertexBuffers[4] ? _vertexBuffers[4].getVertexBuffer3D(context) : null,
				_offsets[4],
				_formats[4]
			);
			context.setVertexBufferAt(
				5,
				_vertexBuffers[5] ? _vertexBuffers[5].getVertexBuffer3D(context) : null,
				_offsets[5],
				_formats[5]
			);
			context.setVertexBufferAt(
				6,
				_vertexBuffers[6] ? _vertexBuffers[6].getVertexBuffer3D(context) : null,
				_offsets[6],
				_formats[6]
			);
			context.setVertexBufferAt(
				7,
				_vertexBuffers[7] ? _vertexBuffers[7].getVertexBuffer3D(context) : null,
				_offsets[7],
				_formats[7]
			);
			
			context.drawTriangles(_indexBuffer.getIndexBuffer3D(context));
			
			return _indexBuffer.numIndices / 3;
		}
		
		public function setParameter(name 	: String,
									 value 	: Object) : void
		{
			var binding : IParameterBinding = _nameToBinding[name] as IParameterBinding;
			
			if (binding)
			{
				binding.set(
					_vertexConstants,
					_fragmentConstants,
					null,
					value
				);
			}
		}
	}
}