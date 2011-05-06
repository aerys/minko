package aerys.minko.effect
{
	import aerys.common.Factory;

	public final class StyleStack
	{
		private static var _emptyObject	: Object = new Object();
		
		private var _data : Vector.<Object>;
		
		public function StyleStack()
		{
			_data = new Vector.<Object>();
			_data[0] = _emptyObject;
		}
		
		public final function get(name : String, defaultValue : * = undefined) : Object
		{
			var stackHeight : uint 		= _data.length;
			var data 		: Object	= null;
			
			for (var i : int = 0; i < stackHeight; ++i)
			{
				data = _data[i];
				
				if (data != _emptyObject)
				{
					if (data[name] != undefined)
						return data[name];
				}
			}
			
			if (defaultValue !== undefined)
				return defaultValue;
			
			throw new Error(name + ' is undefined an no default value was provided');
		}
		
		public function set(name : String, value : Object) : StyleStack
		{
			var current : Object = _data[0];
			
			if (current === _emptyObject)
			{
				current = new Object();
				_data[0] = current;
			}
			
			current[name] = value;
			
			return this;
		}
		
		public function append(name		: String, 
							   value	: IStyleObject = undefined) : StyleStack 
		{
			var stackHeight		: uint = _data.length;
			var styleObjectList	: StyleObjectList;
			
			for (var i : int = 0; i < stackHeight; ++i)
			{
				var data : Object = _data[i];
				if (data != _emptyObject)
				{
					styleObjectList = data[name] as StyleObjectList;
					if (styleObjectList != null)
					{
						var cloned : StyleObjectList;
						cloned = styleObjectList.clone();
						cloned.push(value);
						set(name, cloned);
						return this;
					}
				}
			}
			
			styleObjectList = new StyleObjectList(name);
			styleObjectList.push(value);
			set(name, styleObjectList);
			
			return this;
		}
		
		public function push(style : Style = null) : void
		{
			_data.unshift(style._data);
			_data.unshift(_emptyObject);
		}
		
		public function pop() : void
		{
			_data.shift();
			_data.shift();
		}
		
	}
}
