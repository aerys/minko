package aerys.minko.render.state
{
	public final class RenderState
	{
		public static const BLENDING			: uint	= 1;
		public static const SHADER				: uint	= 2;
		public static const VERTEX_FORMAT		: uint	= 4;
		public static const WRITE_MASK			: uint	= 8;
		public static const FRUSTUM_CULLING		: uint	= 16;
		public static const TRIANGLE_CULLING	: uint	= 32;
		public static const ALL					: uint	= BLENDING | SHADER
													  	  | VERTEX_FORMAT
													  	  | WRITE_MASK
														  | FRUSTUM_CULLING | TRIANGLE_CULLING;
	}
}