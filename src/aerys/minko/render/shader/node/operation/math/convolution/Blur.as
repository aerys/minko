package aerys.minko.render.shader.node.operation.math.convolution
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Sampler;

	/**
	 * Blur filter
	 *
	 * @author Romain Gilliotte
	 */
	public class Blur extends Dummy
	{
		public static const TYPE_1 : uint = 0;
		public static const TYPE_2 : uint = 1;
		public static const TYPE_3 : uint = 2;

		private static const BLUR_1 : Vector.<Number> = Vector.<Number>([ 1/9, 1/9, 1/9, 1/9, 1/9, 1/9, 1/9, 1/9, 1/9 ]);
		private static const BLUR_2 : Vector.<Number> = Vector.<Number>([ 1, 2, 1, 2, 4, 2, 1, 2, 1 ]);
		private static const BLUR_3 : Vector.<Number> = Vector.<Number>([ 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25, 1/25 ]);

		/**
		 *
		 *
		 * @param source
		 * @param uv
		 * @param pixelSize
		 * @param type
		 */
		public function Blur(source 	: Sampler,
							 uv			: INode,
							 pixelSize	: Number,
							 type		: uint = TYPE_1)
		{
			var matrix : Vector.<Number>;
			var result : INode;

			switch (type)
			{
				case TYPE_1:
					matrix = BLUR_1;
					result = new Convolution3(source, uv, pixelSize, matrix);
					break;

				case TYPE_2:
					matrix = BLUR_2;
					result = new Convolution3(source, uv, pixelSize, matrix);
					break;

				case TYPE_3:
					matrix = BLUR_3;
					result = new Convolution5(source, uv, pixelSize, matrix);
					break;
			}

			super(result);
		}
	}
}
