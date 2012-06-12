package aerys.minko.render.resource.texture
{
	import aerys.minko.render.resource.Context3DResource;
	
	import flash.display.BitmapData;
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
		private static const MAX_SIZE			: uint		= 2048;
		private static const TMP_MATRIX			: Matrix	= new Matrix();
		private static const FORMAT_BGRA		: String	= Context3DTextureFormat.BGRA
		private static const FORMAT_COMPRESSED	: String	= Context3DTextureFormat.COMPRESSED;
		
		private var _texture	: Texture		= null;
		private var _mipmap		: Boolean		= false;

		private var _bitmapData	: BitmapData	= null;
		private var _atf		: ByteArray		= null;
		private var _atfFormat	: uint			= 0;

		private var _width		: Number		= 0;
		private var _height		: Number		= 0;

		private var _update		: Boolean		= false;
		private var _resize		: Boolean		= false;

		public function get width() : uint
		{
			return _width;
		}

		public function get height() : uint
		{
			return _height;
		}
		
		public function TextureResource(width : int = 0, height : int = 0)
		{
			if (width != 0 && height != 0)
				setSize(width, height);
		}

		public function setSize(width : uint, height : uint) : void
		{
			//http://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
			if (!(width && !(width & (width - 1)))
				|| !(height && !(height & (height - 1))))
			{
				throw new Error('The size must be a power of 2.');
			}
			
			_width	= width;
			_height	= height;
			_resize = true;
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
			
			if (_bitmapData == null || _bitmapData.width != w || _bitmapData.height != h)
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
			_atf			= atf;
			_update 		= true;
			
			atf.position 	= 6;
			
			_atfFormat 		= atf.readUnsignedByte() & 3;
			_width 			= 1 << atf.readUnsignedByte();
			_height 		= 1 << atf.readUnsignedByte();
			_mipmap 		= atf.readUnsignedByte() > 1;
			
			atf.position 	= 0;
		}

		public function getNativeTexture(context : Context3DResource) : TextureBase
		{
			if ((!_texture || _resize) && _width && _height)
			{
				_resize = false;
				
				if (_texture)
					_texture.dispose();
				
				_texture = context.createTexture(
					_width,
					_height,
					_atf && _atfFormat == 2 ? FORMAT_COMPRESSED : FORMAT_BGRA,
					_bitmapData == null && _atf == null
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

			if (_texture == null)
				throw new Error();
			
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
			if (_texture)
			{
				_texture.dispose();
				_texture = null;
			}
		}
	}
}
