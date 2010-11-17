package aerys.minko.scene.material
{
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.IScene3DVisitor;
	import aerys.minko.render.state.BlendingDestination;
	import aerys.minko.render.state.BlendingSource;
	import aerys.minko.render.state.RenderStatesManager;
	import aerys.minko.scene.AbstractScene3D;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	import flash.display.Loader;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.Texture3D;
	import flash.events.Event;
	import flash.geom.Matrix;
	import flash.utils.ByteArray;
	
	/**
	 * The BitmapMaterial class enables the use of BitmapData and DisplayObject objects as materials.
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public class NativeTexture3D extends AbstractScene3D implements IMaterial3D
	{
		private var _data		: BitmapData	= null;
		private var _texture	: Texture3D		= null;
		private var _alpha		: Boolean		= false;
		private var _update		: Boolean		= true;
		private var _index		: int			= 0;
		private var _mipmapping	: Boolean		= false;

		/**
		 * A transparent or opaque bitmap image.
		 *
		 * @return
		 *
		 */
		public function get bitmapData() : BitmapData
		{
			return _data;
		}
		
		public function set bitmapData(value : BitmapData) : void
		{
			for (var w : int = 1; w < value.width; w *= 2)
				continue ;
			for (var h : int = 1; h < value.height; h *= 2)
				continue ;
			
			if (w != value.width || h != value.height)
			{
				var scaleMatrix : Matrix = new Matrix();
				
				scaleMatrix.scale(w / value.width,
								  h / value.height);
				
				if (!_data || _data.width != w || _data.height != h)
					_data = new BitmapData(w, h, value.transparent, 0);
				
				_data.draw(value, scaleMatrix, null, null, null, true);
			}
			else
			{
				_data = value;
			}
			
			_update = true;
		}
		
		public function get alphaBlending() : Boolean
		{
			return _alpha;
		}
		
		public function set alphaBlending(value : Boolean) : void
		{
			_alpha = value;
		}
		
		public function get index() : int
		{
			return _index;
		}
		
		public function set index(value : int) : void
		{
			_index = value;
		}
		
		public function get mipmapping() : Boolean
		{
			return _mipmapping;
		}
		
		public function set mipmapping(value : Boolean) : void
		{
			_mipmapping = value;
			_update = true;
		}
		
		/**
		 * Creates a new BitmapMaterial object.
		 *
		 * @param myBitmapData
		 *
		 */
		public function NativeTexture3D(data 		: BitmapData 	= null,
										alpha		: Boolean		= false,
										mipmapping	: Boolean		= false)
		{
			bitmapData = data;
			_alpha = alpha;
			_mipmapping = mipmapping;
		}
		
		override public function visited(visitor : IScene3DVisitor) : void
		{
			var renderer : IRenderer3D = visitor.renderer;
			var renderStates : RenderStatesManager = renderer.states;
			var context : Context3D = visitor.renderer.viewport.context;
			var level : int = 0;

			if (!_texture)
			{
				_texture = context.createTexture(_data.width,
												 _data.height,
												 Context3DTextureFormat.BGRA,
												 false);
				
				_update = true;
			}
			
			if (_update)
			{
				_update = false;
				
				if (_mipmapping)
				{
					var ws : int = _data.width;
					var hs : int = _data.height;
					var tmp : BitmapData = new BitmapData(_data.width, _data.height);
					var transform : Matrix = new Matrix();
					
					while (ws > 1 && hs > 1)
					{
						tmp.draw(_data, transform, null, null, null, true);
						_texture.upload(tmp, level);
						transform.scale(.5, .5);
						level++;
						ws >>= 1;
						hs >>= 1;
					}
					tmp.dispose();
				}
				else
				{
					_texture.upload(_data, 0);
				}
			}
			
			if (_alpha)
			{
				renderStates.blending = BlendingSource.SOURCE_ALPHA
										| BlendingDestination.ONE_MINUS_SOURCE_ALPHA;
			}
			
			//renderer.textures[_index] = _texture;
			context.setTexture(_index, _texture);
		}
		
		//{ region statis
		public static function fromDisplayObject(source : DisplayObject,
												 size 	: int 		= 0,
												 smooth : Boolean 	= false) : NativeTexture3D
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
			
			return new NativeTexture3D(bmp);
		}
		
		public static function fromByteArray(source : ByteArray) : NativeTexture3D
		{
			var loader 	: Loader 			= new Loader();
			var mat 	: NativeTexture3D 	= new NativeTexture3D();
			
			loader.loadBytes(source);
			loader.contentLoaderInfo.addEventListener(Event.COMPLETE, mat.loadCompleteHandler);
			
			return mat;
		}
		
		private function loadCompleteHandler(event : Event) : void
		{
			bitmapData = (event.target.content as Bitmap).bitmapData;
		}
		//} endregion
	}
}