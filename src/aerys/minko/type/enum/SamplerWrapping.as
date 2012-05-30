package aerys.minko.type.enum
{
	import aerys.minko.ns.minko_shader;

	public final class SamplerWrapping
	{
		public static const CLAMP	: uint = 0;
		public static const REPEAT	: uint = 1;

		minko_shader static const STRINGS : Vector.<String> = new <String>['clamp', 'repeat'];
	}
}