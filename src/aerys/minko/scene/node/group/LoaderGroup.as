package aerys.minko.scene.node.group
{
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.texture.BitmapTexture;
	import aerys.minko.scene.node.texture.ITexture;
	import aerys.minko.scene.node.texture.MovieClipTexture;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.type.parser.IParser3D;
	
	import flash.display.Bitmap;
	import flash.display.IBitmapDrawable;
	import flash.display.Loader;
	import flash.display.LoaderInfo;
	import flash.display.MovieClip;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.globalization.LastOperationStatus;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	import aerys.minko.scene.node.IScene;

	/**
	 * The Loader3D class is the interface to load any 3D related content
	 * (scenes, textures, meshes, ...).
	 * 
	 * <p>
	 * The Loader3D content implements the IScene interface and, as such,
	 * can be considered just like any other scene graph node. Its behaviour
	 * will be exactly the same as the flash.display.Loader class when added
	 * to the Flash' display list.
	 * </p>
	 * 
	 * <p>
	 * Therefore, it is possible to use a Loader3D object to load 3D content
	 * asynchronously but add it immediately to the scene. When the content
	 * is loaded, it will be added as childs/descendants of the Loader3D
	 * object and will be used during the next rendering traversal.
	 * </p>
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class LoaderGroup extends Group implements IEventDispatcher
	{
		private static const FORMATS	: RegExp	= /^.*\.(swf|jpg|png)$/s
		private static const PARSERS	: Object	= new Object();
		
		private var _loaderToURI		: Dictionary		= new Dictionary(true);
		private var _loaderToPosition	: Dictionary		= new Dictionary(true);
		
		private var _dispatcher			: EventDispatcher	= null;
		
		private var _total				: uint				= 0;
		private var _loaded				: uint				= 0;
		
		public static function registerParser(extension : String,
											  parser 	: IParser3D) : void
		{
			PARSERS[extension] = parser;
		}
		
		public function LoaderGroup()
		{
			super();
			
			_dispatcher = new EventDispatcher(this);
		}
		
		/**
		 * Load the content corresponding to the specified URLRequest object.
		 *  
		 * @param request
		 * @return The Loader3D object itself.
		 * 
		 */
		public function load(request : URLRequest) : LoaderGroup
		{
			if (request.url.match(FORMATS))
			{
				var loader : Loader = new Loader();
				
				_loaderToPosition[loader] = numChildren;
				
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
				_loaderToPosition[loader] = numChildren;
				
				urlLoader.dataFormat = URLLoaderDataFormat.BINARY;
				urlLoader.addEventListener(Event.COMPLETE, urlLoaderCompleteHandler);
				urlLoader.load(request);
			}
			
			++_total;
			
			return this;
		}
		
		public function loadBytes(bytes : ByteArray) : LoaderGroup
		{
			for (var extension : String in PARSERS)
			{
				var parser : IParser3D = PARSERS[extension];
				
				bytes.position = 0;
				
				if (parser.parse(bytes))
				{
					var data	: Vector.<IScene>	= parser.data;
					var length	: int				= data ? data.length : 0;
					
					for (var i : int = 0; i < length; ++i)
						addChild(data[i]);
					
					return this;
				}
			}
			
			var loader 	: Loader 	= new Loader();
			
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, loaderCompleteHandler);
			loader.loadBytes(bytes);
			
			return this;
		}
		
		public function loadAsset(asset : Class) : LoaderGroup
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
				return loadBytes(assetObject as ByteArray);
			}
			
			return this;
		}
		
		public static function loadBytes(bytes : ByteArray) : LoaderGroup
		{
			return new LoaderGroup().loadBytes(bytes);
		}
		
		public static function loadAsset(asset : Class) : LoaderGroup
		{
			return new LoaderGroup().loadAsset(asset);
		}
		
		private function urlLoaderCompleteHandler(event : Event) : void
		{
			var loader 		: URLLoader			= event.target as URLLoader;
			var uri			: String			= _loaderToURI[loader];
			var offset		: uint				= _loaderToPosition[loader];
			var extension	: String			= uri.substr(uri.lastIndexOf(".") + 1);
			var parser		: IParser3D			= PARSERS[extension.toLocaleLowerCase()];
			var data		: Vector.<IScene>	= parser.parse(loader.data as ByteArray)
												  ? parser.data
												  : null;
			var length		: int				= data ? data.length : 0;
			
			for (var i : int = 0; i < length; ++i)
				addChildAt(data[i], offset + i);
			
			++_loaded;

			if (_loaded == _total)
				dispatchEvent(new Event(Event.COMPLETE));
		}
		
		private function loaderCompleteHandler(event : Event) : void
		{
			var info 	: LoaderInfo 	= event.target as LoaderInfo;
			var texture	: ITexture 		= null;
			
			if (info.content is MovieClip)
				texture = new MovieClipTexture(info.content as MovieClip);
			else if (info.content is Bitmap)
				texture = new BitmapTexture((info.content as Bitmap).bitmapData);
			
			addChildAt(texture, _loaderToPosition[info.loader]);
			
			++_loaded;
			
			if (_loaded == _total)
				dispatchEvent(new Event(Event.COMPLETE));
		}
		
		public function addEventListener(type 				: String,
										 listener 			: Function,
										 useCapture 		: Boolean	= false, 
										 priority 			: int		= 0,
										 useWeakReference 	: Boolean	= false) : void
		{
			_dispatcher.addEventListener(type, listener, useCapture, priority, useWeakReference);
		}
		
		public function removeEventListener(type : String, listener : Function, useCapture : Boolean = false) : void
		{
			_dispatcher.removeEventListener(type, listener, useCapture);
		}
		
		public function dispatchEvent(event : Event) : Boolean
		{
			return _dispatcher.dispatchEvent(event);
		}
		
		public function hasEventListener(type : String) : Boolean
		{
			return _dispatcher.hasEventListener(type);
		}
		
		public function willTrigger(type : String) : Boolean
		{
			return _dispatcher.willTrigger(type);
		}
	}
}