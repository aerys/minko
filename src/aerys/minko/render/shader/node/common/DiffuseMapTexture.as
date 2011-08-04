package aerys.minko.render.shader.node.common
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.IFragmentNode;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.Sampler;
	import aerys.minko.render.shader.node.operation.builtin.Texture;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.type.stream.format.VertexComponent;
	
	public class DiffuseMapTexture extends Dummy implements IFragmentNode
	{
		public function DiffuseMapTexture()
		{
			var result : INode = new Texture(
				new Interpolate(new Attribute(VertexComponent.UV)),
				new Sampler(BasicStyle.DIFFUSE, Sampler.FILTER_LINEAR, Sampler.MIPMAP_LINEAR)
			);
			
			super(result);
		}
	}
}
