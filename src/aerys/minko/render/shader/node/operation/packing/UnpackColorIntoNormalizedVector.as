package aerys.minko.render.shader.node.operation.packing
{
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;

	/**
	 * Unpack a vector packed with PackNormalizedVectorToColor
	 *
	 * @author Romain Gilliotte
	 */
	public class UnpackColorIntoNormalizedVector extends Dummy
	{

		/**
		 * @param color
		 */
		public function UnpackColorIntoNormalizedVector(color : INode)
		{
			var result : INode;

			result = new Substract(color, new Constant(0.5));
			result = new Multiply(result, new Constant(2));
			result = new Extract(result, Components.XYZ);

			super(result);
		}
	}
}
