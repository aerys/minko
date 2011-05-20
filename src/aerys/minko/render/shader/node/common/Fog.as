package aerys.minko.render.shader.node.common
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.builtin.Divide;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.builtin.Saturate;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	
	public class Fog extends Dummy
	{
		public function Fog()
		{
			// get fog parameters
			var minDepth		: INode = new Constant(100);
			var deltaDepth		: INode = new Constant(100);
			
			// get the current depth.
			var depth			: INode = new Depth().interpolated;
			
			// interpolate depth with parameters to get the fog intensity
			// fog alpha = (cameraToCurrentSqDistance - minSquareDistance) / deltaSquareDistance
			var fogAlpha : INode = new Saturate(
				new Divide(
					new Substract(depth, minDepth), 
					deltaDepth
				)
			);
			
			// combine with fog color
			var fogColor : INode = new Combine(
				new Constant(0.6, 0.6, 0.6),
				fogAlpha
			);
			
			super(fogColor);
		}
	}
}