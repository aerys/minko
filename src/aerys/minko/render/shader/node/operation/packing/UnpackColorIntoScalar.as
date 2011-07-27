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
	import aerys.minko.type.math.ConstVector4;
	
	public class UnpackColorIntoScalar extends Dummy
	{
		private static const BIT_SH	: Constant	= new Constant(1. / (256. * 256. * 256.),
															   1. / (256. * 256.),
															   1. / 256.,
															   1.);

		public function UnpackColorIntoScalar(node 					: INode,
											  maxValuePerComponent	: INode = null,
											  maxValue				: INode = null)
		{
			maxValuePerComponent ||= new Constant(200);
			maxValue			 ||= new Constant(800);
			
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
			
			//super(new DotProduct4(node, BIT_SH));
			
			if (node.size != 4)
				throw new Error('Node must be of size 4');
			if (maxValuePerComponent.size != 1)
				throw new Error('maxValuePerComponent must be of size 1');
			if (maxValue.size != 1)
				throw new Error('maxValue must be of size 1');
		}
	}
}
