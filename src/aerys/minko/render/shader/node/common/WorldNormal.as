package aerys.minko.render.shader.node.common
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.IVertexNode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Multiply3x3;
	import aerys.minko.render.shader.node.operation.builtin.Normalize;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.render.shader.node.operation.packing.PackNormalizedVectorToColor;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.type.stream.format.VertexComponent;

	public class WorldNormal extends Dummy implements IVertexNode
	{
		public function get interpolated() : INode
		{
			return new Interpolate(this);
		}

		public function WorldNormal()
		{
			var normal : INode = new Multiply(
				new StyleParameter(1, BasicStyle.NORMAL_MULTIPLIER),
				new Attribute(VertexComponent.NORMAL)
			);

			var worldNormal : INode = new Normalize(
				new Multiply3x3(
					normal,
					new TransformParameter(16, TransformData.WORLD)
				)
			);

			var result : INode = new PackNormalizedVectorToColor(worldNormal);

			super(result);
		}
	}
}
