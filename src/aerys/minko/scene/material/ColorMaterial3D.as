package aerys.minko.scene.material
{
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
	public class ColorMaterial3D extends BitmapMaterial3D
	{
		public static const WHITE			: ColorMaterial3D	= new ColorMaterial3D(0xffffffff);
		public static const BLACK			: ColorMaterial3D	= new ColorMaterial3D(0xff000000);
		public static const RED				: ColorMaterial3D	= new ColorMaterial3D(0xffff0000);
		public static const GREEN			: ColorMaterial3D	= new ColorMaterial3D(0xff00ff00);
		public static const BLUE			: ColorMaterial3D	= new ColorMaterial3D(0xff0000ff);
		public static const GREY			: ColorMaterial3D	= new ColorMaterial3D(0xff777777);
		public static const DARK_GREY		: ColorMaterial3D	= new ColorMaterial3D(0xff222222);
		public static const SKY_BLUE		: ColorMaterial3D	= new ColorMaterial3D(0xff38acec);
		public static const NAVY_BLUE		: ColorMaterial3D	= new ColorMaterial3D(0xff000080);
		public static const ROYAL_BLUE		: ColorMaterial3D	= new ColorMaterial3D(0xff4169e1);
		public static const PURPLE			: ColorMaterial3D	= new ColorMaterial3D(0xffA020F0);
		public static const YELLOW			: ColorMaterial3D	= new ColorMaterial3D(0xfffff600);
		public static const ORANGE			: ColorMaterial3D	= new ColorMaterial3D(0xffffa200);
		public static const PINK			: ColorMaterial3D	= new ColorMaterial3D(0xffff00d8);
		
		/**
		 * The color (as an ARGB uint value) in the texture.
		 *  
		 * @return 
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
		
		public function ColorMaterial3D(color : uint)
		{
			super();
			
			this.color = color;
		}
		
	}
}