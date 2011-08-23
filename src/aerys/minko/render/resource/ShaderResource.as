package aerys.minko.render.resource
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.display3D.Context3D;
	import flash.display3D.Program3D;
	import flash.utils.ByteArray;
	
	public final class ShaderResource implements IResource
	{
		use namespace minko_render;
		
		private var _update			: Boolean	= false;
		private var _vertexShader	: ByteArray	= null;
		private var _fragmentShader	: ByteArray	= null;
		
		minko_render var _vertexComponents	: Vector.<VertexComponent> 	= null;
		minko_render var _vertexIndices		: Vector.<uint>				= null;
		minko_render var _nativeProgram		: Program3D					= null;
		
		public function ShaderResource(vertexShader 	: ByteArray,
									   fragmentShader	: ByteArray,
									   vertexComponents	: Vector.<VertexComponent>,
									   vertexIndices	: Vector.<uint>)
		{
			update(vertexShader, fragmentShader, vertexComponents, vertexIndices);
		}
		
		public function update(vertexShader 	: ByteArray,
							   fragmentShader	: ByteArray,
							   vertexComponents	: Vector.<VertexComponent>,
							   vertexIndices	: Vector.<uint>) : void
		{
			_vertexShader		= vertexShader;
			_fragmentShader		= fragmentShader;
			_vertexComponents	= vertexComponents.concat();
			_vertexIndices		= vertexIndices.concat();
			
			_update = true;
		}
		
		public function prepare(context : Context3D) : void
		{
			if (!_nativeProgram)
			{
				_nativeProgram = context.createProgram();
				_update = true;
			}
			
			if (_update)
			{
				_update = false;
				_nativeProgram.upload(_vertexShader, _fragmentShader);
			}
			
			context.setProgram(_nativeProgram);
		}
		
		public function dispose() : void
		{
			_nativeProgram.dispose();
		}
	}
}