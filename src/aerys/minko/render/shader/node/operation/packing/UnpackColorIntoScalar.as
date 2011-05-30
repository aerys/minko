package aerys.minko.render.shader.node.operation.packing
{
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct4;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	
	public class UnpackColorIntoScalar extends Dummy
	{
		public function UnpackColorIntoScalar(node 					: INode,
											  maxValuePerComponent	: INode,
											  maxValue				: INode)
		{
			var unpackedOppositeNode : INode = new DotProduct4(
				new Multiply(
					node,
					maxValuePerComponent
				),
				new Extract(new Constant(1.0), Components.XXXX)
			);
			
			var unpackedNode : INode = new Substract(
				maxValue,
				unpackedOppositeNode
			);
			
			super(unpackedNode);
			
			if (node.size != 4)
				throw new Error('Node must be of size 4');
			if (maxValuePerComponent.size != 1)
				throw new Error('maxValuePerComponent must be of size 1');
			if (maxValue.size != 1)
				throw new Error('maxValue must be of size 1');
		}
	}
}
