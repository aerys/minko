package aerys.minko.render.shader.compiler.visitor.preprocess
{
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;

	public class Merger implements IShaderNodeVisitor
	{
		protected var _stack 	: Vector.<INode>;
		protected var _visited	: Vector.<INode>;

		public function Merger()
		{
			_stack		= new Vector.<INode>();
			_visited	= new Vector.<INode>();
		}

		public function processShader(clipspacePos	: INode,
									  color			: INode) : void
		{
			_stack.length = _visited.length = 0;

			visit(clipspacePos);
			visit(color);
		}

		public function visit(node : INode) : void
		{
			if (_visited.indexOf(node) !== -1)
				return;

			_stack.push(node);

			var replaced		: Boolean	= false;
			var visitedLength	: uint		= _visited.length;

			for (var i : int = 0; i < visitedLength; ++i)
			{
				var tmpNode : INode = _visited[i];
				if (node.isSame(tmpNode))
				{
					replaceCurrentNode(tmpNode);
					replaced = true;
					break;
				}
			}

			if (!replaced)
				_visited.push(node);

			node.accept(this);

			_stack.pop();
		}

		protected function replaceCurrentNode(newNode : INode) : void
		{
			if (_stack.length < 2)
				throw new Error('Root node cannot be a leaf');

			var parent	: AbstractOperation = _stack[_stack.length - 2] as AbstractOperation;
			var current	: INode				= _stack[_stack.length - 1];

			if (!parent)
				throw new Error('Clipspace position node and/or color nodes must ' +
					'extend an AbstractOperation');

			parent.swapChildren(current, newNode);
		}
	}
}
