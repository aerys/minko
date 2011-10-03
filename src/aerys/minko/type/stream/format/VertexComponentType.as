package aerys.minko.type.stream.format
{
	import flash.display3D.Context3DVertexBufferFormat;

	public final class VertexComponentType
	{
		public static const FLOAT_1		: int			= 0;
		public static const FLOAT_2		: int			= 1;
		public static const FLOAT_3		: int			= 2;
		public static const FLOAT_4		: int			= 3;
		public static const BYTES_4		: int			= 4;

		public static const NB_DWORDS	: Vector.<int>		= Vector.<int>([1, 2, 3, 4, 4]);
		public static const STRINGS		: Vector.<String>	= Vector.<String>([Context3DVertexBufferFormat.FLOAT_1,
																			   Context3DVertexBufferFormat.FLOAT_2,
																			   Context3DVertexBufferFormat.FLOAT_3,
																			   Context3DVertexBufferFormat.FLOAT_4,
																			   Context3DVertexBufferFormat.BYTES_4]);
	}
}