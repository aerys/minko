package aerys.minko.render.resource.texture
{
	import flash.display.BitmapData;
	import flash.display3D.textures.Texture;
	import flash.display3D.textures.TextureBase;
	import flash.geom.Matrix;
	import flash.utils.ByteArray;
	
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.type.Signal;
	import aerys.minko.type.enum.SamplerFormat;

	/**
	 * @inheritdoc
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class TextureResource implements ITextureResource
	{
		public static const EMPTY_BITMAPDATA			: BitmapData	= new BitmapData(1, 1, false, 0);
		
		private static const MAX_SIZE					: uint		= 2048;
		private static const TMP_MATRIX					: Matrix	= new Matrix();
		private static const FORMAT_BGRA				: String	= 'bgra';
		private static const FORMAT_COMPRESSED			: String	= 'compressed';
		private static const FORMAT_COMPRESSED_ALPHA 	: String 	= 'compressedAlpha';
		
		private static const TEXTURE_FORMAT_TO_SAMPLER	: Array 	= []
		{
			TEXTURE_FORMAT_TO_SAMPLER[FORMAT_BGRA] 				= SamplerFormat.RGBA;
			TEXTURE_FORMAT_TO_SAMPLER[FORMAT_COMPRESSED] 		= SamplerFormat.COMPRESSED;
			TEXTURE_FORMAT_TO_SAMPLER[FORMAT_COMPRESSED_ALPHA] 	= SamplerFormat.COMPRESSED_ALPHA;
		}
		
		private var _name 						: String		= "";
		
		private var _texture					: Texture;
		private var _mipmap						: Boolean;

		private var _bitmapData					: BitmapData;
		private var _atf						: ByteArray;
		private var _atfFormat					: uint;
		private var _format 					: String;

		private var _width						: Number;
		private var _height						: Number;

		private var _update						: Boolean;
		
		private var _disposed					: Boolean;
		
		private var _contextLost				: Signal		= new Signal("TextureResource.contextLost");
		private var _contextLostHandlerAdded	: Boolean		= false;
		
		public function get name():String
		{
			return _name;
		}

		public function set name(value:String):void
		{
			_name = value;
		}

		public function get contextLost():Signal
		{
			return _contextLost;
		}

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
			_format = FORMAT_BGRA;
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
				&& (_format	!= FORMAT_BGRA
					|| mipmap != _mipmap
					|| bitmapData.width != _width
					|| bitmapData.height != _height))
			{
				_texture.dispose();
				_texture = null;
			}

			_width = _bitmapData.width;
			_height = _bitmapData.height;
			_format = FORMAT_BGRA;

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
			var oldFormat	: String = _format;
			
			if (atf[6] == 0xFF)
				atf.position 	= 12;
			else
				atf.position 	= 6;
			
			var formatByte 	: uint = atf.readUnsignedByte();
			
			_atfFormat 		= formatByte & 0x7F;
			_width 			= 1 << atf.readUnsignedByte();
			_height 		= 1 << atf.readUnsignedByte();
			_mipmap 		= atf.readUnsignedByte() > 1;
			
			atf.position 	= 0;
			
			switch(_atfFormat)
			{
				case 0:
				case 1: 
					_format = FORMAT_BGRA;
					break;
				case 2:
				case 3:
					_format = FORMAT_COMPRESSED;
					break;
				case 4:
				case 5:
					_format = FORMAT_COMPRESSED_ALPHA;
					break;
				default:
					throw new Error("Invalid ATF format");
			}
			
			if (_texture
				&& (oldFormat != _format
					|| oldMipmap != _mipmap
					|| oldWidth != _width
					|| oldHeight != _height))
			{
				_texture.dispose();
				_texture = null;
			}
		}
		
		private function contextLostHandler(context : Context3DResource) : void
		{
			if (_disposed)
				return;
			_texture = null;
			_contextLost.execute(this);
		}

		public function getTexture(context : Context3DResource) : TextureBase
		{
			if (!_contextLostHandlerAdded)
			{
				context.contextChanged.add(contextLostHandler);
				_contextLostHandlerAdded = true;
			}
			
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
				_bitmapData = null;
			}
			else if (_atf)
			{
				_texture.uploadCompressedTextureFromByteArray(_atf, 0);
				
				_atf.clear();
                _atf = null;
			}
		}
		
		public function dispose() : void
		{
			_disposed = true;
			if (_texture)
			{
				_texture.dispose();
				_texture = null;
			}
		}
	}
}
