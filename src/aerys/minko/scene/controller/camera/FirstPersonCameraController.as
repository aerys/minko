package aerys.minko.scene.controller.camera 
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.AbstractScriptController;
	import aerys.minko.scene.controller.EnterFrameController;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.display.BitmapData;
	import flash.events.IEventDispatcher;
	import flash.events.MouseEvent;
	import flash.geom.Point;
	import flash.ui.Keyboard;

	public class FirstPersonCameraController extends AbstractScriptController
	{
		private static const TMP_MATRIX 	: Matrix4x4 = new Matrix4x4();

		private var _ghostMode				: Boolean	= true;
		private var _rotation				: Vector4	= new Vector4();
		private var _enabled				: Boolean	= true;
		private var _update					: Boolean	= false;
		private var _step					: Number	= 1;
		
		private var _mousePosition			: Point		= new Point();
		private var _position				: Vector4	= new Vector4(0, 0, 0, 1);
		private var _lookAt					: Vector4	= new Vector4(0, 0, 0, 1);
		private var _up						: Vector4	= new Vector4(0, 1, 0, 1);
		
		public function get enabled() : Boolean
		{
			return _enabled;
		}

		public function set enabled(value : Boolean) : void
		{
			_enabled = value;
		}
		
		public function get ghostMode() : Boolean
		{
			return _ghostMode;
		}
		
		public function set ghostMode(value : Boolean) : void
		{
			_ghostMode = value;
		}
		
		public function get step() : Number
		{
			return _step;
		}
		
		public function set step(value : Number) : void
		{
			_step = value;
		}
				
		public function FirstPersonCameraController()
		{
			super ();
		}

		public function bindDefaultControls(dispatcher : IEventDispatcher) : FirstPersonCameraController
		{
			dispatcher.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler);
			
			return this;
		}
		
		public function unbindDefaultControls(dispatcher : IEventDispatcher) : FirstPersonCameraController
		{
			dispatcher.removeEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler);
			
			return this;
		}

		override protected function sceneEnterFrameHandler(scene		: Scene,
														   viewport		: Viewport,
														   destination	: BitmapData,
														   time			: Number) : void
		{
			super.sceneEnterFrameHandler(scene, viewport, destination, time);

			if (_update && _enabled)
			{
				_lookAt.x = _position.x + Math.sin(_rotation.y) * Math.cos(_rotation.x);
				_lookAt.y = _position.y + Math.sin(_rotation.x);
				_lookAt.z = _position.z + Math.cos(_rotation.y) * Math.cos(_rotation.x);
				
				TMP_MATRIX.lookAt(_lookAt, _position, _up);
				
				var numTargets : uint = this.numTargets;
				for (var targetId : uint = 0; targetId < numTargets; ++targetId)
					getTarget(targetId).transform.copyFrom(TMP_MATRIX);
				
				_update = false;
			}
		}

		public function walk(distance : Number) : void
		{
			if (_ghostMode)
			{
				_position.y += Math.sin(_rotation.x) * distance;
				_position.x += Math.sin(_rotation.y) * Math.cos(-_rotation.x) * distance;
				_position.z += Math.cos(_rotation.y) * Math.cos(-_rotation.x) * distance;
			}
			else
			{
				_position.x += Math.sin(_rotation.y) * distance;
				_position.z += Math.cos(_rotation.y) * distance;
			}
			
			_update = true;
		}
		
		public function strafe(distance : Number) : void
		{
			_position.x += Math.sin(_rotation.y + Math.PI / 2) * distance;
			_position.z += Math.cos(_rotation.y + Math.PI / 2) * distance;
			
			_update = true;
		}

		override protected function update(target : ISceneNode) : void
		{
			if (keyboard.keyIsDown(Keyboard.RIGHT))
				strafe(_step);
			if (keyboard.keyIsDown(Keyboard.LEFT))
				strafe(-_step);
			if (keyboard.keyIsDown(Keyboard.UP))
				walk(_step);
			if (keyboard.keyIsDown(Keyboard.DOWN))
				walk(-_step);
		}
		
		override protected function targetAddedHandler(ctrl 	: EnterFrameController,
													   target 	: ISceneNode) : void
		{
			super.targetAddedHandler(ctrl, target);
			
			_update = true;
		}
		
		private function mouseMoveHandler(event : MouseEvent) : void
		{
			if (event.buttonDown)
			{
				_rotation.y += -(_mousePosition.x - event.stageX) * .005;
				_rotation.x += (_mousePosition.y - event.stageY) * .005;
				
				_update = true;
			}
			
			_mousePosition.setTo(event.stageX, event.stageY);
		}
	}
}
