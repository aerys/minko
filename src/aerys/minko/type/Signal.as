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
		
		// The ...p rest parameter here was bad because this is called so often and was creating a new
		// array for every call.  Using undefined here to indicate no value passed.
		// It *may* break if someone does pass undefined instead of null...
		// Unfortunately there is no other compile time constant that will work
		// Other options
		//   NaN but it isn't equal to itself so the if tree would become p5 != p5 
		//   Some number value - There is a chance any number chosen may be a real parameter
		//	 Some string value - Same as number
		// Things that don't work
		//	 any static constant of type object as it is runtime constant not compile time
		public function execute(p0:* = undefined, 
														p1:* = undefined, 
														p2:* = undefined, 
														p3:* = undefined, 
														p4:* = undefined, 
														p5:* = undefined, 
														p6:* = undefined, 
														p7:* = undefined, 
														p8:* = undefined, 
														p9:* = undefined) : void
		{
			var i:uint;
			if (_numCallbacks && _enabled)
			{
				_executed = true;
				{
					// Moved loop into each conditional
					// if-else tree http://jacksondunstan.com/articles/2178
					if (p5 === undefined) 
					{
						if (p2 === undefined) 
						{
							if (p1 !== undefined) 
							{
								for (i = 0; i < _numCallbacks; ++i)
									_callbacks[i](p0, p1);
							} 
							else if (p0 !== undefined) 
							{
								for (i = 0; i < _numCallbacks; ++i)
									_callbacks[i](p0);
							} 
							else 
							{
								//if (p0 === undefined) {
								for (i = 0; i < _numCallbacks; ++i)
									_callbacks[i]();
							}
						} 
						else 
						{
							if (p4 !== undefined) 
							{
								for (i = 0; i < _numCallbacks; ++i)
									_callbacks[i](p0, p1, p2, p3, p4);
							} 
							else if (p3 !== undefined) 
							{
								for (i = 0; i < _numCallbacks; ++i)
									_callbacks[i](p0, p1, p2, p3);
							} 
							else 
							{
								// if (p2 !== undefined) {
								for (i = 0; i < _numCallbacks; ++i)
									_callbacks[i](p0, p1, p2);
							}
						}
					} 
					else 
					{
						if (p7 === undefined) 
						{
							if (p6 !== undefined) 
							{
								for (i = 0; i < _numCallbacks; ++i)
									_callbacks[i](p0, p1, p2, p3, p4, p5, p6);
							}
							else if (p5 !== undefined) 
							{
								for (i = 0; i < _numCallbacks; ++i)
									_callbacks[i](p0, p1, p2, p3, p4, p5);
							}
							else
							{
								// if (p7 !== undefined) {
								for (i = 0; i < _numCallbacks; ++i)
									_callbacks[i](p0, p1, p2, p3, p4, p5, p6, p7);
							}			
						} 
						else 
						{
							if (p9 !== undefined) 
							{
								for (i = 0; i < _numCallbacks; ++i)
									_callbacks[i](p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);
							} 
							else if (p8 !== undefined) 
							{
								for (i = 0; i < _numCallbacks; ++i)
									_callbacks[i](p0, p1, p2, p3, p4, p5, p6, p7, p8);
							}
						}
					}
				}
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
