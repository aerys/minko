package aerys.minko.scene.controller.camera
{
	import flash.display.BitmapData;
	import flash.events.IEventDispatcher;
	import flash.events.MouseEvent;
	import flash.geom.Point;
	
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.EnterFrameController;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public class ArcBallController extends EnterFrameController
	{
		private static const TMP_MATRIX : Matrix4x4 = new Matrix4x4();
		private static const EPSILON	: Number	= 0.001;
		
		protected var _mousePosition	: Point		= new Point();
		
		private var _enabled			: Boolean	= true;
		
		private var _previousTime		: uint		= 0;
		
		private var _distance			: Number	= 1.;
		private var _yaw				: Number	= 0;
		private var _pitch				: Number	= 0;
		private var _newDistance		: Number	= 1.;
		private var _newYaw				: Number	= 0;
		private var _newPitch			: Number	= 0;
		private var _update				: Boolean	= true;
		
		private var _position			: Vector4	= new Vector4(0, 0, 0, 1);
		private var _lookAt				: Vector4	= new Vector4(0, 0, 0, 1);
		private var _up					: Vector4	= new Vector4(0, 1, 0, 1);
		
		private var _minDistance		: Number	= 1.;
		private var _maxDistance		: Number	= 1000;
		
		private var _distanceStep		: Number	= 1;
		private var _yawStep			: Number	= 0.01;
		private var _pitchStep			: Number	= 0.01;
		
		private var _inertia			: Number	= 1.0;
		private var _interpolationSpeed	: Number	= 1;
		private var _speed				: Number	= 1;
		
		public function get speed() : Number
		{
			return _speed;
		}

		public function set speed(value : Number) : void
		{
			_speed = value;
			_update = true;
		}

		public function get inertia() : Number
		{
			return _inertia;
		}

		public function set inertia(value : Number) : void
		{
			_inertia = value;
		}

		public function get interpolationSpeed() : Number
		{
			return _interpolationSpeed;
		}

		public function set interpolationSpeed(value : Number) : void
		{
			_interpolationSpeed = value;
		}

		public function get enabled() : Boolean
		{
			return _enabled;
		}
		public function set enabled(value : Boolean) : void
		{
			_enabled = value;
		}
		
		protected function set update(value : Boolean) : void
		{
			_update = value;
		}
		
		/**
		 * Distance between look at point and target
		 */		
		public function get distance() : Number
		{
			return _newDistance;
		}
		public function set distance(value : Number) : void
		{
			_newDistance = value;
			_update = true;
		}
		
		/**
		 * Horizontal rotation angle (in radians)
		 */		
		public function get yaw() : Number
		{
			return _newYaw;
		}
		public function set yaw(value : Number) : void
		{
			_newYaw = value;
			_update = true;
		}
		
		/**
		 * Vertical rotation angle (in radians)
		 */		
		public function get pitch() : Number
		{
			return _newPitch;
		}
		public function set pitch(value : Number)	: void
		{
			_newPitch = value;
			_update = true;
		}
		
		/**
		 * Position the targets will look at
		 */
		public function get lookAt() : Vector4
		{
			return _lookAt;
		}
		
		/**
		 * Up vector the targets will rotate around
		 */
		public function get up() : Vector4
		{
			return _up;
		}
		
		/**
		 * Minimum distance constrain between look at point and target.
		 */		
		public function get minDistance() : Number
		{
			return _minDistance;
		}
		public function set minDistance(value : Number)	: void
		{
			_minDistance = value;
			_update = true;
		}
		
		/**
		 * Maximum distance constrain between look at point and target.
		 */		
		public function get maxDistance() : Number
		{
			return _maxDistance;
		}
		public function set maxDistance(value : Number)	: void
		{
			_maxDistance = value;
			_update = true;
		}
		
		/**
		 * Distance step applied to the camera when the mouse wheel is used in meters/wheel rotation unit
		 */		
		public function get distanceStep() : Number
		{
			return _distanceStep;
		}
		public function set distanceStep(value : Number) : void
		{
			_distanceStep = value;
		}
		
		/**
		 * Horizontal angular step applied to the camera when the mouse is moved in radian/pixel
		 */		
		public function get yawStep() : Number
		{
			return _yawStep;
		}
		public function set yawStep(value : Number) : void
		{
			_yawStep = value;
		}
		
		/**
		 * Vertical angular step applied to the camera when the mouse is moved in radian/pixel
		 */		
		public function get pitchStep() : Number
		{
			return _pitchStep;
		}
		
		public function set pitchStep(value : Number)	: void
		{
			_pitchStep = value;
		}
		
		public function ArcBallController()
		{
			super();
			
			_newPitch = Math.PI * .5;
			_lookAt.changed.add(updateNextFrameHandler);
			_up.changed.add(updateNextFrameHandler);
		}
		
		public function bindDefaultControls(dispatcher : IEventDispatcher) : ArcBallController
		{
			dispatcher.addEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelHandler);
			dispatcher.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler);
			
			return this;
		}
		
		public function unbindDefaultControls(dispatcher : IEventDispatcher) : ArcBallController
		{
			dispatcher.removeEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelHandler);
			dispatcher.removeEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler);
			
			return this;
		}
		
		override protected function targetAddedHandler(ctrl 	: EnterFrameController,
													   target 	: ISceneNode) : void
		{
			super.targetAddedHandler(ctrl, target);
			
			_update = true;
			updateTargets();
		}
		
		override protected function sceneEnterFrameHandler(scene		: Scene,
														   viewport		: Viewport,
														   destination	: BitmapData,
														   time			: Number) : void
		{
			updateTargets((time - _previousTime) * .001);
			_previousTime = time;
		}
		
		private function clampValues() : void
		{
			if (_newDistance < _minDistance)
				_newDistance = _minDistance;
			if (_newDistance > _maxDistance)
				_newDistance = _maxDistance;
			if (_newPitch <= EPSILON)
				_newPitch = EPSILON;
			if (_newPitch > Math.PI - EPSILON)
				_newPitch = Math.PI - EPSILON;
		}
		
		private function inertiaConverged() : Boolean
		{
			var distanceDiff	: Number = _newDistance - _distance;
			var pitchDiff		: Number = _newPitch - _pitch;
			var yawDiff			: Number = _newYaw - _yaw;
			
			return distanceDiff > -EPSILON && distanceDiff < EPSILON &&
				pitchDiff > -EPSILON && pitchDiff < EPSILON &&
				yawDiff > -EPSILON && yawDiff < EPSILON;
		}

		private function updateTargets(time : Number = 1) : void
		{
			var enableInertia	: Boolean	= (_inertia != 1. && _interpolationSpeed != 1.);
			if ((enableInertia && !inertiaConverged()) || (_update && _enabled))
			{
				clampValues();
				
				if (enableInertia)
                {
                    var dt      : Number = time * _interpolationSpeed;
                    var factor  : Number = _inertia;

                    _distance   = (((_distance * factor)    + (_newDistance * dt)) / (factor + dt));
                    _pitch      = (((_pitch * factor)       + (_newPitch * dt)) / (factor + dt));
                    _yaw        = (((_yaw * factor)         + (_newYaw * dt)) / (factor + dt));
                }
				else
				{
					_yaw = _newYaw;
					_pitch = _newPitch;
					_distance = _newDistance;
				}
				
				_position.set(
					_speed * (_lookAt.x + _distance * Math.cos(_yaw) * Math.sin(_pitch)),
					_speed * (_lookAt.y + _distance * Math.cos(_pitch)),
					_speed * (_lookAt.z + _distance * Math.sin(_yaw) * Math.sin(_pitch))
				);
				TMP_MATRIX.lookAt(_lookAt, _position, _up);
				
				var numTargets : uint = this.numTargets;
				for (var targetId : uint = 0; targetId < numTargets; ++targetId)
					getTarget(targetId).transform.copyFrom(TMP_MATRIX);
				
				_update = false;
			}
		}
		
		protected function mouseWheelHandler(e : MouseEvent) : void
		{
			_newDistance	-=	e.delta * _distanceStep;
			_update			=	true;
		}
		
		protected function mouseMoveHandler(e : MouseEvent) : void
		{
			// compute position
			if (e.buttonDown && _enabled)
			{
				_newYaw		+= (_mousePosition.x - e.stageX) * _yawStep;
				_newPitch	+= (_mousePosition.y - e.stageY) * _pitchStep;
				
				_update = true;
			}
			
			_mousePosition.setTo(e.stageX, e.stageY);
		}
		
		private function updateNextFrameHandler(vector : Vector4) : void
		{
			_update = true;
		}
    }
}