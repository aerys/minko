package aerys.minko.type.enum
{
	import aerys.minko.ns.minko_shader;

	public final class SamplerMipMapping
	{
		public static const DISABLE	: uint = 0;
		public static const NEAREST	: uint = 1;
		public static const LINEAR	: uint = 2;
		
		minko_shader static const STRINGS : Vector.<String> = new <String>['mipnone', 'mipnearest', 'miplinear'];
	}
}