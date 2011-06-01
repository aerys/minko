package aerys.minko.scene.action
{
	public final class ActionType
	{
		public static const RENDER				: uint	= 1 << 1;
		
		public static const UPDATE_SELF			: uint	= 1 << 2;
		public static const UPDATE_LOCAL_DATA	: uint	= 1 << 3;
		public static const UPDATE_WORLD_DATA	: uint	= 1 << 4;
		public static const UPDATE_STYLE		: uint	= 1 << 5;
		public static const UPDATE_EFFECTS		: uint	= 1 << 6;
		public static const UPDATE_RENDER_DATA	: uint	= UPDATE_EFFECTS | UPDATE_STYLE;
		
		public static const RECURSE				: uint	= 1 << 7;
		
		public static const ALL					: uint	= 0xffffffff;
	}
}