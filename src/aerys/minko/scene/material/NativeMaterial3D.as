package aerys.minko.scene.material
{
	import aerys.minko.effect.basic.BasicStyle3D;
	import aerys.minko.query.RenderingQuery;
	import aerys.minko.render.state.Blending;
	import aerys.minko.scene.AbstractScene3D;
	
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.Texture;
	import flash.geom.Matrix;
	
	/**
	 * The BitmapMaterial class enables the use of BitmapData and DisplayObject objects as materials.
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public class NativeMaterial3D extends AbstractScene3D implements IMaterial3D
	{
		public static const BLENDING_NORMAL	: uint	= Blending.NORMAL;
		public static const BLENDING_ALPHA	: uint	= Blending.ALPHA;
		
		private var _data		: BitmapData	= null;
		private var _texture	: Texture		= null;
		private var _blending	: uint			= 0;
		private var _update		: Boolean		= true;
		private var _mipmapping	: Boolean		= false;
		
		private var _styleProp	: String		= null;

		public function get styleProperty() : String
		{
			return _styleProp;
		}
		
		public function set styleProperty(value : String) : void
		{
			_styleProp = value;
		}
		
		protected function updateFromBitmapData(value : BitmapData) : void
		{
			for (var w : int = 1; w < value.width; w *= 2)
				continue ;
			for (var h : int = 1; h < value.height; h *= 2)
				continue ;
			
			var scaleMatrix : Matrix = new Matrix();
			
			scaleMatrix.scale(w / value.width, h / value.height);
			
			if (!_data || _data.width != w || _data.height != h)
				_data = new BitmapData(w, h, value.transparent, 0);
			
			_data.draw(value, scaleMatrix, null, null, null, true);
			
			_blending = _data.transparent ? BLENDING_ALPHA
									      : BLENDING_NORMAL;
			
			_update = true;
		}
		
		public function get blending() : uint
		{
			return _blending;
		}
		
		public function set blending(value : uint) : void
		{
			_blending = value;
		}
		
		public function NativeMaterial3D(data 		: BitmapData 	= null,
										 mipmapping	: Boolean		= false)
		{
			if (data)
				updateFromBitmapData(data);
			_mipmapping = mipmapping;
			
			_styleProp = BasicStyle3D.DIFFUSE_MAP;
		}
		
		override protected function acceptRenderingQuery(query : RenderingQuery) : void
		{
			if (_data)
			{
				if (!_texture)
				{
					_texture = query.createTexture(_data.width,
											   	   _data.height,
											   	   Context3DTextureFormat.BGRA,
												   false) as Texture;
					
					_update = true;
				}
				
				if (_update)
				{
					_update = false;
					
					if (_mipmapping)
					{
						var level 		: int 			= 0;
						var ws 			: int 			= _data.width;
						var hs 			: int 			= _data.height;
						var tmp 		: BitmapData 	= new BitmapData(_data.width, _data.height);
						var transform 	: Matrix 		= new Matrix();
						
						while (ws > 1 && hs > 1)
						{
							tmp.draw(_data, transform, null, null, null, true);
							_texture.uploadFromBitmapData(tmp, level);
							transform.scale(.5, .5);
							level++;
							ws >>= 1;
							hs >>= 1;
						}
						
						tmp.dispose();
					}
					else
					{
						_texture.uploadFromBitmapData(_data, 0);
					}
				}
				
				_data.dispose();
				_data = null;
			}
			
			query.style.set(BasicStyle3D.BLENDING, _blending)
					   .set(_styleProp, _texture);
		}
		
		//{ region statis
		public static function fromDisplayObject(source : DisplayObject,
												 size 	: int 		= 0,
												 smooth : Boolean 	= false) : NativeMaterial3D
		{
			var bmp : BitmapData = new BitmapData(size || source.width,
												  size || source.height,
												  true,
												  0);
			var matrix : Matrix = null;
			
			if (size)
			{
				matrix = new Matrix();
				matrix.scale(source.width / size, source.height / size);
			}
			
			bmp.draw(source, matrix, null, null, null, smooth);
			
			return new NativeMaterial3D(bmp);
		}
		//} endregion
	}
}