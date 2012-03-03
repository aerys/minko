package aerys.minko.render.shader.compiler.register
{
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class RegisterType
	{
		public static const ATTRIBUTE	: uint = 0x0;
		public static const CONSTANT	: uint = 0x1;
		public static const TEMPORARY	: uint = 0x2;
		public static const OUTPUT		: uint = 0x3;
		public static const VARYING		: uint = 0x4;
		public static const SAMPLER		: uint = 0x5;

		public static function stringifyType(type			: uint,
											 isVertexShader	: Boolean) : String
		{
			if (type == ATTRIBUTE)
				return 'va';

			if (type == VARYING)
				return 'v';

			if (type == SAMPLER)
				return 'fs';

			if (type == CONSTANT)
				return isVertexShader ? 'vc' : 'fc';

			if (type == TEMPORARY)
				return isVertexShader ? 'vt' : 'ft';

			if (type == OUTPUT)
				return isVertexShader ? 'op' : 'oc';

			throw new Error('Unknown register type');
		}
	}
}
