package aerys.minko.type.parser.atf
{
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.texture.ATFTexture;
	import aerys.minko.type.parser.IParser;
	import aerys.minko.type.parser.ParserOptions;
	
	import flash.utils.ByteArray;
	
	public class ATFParser implements IParser
	{
		private var _data : Vector.<IScene>;
		
		public function get data() : Vector.<IScene>
		{
			return _data;
		}
		
		public function parse(data : ByteArray, options : ParserOptions) : Boolean
		{
			data.position = 0;
			
			if (data.readByte() == 'A'.charCodeAt(0) && 
				data.readByte() == 'T'.charCodeAt(0) && 
				data.readByte() == 'F'.charCodeAt(0))
			{
				data.position = 0;
				_data = Vector.<IScene>([new ATFTexture(data)]);
				
				return true;
			}
			else
			{
				data.position = 0;
				
				return false;
			}
		}
	}
}