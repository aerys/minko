package aerys.minko.type.vertex.format
{
	import flash.display3D.Context3DVertexBufferFormat;

	public final class NativeFormat
	{
		public static const FLOAT_1		: int			= 0;
		public static const FLOAT_2		: int			= 1;
		public static const FLOAT_3		: int			= 2;
		public static const FLOAT_4		: int			= 3;
		
		public static const NB_DWORDS	: Vector.<int>		= Vector.<int>([1, 2, 3, 4]);
		public static const STRINGS		: Vector.<String>	= Vector.<String>([Context3DVertexBufferFormat.FLOAT_1,
																			   Context3DVertexBufferFormat.FLOAT_2,
																			   Context3DVertexBufferFormat.FLOAT_3,
																			   Context3DVertexBufferFormat.FLOAT_4]);
	}
}