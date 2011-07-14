package aerys.minko.render.shader.compiler.register
{
	public final class RegisterMask
	{
		public static const X		: uint = 0x1;
		public static const Y		: uint = 0x2;
		public static const Z		: uint = 0x4;
		public static const W		: uint = 0x8;
		
		public static const XY		: uint = X | Y; // 3
		public static const XZ		: uint = X | Z; // 5
		public static const XW		: uint = X | W; // 9
		public static const YZ		: uint = Y | Z; // 6
		public static const YW		: uint = Y | W; // 10
		public static const ZW		: uint = Z | W; // 12
		
		public static const XYZ		: uint = X | Y | Z; // 7
		public static const XYW		: uint = X | Y | W; // 11
		public static const XZW		: uint = X | Z | W; // 13
		public static const YZW		: uint = Y | Z | W; // 14
		
		public static const XYZW	: uint	= X | Y | Z | W; // 15
		
		public static const STRINGS:Vector.<String> = Vector.<String>([
			'', 'x', 'y', 'xy', 'z', 'xz', 
			'yz', 'xyz', 'w', 'xw', 'yw',
			'xyw', 'zw', 'xzw', 'yzw', 'xyzw'
		]);
		
		public static function createContinuous(size : uint) : uint
		{
			if (size == 1)
				return X;
			else if (size == 2)
				return XY;
			else if (size == 3)
				return XYZ;
			else if (size == 4)
				return XYZW;
			else
				throw new Error('Invalid size for mask');
		}
		
		public static function offset(mask : uint, value : int) : uint
		{
			if (value < 0) 
				return mask >>> (- value);
			else
				return mask << value;
		}
		
		public static function stringifyMask(mask : uint):String
		{
			return mask != XYZW ? '.' + STRINGS[mask] : '';
		}
	}
}
