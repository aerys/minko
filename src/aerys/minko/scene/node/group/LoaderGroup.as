package aerys.minko.scene.node.group
{
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.texture.BitmapTexture;
	import aerys.minko.scene.node.texture.ITexture;
	import aerys.minko.scene.node.texture.MovieClipTexture;
	import aerys.minko.type.parser.IParser;
	import aerys.minko.type.parser.ParserOptions;
	import aerys.minko.type.parser.atf.ATFParser;
	
	import flash.display.Bitmap;
	import flash.display.IBitmapDrawable;
	import flash.display.Loader;
	import flash.display.LoaderInfo;
	import flash.display.MovieClip;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	import flash.utils.describeType;

	/**
	 * The LoaderGroup class is the interface to load any 3D related content
	 * (scenes, textures, meshes, ...).
	 * 
	 * <p>
	 * The LoaderGroup content implements the IScene interface and, as such,
	 * can be considered just like any other scene graph node. Its behaviour
	 * will be exactly the same as the flash.display.Loader class when added
	 * to the Flash' display list.
	 * </p>
	 * 
	 * <p>
	 * Therefore, it is possible to use a LoaderGroup object to load 3D content
	 * asynchronously but add it immediately to the scene. When the content
	 * is loaded, it will be added as childs/descendants of the LoaderGroup
	 * object and will be used during the next rendering traversal.
	 * </p>
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class LoaderGroup extends Group implements IEventDispatcher
	{
		private static const NATIVE_FORMATS	: RegExp	= /^.*\.(swf|jpg|png)$/s
		private static const PARSERS		: Object	= new Object();
		private static const PARSER_CLASS	: String	= "aerys.minko.type.parser::IParser";
		
		private var _loaderToURI		: Dictionary		= new Dictionary(true);
		private var _positions			: Dictionary		= new Dictionary(true);
		private var _loaderToOptions	: Dictionary		= new Dictionary(true);
		
		private var _dispatcher			: EventDispatcher	= null;
		
		private var _total				: uint				= 0;
		private var _loaded				: uint				= 0;
		
		public static function load(request : URLRequest, parserOptions : ParserOptions = null) : LoaderGroup
		{
			return new LoaderGroup().load(request, parserOptions);
		}
		
		public static function loadClass(asset : Class, parserOptions : ParserOptions = null) : LoaderGroup
		{
			return new LoaderGroup().loadClass(asset, parserOptions);
		}
		
		public static function loadBytes(bytes : ByteArray, parserOptions : ParserOptions = null) : LoaderGroup
		{
			return new LoaderGroup().loadBytes(bytes, parserOptions);
		}
		
		public static function registerParser(extension 	: String,
											  parserClass 	: Class) : void
		{
			var interfaces : XMLList = describeType(parserClass).factory.implementsInterface;
			
			if (interfaces.(@type == PARSER_CLASS).length() == 0)
				throw new Error("The parser class must implement IParser.");
			
			PARSERS[extension] = parserClass;
		}
		
		public function LoaderGroup()
		{
			super();
			
			registerParser('atf', ATFParser);
			
			_dispatcher = new EventDispatcher(this);
		}
		
		/**
		 * Load the content corresponding to the specified URLRequest object.
		 *  
		 * @param request
		 * @return The LoaderGroup object itself.
		 * 
		 */
		public function load(request : URLRequest, parserOptions : ParserOptions = null) : LoaderGroup
		{
			var uri	: String	= request.url.toLocaleLowerCase();
			
			if (uri.match(NATIVE_FORMATS))
			{
				var loader : Loader = new Loader();
				
				_positions[loader] = numChildren;
				
				loader.contentLoaderInfo.addEventListener(Event.COMPLETE,
														  loaderCompleteHandler);
				loader.load(request);
			}
			else
			{
				var urlLoader 	: URLLoader	= new URLLoader();
				var extension	: String	= uri.substr(uri.lastIndexOf(".") + 1);
				var parserClass	: Class		= PARSERS[extension];
				
				if (!parserClass)
				{
					throw new Error("No data parser registered for extension '"
									+ extension + "'.");
				}

				_loaderToURI[urlLoader] = request.url;
				_positions[urlLoader] = numChildren;
				_loaderToOptions[urlLoader] = parserOptions;
				
				urlLoader.dataFormat = URLLoaderDataFormat.BINARY;
				urlLoader.addEventListener(Event.COMPLETE, urlLoaderCompleteHandler);
				urlLoader.load(request);
			}
			
			++_total;
			
			return this;
		}
		
		public function loadBytes(bytes : ByteArray, parserOptions : ParserOptions = null) : LoaderGroup
		{
			++_total;
			
			// try to find a parser
			for (var extension : String in PARSERS)
			{
				var parserClass : Class		= PARSERS[extension];
				var parser 		: IParser 	= new parserClass();
				
				bytes.position = 0;
				_positions[parser] = numChildren;
				
				parser.addEventListener(Event.COMPLETE, parserCompleteHandler);
				
				if (parser.parse(bytes, parserOptions))
					return this;
			}
			
			// no parser was found, try to load as a native format
			var loader 	: Loader 	= new Loader();
			
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, loaderCompleteHandler);
			loader.loadBytes(bytes);
			
			return this;
		}
		
		public function loadClass(asset : Class, parserOptions : ParserOptions = null) : LoaderGroup
		{
			var assetObject : Object 	= new asset();
			
			if (assetObject is MovieClip)
			{
				var mc 				: MovieClip	= assetObject as MovieClip;
				var contentLoader 	: Loader 	= null;
				
				if (mc.numChildren == 1 && (contentLoader = mc.getChildAt(0) as Loader))
				{
					var texture : MovieClipTexture = new MovieClipTexture();
					
					contentLoader.contentLoaderInfo.addEventListener(Event.COMPLETE,
					function(e : Event) : void
					{
						texture.source = contentLoader.content as MovieClip;
					});
					
					addChild(texture);
				}
				else
				{
					addChild(new MovieClipTexture(mc));
				}
			}
			else if (assetObject is IBitmapDrawable)
			{
				addChild(BitmapTexture.fromDisplayObject(assetObject as Bitmap));
			}
			else if (assetObject is ByteArray)
			{
				return loadBytes(assetObject as ByteArray, parserOptions);
			}
			
			return this;
		}
		
		private function urlLoaderCompleteHandler(event : Event) : void
		{
			var loader 		: URLLoader			= event.target as URLLoader;
			var uri			: String			= _loaderToURI[loader];
			var offset		: uint				= _positions[loader];
			var options		: ParserOptions		= _loaderToOptions[loader];
			var extension	: String			= uri.substr(uri.lastIndexOf(".") + 1);
			var parserClass	: Class				= PARSERS[extension.toLocaleLowerCase()];
			var parser		: IParser			= new parserClass();
			
			parser.addEventListener(Event.COMPLETE, parserCompleteHandler);
			_positions[parser] = numChildren;
			
			parser.parse(loader.data as ByteArray, options);
		}
		
		private function loaderCompleteHandler(event : Event) : void
		{
			var info 	: LoaderInfo 	= event.target as LoaderInfo;
			var texture	: ITexture 		= null;
			
			if (info.content is MovieClip)
				texture = new MovieClipTexture(info.content as MovieClip);
			else if (info.content is Bitmap)
				texture = new BitmapTexture((info.content as Bitmap).bitmapData);
			
			addChildAt(texture, _positions[info.loader]);
			
			complete()
		}
		
		private function parserCompleteHandler(event : Event) : void
		{
			var parser 		: IParser			= event.target as IParser;
			var data		: Vector.<IScene>	= parser.data;
			var numNodes	: int				= data ? data.length : 0;
			var offset		: int				= _positions[parser];
			
			for (var i : int = 0; i < numNodes; ++i)
				addChildAt(data[i], offset + i);
			
			complete();
		}
		
		private function complete() : void
		{
			++_loaded;
			
			if (_loaded == _total)
				dispatchEvent(new Event(Event.COMPLETE));
		}
	}
}