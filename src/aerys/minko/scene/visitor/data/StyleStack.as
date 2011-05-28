package aerys.minko.scene.visitor.data
{
	import aerys.minko.type.Factory;

	public final class StyleStack
	{
		private static var _emptyObject	: Object = new Object();
		
		private var _data : Vector.<Object>;
		
		public function StyleStack()
		{
			_data = new Vector.<Object>();
			_data[0] = _emptyObject;
		}
		
		public final function get(name : String, defaultValue : Object = null) : Object
		{
			var stackHeight : uint 		= _data.length;
			var data 		: Object 	= null;
			var item 		: Object 	= null;
			
			for (var i : int = 0; i < stackHeight; ++i)
			{
				data = _data[i];
				if (data != _emptyObject)
				{
					item = data[name];
					if (item !== null)
						return item;
				}
			}
			
			if (defaultValue !== null)
				return defaultValue;
			
			throw new Error(name + ' is undefined and no default value was provided');
		}
		
		public final function isSet(name : String) : Object
		{
			return get(name, _emptyObject) !== _emptyObject;
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
							   value	: IWorldData = undefined) : StyleStack 
		{
			var stackHeight		: uint = _data.length;
			var worldDataList	: WorldDataList;
			
			for (var i : int = 0; i < stackHeight; ++i)
			{
				var data : Object = _data[i];
				if (data != _emptyObject)
				{
					worldDataList = data[name] as WorldDataList;
					if (worldDataList != null)
					{
						var cloned : WorldDataList;
						
						cloned = worldDataList.clone();
						cloned.push(value);
						set(name, cloned);
						return this;
					}
				}
			}
	
			worldDataList = new WorldDataList(name);
			worldDataList.push(value);
			set(name, worldDataList);
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
