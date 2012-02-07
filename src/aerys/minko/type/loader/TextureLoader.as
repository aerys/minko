package aerys.minko.type.loader
{
	import aerys.minko.render.resource.texture.TextureResource;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.IBitmapDrawable;
	import flash.display.Loader;
	import flash.events.Event;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;

	public final class TextureLoader
	{
		public static function loadClass(classObject 		: Class,
										 enableMipMapping 	: Boolean = true) : TextureResource
		{
			return new TextureLoader().loadClass(classObject, enableMipMapping);
		}
		
		public function TextureLoader()
		{
		}
		
		public function loadClass(classObject 		: Class,
								  enableMipMapping 	: Boolean = true) : TextureResource
		{
			var assetObject : Object 			= new classObject();
			
			
			if (assetObject is BitmapData)
			{
				return getResourceFromBitmapData(
					assetObject as BitmapData,
					enableMipMapping
				);
			}
			else if (assetObject is IBitmapDrawable)
			{
				return getResourceFromBitmapData(
					(assetObject as Bitmap).bitmapData,
					enableMipMapping
				);
			}
			else if (assetObject is ByteArray)
			{
				return getResourceFromByteArray(
					assetObject as ByteArray,
					enableMipMapping
				);
			}
			
			throw new Error();
		}
		
		public function load(request 			: URLRequest,
							 enableMipMapping 	: Boolean = true) : TextureResource
		{
			var loader 		: Loader			= new Loader();
			var resource 	: TextureResource 	= new TextureResource();
			
			loader.contentLoaderInfo.addEventListener(
				Event.COMPLETE,
				getLoaderCompleteHandler(resource, enableMipMapping)
			);
			loader.load(request);
			
			return resource;
		}
		
		private function getResourceFromBitmapData(source 			: BitmapData,
												   enableMipMapping	: Boolean) : TextureResource
		{
			var resource	: TextureResource	= new TextureResource();
			
			resource.setContentFromBitmapData(source, enableMipMapping);
			
			return resource;
		}
		
		private function getResourceFromByteArray(bytes 			: ByteArray,
												  enableMipMapping 	: Boolean) : TextureResource
		{
			var loader 		: Loader 			= new Loader();
			var resource 	: TextureResource 	= new TextureResource();
			
			bytes.position = 0;
			
			if (bytes.readByte() == 'A'.charCodeAt(0) &&
				bytes.readByte() == 'T'.charCodeAt(0) &&
				bytes.readByte() == 'F'.charCodeAt(0))
			{
				bytes.position = 0;
				resource.setContentFromATF(bytes);
			}
			else
			{
				loader.contentLoaderInfo.addEventListener(
					Event.COMPLETE,
					getLoaderCompleteHandler(resource, enableMipMapping)
				);
				loader.loadBytes(bytes);
			}
			
			return resource;
		}
		
		private function getLoaderCompleteHandler(resource 			: TextureResource,
												  enableMipMapping 	: Boolean) : Function
		{
			return function(e : Event) : void
			{
				resource.setContentFromBitmapData(
					((e.target as Loader).content as Bitmap).bitmapData,
					enableMipMapping
				);
			};
		}
	}
}