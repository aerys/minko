package aerys.minko.render.shader.node.operation.math.convolution
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Sampler;
	
	/**
	 * 
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 */
	public class Blur extends Convolution
	{
		public static const TYPE_1 : uint = 0;
		public static const TYPE_2 : uint = 1;
		
		private static const BLUR_1 : Vector.<Number> = Vector.<Number>([ 1, 1, 1, 1, 1, 1, 1, 1, 1 ]);
		private static const BLUR_2 : Vector.<Number> = Vector.<Number>([ 1, 2, 1, 2, 4, 2, 1, 2, 1 ]);
		
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
			var convolutionMatrix : Vector.<Number> = type == TYPE_1 ? BLUR_1 : BLUR_2
			super(source, uv, pixelSize, convolutionMatrix);
		}
	}
}