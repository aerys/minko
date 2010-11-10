package aerys.minko.render.shader
{
	import flash.display3D.Context3D;
	import flash.display3D.Program3D;
	import flash.utils.ByteArray;
	import flash.utils.Endian;
	
	public class Shader3D
	{
		private var _vertexShader	: ByteArray	= null;
		private var _fragmentShader	: ByteArray	= null;
		
		private var _nativeProgram	: Program3D	= null;
		
		public function Shader3D(myVertexShader 	: ByteArray,
								 myFragmentShader 	: ByteArray)
		{
			_vertexShader = myVertexShader;
			_fragmentShader = myFragmentShader;
		}
		
		public static function fromByteArray(myData : ByteArray) : Shader3D
		{
			var vs : ByteArray = new ByteArray();
			var fs : ByteArray = new ByteArray();
			
			myData.readBytes(vs, 0, myData.readInt());
			myData.readBytes(fs, 0, myData.readInt());
			
			vs.endian = Endian.LITTLE_ENDIAN;
			fs.endian = Endian.LITTLE_ENDIAN;
			
			return new Shader3D(vs, fs);
		}
		
		public function prepareContext(context : Context3D) : void
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