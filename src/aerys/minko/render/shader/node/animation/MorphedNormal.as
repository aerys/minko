package aerys.minko.render.shader.node.animation
{
	import aerys.minko.render.effect.animation.AnimationStyle;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.operation.builtin.Add;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Normalize;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.manipulation.RootWrapper;
	import aerys.minko.type.stream.format.VertexComponent;
		
	public class MorphedNormal extends Dummy
	{
		public function MorphedNormal()
		{
			var lastNormal	: INode = new Attribute(VertexComponent.XYZ, 0);
			var nextNormal	: INode = new Attribute(VertexComponent.XYZ, 1);
			var ratio		: INode = new StyleParameter(1, AnimationStyle.MORPHING_RATIO);
			
			var result		: INode = new Add(
				new Multiply(
					new Substract(
						new RootWrapper(new Constant(1)), 
						ratio
					),
					lastNormal
				),
				new Multiply(ratio, nextNormal)
			);
			
			result = new Normalize(result);
			
			super(result);

		}
	}
}