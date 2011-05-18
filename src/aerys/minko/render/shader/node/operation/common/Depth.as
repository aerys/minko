package aerys.minko.render.shader.node.operation.common
{
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.IFragmentNode;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.operation.builtin.Multiply4x4;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.scene.visitor.data.TransformData;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	public class Depth extends Dummy implements IFragmentNode
	{
		public function Depth()
		{
			var node : INode = new Extract(
				new Multiply4x4(
					new Interpolate(new Attribute(VertexComponent.XYZ)),
					new TransformParameter(16, TransformData.LOCAL_TO_VIEW)
				),
				Components.Z
			);
			
			super(node);
		}
	}
}