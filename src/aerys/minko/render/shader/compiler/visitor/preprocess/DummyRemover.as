package aerys.minko.render.shader.compiler.visitor.preprocess
{
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;

	public class DummyRemover implements IShaderNodeVisitor
	{
		protected var _stack 	: Vector.<INode>;
		protected var _visited	: Vector.<INode>;

		protected var _isVertexShader	: Boolean;

		protected var _clipspacePos	: INode;
		protected var _color		: INode;

		public function get clipspacePos():INode
		{
			return _clipspacePos;
		}

		public function get color():INode
		{
			return _color;
		}

		public function DummyRemover()
		{
			_stack		= new Vector.<INode>();
			_visited	= new Vector.<INode>();
		}

		public function processShader(clipspacePos	: INode,
									  color			: INode) : void
		{
			_stack.length	= _visited.length = 0;
			_clipspacePos	= clipspacePos;
			_color			= color;

			_isVertexShader = true;
			visit(clipspacePos);
			_isVertexShader = false;
			visit(color);
		}

		public function visit(node : INode) : void
		{
			if (_visited.indexOf(node) !== -1)
				return;

			if (node is Dummy)
			{
				var dummyNode : Dummy = node as Dummy;

				if (_stack.length == 0)
				{
					if (_isVertexShader)
						_clipspacePos = dummyNode.node;
					else
						_color = dummyNode.node;
				}
				else
				{
					var parent	: AbstractOperation = _stack[_stack.length - 1] as AbstractOperation;
					parent.swapChildren(dummyNode, dummyNode.node);
				}

				visit(dummyNode.node);
			}
			else
			{
				_stack.push(node);
				_visited.push(node);
				node.accept(this);
				_stack.pop();
			}
		}
	}
}
