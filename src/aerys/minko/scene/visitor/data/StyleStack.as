package aerys.minko.scene.visitor.data
{
	import aerys.minko.type.Factory;

	public final class StyleStack
	{
		private static var _empty	: Array = new Array();
		
		private var _data 	: Vector.<Array>	= new Vector.<Array>();
		private var _top	: Array				= new Array();
		private var _size	: int				= 0;
		
		public final function get(styleId : uint, defaultValue : Object = null) : Object
		{
			var item 		: Object 	= _top[styleId];
			
			if (item !== null)
				return item;
			
			for (var i : int = _size - 1; i >= 0; --i)
			{
				item = _data[i][styleId];
				if (item !== null)
				{
					_top[styleId] = item;
					
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
			_top[styleId] = value;
			
			return this;
		}

		public function push(style : Style = null) : void
		{
			_data[_size] = style._data;
			++_size;
		}
		
		public function pop() : void
		{
			--_size;
			_top.length = 0;
		}
		
	}
}
