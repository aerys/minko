package aerys.minko.render.shader.compiler.visitor.allocator
{
	import aerys.minko.render.shader.compiler.allocator.Allocator;
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IAlignedOperation;
	
	public class AbstractAllocator implements IShaderNodeVisitor
	{
		protected var _operationId	: uint;
		
		protected var _tmpAlloc		: Allocator;
		protected var _constAlloc	: Allocator;
		
		public function AbstractAllocator()
		{
		}
		
		public function visit(node : INode) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function reportOperationArgumentsUsage(operationNode : AbstractOperation) : void
		{
			var aligned	: Boolean	= operationNode is IAlignedOperation;
			var arg1	: INode		= operationNode.arg1;
			var arg2	: INode		= operationNode.arg2;
			
			arg1 !== null && reportArgumentUsage(arg1, aligned);
			arg2 !== null && reportArgumentUsage(arg2, aligned);
		}
		
		protected function reportArgumentUsage(arg:INode, aligned : Boolean) : void
		{
			throw new Error('Must be overriden');
		}
	}
}
