package aerys.minko.render.shader.node.operation.packing
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.builtin.Add;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	
	/**
	 * Pack a normalized vector of size 3 into a color.
	 * 
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 */	
	public class PackNormalizedVectorToColor extends Dummy
	{
		override public function get size() : uint
		{
			return 4;
		}
		
		public function PackNormalizedVectorToColor(normalizedVector : INode)
		{
			var result : INode;
			
			result = new Add(normalizedVector, new Constant(1));
			result = new Multiply(result, new Constant(0.5));
			result = new Combine(result, new Constant(1));
			
			super(result);
		}
	}
}