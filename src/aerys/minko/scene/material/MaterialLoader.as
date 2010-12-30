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
		
		public function MaterialLoader(parser : IMaterialParser = null)
		{
			super();
		
			_parser = parser;
		}
		
		public function loadByteArray(data : ByteArray) : IMaterial3D
		{
			_parser.parse(data);
			_material = _parser.material;
		
			return (_material);
		}

		public function loadAsset(asset : Class) : IMaterial3D
		{
			var assetObject	: Object	= new asset();
			
			if (_parser)
				return loadByteArray(assetObject as ByteArray);
	
			if (assetObject is IBitmapDrawable)
				_material = NativeMaterial3D.fromDisplayObject(assetObject as Bitmap);
			else if (assetObject is ByteArray)
				_material = NativeMaterial3D.fromByteArray(assetObject as ByteArray);
			
			return _material;
		}
		
		public function load(request : URLRequest) : void
		{
			if (_parser)
			{
				var urlLoader : URLLoader	= new URLLoader();
				
				urlLoader.dataFormat = URLLoaderDataFormat.BINARY;
				
				urlLoader.addEventListener(Event.COMPLETE, completeHandler);
				urlLoader.load(request);
			}
			else
			{
				var loader : Loader = new Loader();
				
				loader.contentLoaderInfo.addEventListener(Event.COMPLETE, completeHandler);
				loader.load(request);
			}			
			
		}
		
		private function completeHandler(event : Event) : void
		{
			if (_parser)
				loadByteArray((event.target as URLLoader).data as ByteArray);
			else
				_material = new NativeMaterial3D(event.target.content.bitmapData);
			
			dispatchEvent(new Event(Event.COMPLETE));
		}

		public static function loadAsset(asset 	: Class,
										 parser	: IMaterialParser = null) : IMaterial3D
		{
			return new MaterialLoader(parser).loadAsset(asset);
		}
		
		public static function loadByteArray(data 	: ByteArray,
											 parser	: IMaterialParser = null) : IMaterial3D
		{
			return new MaterialLoader(parser).loadByteArray(data);
		}

	}
}