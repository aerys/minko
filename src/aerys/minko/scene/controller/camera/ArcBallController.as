package aerys.minko.scene.controller.camera
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.EnterFrameController;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.display.BitmapData;
	import flash.events.IEventDispatcher;
	import flash.events.MouseEvent;
	import flash.geom.Point;
	import flash.ui.Mouse;
	import flash.ui.MouseCursor;
	
	public class ArcBallController extends EnterFrameController
	{
		private static const TMP_MATRIX : Matrix4x4 = new Matrix4x4();
		private static const EPSILON	: Number	= 0.001;
		
		protected var _mousePosition	: Point		= new Point();
		
		private var _distance			: Number	= 0;
		private var _theta				: Number	= 0;
		private var _phi				: Number	= 0;
		private var _update				: Boolean	= true;
		
		private var _position			: Vector4	= new Vector4(0, 0, 0, 1);
		private var _lookAt				: Vector4	= new Vector4(0, 0, 0, 1);
		private var _up					: Vector4	= new Vector4(0, 1, 0, 1);
		
		private var _minDistance		: Number	= 0.1;
		private var _maxDistance		: Number	= 1000;
		
		private var _distanceStep		: Number	= 1;
		private var _thetaStep			: Number	= 0.01;
		private var _phiStep			: Number	= 0.01;
		
		public function get distance() : Number
		{
			return _distance;
		}
		public function set distance(v : Number) : void
		{
			_distance = v;
		}
		
		public function get theta() : Number
		{
			return _theta;
		}
		public function set theta(v : Number) : void
		{
			_theta = v;
		}
		
		public function get phi() : Number
		{
			return _phi;
		}
		public function set phi(v : Number)	: void
		{
			_phi = v;
		}
		
		public function get lookAt() : Vector4
		{
			return _lookAt;
		}
		
		public function get up() : Vector4
		{
			return _up;
		}
		
		public function get minDistance() : Number
		{
			return _minDistance;
		}
		public function set minDistance(v : Number)	: void
		{
			_minDistance = v;
		}
		
		public function get maxDistance() : Number
		{
			return _maxDistance;
		}
		public function set maxDistance(v : Number)	: void
		{
			_maxDistance = v;
		}
		
		public function get distanceStep() : Number
		{
			return _distanceStep;
		}
		public function set distanceStep(v : Number) : void
		{
			_distanceStep = v;
		}
		
		public function get thetaStep() : Number
		{
			return _thetaStep;
		}
		public function set thetaStep(v : Number) : void
		{
			_thetaStep = v;
		}
		
		public function get phiStep() : Number
		{
			return _phiStep;
		}
		
		public function set phiStep(v : Number)	: void
		{
			_phiStep = v;
		}
		
		public function ArcBallController()
		{
			super();
			
			_lookAt.changed.add(updateNextFrameHandler);
			_up.changed.add(updateNextFrameHandler);
		}
		
		public function bindDefaultControls(dispatcher : IEventDispatcher) : void
		{
			dispatcher.addEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelHandler);
			dispatcher.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler);
		}
		
		public function unbindDefaultControls(dispatcher : IEventDispatcher) : void
		{
			dispatcher.removeEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelHandler);
			dispatcher.removeEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler);
		}
		
		override protected function targetAddedHandler(ctrl 	: EnterFrameController,
													   target 	: ISceneNode) : void
		{
			super.targetAddedHandler(ctrl, target);
			
			_update = true;
		}
		
		override protected function sceneEnterFrameHandler(scene		: Scene,
														   viewport		: Viewport,
														   destination	: BitmapData,
														   time			: Number) : void
		{
			if (_update)
			{
				if (_distance < _minDistance)
					_distance = _minDistance;
				if (_distance > _maxDistance)
					_distance = _maxDistance;
				if (_phi <= EPSILON)
					_phi = EPSILON;
				if (_phi > Math.PI - EPSILON)
					_phi = Math.PI - EPSILON;
				
				_position.set(
					_distance * Math.cos(_theta) * Math.sin(_phi) + _lookAt.x,
					_distance * Math.cos(_phi) + _lookAt.y,
					_distance * Math.sin(_theta) * Math.sin(_phi) + _lookAt.z
				);
				
				TMP_MATRIX.lookAt(_position, _lookAt, _up).invert();
				
				var numTargets : uint = this.numTargets;
				for (var targetId : uint = 0; targetId < numTargets; ++targetId)
					getTarget(targetId).transform.copyFrom(TMP_MATRIX);
				
				_update = false;
			}
		}
		
		protected function mouseWheelHandler(e : MouseEvent) : void
		{
			_distance	-=	e.delta;
			_update		=	true;
		}
		
		protected function mouseMoveHandler(e : MouseEvent) : void
		{
			// compute position
			if (e.buttonDown)
			{
				_theta	+= (_mousePosition.x - e.stageX) / 100.0;
				_phi	+= (_mousePosition.y - e.stageY) / 100.0;
				
				_update = true;
			}
			
			_mousePosition.setTo(e.stageX, e.stageY);
		}
		
		private function updateNextFrameHandler(vector : Vector4, propertyName : String) : void
		{
			_update = true;
		}
	}
}