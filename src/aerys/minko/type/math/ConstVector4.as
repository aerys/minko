package aerys.minko.type.math
{
	import aerys.minko.ns.minko;

	public final class ConstVector4 extends Vector4
	{
		use namespace minko;
		
		private static const ERROR	: String	= "ConstVector4 objects can not be modified.";
		
		public static const X_AXIS	: Vector4	= new ConstVector4(1., 0., 0.);
		public static const Y_AXIS	: Vector4	= new ConstVector4(0., 1., 0.);
		public static const Z_AXIS	: Vector4	= new ConstVector4(0., 0., 1.);
		public static const ZERO	: Vector4	= new ConstVector4(0., 0., 0., 0.);
		public static const ONE		: Vector4	= new ConstVector4(1., 1., 1., 1.);
		
		public function ConstVector4(x : Number = 0.,
									 y : Number = 0.,
									 z : Number = 0.,
									 w : Number = NaN)
		{
			super(x, y, z, w);
		}
		
		override public function set x(value : Number) : void		{ throw new Error(ERROR); }
		override public function set y(value : Number) : void		{ throw new Error(ERROR); }
		override public function set z(value : Number) : void		{ throw new Error(ERROR); }
		override public function set w(value : Number) : void		{ throw new Error(ERROR); }
		
		override public function set(x : Number, y : Number, z : Number, w : Number = NaN) : Vector4
		{
			throw new Error(ERROR);
		}
		
		override public function add(vector : Vector4) : Vector4
		{
			throw new Error(ERROR);
		}
		
		override public function subtract(vector : Vector4) : Vector4
		{
			throw new Error(ERROR);
		}
		
		override public function scaleBy(scale : Number) : Vector4
		{
			throw new Error(ERROR);
		}
		
		override public function project() : Vector4
		{
			throw new Error(ERROR);
		}
		
		override public function normalize() : Vector4
		{
			throw new Error(ERROR);
		}
		
		override public function crossProduct(vector : Vector4) : Vector4
		{
			throw new Error(ERROR);
		}
	}
}