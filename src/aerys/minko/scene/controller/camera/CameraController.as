package aerys.minko.scene.controller.camera
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.data.CameraDataProvider;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.camera.AbstractCamera;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.binding.IDataProvider;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public final class CameraController extends AbstractController
	{
		use namespace minko_scene;
		
		private var _camera		: AbstractCamera	= null;
		private var _ortho 		: Boolean 			= false;
		
		public function CameraController(ortho : Boolean = false)
		{
			super(AbstractCamera);
			
			_ortho = ortho;
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(controller	: CameraController,
											target		: AbstractCamera) : void
		{
			if (_camera != null)
				throw new Error('The CameraController can target only one Camera object.');
			
			_camera = target;
			_camera.addedToScene.add(addedToSceneHandler);
			_camera.removedFromScene.add(removedFromSceneHandler);
			_camera.worldToLocal.changed.add(worldToLocalChangedHandler);
		}
		
		private function targetRemovedHandler(controller	: CameraController,
											  target		: AbstractCamera) : void
		{
			_camera.addedToScene.remove(addedToSceneHandler);
			_camera.removedFromScene.remove(removedFromSceneHandler);
			_camera.worldToLocal.changed.remove(worldToLocalChangedHandler);
			_camera = null;
		}
		
		private function addedToSceneHandler(camera : AbstractCamera, scene : Scene) : void
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
		
		private function removedFromSceneHandler(camera : AbstractCamera, scene : Scene) : void
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
		
		private function worldToLocalChangedHandler(worldToLocal : Matrix4x4) : void
		{
			var cameraData			: CameraDataProvider	= _camera.cameraData;
			var cameraWorldToScreen	: Matrix4x4				= cameraData.worldToScreen;
			var cameraScreenToWorld	: Matrix4x4				= cameraData.screenToWorld;
			var cameraViewToWorld	: Matrix4x4				= cameraData.viewToWorld;
			var cameraPosition		: Vector4				= cameraData.position;
			var cameraDirection		: Vector4				= cameraData.direction;
			
			cameraWorldToScreen.lock()
			cameraScreenToWorld.lock()
			cameraPosition.lock();
			cameraDirection.lock();
			
			cameraWorldToScreen.copyFrom(_camera.worldToLocal).append(cameraData.projection);
			cameraScreenToWorld.copyFrom(cameraData.screenToView).append(_camera.localToWorld);
			cameraViewToWorld.transformVector(Vector4.ZERO, cameraPosition);
			cameraViewToWorld.deltaTransformVector(Vector4.Z_AXIS, cameraDirection).normalize();
			
			cameraWorldToScreen.unlock();
			cameraScreenToWorld.unlock();
			cameraPosition.unlock();
			cameraDirection.unlock();
			
			cameraData.frustum.updateFromMatrix(cameraWorldToScreen);
		}
		
		private function viewportSizeChanged(bindings 	: DataBindings,
											 key 		: String,
											 oldValue 	: Object,
											 newValue 	: Object) : void
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
			var sceneBindings	: DataBindings			= Scene(_camera.root).bindings;
			var viewportWidth	: Number				= sceneBindings.getProperty('viewportWidth');
			var viewportHeight	: Number				= sceneBindings.getProperty('viewportHeight');
			var ratio			: Number				= viewportWidth / viewportHeight;
			
			var projection		: Matrix4x4				= cameraData.projection;
			var screenToView	: Matrix4x4				= cameraData.screenToView;
			var screenToWorld	: Matrix4x4				= cameraData.screenToWorld;
			var worldToScreen	: Matrix4x4				= cameraData.worldToScreen;
			
			projection.lock();
			screenToView.lock();
			screenToWorld.lock();
			worldToScreen.lock();
			
			if (_ortho)
				projection.ortho(
					viewportWidth / cameraData.zoom,
					viewportHeight / cameraData.zoom,
					cameraData.zNear,
					cameraData.zFar
				);
			else
				projection.perspectiveFoV(
					cameraData.fieldOfView,
					ratio,
					cameraData.zNear,
					cameraData.zFar
				);
			
			screenToView.copyFrom(projection).invert();
			screenToWorld.copyFrom(screenToView).append(_camera.localToWorld);
			worldToScreen.copyFrom(_camera.worldToLocal).append(projection);
			
			projection.unlock();
			screenToView.unlock();
			screenToWorld.unlock();
			worldToScreen.unlock();
			
			cameraData.frustum.updateFromMatrix(worldToScreen);
		}
		
		private function cameraActivatedHandler(camera : AbstractCamera) : void
		{
			var scene : Scene = camera.root as Scene;
			
			scene.bindings.addProvider(camera.cameraData);
			resetSceneCamera(scene);
		}
		
		private function cameraDeactivatedHandler(camera : AbstractCamera) : void
		{
			var scene 	: Scene	= camera.root as Scene;
			
			scene.bindings.removeProvider(camera.cameraData);
			resetSceneCamera(scene);
		}
		
		private function resetSceneCamera(scene : Scene) : void
		{
			var cameras 	: Vector.<ISceneNode> 	= scene.getDescendantsByType(AbstractCamera);
			var numCameras 	: uint 					= cameras.length;
			var cameraId 	: uint 					= 0;
			var camera 		: AbstractCamera 		= null;
			
			if (_camera.enabled)
			{
				scene._camera = _camera;
				for (cameraId; cameraId < numCameras; ++cameraId)
				{
					camera = cameras[cameraId] as AbstractCamera;
					camera.enabled = camera == _camera;
				}
			}
			else
			{
				scene._camera = null;
				for (cameraId; cameraId < numCameras; ++cameraId)
				{
					camera = cameras[cameraId] as AbstractCamera;
					if (camera.enabled)
						scene._camera = camera;
				}
			}
		}
	}
}