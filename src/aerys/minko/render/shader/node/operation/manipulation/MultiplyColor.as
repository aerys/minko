package aerys.minko.render.shader.node.operation.manipulation
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	
	public class MultiplyColor extends Dummy
	{
		public function MultiplyColor(arg1	: INode, 
									  arg2	: INode)
		{
			var product : INode = new Multiply(arg1, arg2);
			
			var combine	: INode = new Combine(
				new Extract(product, Components.XYZ),
				new Extract(arg1, Components.W)
			);
			
			super(combine);
		}
	}
}
