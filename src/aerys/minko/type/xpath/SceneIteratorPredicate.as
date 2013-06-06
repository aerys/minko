package aerys.minko.type.xpath
{
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.binding.DataBindings;
	
	import flash.utils.getQualifiedClassName;
	
	public final class SceneIteratorPredicate implements IPredicate
	{
		public function filterProperty(lexer : XPathLexer, propertyName : String, selection : Vector.<ISceneNode>) : void
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
							removeFromSelection(selection, i);
					}
					catch (e : Error)
					{
						removeFromSelection(selection, i);
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
					removeFromSelection(selection, i);
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
					removeFromSelection(selection, i);
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
		
		private function getValueObject(source : Object, chunks : Array) : Object
		{
			if (chunks)
				for each (var chunk : String in chunks)
				source = source[chunk];
			
			return source;
		}
		
		private function removeFromSelection(selection : Vector.<ISceneNode>, index : uint) : void
		{
			var numNodes : uint = selection.length - 1;
			
			selection[index] = selection[numNodes];
			selection.length = numNodes;
		}
	}
}