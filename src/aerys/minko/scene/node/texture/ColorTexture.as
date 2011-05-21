package aerys.minko.scene.node.texture
{
	import aerys.minko.render.state.Blending;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	
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
				var enableAlpha : Boolean 		= (value >> 24) != 0xff;
				var bmp 		: BitmapData 	= new BitmapData(1, 1, enableAlpha, value);
				
				updateFromBitmapData(bmp, false);
				bmp.dispose();
			}
		}
		
		public function ColorTexture(color : uint)
		{
			super();
			
			this.color = color;
		}
		
		public static function fromRGBA(r : Number, g : Number, b : Number, a : Number = 1.) : ColorTexture
		{
			return new ColorTexture(((int(a * 255) & 0xff) << 24)
									+ ((int(r * 255) & 0xff) << 16)
									+ ((int(g * 255) & 0xff) << 8)
									+ (int(b * 255) & 0xff));
		}
		
		public static function fromHSV(h : Number, s : Number, v : Number) : ColorTexture
		{
			var r	: Number	= 0;
			var g	: Number	= 0;
			var b	: Number	= 0;
			var i	: Number	= 0.;
			var f	: Number	= 0.;
			var p	: Number	= 0.;
			var q	: Number	= 0.;
			var t 	: Number 	= 0.;
			
			if (v == 0.0)
			{
				r = g = b = 0;
			}
			else
			{
				i = Math.floor( h * 6 );
				f = ( h * 6 ) - i;
				p = v * ( 1 - s );
				q = v * ( 1 - ( s * f ) );
				t = v * ( 1 - ( s * ( 1 - f ) ) );
				
				switch (i)
				{
					case 1 :
						return ColorTexture.fromRGBA(q, v, p);
					case 2 :
						return ColorTexture.fromRGBA(p, v, t);
					case 3 :
						return ColorTexture.fromRGBA(p, q, v);
					case 4 :
						return ColorTexture.fromRGBA(t, p, v);
					case 5 :
						return ColorTexture.fromRGBA(v, p, q);
					case 6 :
					case 0 :
						return ColorTexture.fromRGBA(v, t, p);
				}
				
			}
			
			return ColorTexture.fromRGBA(r, g, b);
		}
		
	}
}