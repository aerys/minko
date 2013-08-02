package aerys.minko.type.enum
{
	import aerys.minko.ns.minko_shader;

	public final class SamplerFormat
    {
        public static const RGBA                : uint  = 0;
        public static const COMPRESSED          : uint  = 1;
        public static const COMPRESSED_ALPHA    : uint  = 2;

	    minko_shader static const STRINGS_DESKTOP   : Vector.<String> = new <String>['', 'dxt1', 'dxt5'];
	    minko_shader static const STRINGS_IOS       : Vector.<String> = new <String>['', 'pvrtc', 'pvrtc'];
	    minko_shader static const STRINGS_ANDROID   : Vector.<String> = new <String>['', 'etc1', 'etc1'];
    }
}