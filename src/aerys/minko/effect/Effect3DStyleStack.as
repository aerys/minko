package aerys.minko.effect
{
	import aerys.common.Factory;

	public class Effect3DStyleStack
	{
		private static var _emptyObject	: Object = new Object();
		
		private var _scalarData			: Vector.<Object>;
		private var _listData			: Vector.<Object>;
		
		private var _stackHeight		: uint;
		
		public function Effect3DStyleStack()
		{
			_scalarData		= new Vector.<Object>;
			_listData		= new Vector.<Object>;
			_stackHeight	= 0;
		}
		
		public function get(name : String, defaultValue : * = undefined) : *
		{
			var data : Object, item : *; 
			for (var i : int = 0; i < _stackHeight; ++i)
			{
				data = _scalarData[i];
				item = data[name];
				
				if (data != _emptyObject && item != undefined)
					return item;
				
				data = _listData[i];
				item = data[name];
				
				if (data != _emptyObject && item != undefined)
					return item;
			}
			
			if (defaultValue !== undefined)
				return defaultValue;
			
			throw new Error(name + ' is undefined an no default value was provided');
		}
		
		public function set(name : String, value : *) : Effect3DStyleStack
		{
			var current : Object = _scalarData[0];
			if (current === _emptyObject)
			{
				current = new Object();
				_scalarData[0] = current;
			}
			
			current[name] = value;
			
			return this;
		}
		
		public function append(name : String, value : * = undefined, overwriteList : Boolean = false) : Effect3DStyleStack 
		{
			var current : Object = _listData[0];
			if (current === _emptyObject)
			{
				current = new Object();
				_listData[0] = current;
			}
			
			var appendTo : StyleListElement = overwriteList ? null : get(name, null);
			
			var item : StyleListElement = Factory.getFactory(StyleListElement).create(true);
			item.previous	= appendTo;
			item.element	= value;
			current[name]	= item;
			
			return this;
		}
		
		public function push(style : Effect3DStyle = null) : void
		{
			if (style === null)
			{
				_scalarData.unshift(_emptyObject);
				_scalarData.unshift(_emptyObject);
				
				_listData.unshift(_emptyObject);
				_listData.unshift(_emptyObject);
				
				_stackHeight += 2;
				
				return;
			}
			
			var listContainer : Object = style._listContainer;
			var myLastListElement : StyleListElement;
			
			// connect the lists from the incoming style to the lists we
			// already have in the stack.
			for (var name : String in listContainer)
			{
				// search for the named element in our stack
				myLastListElement = null;
				for (var i : int = 0; i < _stackHeight; ++i)
				{
					var container : Object = _listData[i];
					if (container.hasOwnProperty(name))
					{
						myLastListElement = container[name];
						break;
					}
				}
				
				// if we have found something here, connect the first element from the Effect3DStyle
				// to our last one
				if (myLastListElement !== null)
				{
					var firstListElement : StyleListElement = listContainer[name] as StyleListElement;
					
					while (firstListElement.previous !== null)
						firstListElement = firstListElement.previous;
					
					firstListElement.previous = myLastListElement;
				}
			}
			
			_scalarData.unshift(style._dataContainer);
			_scalarData.unshift(_emptyObject);
			
			_listData.unshift(style._listContainer);
			_listData.unshift(_emptyObject);
			
			_stackHeight += 2;
		}
		
		public function pop() : void
		{
			_scalarData.shift();
			_scalarData.shift();
			
			_listData.shift();
			_listData.shift();
			
			_stackHeight -= 2;
		}
		
	}
}
