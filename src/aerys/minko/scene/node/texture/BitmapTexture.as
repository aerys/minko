package aerys.minko.scene.node.texture
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.resource.TextureResource;
	import aerys.minko.scene.action.texture.TextureAction;
	import aerys.minko.scene.node.AbstractScene;
	
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
	public class BitmapTexture extends AbstractScene implements ITexture
	{
		private var _version	: uint				= 0;
		
		private var _data		: BitmapData		= null;
		private var _mipmapping	: Boolean			= false;
		
		private var _styleProp	: int				= -1;
		
		private var _matrix		: Matrix			= new Matrix();
		
		private var _resource	: TextureResource	= new TextureResource();
				
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
		
		public function get resource() : TextureResource
		{
			return _resource;
		}
		
		protected function get bitmapData() : BitmapData
		{
			return _data;
		}
		
		public function updateFromBitmapData(value 		: BitmapData,
											 smooth		: Boolean	= true,
											 downSample	: Boolean	= false) : void
		{
			var bitmapWidth 	: int 	= value.width;
			var bitmapHeight	: int 	= value.height;
			
			var w				: int	= 1;
			var h				: int	= 1;
			
			while (w < bitmapWidth)
				w <<= 1;
			while (h < bitmapHeight)
				h <<= 1;
			
			if (downSample)
			{
				if (w > bitmapWidth)
					w >>>= 1;
				if (h > bitmapHeight)
					h >>>= 1;
			}
			
			if (!_data || _data.width != w || _data.height != h)
				_data = new BitmapData(w, h, value.transparent, 0);
				
			if (w != bitmapWidth || h != bitmapHeight)
			{
				_matrix.identity();
				_matrix.scale(w / bitmapWidth, h / bitmapHeight);
				_data.draw(value, _matrix, null, null, null, smooth);
			}
			else
			{
				_data.draw(value, null, null, null, null, smooth);
			}
			
			++_version;
			
			_resource.setContentFromBitmapData(_data, _mipmapping);
		}
		
		public function BitmapTexture(bitmapData 	: BitmapData 	= null,
									  mipmapping	: Boolean		= true,
									  styleProp		: int			= -1)
		{
			_mipmapping	= mipmapping;
			
			if (bitmapData)
				updateFromBitmapData(bitmapData);
			
			_styleProp = styleProp != -1 ? styleProp : BasicStyle.DIFFUSE;
			
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