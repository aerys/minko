package aerys.minko.scene.node.texture
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.resource.Texture3DResource;
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
	
	use namespace minko;
	
	public class BitmapTexture extends AbstractScene implements ITexture
	{
		private static const TMP_MATRIX : Matrix = new Matrix();

		private var _version	: uint				= 0;
		private var _data		: BitmapData		= null;
		private var _mipmapping	: Boolean			= false;
		private var _styleProp	: int				= -1;
		private var _resource	: Texture3DResource	= new Texture3DResource();

		public function get version()		: uint				{ return _version;		}
		public function get styleProperty() : int				{ return _styleProp;	}
		public function get resource()		: Texture3DResource	{ return _resource;		}
		public function get mipmapping()	: Boolean			{ return _mipmapping;	}
		minko function get data() 			: BitmapData		{ return _data;			}
		
		protected function get bitmapData()	: BitmapData
		{
			return _data;
		}

		public function set styleProperty(value : int) : void
		{
			_styleProp = value;
		}

		public function updateFromBitmapData(value 		: BitmapData,
											 smooth		: Boolean	= true,
											 downSample	: Boolean	= false) : void
		{
			var bitmapWidth		: uint = value.width;
			var bitmapHeight	: uint = value.height;
			var w				: int;
			var h				: int;

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

			if (!_data || _data.width != w || _data.height != h)
			{
				if (_data)
					_data.dispose();
				_data = new BitmapData(w, h, value.transparent, 0);
			}

			if (w != bitmapWidth || h != bitmapHeight)
			{
				TMP_MATRIX.identity();
				TMP_MATRIX.scale(w / bitmapWidth, h / bitmapHeight);
				_data.draw(value, TMP_MATRIX, null, null, null, smooth);
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
			_styleProp	= styleProp != -1 ? styleProp : BasicStyle.DIFFUSE;

			if (bitmapData)
				updateFromBitmapData(bitmapData);

			actions[0] = TextureAction.textureAction;
		}

		public static function fromDisplayObject(source : DisplayObject,
												 size 	: int 		= 0,
												 smooth : Boolean 	= false) : BitmapTexture
		{
			var bmp : BitmapData = new BitmapData(size || source.width,
												  size || source.height,
												  true,
												  0);

			if (size)
			{
				TMP_MATRIX.identity();
				TMP_MATRIX.scale(source.width / size, source.height / size);
			}

			bmp.draw(source, TMP_MATRIX, null, null, null, smooth);

			return new BitmapTexture(bmp);
		}
	}
}