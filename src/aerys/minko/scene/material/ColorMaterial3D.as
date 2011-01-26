package aerys.minko.scene.material
{
	import aerys.minko.render.state.Blending;
	
	import flash.display.BitmapData;
	
	public class ColorMaterial3D extends NativeMaterial3D
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
		
		public function ColorMaterial3D(color : uint)
		{
			var alpha : Boolean = (color >> 24) != 0xff
			
			super(new BitmapData(1, 1, alpha, color),
				  alpha ? Blending.ALPHA : Blending.NORMAL);
		}
	}
}