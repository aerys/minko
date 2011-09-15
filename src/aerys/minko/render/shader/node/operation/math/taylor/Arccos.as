package aerys.minko.render.shader.node.operation.math.taylor
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	
	public class Arccos extends Substract
	{
		public function Arccos(arg : INode, numIterations : uint = 5)
		{
			super(new Constant(Math.PI / 2), new Arcsin(arg, numIterations));
		}
	}
}
