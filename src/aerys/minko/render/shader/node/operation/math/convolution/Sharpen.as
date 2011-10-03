package aerys.minko.render.shader.node.operation.math.convolution
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Sampler;
	
	/**
	 * 
	 * 
	 * @author Romain Gilliotte
	 */	
	public class Sharpen extends Convolution3
	{
		public static const TYPE_1 : uint = 0;
		public static const TYPE_2 : uint = 1;
		
		private static const SHARPEN_1 : Vector.<Number> = Vector.<Number>([ -1, -1, -1, -1, 9, -1, -1, -1, -1 ]);
		private static const SHARPEN_2 : Vector.<Number> = Vector.<Number>([ 0, -1, 0, -1, 5, -1, 0, -1, 0 ]);
		
		/**
		 * @param source A sampler linking to the texture the sharpen filter is going to be applyed to
		 * @param uv The coordinates the sharpen filter is going to be applyed to. A 3x3 square is read around those uvs.
		 * @param pixelSize The size of a pixel in the texture. If the texture is 1024 wide, pixelSize should be 1./1024.
		 * @param type Must be Sharpen.TYPE_1 or Sharpen.TYPE_2.
		 */
		public function Sharpen(source		: Sampler,
								uv			: INode,
								pixelSize	: Number, 
								type		: uint = TYPE_1)
		{
			var convolutionMatrix : Vector.<Number> = type == TYPE_1 ? SHARPEN_1 : SHARPEN_2;
			super(source, uv, pixelSize, convolutionMatrix);
		}
	}
}