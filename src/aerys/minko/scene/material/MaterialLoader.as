package aerys.minko.scene.material
{
	import aerys.minko.scene.material.parser.IMaterialParser;
	
	import flash.display.Bitmap;
	import flash.display.IBitmapDrawable;
	import flash.display.Loader;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	
	[Event(name="complete", type="flash.events.Event")]
	
	public class MaterialLoader extends EventDispatcher
	{
		private var _parser 	: IMaterialParser	= null;		
		private var _material	: IMaterial3D			= null;
		
		public function get material() : IMaterial3D
		{
			return _material;
		}
		
		public function get parser() : IMaterialParser
		{
			return _parser;
		}
		
		public function MaterialLoader(myParser : IMaterialParser = null)
		{
			super();
		
			_parser = myParser;
		}
		
		public function loadByteArray(myData : ByteArray) : IMaterial3D
		{
			_parser.parse(myData);
			_material = _parser.material;
		
			return (_material);
		}

		public function loadAsset(myAsset : Class) : IMaterial3D
		{
			var asset	: Object	= new myAsset();
			
			if (_parser)
				return loadByteArray(asset as ByteArray);
	
			if (asset is IBitmapDrawable)
				_material = BitmapMaterial3D.fromDisplayObject(asset as Bitmap);
			else if (asset is ByteArray)
				_material = BitmapMaterial3D.fromByteArray(asset as ByteArray);
			
			return _material;
		}
		
		public function load(myRequest : URLRequest) : void
		{
			if (_parser)
			{
				var urlLoader : URLLoader	= new URLLoader();
				
				urlLoader.dataFormat = URLLoaderDataFormat.BINARY;
				
				urlLoader.addEventListener(Event.COMPLETE, completeHandler);
				urlLoader.load(myRequest);
			}
			else
			{
				var loader : Loader = new Loader();
				
				loader.contentLoaderInfo.addEventListener(Event.COMPLETE, completeHandler);
				loader.load(myRequest);
			}			
			
		}
		
		private function completeHandler(event : Event) : void
		{
			if (_parser)
				loadByteArray((event.target as URLLoader).data as ByteArray);
			else
				_material = new BitmapMaterial3D(event.target.content.bitmapData);
			
			dispatchEvent(new Event(Event.COMPLETE));
		}

		public static function loadAsset(myAsset 	: Class,
										 myParser 	: IMaterialParser = null) : IMaterial3D
		{
			return new MaterialLoader(myParser).loadAsset(myAsset);
		}
		
		public static function loadByteArray(myData 	: ByteArray,
											 myParser	: IMaterialParser = null) : IMaterial3D
		{
			return new MaterialLoader(myParser).loadByteArray(myData);
		}

	}
}