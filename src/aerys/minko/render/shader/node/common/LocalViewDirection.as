package aerys.minko.render.shader.node.common
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.IVertexNode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.WorldParameter;
	import aerys.minko.render.shader.node.operation.builtin.Normalize;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.scene.data.CameraData;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	public class LocalViewDirection extends Dummy implements IVertexNode
	{
		public function get interpolated() : INode
		{
			return new Interpolate(this);
		}
		
		public function LocalViewDirection()
		{
			var cameraToPosition	: INode = new Substract(
				new WorldParameter(3, CameraData, CameraData.LOCAL_POSITION),
				new Attribute(VertexComponent.XYZ)
			);
			
			var viewDirection		: INode = new Normalize(cameraToPosition);
			
			super(viewDirection);
		}
	}
}