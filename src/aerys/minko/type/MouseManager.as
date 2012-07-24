package aerys.minko.type
{
	import flash.events.IEventDispatcher;
	import flash.events.MouseEvent;
	import flash.ui.Mouse;

	public final class MouseManager
	{
		private var _x					: Number			= 0.;
		private var _y					: Number			= 0.;
		
		private var _leftButtonDown		: Boolean			= false;
		private var _middleButtonDown	: Boolean			= false;
		private var _rightButtonDown	: Boolean			= false;
		
		private var _mouseDown			: Signal			= new Signal("mouseDown");
		private var _mouseUp			: Signal			= new Signal("mouseUp");
		
		private var _cursors			: Vector.<String>	= new <String>[];
		
		public function get x() : Number
		{
			return _x;
		}
		
		public function get y() : Number
		{
			return _y;
		}
		
		public function get leftButtonDown() : Boolean
		{
			return _leftButtonDown;
		}
		
		public function get middleButtonDown() : Boolean
		{
			return _middleButtonDown;
		}
		
		public function get rightButtonDown() : Boolean
		{
			return _rightButtonDown;
		}
		
		public function get cursor() : String
		{
			return Mouse.cursor;
		}
		
		public function get mouseDown() : Signal
		{
			return _mouseDown;
		}
		
		public function get mouseUp() : Signal
		{
			return _mouseUp;
		}
		
		public function MouseManager()
		{
		}
		
		public function bind(dispatcher : IEventDispatcher) : void
		{
			dispatcher.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler);
			dispatcher.addEventListener(MouseEvent.MOUSE_DOWN, mouseLeftDownHandler);
			dispatcher.addEventListener(MouseEvent.MOUSE_UP, mouseLeftUpHandler);
			dispatcher.addEventListener(MouseEvent.RIGHT_MOUSE_DOWN, mouseRightDownHandler);
			dispatcher.addEventListener(MouseEvent.RIGHT_MOUSE_UP, mouseRightUpHandler);
			dispatcher.addEventListener(MouseEvent.MIDDLE_MOUSE_DOWN, mouseMiddleDownHandler);
			dispatcher.addEventListener(MouseEvent.MIDDLE_MOUSE_UP, mouseMiddleUpHandler);
		}
		
		public function unbind(dispatcher : IEventDispatcher) : void
		{
			dispatcher.removeEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler);
			dispatcher.removeEventListener(MouseEvent.MOUSE_DOWN, mouseLeftDownHandler);
			dispatcher.removeEventListener(MouseEvent.MOUSE_UP, mouseLeftUpHandler);
			dispatcher.removeEventListener(MouseEvent.RIGHT_MOUSE_DOWN, mouseRightDownHandler);
			dispatcher.removeEventListener(MouseEvent.RIGHT_MOUSE_UP, mouseRightUpHandler);
			dispatcher.removeEventListener(MouseEvent.MIDDLE_MOUSE_DOWN, mouseMiddleDownHandler);
			dispatcher.removeEventListener(MouseEvent.MIDDLE_MOUSE_UP, mouseMiddleUpHandler);
		}
		
		private function mouseMoveHandler(event : MouseEvent) : void
		{
			_x = event.localX;
			_y = event.localY;
		}
		
		private function mouseLeftDownHandler(event : MouseEvent) : void
		{
			_leftButtonDown = true;
			_mouseDown.execute();
		}
		
		private function mouseLeftUpHandler(event : MouseEvent) : void
		{
			_leftButtonDown = false;
			_mouseUp.execute();
		}
		
		private function mouseMiddleDownHandler(event : MouseEvent) : void
		{
			_rightButtonDown = true;
		}
		
		private function mouseMiddleUpHandler(event : MouseEvent) : void
		{
			_rightButtonDown = false;
		}
		
		private function mouseRightDownHandler(event : MouseEvent) : void
		{
			_rightButtonDown = true;
		}
		
		private function mouseRightUpHandler(event : MouseEvent) : void
		{
			_rightButtonDown = false;
		}
		
		public function pushCursor(cursor : String) : MouseManager
		{
			_cursors.push(Mouse.cursor);
			Mouse.cursor = cursor;
			
			return this;
		}
		
		public function popCursor() : MouseManager
		{
			Mouse.cursor = _cursors.pop();
			
			return this;
		}
	}
}