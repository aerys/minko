package aerys.minko.scene.controller.camera
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.EnterFrameController;
	import aerys.minko.scene.node.Camera;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.display.BitmapData;
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

	/**
	 * The ArcBallController provides methods and listen for mouse events to
	 * deliver 3rd-person like controls on a scene node.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class ArcBallController extends EnterFrameController
	{
		public static const DEFAULT_MAX_ZOOM		: Number	= 100.0;
		public static const DEFAULT_MIN_ZOOM		: Number	= 0.0;
		public static const DEFAULT_SENSITIVITY		: Number	= 0.001;
		
		private static const MIN_SPEED				: Number	= 0.01;
		
		private		var _enabled		: Boolean	= true;
		
		protected	var _transform		: Matrix4x4	= new Matrix4x4();
		
		private		var _tracking		: Boolean	= false;
		private		var _x				: Number	= 0.;
		private		var _y				: Number	= 0.;
		
		private		var _sensitivity	: Number	= 0.;
		private		var _lockedOnPoles	: Boolean	= true;
		private		var _invertX		: Boolean	= false;
		private		var _invertY		: Boolean	= false;
		
		private		var _rotationX		: Number	= 0.0;
		private		var _rotationY		: Number	= 0.0;
		
		private		var _minZoom		: Number	= 0.0;
		private		var _maxZoom		: Number	= 100.;
		
		private		var _speedScale		: Number	= .9;
		private		var _speed			: Point		= new Point();
		
		private		var _useHandCursor	: Boolean	= true;
		
		protected	var _invalid		: Boolean	= false;
		private		var _lastTime		: Number	= 0.;
		private		var _lastTarget		: Group		= null;
		
		private		var _inScene		: uint		= 0;
		
		protected function get rotationX():Number
		{
			return _rotationX;
		}

		protected function set rotationX(value:Number):void
		{
			_rotationX = value;
		}

		public function get enabled() : Boolean
		{
			return _enabled;
		}
		public function set enabled(value : Boolean) : void
		{
			_enabled = value;
		}

		public function get mouseSensitivity() : Number
		{
			return _sensitivity;
		}
		public function set mouseSensitivity(value : Number)	: void
		{
			_sensitivity = value;
		}
		
		public function get lockedOnPoles() : Boolean
		{
			return _lockedOnPoles;
		}
		public function set lockedOnPoles(value : Boolean)	: void
		{
			_lockedOnPoles = value;
		}
		
		public function get invertX() : Boolean
		{
			return _invertX;
		}
		public function set invertX(value : Boolean) : void
		{
			_invertX = value;
		}
		
		public function get invertY() : Boolean
		{
			return _invertY;
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
		
		public function ArcBallController()
		{
			super();
			
			_sensitivity = DEFAULT_SENSITIVITY;
			
			_minZoom = DEFAULT_MIN_ZOOM;
			_maxZoom = DEFAULT_MAX_ZOOM;
		}
		
		override public function clone() : AbstractController
		{
			var cloned : ArcBallController = new ArcBallController();
			
			cloned._invertX = _invertX;
			cloned._invertY = _invertY;
			cloned._lockedOnPoles = _lockedOnPoles;
			cloned._maxZoom = _maxZoom;
			cloned._minZoom = _minZoom;
			cloned._rotationX = _rotationX;
			cloned._rotationY = _rotationY;
			cloned._sensitivity = _sensitivity;
			cloned._speed.x = _speed.x;
			cloned._speed.y = _speed.y;
			cloned._speedScale = _speedScale;
			cloned._transform.copyFrom(_transform);
			cloned._useHandCursor = _useHandCursor;
			
			return cloned;
		}
		
		public function bindDefaultControls(dispatcher : IEventDispatcher) : void
		{
			Multitouch.inputMode = MultitouchInputMode.GESTURE;
			
			dispatcher.addEventListener(MouseEvent.MOUSE_DOWN, startDrag);
			dispatcher.addEventListener(MouseEvent.MOUSE_UP, stopDrag);
			dispatcher.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler); 
			dispatcher.addEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelHandler);
		}
		
		public function unbindDefaultControls(dispatcher : IEventDispatcher) : void
		{
			dispatcher.removeEventListener(MouseEvent.MOUSE_DOWN, startDrag);
			dispatcher.removeEventListener(MouseEvent.MOUSE_UP, stopDrag);
			dispatcher.removeEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler); 
			dispatcher.removeEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelHandler);
			
			stopDrag(null);
		}
		
		override protected function sceneEnterFrameHandler(scene	: Scene,
														   viewport	: Viewport,
														   destination	: BitmapData,
														   time		: Number) : void
		{
			if (!_enabled)
				return ;
			
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
			
			var mustUpdate : Boolean = _invalid;
			
			_invalid = false;
			
			if (mustUpdate)
				for (var i : uint = 0; i < numTargets; ++i)
					getTarget(i).transform.copyFrom(_transform);
		}
		
		private function startDrag(event : Event) : void
		{
			if (!_enabled)
				return ;
			
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
				_transform.prependRotation(angle, Vector4.X_AXIS);
				_invalid = true;
			}
		}
		
		public function rotateY(angle : Number) : void
		{
			if (angle != 0.)
			{
				_transform.appendRotation(angle, Vector4.Y_AXIS);
				_invalid = true;
			}
		}
		
		public function zoom(distance : Number) : void
		{
			distance += 1;
			
			_transform.prependScale(
					distance,
					Math.abs(distance),
					distance
			);
			_invalid = true;
		}
		
		private function drag(x : Number, y : Number) : void
		{
			if (_tracking)
			{
				_speed.x += (y - _y) * _sensitivity * (_invertX ? -1 : 1);
				_speed.y += (x - _x) * _sensitivity * (_invertY ? -1 : 1);
				_invalid = true;
			}
			
			_x = x;
			_y = y;
		}
		
		public function setPivot(x : Number, y : Number, z : Number) : void
		{
			_transform.setTranslation(x, y, z);
			_invalid = true;
		}
		
		private function mouseMoveHandler(event : MouseEvent) : void
		{
			drag(event.localX, event.localY);
		}
		
		private function touchMoveHander(event : TouchEvent) : void
		{
			drag(event.localX, event.localY);
		}
		
		protected function mouseWheelHandler(event : MouseEvent) : void
		{
			zoom(event.delta * -0.01);
		}
		
		private function gestureZoomHandler(event : TransformGestureEvent) : void
		{
			zoom((1 - (event.scaleX + event.scaleY) * .5) * .1);
		}
	}
}