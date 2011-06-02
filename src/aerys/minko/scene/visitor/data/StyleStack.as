package aerys.minko.scene.visitor.data
{
	import aerys.minko.type.Factory;

	public final class StyleStack
	{
		private static var _empty	: Array 			= new Array();
		private static var _free	: Vector.<Array>	= new Vector.<Array>();
		private static var _numFree	: int				= 0;
		
		private var _data 	: Vector.<Array>	= new Vector.<Array>();
		private var _size	: int				= 0;
		private var _cache	: Array				= new Array();
		
		public final function get(styleId : uint, defaultValue : Object = null) : Object
		{
			if (_cache.hasOwnProperty(styleId))
				return _cache[styleId];
			
			for (var i : int = _size - 1; i >= 0; --i)
			{
				var data : Array = _data[i];
				
				if (data.hasOwnProperty(styleId))
				{
					var item : Object = data[styleId];
					
					_cache[styleId] = item;
					
					return item;
				}
			}
			
			if (defaultValue !== null)
				return defaultValue;
			
			throw new Error(Style.getStyleName(styleId) + ' is undefined and no default value was provided');
		}
		
		public final function isSet(id : int) : Object
		{
			return get(id, _empty) !== _empty;
		}
		
		public function set(styleId : int, value : Object) : StyleStack
		{
			var top : Array = _data[int(_size - 1)];
			
			if (top === _empty)
			{
				if (_numFree > 0)
				{
					top = _free[int(--_numFree)];
					top.length = 0;
				}
				else
				{
					top = new Array();
				}
				
				_data[int(_size - 1)] = top;
			}
			
			_cache[styleId] = value;
			top[styleId] = value;
			
			return this;
		}

		public function push(style : Style = null) : void
		{
			_data[int(_size++)] = style._data;
			_data[int(_size++)] = _empty;
		}
		
		public function pop() : void
		{
			var free : Array = _data[_size - 1];
			
			if (free !== _empty)
				_free[int(_numFree++)] = free;
			
			_size -= 2;
			
			_cache.length = 0;
		}
		
	}
}
