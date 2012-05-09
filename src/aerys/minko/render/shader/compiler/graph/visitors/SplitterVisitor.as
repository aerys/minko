package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.graph.nodes.ANode;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Interpolate;
	
	import flash.utils.Dictionary;

	/**
	 * This visitor remove common nodes between vertex and fragment shader by cloning them
	 * 
	 * @author Romain Gilliotte
	 */	
	public class SplitterVisitor extends AbstractVisitor
	{
		private var _vertexNode		: Dictionary;
		private var _fragmentNode	: Dictionary;
		
		public function SplitterVisitor()
		{
		}
		
		override protected function start():void
		{
			super.start();
			
			_vertexNode	  = new Dictionary();
			_fragmentNode = new Dictionary();
		}
		
		override protected function finish() : void
		{
			super.finish();
			
			// all nodes in the fragment shader that are present in the vertex shader are to be replaced.
			var vertexDuplicates : Dictionary = cloneDuplicatedNodes();
			generateClonedSubGraphFromUnlikedClones(vertexDuplicates);
			connectClonedSubGraphsInFragmentShader(vertexDuplicates);
			
			_vertexNode	  = null;
			_fragmentNode = null;
		}
		
		private function cloneDuplicatedNodes() : Dictionary
		{
			var vertexDuplicates : Dictionary = new Dictionary();
			
			// first we clone every node present in both shaders
			for (var node : Object in _vertexNode)
				if (_fragmentNode[node])
					vertexDuplicates[node] = node.clone();
			
			return vertexDuplicates;
		}
		
		private function generateClonedSubGraphFromUnlikedClones(vertexDuplicates : Dictionary) : void
		{
			// Then for duplicate, we replace it's children by an unique duplicate
			// This will build valid subgraphs
			for (var node : Object in vertexDuplicates)
			{
				var duplicate		: ANode = ANode(vertexDuplicates[node]);
				var numArguments	: uint	= duplicate.numArguments;
				
				for (var argumentId : uint = 0; argumentId < numArguments; ++argumentId)
				{
					var oldArgument : ANode = duplicate.getArgumentAt(argumentId);
					var newArgument : ANode = vertexDuplicates[oldArgument];
					duplicate.setArgumentAt(argumentId, newArgument);
				}
			}
		}
		
		private function connectClonedSubGraphsInFragmentShader(vertexDuplicates : Dictionary) : void
		{
			// we now need to connect the valid cloned subgraphs we created into the fragment shader.
			for (var node : Object in vertexDuplicates)
			{
				var original	: ANode = ANode(node);
				var duplicate	: ANode = ANode(vertexDuplicates[original]);
				var numParents	: uint	= original.numParents;
				
				// if original has parents in vertex shader and fragment shader, we split it.
				for (var parentId : int = numParents - 1; parentId >= 0; --parentId)
				{
					var parent : ANode = original.getParentAt(parentId);
					if (_fragmentNode[parent])
					{
						var numArguments : uint = parent.numArguments;
						for (var argumentId : uint = 0; argumentId < numArguments; ++argumentId)
							if (parent.getArgumentAt(argumentId) === original)
								parent.setArgumentAt(argumentId, duplicate);
					}
				}
			}
		}
		
		override protected function visit(node : ANode, isVertexShader : Boolean) : void
		{
			if ((isVertexShader && _vertexNode[node]) || (!isVertexShader && _fragmentNode[node]))
				return;
			
			if (isVertexShader)
				_vertexNode[node] = true;
			else
				_fragmentNode[node] = true;
			
			if (node is Interpolate)
				isVertexShader = true;
			
			visitArguments(node, isVertexShader);
		}
	}
}
