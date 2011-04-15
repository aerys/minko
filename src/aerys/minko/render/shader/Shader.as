package aerys.minko.render.shader
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.Program3D;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class Shader
	{
		use namespace minko;
		
		private var _vertexShader	: ByteArray	= null;
		private var _fragmentShader	: ByteArray	= null;
		
		minko var _vertexInput		: Vector.<VertexComponent> = null;
		minko var _nativeProgram	: Program3D	= null;
		
		public function Shader(vertexShader 	: ByteArray,
								 fragmentShader : ByteArray,
								 vertexInput	: Vector.<VertexComponent>)
		{
			_vertexShader	= vertexShader;
			_fragmentShader	= fragmentShader;
			_vertexInput	= vertexInput;
		}
		
		public static function fromByteArray(myData 	: ByteArray,
											 vertexInput: Vector.<VertexComponent>) : Shader
		{
			var vs : ByteArray = new ByteArray();
			var fs : ByteArray = new ByteArray();
			
			myData.readBytes(vs, 0, myData.readInt());
			myData.readBytes(fs, 0, myData.readInt());
			
			vs.endian = Endian.LITTLE_ENDIAN;
			fs.endian = Endian.LITTLE_ENDIAN;
			
			return new Shader(vs, fs, vertexInput);
		}
		
		public function prepare(context : Context3D) : void
		{
			if (!_nativeProgram)
			{
				_nativeProgram = context.createProgram();
				_nativeProgram.upload(_vertexShader, _fragmentShader);
			}
			
			context.setProgram(_nativeProgram);
		}
		
	}
}