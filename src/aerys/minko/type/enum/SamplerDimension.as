package aerys.minko.type.enum
{
	import aerys.minko.ns.minko_shader;

	public final class SamplerDimension
	{
		public static const FLAT	: uint = 0;
		public static const CUBE	: uint = 1;
		
		minko_shader static const STRINGS : Vector.<String> = new <String>['2d', 'cube', '3d'];
	}
}
