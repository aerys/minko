package aerys.minko.type
{
	public final class Signal
	{
		private var _callbacks		: Vector.<Function>	= new <Function>[];
		private var _numCallbacks	: uint				= 0;
		
		private var _executed		: Boolean			= false;
		private var _toAdd			: Vector.<Function>	= new <Function>[];
		private var _toRemove		: Vector.<Function>	= new <Function>[];
		
		public function get numCallbacks() : uint
		{
			return _numCallbacks;
		}
		
		public function add(callback : Function) : void
		{
			if (_executed)
			{
				_toAdd.push(callback);
				return ;
			}
			
			_callbacks[_numCallbacks] = callback;
			++_numCallbacks;
		}
		
		public function remove(callback : Function) : void
		{
			if (_executed)
			{
				_toRemove.push(callback);
				return ;
			}
			
			var startIndex : int = _numCallbacks - 1;
			
			while (startIndex >= 0 && _callbacks[startIndex] != callback)
				--startIndex;
			
			if (startIndex < 0)
				throw new Error("This callback does not exist.");
			
			--_numCallbacks;
			_callbacks[startIndex] = _callbacks[_numCallbacks];
			_callbacks.length = _numCallbacks;
		}
		
		public function execute(...params) : void
		{
			_executed = true;
			for (var i : uint = 0; i < _numCallbacks; ++i)
				(_callbacks[i] as Function).apply(null, params);
			_executed = false;
			
			var numAdded : uint = _toAdd.length;
			for (var k : uint = 0; k < numAdded; ++k)
				add(_toAdd[k]);
			
			var numRemoved : uint = _toRemove.length;
			for (var j : uint = 0; j < numRemoved; ++j)
				remove(_toRemove[j]);
		}
	}
}