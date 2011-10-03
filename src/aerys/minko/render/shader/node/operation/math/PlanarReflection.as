package aerys.minko.render.shader.node.operation.math
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.IFragmentNode;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.IVertexNode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct3;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	
	/**
	 * Reflect a vector against a plane
	 * 
	 * @author Romain Gilliotte
	 */	
	public class PlanarReflection extends Dummy
	{
		public function get interpolated() : IFragmentNode
		{
			return new Interpolate(this);
		}
		
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