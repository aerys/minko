package aerys.minko.render.resource.texture
{
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.type.enum.SamplerFormat;

	import flash.display.BitmapData;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.CubeTexture;
	import flash.display3D.textures.TextureBase;
	import flash.geom.Matrix;
	import flash.utils.ByteArray;

	/**
	 * @inheritdoc
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class CubeTextureResource implements ITextureResource
	{
		private static const SIDE_X : Vector.<Number> = new <Number>[2, 0, 1, 1, 1, 3];
		private static const SIDE_Y : Vector.<Number> = new <Number>[1, 1, 0, 2, 1, 1];

		private static const FORMAT_BGRA				: String	= 'bgra';
		private static const FORMAT_COMPRESSED			: String	= 'compressed';
		private static const FORMAT_COMPRESSED_ALPHA 	: String 	= 'compressedAlpha';

		private static const TEXTURE_FORMAT_TO_SAMPLER	: Array 	= []
		{
			TEXTURE_FORMAT_TO_SAMPLER[FORMAT_BGRA] 				= SamplerFormat.RGBA;
			TEXTURE_FORMAT_TO_SAMPLER[FORMAT_COMPRESSED] 		= SamplerFormat.COMPRESSED;
			TEXTURE_FORMAT_TO_SAMPLER[FORMAT_COMPRESSED_ALPHA] 	= SamplerFormat.COMPRESSED_ALPHA;
		}

		private var _bitmapDatas	: Vector.<BitmapData>;
		private var _resource		: CubeTexture;
		private var _size			: uint;
        private var _mipMapping     : Boolean;
		private var _atf			: ByteArray;
		private var _atfFormat		: uint;
		private var _format 		: String;
		
        public function get format() : uint
        {
	        return TEXTURE_FORMAT_TO_SAMPLER[_format];
        }
        
        public function get mipMapping() : Boolean
        {
            return _mipMapping;
        }
		
		public function get width() : uint
		{
			return size;
		}
		
		public function get height() : uint
		{
			return size;
		}
		
		public function get size() : uint
		{
			return _size;
		}

		public function CubeTextureResource(size : uint)
		{
			_size = size;
		}

		public function setContentFromBitmapData(bitmapData	: BitmapData,
												 mipmap		: Boolean,
												 downSample	: Boolean	= false) : void
		{
			_bitmapDatas = new <BitmapData>[];
            _mipMapping = mipmap;
			_format = FORMAT_BGRA;

			var width	: Number = bitmapData.width / 4;
			var height	: Number = bitmapData.height / 3;
			
			var tmpMatrix		: Matrix		= new Matrix(1, 0, 0, 1);
			for (var side : uint = 0; side < 6; ++side)
			{
				var sideBitmapData	: BitmapData	= new BitmapData(width, height, false, 0);
                
				tmpMatrix.tx	= - SIDE_X[side] * width;
				tmpMatrix.ty	= - SIDE_Y[side] * height;
				
				sideBitmapData.draw(bitmapData, tmpMatrix);
				_bitmapDatas.push(sideBitmapData);
			}
		}
		
		public function setSize(w : uint, h : uint) : void
		{
			_size = w;
		}
		
		public function setContentFromBitmapDatas(right 	: BitmapData,
												  left		: BitmapData,
												  top		: BitmapData,
												  bottom	: BitmapData,
												  front		: BitmapData,
												  back		: BitmapData,
												  mipmap	: Boolean) : void
		{
			_bitmapDatas = new <BitmapData>[right, left, top, bottom, front, back];
            _mipMapping = mipmap;
		}
		
		public function setContentFromATF(atf : ByteArray) : void
		{
			_atf			= atf;
			_bitmapDatas    = null;

			var oldSize 	: uint = _size;
			var oldFormat	: String = _format;

			atf.position 	= 6;

			var formatByte 	: uint  = atf.readUnsignedByte();
			var atfFormat   : uint	= formatByte & 7;
			_size 			        = 1 << atf.readUnsignedByte();

			atf.position 	= 0;

			switch (atfFormat) {
				case 0: case 1: _format = FORMAT_BGRA; break;
				case 2: case 3: _format = FORMAT_COMPRESSED; break;
				case 4: case 5: _format = FORMAT_COMPRESSED_ALPHA; break
				default: throw new Error("Invalid ATF format");
			}

			if (_resource
					&& (oldFormat != _format
					|| oldSize != _size
					))
			{
				_resource.dispose();
				_resource = null;
			}
		}
		
		public function getTexture(context : Context3DResource) : TextureBase
		{
			if (!_resource)
				_resource = context.createCubeTexture(_size, _format, _bitmapDatas != null && _atf == null);
			else
				return _resource;

			if (_bitmapDatas != null)
			{
				for (var side : uint = 0; side < 6; ++side)
				{
					var mipmapId	: uint			= 0;
					var mySize		: uint			= _size;
					var bitmapData	: BitmapData	= _bitmapDatas[side];
                    
                    if (!_mipMapping)
                        _resource.uploadFromBitmapData(bitmapData, side);
                    else
                    {
                        while (mySize >= 1)
    					{
    						var tmpMatrix		: Matrix		= new Matrix();
    						var tmpBitmapData	: BitmapData	= new BitmapData(
                                mySize, mySize, false, 0x005500
                            );
    						
    						tmpMatrix.a		= mySize / bitmapData.width;
    						tmpMatrix.d		= mySize / bitmapData.height;
    						
    						tmpBitmapData.draw(bitmapData, tmpMatrix);
    						_resource.uploadFromBitmapData(tmpBitmapData, side, mipmapId);
    						
    						++mipmapId;
    						mySize /= 2;
    					}
                    }
				}
				
				_bitmapDatas = null;
			}
			else if (_atf)
			{
				_resource.uploadCompressedTextureFromByteArray(_atf, 0);
				_atf = null;
			}
			
			if (!_resource)
				throw new Error();
			
			return _resource;
		}
		
		public function dispose() : void
		{
			if (_resource)
				_resource.dispose();
		}
	}
}
