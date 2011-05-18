package aerys.minko.render.shader
{
	import aerys.minko.effect.basic.BasicStyle;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.Sampler;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.operation.builtin.Multiply4x4;
	import aerys.minko.render.shader.node.operation.builtin.Texture;
	import aerys.minko.render.shader.node.operation.common.ClipspacePosition;
	import aerys.minko.render.shader.node.operation.common.DiffuseMapTexture;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.scene.visitor.data.TransformData;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	public class BasicShader extends DynamicShader
	{
		public function BasicShader()
		{
			var vsOp : INode = new ClipspacePosition();
			var fsOp : INode = new DiffuseMapTexture();
			
			super(vsOp, fsOp);
		}
	}
}
