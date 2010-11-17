package aerys.minko.scene.mesh
{
	import aerys.minko.scene.mesh.parser.IMeshParser;
	
	import flash.events.Event;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.utils.ByteArray;

	public class Mesh3DLoader extends URLLoader
	{
		private var _parser : IMeshParser	= null;
		
		public function get meshes() : Vector.<IMesh3D>
		{
			return _parser.meshes;
		}
		
		public function get parser() : IMeshParser
		{
			return _parser;
		}
		
		public function Mesh3DLoader(myParserClass : Class)
		{
			super ();
		
			dataFormat = URLLoaderDataFormat.BINARY;
			
			addEventListener(Event.COMPLETE, completeHandler, false, 1);
			
			_parser = new myParserClass();
		}
		
		public function loadAsset(myAsset : Class) : void
		{
			loadByteArray(new myAsset() as ByteArray);
		}
		
		public function loadByteArray(myData : ByteArray) : void
		{
			_parser.parse(myData);
		}
		
		private function completeHandler(event : Event) : void
		{
			loadByteArray(data);
		}
		
		public static function loadAsset(asset : Class, parser : Class = null) : Vector.<IMesh3D>
		{
			return loadByteArray(new asset() as ByteArray, parser);
		}
		
		public static function loadByteArray(myData : ByteArray, myParser : Class = null) : Vector.<IMesh3D>
		{
			if (myParser)
			{
				var parser : IMeshParser = new myParser();
				
				parser.parse(myData);
				
				return parser.meshes;
			}
			else
			{
				myData.inflate();
				
				return Vector.<IMesh3D>([myData.readObject()]);
			}
		}
	}
}