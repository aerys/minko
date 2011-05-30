package aerys.minko.render.shader.node.operation.packing
{
	import aerys.minko.render.shader.compiler.register.RegisterSwizzling;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.IFragmentNode;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.builtin.Divide;
	import aerys.minko.render.shader.node.operation.builtin.Saturate;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;

	public class PackScalarToColor extends Dummy
	{
		override public function get size() : uint
		{
			return 4;
		}
		
		public function PackScalarToColor(node			: INode, 
										  maxValueParts	: INode,
										  maxValue		: INode)
		{
			var nodeOpposite		: INode = new Substract(maxValue, node);
			var packedDepthOpposite	: INode = new Saturate(
				new Divide(
					new Substract(nodeOpposite, maxValueParts),
					new Extract(maxValueParts, Components.Y)
				)
			);
			
			super(packedDepthOpposite);
			
			if (node.size != 1)
				throw new Error('node must be of size 1');
			if (maxValueParts.size != 4)
				throw new Error('maxValueParts must be of size 4');
			if (maxValue.size != 1)
				throw new Error('maxValue must be of size 1');
		}
	}
}