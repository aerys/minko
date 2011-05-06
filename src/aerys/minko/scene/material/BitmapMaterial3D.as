package aerys.minko.scene.material
{
	import aerys.common.IVersionnable;
	import aerys.minko.effect.basic.BasicStyle3D;
	import aerys.minko.ns.minko;
	import aerys.minko.render.ressource.IRessource3D;
	import aerys.minko.render.ressource.TextureRessource;
	import aerys.minko.render.state.Blending;
	import aerys.minko.scene.AbstractScene3D;
	
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	import flash.geom.Matrix;
	
	/**
	 * The BitmapMaterial3D class enables the use of BitmapData objects as materials.
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public class BitmapMaterial3D extends AbstractScene3D implements IMaterial3D, IVersionnable
	{
		private var _version	: uint			= 0;
		
		private var _data		: BitmapData	= null;
		private var _mipmapping	: Boolean		= false;
		
		private var _styleProp	: String		= null;
		
		private var _matrix		: Matrix		= new Matrix();
		
		private var _ressource	: TextureRessource	= new TextureRessource();
		
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
		
		public function get ressource() : IRessource3D
		{
			return _ressource;
		}
		
		protected function get bitmapData() : BitmapData
		{
			return _data;
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
			
			++_version;
			
			_ressource.setContentFromBitmapData(_data, _mipmapping);
		}
		
		public function BitmapMaterial3D(bitmapData : BitmapData 	= null,
										 mipmapping	: Boolean		= true,
										 styleProp	: String		= BasicStyle3D.DIFFUSE_MAP)
		{
			if (bitmapData)
				updateFromBitmapData(bitmapData);
			
			_mipmapping	= mipmapping;
			_styleProp	= styleProp;
		}
		
		public static function fromDisplayObject(source : DisplayObject,
												 size 	: int 		= 0,
												 smooth : Boolean 	= false) : BitmapMaterial3D
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
			
			return new BitmapMaterial3D(bmp);
		}
	}
}