package aerys.minko.render.shader.compiler.visitor.allocator
{
	import aerys.minko.render.shader.compiler.allocator.Allocator;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.AbstractConstant;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.render.shader.node.operation.manipulation.VariadicExtract;
	
	public class VertexAllocator extends AbstractAllocator
	{
		protected var _attrAlloc	: Allocator;
		
		protected var _visited		: Vector.<INode>;
		protected var _stack		: Vector.<INode>;
		
		public function VertexAllocator(attrAlloc	: Allocator,
										tmpAlloc	: Allocator,
										constAlloc	: Allocator)
		{
			_operationId = 0;
			
			_attrAlloc		= attrAlloc;
			_tmpAlloc		= tmpAlloc;
			_constAlloc		= constAlloc;
			
			_visited		= new Vector.<INode>();
			_stack			= new Vector.<INode>();
		}
		
		public function processVertexShader(nodes:Vector.<INode>):void
		{
			for each (var node:INode in nodes)
				visit(node);
		}
		
		override public function visit(shaderNode : INode):void
		{
			if (_visited.indexOf(shaderNode) !== -1)
				return;
			
			_visited.push(shaderNode);
			_stack.push(shaderNode);
			
			shaderNode.accept(this);
			
			if (shaderNode is Interpolate)
			{
				++_operationId;
				reportOperationArgumentsUsage(AbstractOperation(shaderNode));
			}
			else if (shaderNode is Extract)
			{
//				reportOperationArgumentsUsage(AbstractOperation(shaderNode));
			}
			else if (shaderNode is VariadicExtract)
			{
//				reportOperationArgumentsUsage(AbstractOperation(shaderNode));
			}
			else if (shaderNode is Combine)
			{
				var combineNode : Combine = shaderNode as Combine;
				
				++_operationId;
				reportArgumentUsage(combineNode.arg1, false);
				
				_tmpAlloc.allocate(combineNode, _operationId);
				
				++_operationId;
				reportArgumentUsage(combineNode.arg2, false);
			}
			else if (shaderNode is AbstractOperation)
			{
				var operationNode : AbstractOperation = AbstractOperation(shaderNode);
				
				++_operationId;
				reportOperationArgumentsUsage(operationNode);
				
				// on ne traite pas le noeud final.
				if (_stack.length > 1)
					_tmpAlloc.allocate(operationNode, _operationId);
			}
			else if (shaderNode is Attribute)
			{
				_attrAlloc.allocate(shaderNode);
			}
			else if (shaderNode is AbstractConstant)
			{
				_constAlloc.allocate(shaderNode);
			}
			
			_stack.pop();
		}
		
		override protected function reportArgumentUsage(arg : INode, aligned : Boolean) : void
		{
			if (arg is Attribute)
				_attrAlloc.reportUsage(arg, _operationId, aligned);
			
			else if (arg is VariadicExtract)
			{
				var variadic : VariadicExtract = arg as VariadicExtract;
				_tmpAlloc.reportUsage(variadic.arg1, _operationId, aligned);
				_constAlloc.reportUsage(variadic.arg2, _operationId, aligned);
			}
			
			else if (arg is AbstractConstant)
				_constAlloc.reportUsage(arg, _operationId, aligned);
			
			else if (arg is Extract)
				reportArgumentUsage((arg as Extract).arg1, aligned);
			
			else if (arg is AbstractOperation)
				_tmpAlloc.reportUsage(arg, _operationId, aligned);
		}
		
	}
}
