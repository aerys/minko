package aerys.minko.render.shader.node.operation.math.taylor
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Power;
	import aerys.minko.render.shader.node.operation.math.Sum;

	/**
	 * FIXME: it should be less error prone to use arcsin(x) = 2*arctan(sqrt(1 - x^2) / (1 + x))
	 * (and btw this node is completely untested)
	 * @author Romain Gilliotte
	 */
	public class Arcsin extends Sum
	{
		public function Arcsin(arg : INode, numTerms : uint = 5)
		{
			for (var k : uint = 0; k < numTerms; ++k)
			{
				var coeff	: INode = new Constant(
					factorial(2 * k) /
					(Math.pow(4, k) * Math.pow(factorial(k), 2) * (2 * k + 1))
				);
				var pow		: INode = new Constant(2 * k + 1);
				var result	: INode = new Multiply(coeff, new Power(arg, pow));

				addTerm(result);
			}
		}

		private function factorial(n : uint) : uint
		{
			return n == 0 ? 1 : n * factorial(n - 1);
		}
	}
}
