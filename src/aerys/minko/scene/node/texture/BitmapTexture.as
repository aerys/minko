package aerys.minko.scene.node.texture
{
	
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.ressource.IRessource;
	import aerys.minko.render.ressource.TextureRessource;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.action.IActionTarget;
	import aerys.minko.scene.action.texture.TextureAction;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.type.IVersionnable;
	
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	import flash.geom.Matrix;
	
	/**
	 * The BitmapTexture class enables the use of BitmapData objects as
	 * textures.
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public class BitmapTexture extends AbstractScene implements ITexture, IVersionnable, IActionTarget
	{
		private var _version	: uint				= 0;
		
		private var _data		: BitmapData		= null;
		private var _mipmapping	: Boolean			= false;
		
		private var _styleProp	: int				= -1;
		
		private var _matrix		: Matrix			= new Matrix();
		
		private var _ressource	: TextureRessource	= new TextureRessource();
				
		public function get version() : uint
		{
			return _version;
		}
		
		public function get styleProperty() : int
		{
			return _styleProp;
		}
		
		public function set styleProperty(value : int) : void
		{
			_styleProp = value;
		}
		
		public function get ressource() : TextureRessource
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
		
		public function BitmapTexture(bitmapData 	: BitmapData 	= null,
									  mipmapping	: Boolean		= true)
		{
			_mipmapping	= mipmapping;
			
			if (bitmapData)
				updateFromBitmapData(bitmapData);
			
			_styleProp = BasicStyle.DIFFUSE_MAP;
			
			actions[0] = TextureAction.textureAction;
		}
		
		public static function fromDisplayObject(source : DisplayObject,
												 size 	: int 		= 0,
												 smooth : Boolean 	= false) : BitmapTexture
		{
			var matrix : Matrix = null;
			var bmp : BitmapData = new BitmapData(size || source.width,
												  size || source.height,
												  true,
												  0);
			
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