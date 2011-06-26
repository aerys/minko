package aerys.minko.render.shader.node.fog
{
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.common.Depth;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.leaf.WorldParameter;
	import aerys.minko.render.shader.node.operation.builtin.Divide;
	import aerys.minko.render.shader.node.operation.builtin.Saturate;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.scene.visitor.data.CameraData;
	
	public class Fog extends Dummy
	{
		public function Fog(start : INode = null, distance : INode = null, color : INode = null)
		{
			start ||= new Constant(0.);
			distance ||= new WorldParameter(1, CameraData, CameraData.Z_FAR);
			color ||= new Extract(new Constant(0.), Components.XXX);
						
			// get the current depth.
			var depth	: INode = new Depth().interpolated;
			
			// interpolate depth with parameters to get the fog intensity
			// fog alpha = (cameraToCurrentSqDistance - minSquareDistance) / deltaSquareDistance
			var fogAlpha : INode = new Saturate(
				new Divide(
					new Substract(depth, start), 
					distance
				)
			);
			
			// combine with fog color
			var fogColor : INode = new Combine(
				color,
				fogAlpha
			);
			
			super(fogColor);
		}
	}
}