package aerys.minko.type
{
	public final class Signal
	{
		private var _name					: String;
        private var _enabled        		: Boolean;
		private var _disableWhenNoCallbacks	: Boolean;
		
		private var _callbacks				: Vector.<Function>;
		private var _numCallbacks			: uint;
		
		private var _executed				: Boolean;
		private var _numAdded				: uint;
		private var _toAdd					: Vector.<Function>;
		private var _numRemoved				: uint;
		private var _toRemove				: Vector.<Function>;
		
        public function get enabled() : Boolean
        {
            return _enabled;
        }
        public function set enabled(value : Boolean) : void
        {
            _enabled = value;
        }
        
		public function get numCallbacks() : uint
		{
			return _numCallbacks;
		}
		
		public function Signal(name 					: String,
							   enabled 					: Boolean 	= true,
							   disableWhenNoCallbacks	: Boolean	= false)
		{
			_name = name;
            _enabled = enabled;
			_disableWhenNoCallbacks = disableWhenNoCallbacks;
		}
		
		public function add(callback : Function) : void
		{
            if (_callbacks && _callbacks.indexOf(callback) >= 0)
            {
                var removeIndex : int = _toRemove ? _toRemove.indexOf(callback) : -1;
                
                // if that callback is in the temp. remove list, we simply remove it from this list
                // instead of removing/adding it all over again
                if (removeIndex >= 0)
                {
                    --_numRemoved;
                    _toRemove[removeIndex] = _toRemove[_numRemoved];
                    _toRemove.length = _numRemoved;
                    
                    return;
                }
                else
                    throw new Error('The same callback cannot be added twice.');
            }
            
			if (_executed)
			{
				if (_toAdd)
					_toAdd.push(callback);
				else
					_toAdd = new <Function>[callback];
				++_numAdded;
				
				return ;
			}
			
			if (_callbacks)
				_callbacks[_numCallbacks] = callback;
			else
				_callbacks = new <Function>[callback];
			++_numCallbacks;
			
			if (_numCallbacks == 1 && _disableWhenNoCallbacks)
				_enabled = true;
		}
		
		public function remove(callback : Function) : void
		{
			var index : int = (_callbacks ? _callbacks.indexOf(callback) : -1);
			
			if (index < 0)
            {
                var addIndex : int = _toAdd ? _toAdd.indexOf(callback) : -1;
                
                // if that callback is in the temp. add list, we simply remove it from this list
                // instead of adding/removing it all over again
                if (addIndex >= 0)
                {
                    --_numAdded;
                    _toAdd[addIndex] = _toAdd[_numAdded];
                    _toAdd.length = _numAdded;
                }
                else
    				throw new Error('This callback does not exist.');
            }
            
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
			
			if (!_numCallbacks && _disableWhenNoCallbacks)
				_enabled = false;
		}
		
		public function hasCallback(callback : Function) : Boolean
		{
			return _callbacks && _callbacks.indexOf(callback) >= 0;
		}
		
		public function execute(...params) : void
		{
			if (_numCallbacks && _enabled)
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
