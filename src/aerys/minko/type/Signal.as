package aerys.minko.type
{
	public final class Signal
	{
		private var _name			: String;
		
		private var _callbacks		: Vector.<Function>;
		private var _numCallbacks	: uint;
		
		private var _executed		: Boolean;
		private var _numAdded		: uint;
		private var _toAdd			: Vector.<Function>;
		private var _numRemoved		: uint;
		private var _toRemove		: Vector.<Function>;
		
		public function get numCallbacks() : uint
		{
			return _numCallbacks;
		}
		
		public function Signal(name : String)
		{
			_name = name;
            _callbacks = new <Function>[];
		}
		
		public function add(callback : Function) : void
		{
            if (_callbacks.indexOf(callback) >= 0)
                throw new Error('The same callback cannot be added twice.');
            
			if (_executed)
			{
				if (_toAdd)
					_toAdd.push(callback);
				else
					_toAdd = new <Function>[callback];
				++_numAdded;
				
				return ;
			}
			
			_callbacks[_numCallbacks] = callback;
			++_numCallbacks;
		}
		
		public function remove(callback : Function) : void
		{
			var index : int = _callbacks.indexOf(callback);
			
			if (index < 0)
				throw new Error('This callback does not exist.');
            
			if (_executed)
			{
				if (_toRemove)
					_toRemove.push(callback);
				else
					_toRemove = new <Function>[callback];
				++_numRemoved;
				
				return ;
			}
			
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
			if (_numCallbacks)
			{
				_executed = true;
				for (var i : uint = 0; i < _numCallbacks; ++i)
					_callbacks[i].apply(null, params);
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
}