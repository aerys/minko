package aerys.minko.data
{
	import aerys.minko.data.parser.IParser3D;
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.scene.interfaces.IScene3D;
	import aerys.minko.scene.group.Group3D;
	import aerys.minko.scene.material.AnimatedMaterial3D;
	import aerys.minko.scene.material.BitmapMaterial3D;
	import aerys.minko.scene.material.IMaterial3D;
	import aerys.minko.scene.material.MovieClipMaterial3D;
	
	import flash.display.Bitmap;
	import flash.display.IBitmapDrawable;
	import flash.display.Loader;
	import flash.display.LoaderInfo;
	import flash.display.MovieClip;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;

	public class Loader3D extends EventDispatcher implements IScene3D
	{
		private static const FORMATS	: RegExp	= /^.*\.(swf|jpg|png)$/s
		private static const PARSERS	: Object	= new Object();
		
		private var _loaderToURI		: Dictionary		= new Dictionary(true);
		private var _content			: Group3D			= new Group3D();
		
		public function get name()	 	: String	{ return _content.name; }
		public function get content() 	: Group3D	{ return _content; }
		
		public static function registerParser(extension : String,
											  parser 	: IParser3D) : void
		{
			PARSERS[extension] = parser;
		}
		
		public function Loader3D()
		{
			
		}
		
		public function load(request : URLRequest) : Group3D
		{
			if (request.url.match(FORMATS))
			{
				var loader : Loader = new Loader();
				
				loader.contentLoaderInfo.addEventListener(Event.COMPLETE,
														  loaderCompleteHandler);
				loader.load(request);
			}
			else
			{
				var urlLoader 	: URLLoader	= new URLLoader();
				var uri			: String	= request.url;
				var extension	: String	= uri.substr(uri.lastIndexOf(".") + 1);
				var parser		: IParser3D	= PARSERS[extension.toLocaleLowerCase()];
				
				if (!parser)
					throw new Error("No data parser registered for extension '" + extension + "'");

				_loaderToURI[urlLoader] = request.url;
				
				urlLoader.dataFormat = URLLoaderDataFormat.BINARY;
				urlLoader.addEventListener(Event.COMPLETE, urlLoaderCompleteHandler);
				urlLoader.load(request);
			}
			
			return _content;
		}
		
		public static function loadBytes(bytes : ByteArray) : Group3D
		{
			for (var extension : String in PARSERS)
			{
				var parser : IParser3D = PARSERS[extension];
				
				bytes.position = 0;
				
				if (parser.parse(bytes))
				{
					var data	: Vector.<IScene3D>	= parser.data;
					var length	: int				= data ? data.length : 0;
					var content : Group3D			= new Group3D();

					for (var i : int = 0; i < length; ++i)
						content.addChild(data[i]);
					
					return content;
				}
			}
			
			throw new Error("Unable to find a proper data parser.");
			
			return null;
		}
		
		public static function loadAsset(asset : Class) : Group3D
		{
			var assetObject : Object 	= new asset();
			var content 	: Group3D 	= null;
			
			if (assetObject is MovieClip)
			{
				var mc : MovieClip = assetObject as MovieClip;
				var loader : Loader = null;
				
				if (mc.numChildren == 1 && (loader = mc.getChildAt(0) as Loader))
				{
					var mat : AnimatedMaterial3D = new AnimatedMaterial3D();
					
					loader.contentLoaderInfo.addEventListener(Event.COMPLETE,
															  function(e : Event) : void
					{
						mat.addChild(new MovieClipMaterial3D(loader.content as MovieClip));
					});
					
					
					content = new Group3D(mat);
				}
				else
				{
					content = new Group3D(new MovieClipMaterial3D(mc));
				}
			}
			else if (assetObject is IBitmapDrawable)
			{
				content = new Group3D(BitmapMaterial3D.fromDisplayObject(assetObject as Bitmap));
			}
			else if (assetObject is ByteArray)
			{
				content = loadBytes(assetObject as ByteArray);
			}
			
			return content;
		}
		
		private function urlLoaderCompleteHandler(event : Event) : void
		{
			var loader 		: URLLoader			= event.target as URLLoader;
			var uri			: String			= _loaderToURI[loader];
			var extension	: String			= uri.substr(uri.lastIndexOf(".") + 1);
			var parser		: IParser3D			= PARSERS[extension.toLocaleLowerCase()];
			var data		: Vector.<IScene3D>	= parser.parse(loader.data as ByteArray)
												  ? parser.data
												  : null;
			var length		: int				= data ? data.length : 0;
			
			_content.removeAllChildren();
			for (var i : int = 0; i < length; ++i)
				_content.addChild(data[i]);

			dispatchEvent(new Event(Event.COMPLETE));
		}
		
		private function loaderCompleteHandler(event : Event) : void
		{
			var info 	: LoaderInfo 	= event.target as LoaderInfo;
			var mat 	: IMaterial3D 	= null;
						
			if (info.content is MovieClip)
				mat = new MovieClipMaterial3D(info.content as MovieClip);
			else if (info.content is Bitmap)
				mat = new BitmapMaterial3D((info.content as Bitmap).bitmapData);
			
			_content.removeAllChildren();
			_content.addChild(mat);
			
			dispatchEvent(new Event(Event.COMPLETE));
		}
		
		public function accept(query : IScene3DQuery) : void
		{
			query.query(_content);
		}
	}
}