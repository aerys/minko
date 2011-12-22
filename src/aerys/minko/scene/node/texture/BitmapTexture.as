package aerys.minko.scene.node.texture
{
	import aerys.minko.ns.minko;
	
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	import flash.geom.Matrix;

	/**
	 * The BitmapTexture class enables the use of BitmapData objects as
	 * textures.
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	
	use namespace minko;
	
	public class BitmapTexture extends Texture
	{
		private static const MAX_SIZE	: int		= 2048;
		private static const TMP_MATRIX : Matrix 	= new Matrix();

		minko var _data		: BitmapData	= null;
		private var _mipmapping	: Boolean		= false;

		protected function get bitmapData()	: BitmapData
		{
			return _data;
		}
		
		public function get mipmapping():Boolean
		{
			return _mipmapping;
		}

		public function updateFromBitmapData(value 		: BitmapData,
											 smooth		: Boolean	= true,
											 downSample	: Boolean	= false) : void
		{
			var bitmapWidth		: uint = value.width;
			var bitmapHeight	: uint = value.height;
			var w				: int;
			var h				: int;

			if (downSample)
			{
				w = 1 << Math.floor(Math.log(bitmapWidth) * Math.LOG2E);
				h = 1 << Math.floor(Math.log(bitmapHeight) * Math.LOG2E);
			}
			else
			{
				w = 1 << Math.ceil(Math.log(bitmapWidth) * Math.LOG2E);
				h = 1 << Math.ceil(Math.log(bitmapHeight) * Math.LOG2E);
			}
			
			if (w > MAX_SIZE)
				w = MAX_SIZE;
			if (h > MAX_SIZE)
				h = MAX_SIZE;

			if (!_data || _data.width != w || _data.height != h)
			{
				if (_data)
				{
					_data.dispose();
					_data = null;
				}
				
				_data = new BitmapData(w, h, value.transparent, 0);
			}

			if (w != bitmapWidth || h != bitmapHeight)
			{
				TMP_MATRIX.identity();
				TMP_MATRIX.scale(w / bitmapWidth, h / bitmapHeight);
				_data.draw(value, TMP_MATRIX, null, null, null, smooth);
			}
			else
			{
				_data.draw(value, null, null, null, null, smooth);
			}

			resource.setContentFromBitmapData(_data, _mipmapping);
			_data = null;
		}

		public function BitmapTexture(bitmapData 	: BitmapData 	= null,
									  mipmapping	: Boolean		= true,
									  styleProperty	: int			= -1)
		{
			super(null, styleProperty);
			
			_mipmapping	= mipmapping;

			if (bitmapData)
				updateFromBitmapData(bitmapData);
		}

		public static function fromDisplayObject(source : DisplayObject,
												 size 	: int 		= 0,
												 smooth : Boolean 	= false) : BitmapTexture
		{
			var bmp : BitmapData = new BitmapData(size || source.width,
												  size || source.height,
												  true,
												  0);

			if (size)
			{
				TMP_MATRIX.identity();
				TMP_MATRIX.scale(source.width / size, source.height / size);
			}

			bmp.draw(source, TMP_MATRIX, null, null, null, smooth);

			return new BitmapTexture(bmp);
		}


	}
}