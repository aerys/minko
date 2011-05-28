package aerys.minko.render.shader.node.common
{
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.IVertexNode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.operation.builtin.Multiply4x4;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	public class Depth extends Dummy implements IVertexNode
	{
		public function get interpolated() : INode
		{
			return new Interpolate(this);
		}
		
		public function Depth()
		{
			var viewPosition	: INode = new Multiply4x4(
				new Attribute(VertexComponent.XYZ),
				new TransformParameter(16, LocalData.LOCAL_TO_VIEW)
			);
			
			var depth : INode = new Extract(viewPosition, Components.Z);
			
			super(depth);
		}
	}
}