package aerys.minko.type
{
	public final class Signal
	{
		private var _name			: String			= null;
		
		private var _callbacks		: Array				= [];
		private var _numCallbacks	: uint				= 0;
		
		private var _executed		: Boolean			= false;
		private var _numAdded		: uint				= 0;
		private var _toAdd			: Array				= null;
		private var _numRemoved		: uint				= 0;
		private var _toRemove		: Array				= null;
		
		public function get numCallbacks() : uint
		{
			return _numCallbacks;
		}
		
		public function Signal(name : String)
		{
			_name = name;
		}
		
		public function add(callback : Function) : void
		{
			if (_executed)
			{
				if (_toAdd)
					_toAdd.push(callback);
				else
					_toAdd = [callback];
				++_numAdded;
				
				return ;
			}
			
			_callbacks[_numCallbacks] = callback;
			++_numCallbacks;
		}
		
		public function remove(callback : Function) : void
		{
			if (_executed)
			{
				if (_toRemove)
					_toRemove.push(callback);
				else
					_toRemove = [callback];
				++_numRemoved;
				
				return ;
			}
			
			var index : int = _callbacks.indexOf(callback);
			
			if (index < 0)
				throw new Error("This callback does not exist.");
			
			--_numCallbacks;
			_callbacks[index] = _callbacks[_numCallbacks];
			_callbacks.length = _numCallbacks;
		}
		
		public function hasCallback(callback : Function) : Boolean
		{
			return _callbacks.indexOf(callback) >= 0;
		}
		
		public function execute(...params) : void
		{
			_executed = true;
			for (var i : uint = 0; i < _numCallbacks; ++i)
				(_callbacks[i] as Function).apply(null, params);
			_executed = false;
			
			for (i = 0; i < _numAdded; ++i)
				add(_toAdd[i]);
			_numAdded = 0;
			_toAdd = null;
			
			for (i = 0; i < _numRemoved; ++i)
				remove(_toRemove[i]);
			_numRemoved = 0;
			_toRemove = null;
		}
	}
}