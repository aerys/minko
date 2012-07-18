package aerys.minko.type
{
	import flash.events.IEventDispatcher;
	import flash.events.KeyboardEvent;

	public final class KeyboardManager
	{
		private var _keys	: Array	= [];
		
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
		
		private function keyDownHandler(event : KeyboardEvent) : void
		{
			_keys[event.keyCode] = true;
		}
		
		private function keyUpHandler(event : KeyboardEvent) : void
		{
			_keys[event.keyCode] = false;
		}
		
		public function keyIsDown(keyCode : uint) : Boolean
		{
			return !!_keys[keyCode];
		}
	}
}