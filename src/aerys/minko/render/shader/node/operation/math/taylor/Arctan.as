package aerys.minko.render.shader.node.operation.math.taylor
{
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.builtin.Absolute;
	import aerys.minko.render.shader.node.operation.builtin.Add;
	import aerys.minko.render.shader.node.operation.builtin.Divide;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Power;
	import aerys.minko.render.shader.node.operation.builtin.Reciprocal;
	import aerys.minko.render.shader.node.operation.builtin.SetIfLessThan;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.math.Sum;

	/**
	 * Use a taylor serie to calculate arctan(x).
	 * This can be seldomly used in shaders that need to switch from uv textures to polar coordinates,
	 * but can be replaced by a less expensive function almost every time.
	 * 
	 * We are using the fact that arctan(x) = Pi / 2 - arctan(1/x), and arctan(-x) = - arctan(x)
	 * to compute only in the [0, 1] interval
	 * 
	 * Maybe we should center the taylor serie in 1/2, better than 0.
	 * 
	 * Also, http://en.wikipedia.org/wiki/Inverse_trigonometric_functions features
	 * another serie from Euler, which is said to be better.
	 * 
	 * @author Romain Gilliotte
	 */	
	public class Arctan extends Dummy
	{
		public function Arctan(arg : INode, numIterations : uint = 9)
		{
			var xSign : INode = new Divide(arg, new Absolute(arg));
			arg = new Absolute(arg);
			
			// x = (arg, arg, 1/arg, 1/arg)
			var x	: INode	= new Combine(
				new Extract(arg, Components.XX),
				new Extract(new Reciprocal(arg), Components.XX)
			);
			
			var sum	: Sum	= new Sum();
			for (var k : uint = 0; k < numIterations; k += 2)
			{
				// pow and mul are component-wise: we pack the operations into float4
				// (2 operations with x, and 2 operations with 1 / x)
				var coeff	: INode = new Constant(Math.pow(-1, k) / (2 * k + 1), Math.pow(-1, (k + 1)) / (2 * (k + 1) + 1));
				var pow		: INode = new Constant(2 * k + 1, 2 * (k + 1) + 1);
				var term	: INode = new Multiply(new Extract(coeff, Components.XYXY), new Power(x, new Extract(pow, Components.XYXY)));
				
				sum.addTerm(new Extract(term, Components.XZ));
				sum.addTerm(new Extract(term, Components.YW));
			}
			
			var xIsLowerThan1	: INode = new SetIfLessThan(new Absolute(arg), new Constant(1));
			var xIsBiggerThan1	: INode = new Substract(new Constant(1), xIsLowerThan1);
			var resultLower		: INode = new Extract(sum, Components.X);
			var resultBigger	: INode = new Substract(new Constant(Math.PI / 2), new Extract(sum, Components.Y));
			
			var result			: INode = new Add(
				new Multiply(xIsLowerThan1, resultLower),
				new Multiply(xIsBiggerThan1, resultBigger)
			);
			result = new Multiply(xSign, result);
			super(result);
		}
	}
}
