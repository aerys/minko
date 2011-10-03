package aerys.minko.render.resource
{
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.Texture;
	import flash.geom.Matrix;
	import flash.utils.ByteArray;

	public class Texture3DResource implements IResource
	{
		private var _texture	: Texture	= null;
		private var _mipmap		: Boolean;

		private var _bitmapData	: BitmapData;
		private var _atf		: ByteArray;

		private var _width		: Number;
		private var _height		: Number;

		private var _update		: Boolean;

		public function Texture3DResource()
		{
		}

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
								   				 mipmap		: Boolean) : void
		{
			if (_texture
				&& (mipmap != _mipmap
					|| bitmapData.width != _width
					|| bitmapData.height != _height))
			{
				_texture.dispose();
				_texture = null;
			}

			_bitmapData	= bitmapData;
			_width = bitmapData.width;
			_height = bitmapData.height;

			_mipmap	= mipmap;
			_update	= true;
		}

		public function setContentFromATF(atf : ByteArray) : void
		{
			_atf = atf;
		}

		public function getNativeTexture(context : Context3D) : Texture
		{
			if (!_texture && _width && _height)
			{
				_texture = context.createTexture(_width,
												 _height,
												 Context3DTextureFormat.BGRA,
												 _bitmapData == null);

				_update = true;
			}

			if (_update)
			{
				_update = false;

				if (_bitmapData)
				{
					if (_mipmap)
					{
						var level 		: int 			= 0;
						var size		: int 			= _width > _height ? _width : _height;
						var transparent	: Boolean		= _bitmapData.transparent;
						var tmp 		: BitmapData 	= new BitmapData(size,
																		 size,
																		 transparent,
																		 0);
						var transform 	: Matrix 		= new Matrix();

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

			_atf = null;
			_bitmapData = null;

			return _texture;
		}

		public function dispose() : void
		{
			_texture.dispose();
			_texture = null;
		}
	}
}
