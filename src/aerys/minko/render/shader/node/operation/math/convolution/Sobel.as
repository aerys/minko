package aerys.minko.render.shader.node.operation.math.convolution
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Sampler;
	import aerys.minko.render.shader.node.operation.builtin.Add;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.SquareRoot;

	/**
	 * Sobel filter
	 * This implementation uses two 3x3 convolution matrices.
	 *
	 * @author Romain Gilliotte
	 * @see http://en.wikipedia.org/wiki/Sobel_operator
	 */
	public class Sobel extends Dummy
	{
		protected static const SOBEL_X : Vector.<Number> = new Vector.<Number>([-1, 0, 1, -2, 0, +2, -1, 0, 1]);
		protected static const SOBEL_Y : Vector.<Number> = new Vector.<Number>([-1, -2, -1, 0, 0, 0, 1, 2, 1]);

		/**
		 * @param source A sampler linking to the texture the sobel filter is going to be applyed to
		 * @param uv The coordinates the sobel filter is going to be applyed to. A 3x3 square is read around those uvs.
		 * @param pixelSize The size of a pixel in the texture. If the texture is 1024 wide, pixelSize should be 1./1024
		 */
		public function Sobel(source	: Sampler,
							  uv		: INode,
							  pixelSize	: Number)
		{
			var xConvolution : INode = new Convolution3(source, uv, pixelSize, SOBEL_X);
			var yConvolution : INode = new Convolution3(source, uv, pixelSize, SOBEL_Y);

			var node : INode = new SquareRoot(
				new Add(
					new Multiply(xConvolution, xConvolution),
					new Multiply(yConvolution, yConvolution)
				)
			);

			super(node);
		}
	}
}
