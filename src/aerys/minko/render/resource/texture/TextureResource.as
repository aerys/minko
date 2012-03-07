package aerys.minko.render.resource.texture
{
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.Texture;
	import flash.display3D.textures.TextureBase;
	import flash.geom.Matrix;
	import flash.utils.ByteArray;

	/**
	 * @inheritdoc
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class TextureResource implements ITextureResource
	{
		private static const MAX_SIZE	: uint		= 2048;
		private static const TMP_MATRIX	: Matrix	= new Matrix();
		
		private var _texture	: Texture	= null;
		private var _mipmap		: Boolean;

		private var _bitmapData	: BitmapData;
		private var _atf		: ByteArray;

		private var _width		: Number;
		private var _height		: Number;

		private var _update		: Boolean;

		public function get width() : Number
		{
			return _width;
		}

		public function get height() : Number
		{
			return _height;
		}

		public function setSize(width : uint, height : uint) : void
		{
			_width	= width;
			_height	= height;
		}

		public function setContentFromBitmapData(bitmapData	: BitmapData,
								   				 mipmap		: Boolean,
												 downSample	: Boolean	= false) : void
		{
			var bitmapWidth		: uint 	= bitmapData.width;
			var bitmapHeight	: uint 	= bitmapData.height;
			var w				: int	= 0;
			var h				: int	= 0;
			
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
			
			_bitmapData	= new BitmapData(w, h, bitmapData.transparent, 0);
			
			if (w != bitmapWidth || h != bitmapHeight)
			{
				TMP_MATRIX.identity();
				TMP_MATRIX.scale(w / bitmapWidth, h / bitmapHeight);
				_bitmapData.draw(bitmapData, TMP_MATRIX);
			}
			else
			{
				_bitmapData.draw(bitmapData);
			}
			
			if (_texture
				&& (mipmap != _mipmap
					|| bitmapData.width != _width
					|| bitmapData.height != _height))
			{
				_texture.dispose();
				_texture = null;
			}

			_width = _bitmapData.width;
			_height = _bitmapData.height;

			_mipmap	= mipmap;
			_update	= true;
		}

		public function setContentFromATF(atf : ByteArray) : void
		{
			_atf = atf;
		}

		public function getNativeTexture(context : Context3D) : TextureBase
		{
			if (!_texture && _width && _height)
			{
				_texture = context.createTexture(
					_width,
					_height,
					Context3DTextureFormat.BGRA,
					_bitmapData == null
				);

				_update = true;
			}

			if (_update)
			{
				_update = false;

				uploadTextureWithMipMaps();
			}

			_atf = null;
			_bitmapData = null;

			return _texture;
		}
		
		private function uploadTextureWithMipMaps() : void
		{
			if (_bitmapData)
			{
				if (_mipmap)
				{
					var level 		: int 			= 0;
					var size		: int 			= _width > _height ? _width : _height;
					var transparent	: Boolean		= _bitmapData.transparent;
					var transform 	: Matrix 		= new Matrix();
					var tmp 		: BitmapData 	= new BitmapData(
						size,
						size,
						transparent,
						0
					);
					
					while (size >= 1)
					{
						tmp.draw(_bitmapData, transform, null, null, null, true);
						_texture.uploadFromBitmapData(tmp, level);
						
						transform.scale(.5, .5);
						level++;
						size >>= 1;
						if (tmp.transparent)
							tmp.fillRect(tmp.rect, 0);
					}
					
					tmp.dispose();
				}
				else
				{
					_texture.uploadFromBitmapData(_bitmapData, 0);
				}
				
				_bitmapData.dispose();
			}
			else if (_atf)
			{
				_texture.uploadCompressedTextureFromByteArray(_atf, 0, false);
			}
		}
		
		public function dispose() : void
		{
			_texture.dispose();
			_texture = null;
		}
	}
}
