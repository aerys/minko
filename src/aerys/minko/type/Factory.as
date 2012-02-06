package aerys.minko.type
{
	import flash.utils.Dictionary;

	public final class Factory
	{
		private static var _factories	: Dictionary		= new Dictionary(true);

		private var _numFree	: uint				= 0;
		private var _free		: Vector.<Object>	= new Vector.<Object>();

		private var _numMarked	: uint				= 0;
		private var _mark		: Vector.<Object>	= new Vector.<Object>();

		private var _class		: Class				= null;

		public static function getFactory(classObject	: Class) : Factory
		{
			return _factories[classObject] || (_factories[classObject] = new Factory(classObject));
		}

		public function Factory(classObject : Class) : void
		{
			_class = classObject;
		}

		public function create(autoMark : Boolean 	= false,
							   values 	: Object 	= null) : Object
		{
			var result : Object = null;

			if (_numFree != 0)
				result = _free[int(--_numFree)];
			else
				result = new _class();

			if (values)
				for (var property : String in values)
					result[property] = values[property];

			if (autoMark)
				_mark[int(_numMarked++)] = result;

			return result;
		}

		public function free(value : Object) : void
		{
			_free[int(_numFree++)] = value;
		}

		public function mark(value : Object) : void
		{
			_mark[int(_numMarked++)] = value;
		}

		public function sweep() : uint
		{
			var count : uint = _numMarked;

			while (_numMarked)
				_free[int(_numFree++)] = _mark[int(--_numMarked)];

			_mark.length = 0;

			return count;
		}

		public static function sweep() : uint
		{
			var count : uint = 0;

			for each (var f : Factory in _factories)
				count += f.sweep();

			return count;
		}

		public function clear() : void
		{
			_free.length = _numFree = 0;
			_mark.length = _numMarked = 0;
		}
	}
}