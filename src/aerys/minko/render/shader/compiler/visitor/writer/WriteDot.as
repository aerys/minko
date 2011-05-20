package aerys.minko.render.shader.compiler.visitor.writer
{
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.AbstractLeaf;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;

	public class WriteDot implements IShaderNodeVisitor
	{
		protected var _stack 		: Vector.<INode>;
		protected var _lastRight	: uint;
		
		protected var _visited		: Vector.<INode>;
		protected var _visitedIsVs	: Vector.<Boolean>;
		
		protected var _isVertexShader	: Boolean;
		
		private var _dot	: String;
		
		public function get dot():String
		{
			return _dot;
		}
		
		public function WriteDot()
		{
			_stack			= new Vector.<INode>();
			_visited		= new Vector.<INode>();
			_visitedIsVs	= new Vector.<Boolean>();
		}
		
		public function processShader(clipspacePos	: INode, 
									  color			: INode) : String 
		{
			_stack.length = 0;
			_visited.length = 0;
			_lastRight = 0
			
			_dot = "";
			_isVertexShader = true;
			visit(clipspacePos);
			
			_isVertexShader = false;
			visit(color);
			
			var nodeCount : uint = _visited.length;
			var nodeData : String = "";
			
			for (var i : int = 0; i < nodeCount; ++i)
			{
				var node			: INode = _visited[i];
				var nodeFillColor	: String = _visitedIsVs[i] ? 'blue' : 'sienna';
				var nodeShape		: String = 
					node is AbstractLeaf ? node is Dummy ? 'point' : 'box' : 'ellipse';
				
				nodeData += "\t" + 'node' + _visited.indexOf(node) 
					+ ' [' 
						+ 'label="' + node.toString() + '", '
						+ 'color="' + nodeFillColor + '", '
						+ 'shape="' + nodeShape + '"'
					+ ']' + "\n";
			}
			_dot = "digraph shader {\n" + nodeData + "\n" + _dot + "}";
			
			return _dot;
		}
		
		public function visit(node : INode) : void
		{
			if (_visited.indexOf(node) === -1)
			{
				_visited.push(node);
				_visitedIsVs.push(_isVertexShader || (node is Interpolate));
			}
			
			_stack.push(node);
			
			if (node is AbstractLeaf || _visited.indexOf(node) != _visited.length - 1)
			{
				for (var i : int = _lastRight + 1; i < _stack.length; ++i)
				{
					_dot += "\t";
					_dot += 'node' + _visited.indexOf(_stack[i - 1]);
					_dot += ' -> ';
					_dot += 'node' + _visited.indexOf(_stack[i]);
					_dot += ' [label=' + _stack[i].size + ']' + ";\n";
				}
			}
			else if (node is Interpolate)
			{
				_isVertexShader = true;
				node.accept(this);
				_isVertexShader = false;
			}
			else if (node is Dummy)
			{
				var dummyNode : Dummy = node as Dummy;
				dummyNode.accept(this);
			}
			else if (node is AbstractOperation)
			{
				var opNode : AbstractOperation = node as AbstractOperation;
				if (opNode.arg1)
				{
					this.visit(opNode.arg1);
				}
				if (opNode.arg2)
				{
					_lastRight = _stack.length - 1;
					this.visit(opNode.arg2);
				}
			}
			
			_stack.pop();
		}
		
	}
}
