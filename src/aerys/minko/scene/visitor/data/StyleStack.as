package aerys.minko.scene.visitor.data
{
	import aerys.minko.type.Factory;

	public final class StyleStack
	{
		private static var _empty	: Array = new Array();
		
		private var _data : Vector.<Array>;
		
		public function StyleStack()
		{
			_data = new Vector.<Array>();
			_data[0] = _empty;
		}
		
		public final function get(id : uint, defaultValue : Object = null) : Object
		{
			var stackHeight : uint 		= _data.length;
			var data 		: Array 	= null;
			var item 		: Object 	= null;
			
			for (var i : int = 0; i < stackHeight; ++i)
			{
				data = _data[i];
				if (data != _empty)
				{
					item = data[id];
					if (item !== null)
						return item;
				}
			}
			
			if (defaultValue !== null)
				return defaultValue;
			
			throw new Error(id + ' is undefined and no default value was provided');
		}
		
		public final function isSet(id : int) : Object
		{
			return get(id, _empty) !== _empty;
		}
		
		public function set(styleId : int, value : Object) : StyleStack
		{
			var current : Array = _data[0];
			
			if (current === _empty)
			{
				current = new Array();
				_data[0] = current;
			}
			
			current[styleId] = value;
			
			return this;
		}
		
		public function append(id		: int, 
							   value	: IWorldData = undefined) : StyleStack 
		{
			var stackHeight		: uint = _data.length;
			var worldDataList	: WorldDataList;
			
			for (var i : int = 0; i < stackHeight; ++i)
			{
				var data : Object = _data[i];
				if (data != _empty)
				{
					worldDataList = data[id] as WorldDataList;
					if (worldDataList != null)
					{
						var cloned : WorldDataList;
						
						cloned = worldDataList.clone();
						cloned.push(value);
						set(id, cloned);
						return this;
					}
				}
			}
	
			worldDataList = new WorldDataList(id);
			worldDataList.push(value);
			set(id, worldDataList);
			return this;
		}

		public function push(style : Style = null) : void
		{
			_data.unshift(style._data);
			_data.unshift(_empty);
		}
		
		public function pop() : void
		{
			_data.shift();
			_data.shift();
		}
		
	}
}
