package aerys.minko.scene.graph.texture
{
	import aerys.common.IVersionnable;
	import aerys.minko.effect.basic.BasicStyle;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	import aerys.minko.render.state.Blending;
	import aerys.minko.scene.graph.AbstractScene;
	
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.Texture;
	import flash.geom.Matrix;
	
	/**
	 * The BitmapMaterial3D class enables the use of BitmapData objects as materials.
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public class BitmapTexture extends AbstractScene implements ITexture, IVersionnable
	{
		public static const BLENDING_NORMAL	: uint	= Blending.NORMAL;
		public static const BLENDING_ALPHA	: uint	= Blending.ALPHA;
		
		private var _version	: uint			= 0;
		
		private var _data		: BitmapData	= null;
		private var _texture	: Texture		= null;
		private var _blending	: uint			= 0;
		private var _update		: Boolean		= true;
		private var _mipmapping	: Boolean		= false;
		
		private var _styleProp	: String		= null;
		
		private var _matrix		: Matrix		= new Matrix();
		
		public function get version() : uint
		{
			return _version;
		}

		public function get styleProperty() : String
		{
			return _styleProp;
		}
		
		public function set styleProperty(value : String) : void
		{
			_styleProp = value;
		}
		
		public function updateFromBitmapData(value 	: BitmapData,
											 smooth	: Boolean	= true) : void
		{
			var size 	: int = 1;
			var w 		: int = value.width;
			var h 		: int = value.height;
			
			while (size < w || size < h)
				size <<= 1;
			
			if (!_data || _data.width != size || _data.height != size)
				_data = new BitmapData(size, size, value.transparent, 0);
				
			if (size != w || size != h)
			{
				_matrix.identity();
				_matrix.scale(size / value.width, size / value.height);
				_data.draw(value, _matrix, null, null, null, smooth);
			}
			else
			{
				_data.draw(value, null, null, null, null, smooth);
			}
			
			_blending = _data.transparent ? BLENDING_ALPHA
									      : BLENDING_NORMAL;
			
			_update = true;
			++_version;
		}
		
		public function get blending() : uint
		{
			return _blending;
		}
		
		public function set blending(value : uint) : void
		{
			_blending = value;
		}
		
		public function BitmapTexture(bitmapData : BitmapData 	= null,
										 mipmapping	: Boolean		= true)
		{
			if (bitmapData)
				updateFromBitmapData(bitmapData);
			
			_mipmapping = mipmapping;
			
			_styleProp = BasicStyle.DIFFUSE_MAP;
		}
		
		override protected function visitedByRenderingVisitor(query : RenderingVisitor) : void
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
						var size		: int 			= _data.width;
						var tmp 		: BitmapData 	= new BitmapData(_data.width, _data.height, _data.transparent, 0);
						var transform 	: Matrix 		= new Matrix();
						
						while (size >= 1)
						{
							tmp.draw(_data, transform, null, null, null, true);
							_texture.uploadFromBitmapData(tmp, level);
							
							transform.scale(.5, .5);
							level++;
							size >>= 1;
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
			
			query.style.set(_styleProp, _texture);
		}
		
		public static function fromDisplayObject(source : DisplayObject,
												 size 	: int 		= 0,
												 smooth : Boolean 	= false) : BitmapTexture
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
			
			return new BitmapTexture(bmp);
		}
	}
}