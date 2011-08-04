package aerys.minko.render.ressource
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.display3D.Context3D;
	import flash.display3D.Program3D;
	import flash.utils.ByteArray;
	
	public class ShaderRessource implements IRessource
	{
		use namespace minko_render;
		
		private var _update			: Boolean	= false;
		private var _vertexShader	: ByteArray	= null;
		private var _fragmentShader	: ByteArray	= null;
		
		minko_render var _vertexInput	: Vector.<VertexComponent> 	= null;
		minko_render var _nativeProgram	: Program3D					= null;
		
		public function ShaderRessource(vertexShader 	: ByteArray,
							   fragmentShader	: ByteArray,
							   vertexInput		: Vector.<VertexComponent>)
		{
			update(vertexShader, fragmentShader, vertexInput);
		}
		
		public function update(vertexShader 	: ByteArray,
							   fragmentShader	: ByteArray,
							   vertexInput		: Vector.<VertexComponent>) : void
		{
			_vertexShader = vertexShader;
			_fragmentShader = fragmentShader;
			_vertexInput = vertexInput.concat();
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