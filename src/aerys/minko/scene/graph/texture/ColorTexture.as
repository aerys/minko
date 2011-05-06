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
		
		/**
		 * The color (as an ARGB uint value) in the texture.
		 *  
		 * @return 
		 * 
		 */
		public function get color() : uint
		{
			return bitmapData.getPixel32(0, 0);
		}
		
		public function set color(value : uint) : void
		{
			if (bitmapData)
				bitmapData.setPixel32(0, 0, value);
			else
			{
				var enableAlpha : Boolean = (value >> 24) != 0xff;
				var bmp : BitmapData = new BitmapData(1, 1, enableAlpha, value);
				
				updateFromBitmapData(bmp, false);
				bmp.dispose();
			}
		}
		
		public function ColorTexture(color : uint)
		{
			super();
			
			this.color = color;
		}
		
	}
}