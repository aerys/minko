package aerys.minko.type.vertex
{
	public final class Vertex3DInput
	{
		private static var _mask	: uint	= 1;
		
		public static const XYZ		: uint	= create();
		public static const UV		: uint	= create();
		public static const COLOR	: uint	= create();
		public static const NORMAL	: uint	= create();
		
		public static function create() : uint
		{
			var result : uint = _mask;
			
			_mask <<= 1;
			
			return result;
		}
	}
}