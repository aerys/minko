package aerys.minko.type
{
	public final class Signal
	{
		private var _callbacks		: Vector.<Function>	= new <Function>[];
		private var _numCallbacks	: uint				= 0;
		
		public function get numCallbacks() : uint
		{
			return _numCallbacks;
		}
		
		public function add(callback : Function) : void
		{
			_callbacks[_numCallbacks] = callback;
			++_numCallbacks;
		}
		
		public function remove(callback : Function) : void
		{
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
			var numCallbacks : uint = _numCallbacks;
			
			for (var i : uint = 0; i < numCallbacks; ++i)
				(_callbacks[i] as Function).apply(null, params);
		}
	}
}