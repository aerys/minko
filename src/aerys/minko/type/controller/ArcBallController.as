package aerys.minko.type.controller
{
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.events.Event;
	import flash.events.IEventDispatcher;
	import flash.events.MouseEvent;
	import flash.events.TouchEvent;
	import flash.events.TransformGestureEvent;
	import flash.geom.Point;
	import flash.ui.Mouse;
	import flash.ui.MouseCursor;
	import flash.ui.Multitouch;
	import flash.ui.MultitouchInputMode;

	public class ArcBallController implements IController
	{
		public static const DEFAULT_MAX_ZOOM		: Number	= 100.0;
		public static const DEFAULT_MIN_ZOOM		: Number	= 0.0;
		public static const DEFAULT_SENSITIVITY		: Number	= 0.001;
		
		private static const MIN_SPEED	: Number	= 0.01;
		
		private var _targets		: Vector.<IScene>	= null;
		
		private var _tracking		: Boolean			= false;
		private var _x				: Number			= 0.;
		private var _y				: Number			= 0.;
		
		private var _sensitivity	: Number			= 0.;
		private var _lockedOnPoles	: Boolean			= true;
		private var _invertX		: Boolean			= false;
		private var _invertY		: Boolean			= false;
		
		private var _rotationX		: Number			= 0.0;
		private var _rotationY		: Number			= 0.0;
		
		private var _minZoom		: Number			= 0.0;
		private var _maxZoom		: Number			= 100.;
		
		private var _speedScale		: Number			= .9;
		private var _speed			: Point				= new Point();
		
		private var _useHandCursor	: Boolean			= true;
		
		private var _touchPointId	: int				= 0;

		public function get targets() : Vector.<IScene>
		{
			return _targets;
		}

		public function get mouseSensitivity() : Number
		{
			return _sensitivity;
		}
		
		public function get lockedOnPoles() : Boolean
		{
			return _lockedOnPoles;
		}
		
		public function get invertX() : Boolean
		{
			return _invertX;
		}
		
		public function get invertY() : Boolean
		{
			return _invertY;
		}
		
		public function set mouseSensitivity(value : Number)	: void
		{
			_sensitivity = value;
		}
		
		public function set lockedOnPoles(value : Boolean)	: void
		{
			_lockedOnPoles = value;
		}
		
		public function set invertX(value : Boolean) : void
		{
			_invertX = value;
		}
		
		public function set invertY(value : Boolean) : void
		{
			_invertY = value;
		}
		
		public function get speedScale() : Number
		{
			return _speedScale;
		}
		
		public function set speedScale(value : Number) : void
		{
			_speedScale = value;
		}
		
		public function get useHandCursor() : Boolean
		{
			return _useHandCursor;
		}
		
		public function set useHandCursor(value : Boolean) : void
		{
			_useHandCursor = value;
		}
		
		public function ArcBallController(...targets)
		{
			_targets = Vector.<IScene>(targets);
		
			_sensitivity = DEFAULT_SENSITIVITY;
			
			_minZoom = DEFAULT_MIN_ZOOM;
			_maxZoom = DEFAULT_MAX_ZOOM;
		}
		
		public function bindDefaultControls(dispatcher : IEventDispatcher) : void
		{
			Multitouch.inputMode = MultitouchInputMode.GESTURE;
			
			dispatcher.addEventListener(Event.ENTER_FRAME, enterFrameHandler);

			dispatcher.addEventListener(MouseEvent.MOUSE_DOWN, startDrag);
			dispatcher.addEventListener(MouseEvent.MOUSE_UP, stopDrag);
			dispatcher.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler); 
			dispatcher.addEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelHandler);
		}
		
		public function unbindDefaultControls(dispatcher : IEventDispatcher) : void
		{
			dispatcher.removeEventListener(Event.ENTER_FRAME, enterFrameHandler);
			
			dispatcher.removeEventListener(MouseEvent.MOUSE_DOWN, startDrag);
			dispatcher.removeEventListener(MouseEvent.MOUSE_UP, stopDrag);
			dispatcher.removeEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler); 
			dispatcher.removeEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelHandler);
		}
		
		private function enterFrameHandler(event : Event) : void
		{
			if (_speed.x != 0. || _speed.y != 0.)
			{
				rotateX(_speed.x);
				rotateY(_speed.y);
			}
			
			_speed.x *= _speedScale;
			if (_speed.x < MIN_SPEED && _speed.x > -MIN_SPEED)
				_speed.x = 0.;
			
			_speed.y *= _speedScale;
			if (_speed.y < MIN_SPEED && _speed.y > -MIN_SPEED)
				_speed.y = 0.;
		}
		
		private function startDrag(event : Event) : void
		{
			_tracking = true;
			_speed.setTo(0, 0);
			
			if (_useHandCursor)
				Mouse.cursor = MouseCursor.HAND;
		}
		
		private function stopDrag(event : Event) : void
		{
			_tracking = false;
			
			Mouse.cursor = MouseCursor.AUTO;
		}
		
		public function rotateX(angle : Number) : void
		{
			var rotX 	: Number	= _rotationX + angle;
			
			if (_lockedOnPoles)
			{
				if (rotX > Math.PI * .5)
					rotX = Math.PI * .5;
				if (rotX < -Math.PI * .5)
					rotX = -Math.PI * .5;
				
				angle = rotX - _rotationX;
			}
			
			_rotationX = rotX;
			
			if (angle != 0.)
			{
				for (var i : int = _targets.length - 1; i >= 0; --i)
				{
					var target : Group = Group(_targets[i]);
					
					target.transform.prependRotation(angle, Vector4.X_AXIS);
				}
			}
		}
		
		public function rotateY(angle : Number) : void
		{
			for (var i : int = _targets.length - 1; i >= 0; --i)
			{
				var target : Group = Group(_targets[i]);

				target.transform.appendRotation(angle, Vector4.Y_AXIS);
			}
		}
		
		public function zoom(distance : Number) : void
		{
			distance += 1;
			
			for (var i : int = _targets.length - 1; i >= 0; --i)
			{
				var target : Group = Group(_targets[i]);
				
				target.transform.prependScale(
					distance,
					Math.abs(distance),
					distance
				);
			}
		}
		
		private function drag(x : Number, y : Number) : void
		{
			if (_tracking)
			{
				_speed.x += (y - _y) * _sensitivity * (_invertX ? -1 : 1);
				_speed.y += (x - _x) * _sensitivity * (_invertY ? -1 : 1);
			}
			
			_x = x;
			_y = y;
		}
		
		public function setPivot(x : Number, y : Number, z : Number) : void
		{
			for (var i : int = _targets.length - 1; i >= 0; --i)
			{
				var transform : Matrix4x4 = Group(_targets[i]).transform;
				
				transform.setTranslation(x, y, z);
			}
		}
		
		private function mouseMoveHandler(event : MouseEvent) : void
		{
			drag(event.localX, event.localY);
		}
		
		private function touchMoveHander(event : TouchEvent) : void
		{
			drag(event.localX, event.localY);
		}
		
		private function mouseWheelHandler(event : MouseEvent) : void
		{
			zoom(event.delta * -0.01);
		}
		
		private function gestureZoomHandler(event : TransformGestureEvent) : void
		{
			zoom((1 - (event.scaleX + event.scaleY) * .5) * .1);
		}
	}
}