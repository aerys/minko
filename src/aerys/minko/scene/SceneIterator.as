package aerys.minko.scene
{
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.data.DataBindings;
	
	import avmplus.getQualifiedClassName;
	
	import flash.utils.Dictionary;
	import flash.utils.Proxy;
	import flash.utils.describeType;
	import flash.utils.flash_proxy;
	import flash.utils.getQualifiedClassName;
	
	public dynamic class SceneIterator extends Proxy
	{
		private static const TYPE_CACHE	: Dictionary		= new Dictionary(true);
		private static const OPERATORS	: Vector.<String>	= new <String>[
			"//", "/", "[", "]", "..", ".", "~=", "?=", "=", "@", "*"
		];
		private static const REGEX_TRIM	: RegExp			= /^\s+|\s+$/;

		private var _path		: String				= null;
		private var _selection	: Vector.<ISceneNode>	= null;
		private var _modifier	: String				= null;
		
		public function get length() : uint
		{
			return _selection.length;
		}
		
		public function SceneIterator(path 		: String,
									  selection : Vector.<ISceneNode>,
									  modifier	: String	= "")
		{
			super();
			
			_modifier = modifier;
			
			initialize(path, selection);
		}
		
		public function toString() : String
		{
			return _selection.toString();
		}
		
		override flash_proxy function setProperty(name : *, value : *):void
		{
			var propertyName : String = name;
			
			for each (var node : ISceneNode in _selection)
				getValueObject(node, _modifier)[propertyName] = value;
		}
		
		override flash_proxy function getProperty(name : *) : *
		{
			var index : int = parseInt(name);
			
			if (index == name)
				return index < _selection.length ? _selection[index] : null;
			else
			{
				return new SceneIterator(
					null, _selection, _modifier ? _modifier + "." + name : name
				);
			}
		}
		
		override flash_proxy function nextNameIndex(index : int) : int
		{
			return index < _selection.length ? index + 1 : 0;
		}
		
		override flash_proxy function nextName(index : int) : String
		{
			return String(index - 1);
		}
		
		override flash_proxy function nextValue(index : int) : *
		{
			return _selection[int(index - 1)];
		}
		
		override flash_proxy function callProperty(name:*, ...parameters):*
		{
			var methodName : String = name;
			
			for each (var node : ISceneNode in _selection)
			{
				var method : Function = getValueObject(node, _modifier)[methodName];
				
				method.apply(null, parameters);
			}
			
			return this;
		}
		
		private function initialize(path : String, selection : Vector.<ISceneNode>) : void
		{
			_path = path;
			
			// update root
			var token : String = getToken();
			
			_selection = selection.slice();
			if (token == "/")
			{
				selectRoots();
				nextToken(token);
			}
			
			// parse
			while (token = getToken())
			{
				switch (token)
				{
					case '//' :
						nextToken(token);
						selectDescendants();
						break ;
					case '/' :
						nextToken(token);
						selectChildren();
						break ;
					default :
						nextToken(token);
						parseNodeType(token);
						break ;
				}
			}
		}
		
		private function getToken(doNext : Boolean = false) : String
		{
			var token	: String	= null;
			
			if (!_path)
				return null;
			
			_path = _path.replace(/^\s+/, '');
			
			var nextOpIndex : int = int.MAX_VALUE;
			
			for each (var op : String in OPERATORS)
			{
				var opIndex : int = _path.indexOf(op);
				
				if (opIndex > 0 && opIndex < nextOpIndex)
					nextOpIndex = opIndex;
				
				if (opIndex == 0)
				{
					token = op;
					break ;
				}
			}
			
			if (!token)
				token = _path.substring(0, nextOpIndex);
			
			if (doNext)
				nextToken(token);
			
			return token;
		}
		
		private function getValueToken() : Object
		{
			var value : Object	= null;
			
			_path = _path.replace(/^\s+/, '');
			
			if (_path.charAt(0) == "'")
			{
				var endOfStringIndex : int = _path.indexOf("'", 1);
				
				if (endOfStringIndex < 0)
					throw new Error("Unterminated string expression.");
				
				var stringValue	: String	=  _path.substring(1, endOfStringIndex);
				
				_path = _path.substring(endOfStringIndex + 1);
				
				value = stringValue;
			}
			else
			{
				var token : String	= getToken(true);
				
				if (token == "true")
					value = true;
				else if (token == "false")
					value = false;
				else if (token.indexOf("0x") == 0)
					value = parseInt(token, 16);
			}
			
			return value;
		}
		
		private function nextToken(token : String) : void
		{
			_path = _path.substring(_path.indexOf(token) + token.length);
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
			var token : String = getToken();
			
			while (token == "[")
			{
				nextToken(token);
				parsePredicate();
				
				token = getToken();
			}
		}
		
		private function parsePredicate() : void
		{
			var propertyName	: String	= getToken(true);
			var isBinding		: Boolean	= propertyName == '@';
			
			if (isBinding)
				propertyName = getToken(true);
			
			var index	: int	= parseInt(propertyName);
			
			if (index.toString() == propertyName)
			{
				if (index < _selection.length)
				{
					_selection[0] = _selection[index];
					_selection.length = 1;
				}
				else
					_selection.length = 0;
			}
			else
			{
				var operator	: String	= getToken(true);
				var value		: Object	= getValueToken();
				
				if (operator == "~=")
					filterOnRegExp(propertyName, String(value), isBinding);
				else if (operator == "?=")
					filterOnController(String(value));
				else
					filterOnValue(propertyName, value, isBinding);
			}
			
			checkNextToken("]");
		}
		
		private function filterOnRegExp(propertyName : String, regExpString : String, isBinding : Boolean) : void
		{
			var numNodes	: int		= _selection.length;
			
			for (var i : int = numNodes - 1; i >= 0; --i)
			{
				var node		: ISceneNode	= _selection[i];
				var nodeValue 	: String		= null;
				
				if (isBinding && (node['bindings'] is DataBindings))
					nodeValue = String((node['bindings'] as DataBindings).getProperty(propertyName));
				else
					nodeValue = String(getValueObject(node, propertyName));
				
				var matches		: Array			= nodeValue.match(regExpString);
				
				if (!matches || matches.length == 0)
					removeFromSelection(i);
			}
		}
		
		private function filterOnValue(propertyName : String, value : Object, isBinding : Boolean) : void
		{
			var numNodes	: int	= _selection.length;
			
			for (var i : int = numNodes - 1; i >= 0; --i)
			{
				var node		: ISceneNode	= _selection[i];
				var nodeValue 	: Object 		= null;
				
				if (isBinding && (node['bindings'] is DataBindings))
					nodeValue = (node['bindings'] as DataBindings).getProperty(propertyName);
				else
					nodeValue = getValueObject(node, propertyName);
				
				if (nodeValue != value)
					removeFromSelection(i);
			}
		}
		
		private function filterOnController(controllerName : Object) : Object
		{
			var numNodes	: int		= _selection.length;
			
			for (var i : int = numNodes - 1; i >= 0; --i)
			{
				var node			: ISceneNode	= _selection[i];
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
		
		private function getValueObject(source : Object, modifier : String) : Object
		{
			if (modifier)
			{
				var tokens : Array = modifier.split(".");
				
				for each (var token : String in tokens)
					source = source[token];
			}
			
			return source;
		}
		
		private function removeFromSelection(index : int) : void
		{
			var numNodes : int = _selection.length;
			
			--numNodes;
			_selection[index] = _selection[numNodes];
			_selection.length = numNodes;
		}
		
		private function checkNextToken(expected : String) : void
		{
			var token : String = getToken(true);
			
			if (token != expected)
				throwParseError(expected, token);
		}
		
		private function throwParseError(expected	: String,
										 got		: String) : void
		{
			throw new Error(
				"Parse error: expected '" + expected
				+ "', got '" + got + "'."
			);
		}
	}
}