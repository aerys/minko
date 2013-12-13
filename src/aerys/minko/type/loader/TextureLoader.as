package aerys.minko.type.loader
{
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	import flash.display.Loader;
	import flash.display.LoaderInfo;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	import flash.utils.getQualifiedClassName;
	
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.type.Signal;

	public class TextureLoader implements ILoader
	{
		private var _mipMapping	: Boolean;
		
		private var _progress	: Signal;
		private var _error		: Signal;
		private var _complete	: Signal;
		
		private var _isComplete	: Boolean;
		
		protected var _textureResource	: TextureResource;
		
		private static var _textureResourceToClass		: Dictionary = new Dictionary();
		private static var _textureResourceToRequest	: Dictionary = new Dictionary();
		
		public function get progress() : Signal
		{
			return _progress;
		}
		
		public function get error() : Signal
		{
			return _error;
		}
		
		public function get complete() : Signal
		{
			return _complete;
		}
		
		public function get isComplete() : Boolean
		{
			return _isComplete;
		}
		
		public function get textureResource() : TextureResource
		{
			return _textureResource;
		}
		
		public function TextureLoader(enableMipmapping : Boolean = true, textureResource : TextureResource = null)
		{
			_mipMapping	= enableMipmapping;
			
			initialize(textureResource);
		}
		
		private function initialize(textureResource : TextureResource) : void
		{
			_textureResource 	= textureResource ? textureResource : new TextureResource();
			_textureResource.setContentFromBitmapData(TextureResource.EMPTY_BITMAPDATA, _mipMapping);
			
			_isComplete 		= false;
			_error				= new Signal('TextureLoader.error');
			_progress			= new Signal('TextureLoader.progress');
			_complete			= new Signal('TextureLoader.complete');
		}
		
		public function load(request : URLRequest) : void
		{
			var loader : URLLoader = new URLLoader();
			
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(ProgressEvent.PROGRESS, loadProgressHandler);
			loader.addEventListener(Event.COMPLETE, loadCompleteHandler);
			loader.addEventListener(IOErrorEvent.IO_ERROR, loadIoErrorHandler);
			loader.load(request);
			
			if (!_textureResource.contextLost.hasCallback(urlRequestTexturecontextLostHandler))
			{
				_textureResourceToRequest[_textureResource] = request;
				_textureResource.contextLost.add(urlRequestTexturecontextLostHandler)
			}
		}
		
		private function loadIoErrorHandler(e : IOErrorEvent) : void
		{
			_textureResource = null;
			
			if (_error.numCallbacks == 0)
				throw e;
			else
				_error.execute(this, e.errorID, e.text);
		}
		
		private function loadProgressHandler(e : ProgressEvent) : void
		{
			_progress.execute(this, e.bytesLoaded, e.bytesTotal);
		}
		
		private function loadCompleteHandler(e : Event) : void
		{
			loadBytes(URLLoader(e.currentTarget).data);
		}
		
		public function loadClass(classObject : Class) : void
		{			
			var assetObject : Object		= new classObject();
			
			if (assetObject is Bitmap || assetObject is BitmapData)
			{
				var bitmapData : BitmapData = assetObject as BitmapData;
				
				if (bitmapData == null)
					bitmapData = Bitmap(assetObject).bitmapData;
				
				_textureResource.setContentFromBitmapData(bitmapData, _mipMapping);
				bitmapData.dispose();
				
				_isComplete = true;
				_complete.execute(this, _textureResource);
			}
			else if (assetObject is ByteArray)
			{
				loadBytes(ByteArray(assetObject));
			}
			else
			{
				var className : String = getQualifiedClassName(classObject);
				className = className.substr(className.lastIndexOf(':') + 1);
				
				_isComplete = true;
				
				throw new Error('No texture can be created from an object of type \'' + className + '\'');
			}
			
			if (!_textureResource.contextLost.hasCallback(classTexturecontextLostHandler))
			{
				_textureResourceToClass[_textureResource] = classObject;
				_textureResource.contextLost.add(classTexturecontextLostHandler);
			}
		}
		
		public function loadBytes(bytes : ByteArray) : void
		{
			bytes.position = 0;
			
			if (bytes.readByte() == 'A'.charCodeAt(0) &&
				bytes.readByte() == 'T'.charCodeAt(0) &&
				bytes.readByte() == 'F'.charCodeAt(0))
			{
				bytes.position = 0;
				_textureResource.setContentFromATF(bytes);
				_isComplete = true;
				_complete.execute(this, _textureResource);
			}
			else
			{
				bytes.position = 0;
				
				var loader : Loader = new Loader();
				
				loader.contentLoaderInfo.addEventListener(Event.COMPLETE, loadBytesCompleteHandler);
				loader.loadBytes(bytes);
			}
		}
		
		private function loadBytesCompleteHandler(e : Event) : void
		{
			var displayObject : DisplayObject = LoaderInfo(e.currentTarget).content;
			
			if (displayObject is Bitmap)
			{
				var bitmapData : BitmapData = Bitmap(displayObject).bitmapData;
				
				_textureResource.setContentFromBitmapData(bitmapData, _mipMapping);
				bitmapData.dispose();
				
				_isComplete = true;
				_complete.execute(this, _textureResource);
			}
			else
			{
				var className : String = getQualifiedClassName(displayObject);
				
				className = className.substr(className.lastIndexOf(':') + 1);
				_isComplete = true;
				
				throw new Error(
					'No texture can be created from an object of type \'' + className + '\''
				);
			}
		}
		
		public static function loadClass(classObject 			: Class,
										 enableMipMapping 		: Boolean = true, 
										 textureResource		: TextureResource = null) : TextureResource
		{
			var textureLoader : TextureLoader = new TextureLoader(enableMipMapping, textureResource);
			
			textureLoader.loadClass(classObject);
			
			return textureLoader.textureResource;
		}
		
		public static function load(request				: URLRequest,
									enableMipMapping 	: Boolean = true, 
									textureResource		: TextureResource = null) : TextureResource
		{
			var textureLoader : TextureLoader = new TextureLoader(enableMipMapping, textureResource);
			
			textureLoader.load(request);
						
			return textureLoader.textureResource;
		}
		
		public static function loadBytes(bytes				: ByteArray,
										 enableMipMapping 	: Boolean = true, 
										 textureResource	: TextureResource = null) : TextureResource
		{
			var textureLoader : TextureLoader = new TextureLoader(enableMipMapping, textureResource);
			
			textureLoader.loadBytes(bytes);
			
			return textureLoader.textureResource;
		}
		
		private static function classTexturecontextLostHandler(textureResource : TextureResource) : void
		{
			TextureLoader.loadClass(_textureResourceToClass[textureResource], textureResource.mipMapping, textureResource);
		}
		private static function urlRequestTexturecontextLostHandler(textureResource : TextureResource) : void
		{
			TextureLoader.load(_textureResourceToRequest[textureResource], textureResource.mipMapping, textureResource);
		}
	}
}