package aerys.minko.scene.controller.camera
{
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.data.CameraDataProvider;
	import aerys.minko.scene.node.Camera;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public final class CameraController extends AbstractController
	{
		private var _camera		: Camera	= null;
		
		public function CameraController()
		{
			super(Camera);
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(controller	: CameraController,
											target		: Camera) : void
		{
			if (_camera != null)
				throw new Error();
			
			_camera = target;
			_camera.addedToScene.add(addedToSceneHandler);
			_camera.removedFromScene.add(removedFromSceneHandler);
			_camera.localToWorld.changed.add(localToWorldChangedHandler);
		}
		
		private function targetRemovedHandler(controller	: CameraController,
											  target		: Camera) : void
		{
			_camera.addedToScene.remove(addedToSceneHandler);
			_camera.removedFromScene.remove(removedFromSceneHandler);
			_camera.localToWorld.changed.remove(localToWorldChangedHandler);
			_camera = null;
		}
		
		private function addedToSceneHandler(camera : Camera, scene : Scene) : void
		{
			var sceneBindings : DataBindings = scene.bindings;
			
			sceneBindings.addProvider(camera.cameraData);
			sceneBindings.addCallback('viewportWidth', viewportSizeChanged);
			sceneBindings.addCallback('viewportHeight', viewportSizeChanged);
			
			updateProjection();
			localToWorldChangedHandler(_camera.localToWorld, null);
		}
		
		private function removedFromSceneHandler(camera : Camera, scene : Scene) : void
		{
			var sceneBindings : DataBindings = scene.bindings;
			
			sceneBindings.removeProvider(camera.cameraData);
			sceneBindings.removeCallback('viewportWidth', viewportSizeChanged);
			sceneBindings.removeCallback('viewportHeight', viewportSizeChanged);
		}
		
		private function localToWorldChangedHandler(localToWorld : Matrix4x4, propertyName : String) : void
		{
			var transform 	: Matrix4x4 			= _camera.transform;
			var cameraData	: CameraDataProvider	= _camera.cameraData;
			
			transform.transformVector(Vector4.ZERO,		cameraData.position);
			transform.transformVector(Vector4.Z_AXIS,	cameraData.lookAt);
			transform.transformVector(Vector4.Y_AXIS,	cameraData.up);
			
			localToWorld.transformVector(Vector4.ZERO,		cameraData.worldPosition);
			localToWorld.transformVector(Vector4.Z_AXIS,	cameraData.worldLookAt);
			localToWorld.transformVector(Vector4.Y_AXIS,	cameraData.worldUp);
			
			cameraData.screenToWorld.lock()
				.copyFrom(cameraData.screenToView)
				.append(localToWorld)
				.unlock();
			
			cameraData.worldToScreen.lock()
				.copyFrom(_camera.worldToLocal)
				.append(cameraData.projection)
				.unlock();
		}
		
		private function viewportSizeChanged(bindings : DataBindings, key : String, newValue : Object) : void
		{
			updateProjection();
		}
		
		private function updateProjection() : void
		{
			if (_camera.root is Scene)
			{
				var cameraData		: CameraDataProvider	= _camera.cameraData;
				var sceneBindings	: DataBindings			= Scene(_camera.root).bindings;
				var viewportWidth	: Number				= sceneBindings.getProperty('viewportWidth');
				var viewportHeight	: Number				= sceneBindings.getProperty('viewportHeight');
				var ratio			: Number				= viewportWidth / viewportHeight;
				var screenToView	: Matrix4x4				= cameraData.screenToView;
				
				cameraData.projection.perspectiveFoV(cameraData.fieldOfView, ratio, cameraData.zNear, cameraData.zFar);
				screenToView.lock().copyFrom(cameraData.projection).invert().unlock();
				
				cameraData.screenToWorld.lock().copyFrom(screenToView).append(_camera.localToWorld).unlock();
				cameraData.worldToScreen.lock().copyFrom(_camera.worldToLocal).append(cameraData.projection).unlock();
			}
		}
	}
}