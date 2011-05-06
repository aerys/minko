package aerys.minko.render.ressource
{
	import aerys.minko.render.RenderTarget;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.Texture;
	import flash.geom.Matrix;

	public class TextureRessource implements IRessource3D
	{
		private var _texture	: Texture	= null;
		private var _mipmap		: Boolean;
		
		private var _bitmapData	: BitmapData;
		private var _width		: Number;
		private var _height		: Number;
		
		private var _update		: Boolean;
		
		public function TextureRessource()
		{
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
		
		public function getNativeTexture(context : Context3D) : Texture
		{
//			if (!_bitmapData && !_texture)
//				throw new Error('A content must be set to use this ressource.');
			
			if (!_texture)
			{
				_texture = context.createTexture(_width,
												 _height,
												 Context3DTextureFormat.BGRA,
												 true);
				
				_update = true;
			}
			
			if (_bitmapData)
			{
				if (_update)
				{
					_update = false;
					
					if (_mipmap)
					{
						var level 		: int 			= 0;
						var size		: int 			= _bitmapData.width;
						var tmp 		: BitmapData 	= new BitmapData(_bitmapData.width,
																		 _bitmapData.height,
																		 _bitmapData.transparent, 0);
						var transform 	: Matrix 		= new Matrix();
						
						while (size >= 1)
						{
							tmp.draw(_bitmapData, transform, null, null, null, true);
							_texture.uploadFromBitmapData(tmp, level);
							
							transform.scale(.5, .5);
							level++;
							size >>= 1;
						}
						
						tmp.dispose();
					}
					else
					{
						_texture.uploadFromBitmapData(_bitmapData, 0);
					}
				}
			}
			
			_bitmapData = null;
			
			return _texture;
		}
		
		public function dispose() : Boolean
		{
			if (_texture)
			{
				_texture.dispose();
				_texture = null;
				
				return true;
			}
			
			return false;
		}
	}
}