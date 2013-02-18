package aerys.minko.type.xpath
{
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.binding.DataBindings;
	
	import flash.utils.Dictionary;
	import flash.utils.getQualifiedClassName;

	public final class XPathEvaluator
	{
		private var _path 		: String;
		private var _selection	: Vector.<ISceneNode>		= null;
		private var _modifier	: String					= null;
		private var _lexer		: XPathLexer				= null;
		private var _predicate	: IPredicate				= null;
		
		public function get selection():Vector.<ISceneNode>
		{
			return _selection;
		}
		
		public function XPathEvaluator(path			: String,
									   selection	: Vector.<ISceneNode>,
									   modifier		: String,
									   predicate	: IPredicate)
		{
			_modifier = modifier;
			_predicate = predicate
				
			initialize(path, selection);
		}

		private function initialize(path : String, selection : Vector.<ISceneNode>) : void
		{
			_path = path;
			_lexer = new XPathLexer(_path);
			// update root
			var token : String = _lexer.getToken();
			
			_selection = selection.slice();
			if (token == "/")
			{
				selectRoots();
				_lexer.nextToken(token);
			}
			
			// parse
			while ((token = _lexer.getToken()) != null)
			{
				switch (token)
				{
					case '//' :
						_lexer.nextToken(token);
						selectDescendants();
						break ;
					case '/' :
						_lexer.nextToken(token);
						selectChildren();
						break ;
					default :
						_lexer.nextToken(token);
						parseNodeType(token);
						break ;
				}
			}
		}
		
		private function selectChildren(typeName : String = null) : void
		{
			var selection : Vector.<ISceneNode> = _selection.slice();
			
			if (typeName != null)
				typeName = typeName.toLowerCase();
			
			_selection.length = 0;
			for each (var node : ISceneNode in selection)
			{
				if (node is Group)
				{
					var group 		: Group = node as Group;
					var numChildren : uint 	= group.numChildren;
					
					for (var i : uint = 0; i < numChildren; ++i)
					{
						var child 		: ISceneNode 	= group.getChildAt(i);
						var className	: String		= getQualifiedClassName(child)
						var childType 	: String 		= className.substr(className.lastIndexOf(':') + 1);
						
						if (typeName == null || childType.toLowerCase() == typeName)
							_selection.push(child);
					}
				}
			}
		}
		
		private function selectRoots() : void
		{
			var selection	: Vector.<ISceneNode>	= _selection.slice();
			
			_selection.length = 0;
			for each (var node : ISceneNode in selection)
			if (_selection.indexOf(node.root) < 0)
				_selection.push(node);
		}
		
		private function selectDescendants() : void
		{
			var selection : Vector.<ISceneNode> = _selection.slice();
			
			_selection.length = 0;
			for each (var node : ISceneNode in selection)
			{
				_selection.push(node);
				if (node is Group)
					(node as Group).getDescendantsByType(ISceneNode, _selection);
			}
		}
		
		private function selectParents() : void
		{
			var selection : Vector.<ISceneNode> = _selection.slice();
			
			_selection.length = 0;
			for each (var node : ISceneNode in selection)
			{
				if (node.parent)
					_selection.push(node.parent);
				else
					_selection.push(node);
			}
		}
		
		private function parseNodeType(nodeType : String) : void
		{
			if (nodeType == '.')
			{
				// nothing
			}
			if (nodeType == '..')
				selectParents();
			else if (nodeType == '*')
				selectChildren();
			else
				selectChildren(nodeType);
			
			// apply predicates
			var token : String = _lexer.getToken();
			
			while (token == '[')
			{
				_lexer.nextToken(token);
				parsePredicate();
				
				token = _lexer.getToken();
			}
		}
		
		private function parsePredicate() : void
		{
			var propertyName	: String	= _lexer.getToken(true);
			var isBinding		: Boolean	= propertyName == '@';
			
			if (isBinding)
				propertyName = _lexer.getToken(true);
			
			var index			: int		= parseInt(propertyName);
			
			_predicate.filterProperty(_lexer, propertyName, _selection);
			
			_lexer.checkNextToken(']');
		}

		private function getValueObject(source : Object, chunks : Array) : Object
		{
			if (chunks)
				for each (var chunk : String in chunks)
					source = source[chunk];
			
			return source;
		}
		
		private function removeFromSelection(index : uint) : void
		{
			var numNodes : uint = _selection.length - 1;
			
			_selection[index] = _selection[numNodes];
			_selection.length = numNodes;
		}
	}
}