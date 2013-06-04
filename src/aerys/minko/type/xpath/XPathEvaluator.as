package aerys.minko.type.xpath
{
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.binding.DataBindings;
	
	import flash.utils.getQualifiedClassName;
	
	public final class XPathEvaluator
	{
		private var _path 		: String;
		private var _selection	: Vector.<ISceneNode>		= null;
		private var _modifier	: String					= null;
		private var _lexer		: XPathLexer				= null;
		
		public function get selection() : Vector.<ISceneNode>
		{
			return _selection;
		}
		
		public function XPathEvaluator(path			: String,
									   selection	: Vector.<ISceneNode>,
									   modifier		: String    = null)
		{
			_modifier = modifier;
			
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
			
			filterProperty(_lexer, propertyName, _selection);
			
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
		
		private function filterProperty(lexer : XPathLexer, propertyName : String, selection : Vector.<ISceneNode>) : void
		{
			var isBinding		: Boolean	= propertyName == '@';
			var index			: int		= parseInt(propertyName);
			
			if (propertyName == 'hasController')
				filterOnController(lexer, selection);
			else if (propertyName == 'hasProperty')
				filterOnProperty(lexer, selection);
			else if (propertyName == 'position')
				filterOnPosition(lexer, selection);
			else if (propertyName == 'last')
				filterLast(lexer, selection);
			else if (propertyName == 'first')
				filterFirst(lexer, selection);
			else if (index.toString() == propertyName)
			{
				if (index < selection.length)
				{
					selection[0] = selection[index];
					selection.length = 1;
				}
				else
					selection.length = 0;
			}
			else
				filterOnValue(lexer, selection, propertyName, isBinding);
		}
		
		private function filterOnValue(lexer		: XPathLexer,
									   selection	: Vector.<ISceneNode>,
									   propertyName : String,
									   isBinding	: Boolean = false) : void
		{
			var operator	: String	= lexer.getToken(true);
			var chunks		: Array		= [propertyName];
			
			while (operator == '.')
			{
				chunks.push(lexer.getToken(true));
				operator = lexer.getToken(true);
			}
			
			var value		: Object	= lexer.getValueToken();
			var numNodes	: uint		= selection.length;
			
			for (var i : int = numNodes - 1; i >= 0; --i)
			{
				var node		: ISceneNode	= selection[i];
				var nodeValue 	: Object 		= null;
				
				if (isBinding && (node['bindings'] is DataBindings))
					nodeValue = (node['bindings'] as DataBindings).getProperty(propertyName);
				else
				{
					try
					{
						nodeValue = getValueObject(node, chunks);
						if (!compare(operator, nodeValue, value))
							removeFromSelection(i);
					}
					catch (e : Error)
					{
						removeFromSelection(i);
					}
				}
				
			}
		}
		
		private function filterLast(lexer : XPathLexer, selection : Vector.<ISceneNode>) : void
		{
			lexer.checkNextToken('(');
			lexer.checkNextToken(')');
			
			if (selection.length)
			{
				selection[0] = selection[uint(selection.length - 1)];
				selection.length = 1;
			}
		}
		
		private function filterFirst(lexer : XPathLexer, selection : Vector.<ISceneNode>) : void
		{
			lexer.checkNextToken('(');
			lexer.checkNextToken(')');
			
			selection.length = 1;
		}
		
		private function filterOnController(lexer : XPathLexer, selection : Vector.<ISceneNode>) : Object
		{
			lexer.checkNextToken('(');
			var controllerName : String = lexer.getToken(true);
			lexer.checkNextToken(')');
			
			var numNodes	: uint	= selection.length;
			
			for (var i : int = numNodes - 1; i >= 0; --i)
			{
				var node			: ISceneNode	= selection[i];
				var numControllers	: uint			= node.numControllers;
				var keepSceneNode	: Boolean		= false;
				
				for (var controllerId : uint = 0; controllerId < numControllers; ++controllerId)
				{
					var controllerType : String = getQualifiedClassName(node.getController(controllerId));
					controllerType = controllerType.substr(controllerType.lastIndexOf(':') + 1);
					
					if (controllerType == controllerName)
					{
						keepSceneNode = true;
						break;
					}
				}
				
				if (!keepSceneNode)
					removeFromSelection(i);
			}
			
			return null;
		}
		
		private function filterOnPosition(lexer : XPathLexer, selection : Vector.<ISceneNode>) : void
		{
			lexer.checkNextToken('(');
			lexer.checkNextToken(')');
			
			var operator 	: String 	= lexer.getToken(true);
			var value 		: uint 		= uint(parseInt(lexer.getToken(true)));
			
			switch (operator)
			{
				case '>':
					++value;
				case '>=':
					selection = selection.slice(value);
					break;
				case '<':
					--value;
				case '<=':
					selection = selection.slice(0, value);
					break;
				case '=':
				case '==':
					selection[0] = selection[value];
					selection.length = 1;
				default:
					throw new Error('Unknown comparison operator \'' + operator + '\'');
			}
		}
		
		private function filterOnProperty(lexer : XPathLexer, selection : Vector.<ISceneNode>) : void
		{
			lexer.checkNextToken('(');
			
			var chunks		: Array 	= [lexer.getToken(true)];
			var operator	: String 	= lexer.getToken(true);
			
			while (operator == '.')
			{
				chunks.push(operator);
				operator = lexer.getToken(true);
			}
			
			if (operator != ')')
				lexer.throwParseError(')', operator);
			
			var numNodes : uint	= selection.length;
			for (var i : int = numNodes - 1; i >= 0; --i)
			{
				try
				{
					getValueObject(selection[i], chunks);
				}
				catch (e : Error)
				{
					removeFromSelection(i);
				}
			}
		}
		
		private function compare(operator : String, a : Object, b : Object) : Boolean
		{
			switch (operator)
			{
				case '>' :
					return a > b;
				case '>=' :
					return a >= b;
				case '<' :
					return a >= b;
				case '<=' :
					return a <= b;
				case '=' :
				case '==' :
					return a == b;
				case '~=' :
					var matches	: Array	= String(a).match(b);
					
					return matches && matches.length != 0;
				default:
					throw new Error('Unknown comparison operator \'' + operator + '\'');
			}
		}
	}
}