package aerys.minko.type.loader
{
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.type.Signal;
	
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
	import flash.system.LoaderContext;
	import flash.utils.ByteArray;
	import flash.utils.getQualifiedClassName;

	public final class TextureLoader implements ILoader
	{
		private var _progress			: Signal;
		private var _error				: Signal;
		private var _complete			: Signal;
		
		private var _mipmap				: Boolean;
		private var _textureResource	: TextureResource;
		
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
		
		public function get textureResource() : TextureResource
		{
			return _textureResource;
		}
		
//		public static function loadClass(classObject 		: Class,
//										 enableMipMapping 	: Boolean = true) : TextureLoader
//		{
//			var textureLoader : TextureLoader = new TextureLoader(enableMipMapping);
//			textureLoader.loadClass(classObject);
//			return textureLoader;
//		}
		
		public function TextureLoader(enableMipmapping : Boolean = true)
		{
			_mipmap		= enableMipmapping;
			
			_error		= new Signal();
			_progress	= new Signal();
			_complete	= new Signal();
		}
		
		public function load(request : URLRequest) : void
		{
			var loader : URLLoader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(ProgressEvent.PROGRESS, onLoadProgressHandler);
			loader.addEventListener(Event.COMPLETE, onLoadCompleteHandler);
			loader.addEventListener(IOErrorEvent.IO_ERROR, onLoadIoErrorEvent);
			loader.load(request);
		}
		
		private function onLoadIoErrorEvent(e : IOErrorEvent) : void
		{
			_error.execute(this, e.errorID, e.text);
		}
		
		private function onLoadProgressHandler(e : ProgressEvent) : void
		{
			_progress.execute(this, e.bytesLoaded, e.bytesTotal);
		}
		
		private function onLoadCompleteHandler(e : Event) : void
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
				
				_textureResource = new TextureResource();
				_textureResource.setContentFromBitmapData(bitmapData, _mipmap);
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
				
				throw new Error('No texture can be created from an object of type \'' + className + '\'');
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
				_textureResource = new TextureResource();
				_textureResource.setContentFromATF(bytes);
				_complete.execute(this, _textureResource);
			}
			else
			{
				bytes.position = 0;
				
				var loader : Loader = new Loader();
				loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onLoadBytesComplete);
				loader.loadBytes(bytes);
			}
		}
		
		private function onLoadBytesComplete(e : Event) : void
		{
			var displayObject : DisplayObject = LoaderInfo(e.currentTarget).content;
			
			if (displayObject is Bitmap)
			{
				_textureResource = new TextureResource();
				_textureResource.setContentFromBitmapData(Bitmap(displayObject).bitmapData, _mipmap);
				_complete.execute(this, _textureResource);
			}
			else
			{
				var className : String = getQualifiedClassName(displayObject);
				className = className.substr(className.lastIndexOf(':') + 1);
				
				throw new Error('No texture can be created from an object of type \'' + className + '\'');
			}
		}
	}
}