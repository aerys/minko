package aerys.minko.render.shader.node.operation.math
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct3;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	
	/**
	 * Reflect a vector against a plane
	 * 
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 */	
	public class PlanarReflection extends Dummy
	{
		/**
		 * @param vector the vector to be reflected
		 * @param planeNormal a normal of the plane the vector is reflecting in.
		 */		
		public function PlanarReflection(vector			: INode, 
									  	 planeNormal	: INode)
		{
			var reflectedVector : INode = new Substract(
				new Product(
					new Constant(2),
					new DotProduct3(vector, planeNormal),
					planeNormal
				),
				vector
			);
			
			super(reflectedVector);
		}
	}
}