package aerys.minko.type
{
	import flash.utils.getDefinitionByName;
	import flash.utils.getQualifiedClassName;

	public class AbstractStatesManager
	{
		private var _numStates	: int				= 0;
		private var _stacks		: Array				= new Array();
		private var _masks		: Vector.<uint>		= new Vector.<uint>(32, true);
		private var _properties	: Vector.<String> 	= new Vector.<String>(32, true);
		private var _push		: Vector.<uint>		= new Vector.<uint>();
		
		private var _lock		: uint				= 0;
		private var _lockStack	: Vector.<uint>		= new Vector.<uint>();
		private var _lockOffset	: uint				= 0;
		
		protected function get lockedStates() : uint	{ return _lock; }
		
		public function AbstractStatesManager()
		{
		}
		
		protected function register(bitmask 	: uint,
									property 	: String,
									stack 		: Object = null) : void
		{
			_masks[_numStates] = bitmask;
			_properties[_numStates] = property;
			
			_stacks[_numStates] = stack || getStackObject(getQualifiedClassName(this[property]));
		
			++_numStates;
		}
		
		protected function getStackObject(propertyType : String) : Object
		{
			return new (getDefinitionByName("Vector.<" + propertyType + ">") as Class);
		}
		
		public function lock(statesMask : uint) : void
		{
			_lock |= statesMask;
		}
		
		public final function push(statesMask : uint) : void
		{
			var mask 		: uint = 0;
			var pushMask 	: uint = 0;
			
			if (!(statesMask &= ~_lock))
			{
				++_lockOffset;
				
				return;
			}
			
			for (var i : int = 0; statesMask && i < _numStates; ++i)
			{
				if (statesMask & (mask = _masks[i]))
				{
					pushState(1 << i, this[_properties[i]], _stacks[i]);
					statesMask ^= mask;
					pushMask |= mask;
				}				
			}
			
			_push[_push.length] = pushMask;
			_lockStack[_lockStack.length] = _lock;
		}
		
		public final function pop() : void
		{
			if (_lockOffset)
			{
				--_lockOffset;
				
				return ;
			}
			
			var statesMask 	: uint	= _push[int(_push.length - 1)];
			var mask 		: uint 	= 0;
			
			_push.length--;
			
			_lock = _lockStack[int(_lockStack.length - 1)];
			_lockStack.length--;
			
			for (var i : int = 0; statesMask && i < _numStates; ++i)
			{
				if ((mask = _masks[i]) & statesMask)
				{
					popState(mask, _properties[i], _stacks[i]);
					statesMask ^= mask;
				}
			}
		}
		
		public function pushState(mask : uint, value : Object, stack : Object) : void
		{
			stack[stack.length] = value;
		}
		
		public function popState(mask : uint, property : String, stack : Object) : void
		{
			this[property] = stack[int(stack.length - 1)];
			stack.length--;
		}
	}
}