package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	
	public class VariadicExtract implements INode
	{
		public function VariadicExtract(key			: INode, 
										constant	: INode,
										isMatrix	: Boolean)
		{
		}
		
		public function get components() : uint
		{
			throw new Error('implement me');
		}
		
		public function get keyComponentSelect() : uint
		{
			throw new Error('implement me');
		}
		
		public function get hash():uint
		{
			throw new Error('implement me');
		}
		
		public function get size():uint
		{
			throw new Error('implement me');
		}
		
		public function toString():String
		{
			throw new Error('implement me');
		}
	}
}