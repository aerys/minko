package aerys.minko.scene.controller.camera
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.data.CameraDataProvider;
	import aerys.minko.scene.node.Camera;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.data.IDataProvider;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public final class CameraController extends AbstractController
	{
		use namespace minko_scene;
		
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
				throw new Error('The CameraController can target only one Camera object.');
			
			_camera = target;
			_camera.addedToScene.add(addedToSceneHandler);
			_camera.removedFromScene.add(removedFromSceneHandler);
			_camera.worldToLocal.changed.add(worldToLocalChangedHandler);
		}
		
		private function targetRemovedHandler(controller	: CameraController,
											  target		: Camera) : void
		{
			_camera.addedToScene.remove(addedToSceneHandler);
			_camera.removedFromScene.remove(removedFromSceneHandler);
			_camera.worldToLocal.changed.remove(worldToLocalChangedHandler);
			_camera = null;
		}
		
		private function addedToSceneHandler(camera : Camera, scene : Scene) : void
		{
			var sceneBindings : DataBindings = scene.bindings;
			
			resetSceneCamera(scene);

			if (camera.enabled)
				sceneBindings.addProvider(camera.cameraData);

			camera.activated.add(cameraActivatedHandler);
			camera.deactivated.add(cameraDeactivatedHandler);
			
			camera.cameraData.changed.add(cameraPropertyChangedHandler);
			
			sceneBindings.addCallback('viewportWidth', viewportSizeChanged);
			sceneBindings.addCallback('viewportHeight', viewportSizeChanged);
		}
		
		private function removedFromSceneHandler(camera : Camera, scene : Scene) : void
		{
			var sceneBindings : DataBindings = scene.bindings;
			
			resetSceneCamera(scene);
			
			if (camera.enabled)
				sceneBindings.removeProvider(camera.cameraData);
			
			camera.activated.remove(cameraActivatedHandler);
			camera.deactivated.remove(cameraDeactivatedHandler);
			
			camera.cameraData.changed.remove(cameraPropertyChangedHandler);
			
			sceneBindings.removeCallback('viewportWidth', viewportSizeChanged);
			sceneBindings.removeCallback('viewportHeight', viewportSizeChanged);
		}
		
		private function worldToLocalChangedHandler(worldToLocal : Matrix4x4, propertyName : String) : void
		{
			var cameraData	: CameraDataProvider	= _camera.cameraData;
			
			cameraData.worldToScreen.lock()
				.copyFrom(_camera.worldToLocal)
				.append(cameraData.projection)
				.unlock();
			
			cameraData.screenToWorld.lock()
				.copyFrom(cameraData.screenToView)
				.append(_camera.localToWorld)
				.unlock();
			
			cameraData.viewToWorld.transformVector(Vector4.ZERO, cameraData.position);
			cameraData.viewToWorld.deltaTransformVector(Vector4.Z_AXIS, cameraData.direction).normalize();
		}
		
		private function viewportSizeChanged(bindings : DataBindings, key : String, newValue : Object) : void
		{
			updateProjection();
		}
		
		private function cameraPropertyChangedHandler(provider : IDataProvider, property : String) : void
		{
			updateProjection();
		}
		
		private function updateProjection() : void
		{
			var cameraData		: CameraDataProvider	= _camera.cameraData;
			var screenToView	: Matrix4x4				= cameraData.screenToView;
			var sceneBindings	: DataBindings			= Scene(_camera.root).bindings;
			var viewportWidth	: Number				= sceneBindings.getProperty('viewportWidth');
			var viewportHeight	: Number				= sceneBindings.getProperty('viewportHeight');
			var ratio			: Number				= viewportWidth / viewportHeight;
			
			cameraData.projection.perspectiveFoV(cameraData.fieldOfView, ratio, cameraData.zNear, cameraData.zFar);
			
			screenToView.lock()
				.copyFrom(cameraData.projection)
				.invert()
				.unlock();
			
			cameraData.screenToWorld.lock()
				.copyFrom(cameraData.screenToView)
				.append(_camera.localToWorld)
				.unlock();
			
			cameraData.worldToScreen.lock()
				.copyFrom(_camera.worldToLocal)
				.append(cameraData.projection)
				.unlock();
		}
		
		private function cameraActivatedHandler(camera : Camera) : void
		{
			var scene : Scene = camera.root as Scene;
			
			scene.bindings.addProvider(camera.cameraData);
			resetSceneCamera(scene);
		}
		
		private function cameraDeactivatedHandler(camera : Camera) : void
		{
			var scene 	: Scene	= camera.root as Scene;
			
			scene.bindings.removeProvider(camera.cameraData);
			resetSceneCamera(scene);
		}
		
		private function resetSceneCamera(scene : Scene) : void
		{
			var cameras 	: Vector.<ISceneNode> 	= scene.getDescendantsByType(Camera);
			var numCameras 	: uint 					= cameras.length;
			var cameraId 	: uint 					= 0;
			var camera 		: Camera 				= null;
			
			if (_camera.enabled)
			{
				scene._camera = _camera;
				for (cameraId; cameraId < numCameras; ++cameraId)
				{
					camera = cameras[cameraId] as Camera;
					camera.enabled = camera == _camera;
				}
			}
			else
			{
				scene._camera = null;
				for (cameraId; cameraId < numCameras; ++cameraId)
				{
					camera = cameras[cameraId] as Camera;
					if (camera.enabled)
						scene._camera = camera;
				}
			}
		}
	}
}