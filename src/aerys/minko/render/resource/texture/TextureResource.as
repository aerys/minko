package aerys.minko.render.resource.texture
{
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.type.enum.SamplerFormat;
	
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
	public class TextureResource implements ITextureResource
	{
		private static const MAX_SIZE					: uint		= 2048;
		private static const TMP_MATRIX					: Matrix	= new Matrix();
		private static const FORMAT_BGRA				: String	= Context3DTextureFormat.BGRA
		private static const FORMAT_COMPRESSED			: String	= Context3DTextureFormat.COMPRESSED;
		private static const FORMAT_COMPRESSED_ALPHA 	: String 	= Context3DTextureFormat.COMPRESSED_ALPHA;
		
		private static const TEXTURE_FORMAT_TO_SAMPLER	: Array 	= []
		{
			TEXTURE_FORMAT_TO_SAMPLER[FORMAT_BGRA] 				= SamplerFormat.RGBA;
			TEXTURE_FORMAT_TO_SAMPLER[FORMAT_COMPRESSED] 		= SamplerFormat.COMPRESSED;
			TEXTURE_FORMAT_TO_SAMPLER[FORMAT_COMPRESSED_ALPHA] 	= SamplerFormat.COMPRESSED_ALPHA;
		}
		
		private var _texture		: Texture		= null;
		private var _mipmap			: Boolean		= false;

		private var _bitmapData		: BitmapData	= null;
		private var _atf			: ByteArray		= null;
		private var _atfFormat		: uint			= 0;
		private var _format 		: String 		= FORMAT_BGRA;

		private var _width			: Number		= 0;
		private var _height			: Number		= 0;

		private var _update			: Boolean		= false;

		public function get format() : uint
		{
			return TEXTURE_FORMAT_TO_SAMPLER[_format];
		}
        
        public function get mipMapping() : Boolean
        {
            return _mipmap;
        }
		
		public function get width() : uint
		{
			return _width;
		}

		public function get height() : uint
		{
			return _height;
		}
		
		public function TextureResource(width   : uint  = 0,
                                        height  : uint  = 0)
		{
            _width = width;
            _height = height;
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
			_bitmapData		= null;
			_update 		= true;

			var oldWidth	: Number = _width;
			var oldHeight	: Number = _height;
			var oldMipmap	: Boolean = _mipmap;
			
			atf.position 	= 6;
			
			var formatByte 	: uint = atf.readUnsignedByte();
			
			_atfFormat 		= formatByte & 7;
			_width 			= 1 << atf.readUnsignedByte();
			_height 		= 1 << atf.readUnsignedByte();
			_mipmap 		= atf.readUnsignedByte() > 1;
			
			atf.position 	= 0;
			
			if (_atfFormat == 5)
				_format = FORMAT_COMPRESSED_ALPHA;
			else if (_atfFormat == 3)
				_format = FORMAT_COMPRESSED;
			
			if (_texture
				&& (oldMipmap != _mipmap
					|| oldWidth != _width
					|| oldHeight != _height))
			{
				_texture.dispose();
				_texture = null;
			}
		}

		public function getTexture(context : Context3DResource) : TextureBase
		{
			if (!_texture && _width && _height)
			{
				if (_texture)
					_texture.dispose();
				
				_texture = context.createTexture(
					_width,
					_height,
					_format,
					_bitmapData == null && _atf == null
				);
			}

			if (_update)
			{
				_update = false;
				uploadBitmapDataWithMipMaps();
			}

			_atf = null;
			_bitmapData = null;
			
			return _texture;
		}
		
		private function uploadBitmapDataWithMipMaps() : void
		{
			if (_bitmapData)
			{
				if (_mipmap)
				{
					var level 		: uint 			= 0;
					var size		: uint 			= _width > _height ? _width : _height;
					var transparent	: Boolean		= _bitmapData.transparent;
					var tmp 		: BitmapData 	= new BitmapData(size, size, transparent, 0);
					var transform 	: Matrix		= new Matrix();
					
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
				_texture.uploadCompressedTextureFromByteArray(_atf, 0);
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
