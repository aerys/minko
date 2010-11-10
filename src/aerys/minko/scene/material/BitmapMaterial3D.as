package aerys.minko.scene.material
{
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.IScene3DVisitor;
	import aerys.minko.render.NativeRenderer3D;
	import aerys.minko.scene.AbstractScene3D;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	import flash.display.GraphicsBitmapFill;
	import flash.display.GraphicsStroke;
	import flash.display.IGraphicsData;
	import flash.display.Loader;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.Texture3D;
	import flash.events.Event;
	import flash.geom.Matrix;
	import flash.utils.ByteArray;
	
	/**
	 * The BitmapMaterial class enables the use of BitmapData and DisplayObject objects as materials.
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public class BitmapMaterial3D extends AbstractScene3D implements IMaterial3D
	{
		private var _bitmapData	: BitmapData	= null;
		private var _texture	: Texture3D		= null;

		/**
		 * A transparent or opaque bitmap image.
		 *
		 * @return
		 *
		 */
		public function get bitmapData() : BitmapData
		{
			return _bitmapData;
		}
		
		public function set bitmapData(value : BitmapData) : void
		{
			_bitmapData = value;
		}
			
		/**
		 * Creates a new BitmapMaterial object.
		 *
		 * @param myBitmapData
		 *
		 */
		public function BitmapMaterial3D(myBitmapData : BitmapData = null)
		{
			_bitmapData = myBitmapData;
		}
		
		override public function visited(myVisitor : IScene3DVisitor) : void
		{
			var renderer	: IRenderer3D	= myVisitor.renderer;
			var context 	: Context3D 	= myVisitor.renderer.viewport.context;
			
			
			if (!_texture)
			{
				_texture = context.createTexture(_bitmapData.width,
												 _bitmapData.height,
												 Context3DTextureFormat.BGRA,
												 false);
				
				_texture.upload(_bitmapData, 0);
			}
			
			renderer.textures[0] = _texture;
		}
		
		//{ region statis
		public static function fromDisplayObject(mySource : DisplayObject, mySize : int = 0, mySmooth : Boolean = false) : BitmapMaterial3D
		{
			var bmp : BitmapData = new BitmapData(mySize || mySource.width,
												  mySize || mySource.height,
												  true,
												  0);
			var matrix : Matrix = null;
			
			if (mySize)
			{
				matrix = new Matrix();
				matrix.scale(mySource.width / mySize, mySource.height / mySize);
			}
			
			bmp.draw(mySource, matrix, null, null, null, mySmooth);
			
			return new BitmapMaterial3D(bmp);
		}
		
		public static function fromByteArray(mySource : ByteArray) : BitmapMaterial3D
		{
			var loader 	: Loader 			= new Loader();
			var mat 	: BitmapMaterial3D 	= new BitmapMaterial3D();
			
			loader.loadBytes(mySource);
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, mat.loadCompleteHandler);
			
			return mat;
		}
		
		private function loadCompleteHandler(event : Event) : void
		{
			_bitmapData = (event.target.content as Bitmap).bitmapData;
		}
		//} endregion
	}
}