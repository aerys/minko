package aerys.minko.scene.controller.camera
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Camera;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public final class CameraController extends AbstractController
	{
		private var _camera		: Camera	= null;
		private var _viewport	: Viewport	= null;
		
		public function CameraController(viewport	: Viewport)
		{
			super(Camera);
			
			_viewport = viewport;
			
			initialize();
		}
		
		override public function clone() : AbstractController
		{
			return new CameraController(_viewport);
		}
		
		private function initialize() : void
		{
			_viewport.resized.add(viewportResizedHandler);
			
			targetAdded.add(targetAddedHandler);
		}
		
		private function targetAddedHandler(ctrl	: CameraController,
											target	: Camera) : void
		{
			if (_camera)
				throw new Error();
			
			_camera = target;
			_camera.changed.add(cameraChangedHandler);
			_camera.addedToScene.add(addedToSceneHandler);
			_camera.removedFromScene.add(removedFromSceneHandler);
			_camera.position.changed.add(viewPropertyChangedHandler);
			_camera.lookAt.changed.add(viewPropertyChangedHandler);
			_camera.up.changed.add(viewPropertyChangedHandler);
			_camera.localToWorld.changed.add(transformChangedHandler);
			
			updateProjection();
		}
		
		private function cameraChangedHandler(camera 	: Camera,
											  property	: String) : void
		{
			if (property == "fieldOfView" || property == "zNear" || property == "zFar")
				updateProjection();
			else
				updateWorldToView();
		}
		
		private function transformChangedHandler(transform	: Matrix4x4,
												 property	: String) : void
		{
			updateWorldToView();
		}
		
		private function viewPropertyChangedHandler(value : Object, property : Object) : void
		{
			updateWorldToView();
		}
		
		private function viewportResizedHandler(viewport 	: Viewport,
												width	 	: Number,
												height		: Number) : void
		{
			updateProjection();
		}
		
		private function updateProjection() : void
		{
			var viewport	: Viewport 	= _camera.viewport;
			var aspectRatio : Number 	= viewport.width / viewport.height;
			
			Matrix4x4.perspectiveFoV(
				_camera.fieldOfView,
				aspectRatio,
				_camera.zNear,
				_camera.zFar,
				_camera.projection
			);
			_camera.frustum.updateFromDescription(
				_camera.fieldOfView,
				aspectRatio,
				_camera.zNear,
				_camera.zFar
			);
			
			updateWorldToScreen();
		}
		
		private function updateWorldToView() : void
		{
			if (!_camera.parent)
				return ;
			
			var localToWorld 	: Matrix4x4 = _camera.localToWorld;
			var worldPosition	: Vector4	= _camera.worldPosition;
			var worldLookAt		: Vector4	= _camera.worldLookAt;
			var worldUp			: Vector4	= _camera.worldUp;
						
			localToWorld.transformVector(_camera.position, worldPosition);
			localToWorld.transformVector(_camera.lookAt, worldLookAt);
			localToWorld.deltaTransformVector(_camera.up, worldUp);
			worldUp.normalize();
			
			Matrix4x4.lookAt(
				worldPosition,
				worldLookAt,
				worldUp,
				_camera.worldToView
			);
			
			updateWorldToScreen();
		}
		
		private function updateWorldToScreen() : void
		{
			Matrix4x4.multiply(_camera.projection, _camera.worldToView, _camera.worldToScreen);
		}
		
		private function addedToSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			scene.bindings.add(getTarget(0) as Camera);
		}
		
		private function removedFromSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			scene.bindings.remove(getTarget(0) as Camera);
		}
	}
}