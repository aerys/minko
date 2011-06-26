package aerys.minko.render.shader.node.common
{
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.IVertexNode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.operation.builtin.Divide;
	import aerys.minko.render.shader.node.operation.builtin.Multiply4x4;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	public class UV extends Dummy implements IVertexNode
	{
		public function get interpolated() : INode
		{
			var simpleUv : INode = new Interpolate(
				new Multiply4x4(
					new Attribute(VertexComponent.XYZ),
					new TransformParameter(16, LocalData.LOCAL_TO_UV)
				)
			);
			
			var uv : INode = new Divide(
				simpleUv, 
				new Extract(simpleUv, Components.W)
			);
			
			return uv;
		}
		
		public function UV()
		{
			var simpleUv : INode = new Multiply4x4(
				new Attribute(VertexComponent.XYZ),
				new TransformParameter(16, LocalData.LOCAL_TO_UV)
			);
			
			var uv : INode = new Divide(
				simpleUv, 
				new Extract(simpleUv, Components.W)
			);
			
			super(uv);
		}
	}
}
