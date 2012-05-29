package aerys.minko.type.enum
{
	import aerys.minko.ns.minko_shader;

	public final class SamplerFiltering
	{
		public static const NEAREST	: uint = 0;
		public static const LINEAR	: uint = 1;
		
		minko_shader static const STRINGS : Vector.<String> = new <String>['nearest', 'linear'];
	}
}