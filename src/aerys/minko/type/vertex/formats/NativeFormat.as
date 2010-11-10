package aerys.minko.type.vertex.formats
{
	import flash.display3D.Context3DVertexFormat;

	public final class NativeFormat
	{
		public static const DISABLED	: int			= 0;
		public static const FLOAT_1		: int			= 1;
		public static const FLOAT_2		: int			= 2;
		public static const FLOAT_3		: int			= 3;
		public static const FLOAT_4		: int			= 4;
		public static const RGBA		: int			= 5;
		
		public static const NB_DWORDS	: Vector.<int>		= Vector.<int>([0, 1, 2, 3, 4, 4]);
		public static const STRINGS		: Vector.<String>	= Vector.<String>([Context3DVertexFormat.DISABLED,
																			   Context3DVertexFormat.FLOAT_1,
																			   Context3DVertexFormat.FLOAT_2,
																			   Context3DVertexFormat.FLOAT_3,
																			   Context3DVertexFormat.RGBA]);

	}
}