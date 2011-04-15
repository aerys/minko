package aerys.minko.scene.graph.texture
{
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	import aerys.minko.render.state.Blending;
	
	import flash.display.BitmapData;
	
	/**
	 * ColorMaterials3D objects represent solid color ARGB textures.
	 * 
	 * It is recommended to use static constants such as ColorMaterial3D.GREEN
	 * or ColorMaterial3D.BLUE whenever possible to minimize GPU memory footprint.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class ColorTexture extends BitmapTexture
	{
		public static const WHITE			: ColorTexture	= new ColorTexture(0xffffffff);
		public static const BLACK			: ColorTexture	= new ColorTexture(0xff000000);
		public static const RED				: ColorTexture	= new ColorTexture(0xffff0000);
		public static const GREEN			: ColorTexture	= new ColorTexture(0xff00ff00);
		public static const BLUE			: ColorTexture	= new ColorTexture(0xff0000ff);
		public static const GREY			: ColorTexture	= new ColorTexture(0xff777777);
		public static const DARK_GREY		: ColorTexture	= new ColorTexture(0xff222222);
		public static const SKY_BLUE		: ColorTexture	= new ColorTexture(0xff38acec);
		public static const NAVY_BLUE		: ColorTexture	= new ColorTexture(0xff000080);
		public static const ROYAL_BLUE		: ColorTexture	= new ColorTexture(0xff4169e1);
		public static const PURPLE			: ColorTexture	= new ColorTexture(0xffA020F0);
		public static const YELLOW			: ColorTexture	= new ColorTexture(0xfffff600);
		public static const ORANGE			: ColorTexture	= new ColorTexture(0xffffa200);
		public static const PINK			: ColorTexture	= new ColorTexture(0xffff00d8);
		
		private var _bmp	: BitmapData	= null;
		private var _color	: uint			= 0;
		
		private var _update	: Boolean		= true;
		
		/**
		 * The color (as an ARGB uint value) in the texture.
		 *  
		 * @return 
		 * 
		 */
		public function get color() : uint
		{
			return _color;
		}
		
		public function set color(value : uint) : void
		{
			if (_color != value)
			{
				var enableAlpha : Boolean = (value >> 24) != 0xff;
				
				_color = value;
				
				if (!_bmp || enableAlpha != _bmp.transparent)
					_bmp = new BitmapData(1, 1, enableAlpha, _color);
				else
					_bmp.fillRect(_bmp.rect, _color);
				
				_update = true;
			}
		}
		
		public function ColorTexture(color : uint)
		{
			super();
			
			this.color = color;
		}
		
		override protected function visitedByRenderingVisitor(query : RenderingVisitor) : void
		{
			if (_update)
			{
				_update = false;
				updateFromBitmapData(_bmp);
			}
			
			super.visitedByRenderingVisitor(query);
		}
	}
}