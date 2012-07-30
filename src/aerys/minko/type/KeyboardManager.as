package aerys.minko.type
{
	import flash.events.IEventDispatcher;
	import flash.events.KeyboardEvent;

	public final class KeyboardManager
	{
		public static const NO_KEY	: uint		= uint(-1);
		
		private var _keys			: Array		= [];
		
		private var _downSignals	: Array		= [];
		private var _upSignals		: Array		= [];
		private var _keyDown		: Signal	= new Signal('KeyboardManager.keyDown');
		private var _keyUp			: Signal	= new Signal('KeyboardManager.keyDown');
		
		public function KeyboardManager()
		{
		}
		
		public function bind(dispatcher : IEventDispatcher) : void
		{
			dispatcher.addEventListener(KeyboardEvent.KEY_DOWN, keyDownHandler);
			dispatcher.addEventListener(KeyboardEvent.KEY_UP, keyUpHandler);
		}
		
		public function unbind(dispatcher : IEventDispatcher) : void
		{
			dispatcher.removeEventListener(KeyboardEvent.KEY_DOWN, keyDownHandler);
			dispatcher.removeEventListener(KeyboardEvent.KEY_UP, keyUpHandler);
		}
		
		public function keyDown(keyCode : uint = 0xffffffff) : Signal
		{
			if (keyCode == NO_KEY)
				return _keyDown;
			else
				return _downSignals[keyCode] || (_downSignals[keyCode] = new Signal('KeyDown[' + keyCode + ']'));
		}
		
		public function keyUp(keyCode : uint = 0xffffffff) : Signal
		{
			if (keyCode == NO_KEY)
				return _keyUp;
			else
				return _upSignals[keyCode] || (_upSignals[keyCode] = new Signal('KeyUp[' + keyCode + ']'));
		}
		
		private function keyDownHandler(event : KeyboardEvent) : void
		{
			var keyCode : uint		= event.keyCode;
			var signal 	: Signal 	= _downSignals[keyCode] as Signal;
			
			_keys[keyCode] = true;
			
			if (signal != null)
				signal.execute(this, keyCode);
		}
		
		private function keyUpHandler(event : KeyboardEvent) : void
		{
			var keyCode : uint		= event.keyCode;
			var signal 	: Signal 	= _upSignals[keyCode] as Signal;
			
			_keys[keyCode] = false;
			
			if (signal != null)
				signal.execute(this, keyCode);
		}
		
		public function keyIsDown(keyCode : uint) : Boolean
		{
			return !!_keys[keyCode];
		}
	}
}