package aerys.minko.scene.controller.camera
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.data.CameraDataProvider;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.camera.AbstractCamera;
	import aerys.minko.scene.node.camera.Camera;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.binding.IDataProvider;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public final class CameraController extends AbstractController
	{
		use namespace minko_scene;
		
		private var _data	: CameraDataProvider;
		private var _camera	: AbstractCamera;
		private var _ortho 	: Boolean;
		
		public function get cameraData() : CameraDataProvider
		{
			return _data;
		}
		
		public function get orthographic() : Boolean
		{
			return _ortho;
		}
		public function set orthographic(value : Boolean) : void
		{
			_ortho = value;
		}
		
		public function CameraController()
		{
			super(AbstractCamera);
			
			_data = new CameraDataProvider();
			
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
			_camera.localToWorldTransformChanged.add(localToWorldChangedHandler);
		}
		
		private function targetRemovedHandler(controller	: CameraController,
											  target		: AbstractCamera) : void
		{
			_camera.addedToScene.remove(addedToSceneHandler);
			_camera.removedFromScene.remove(removedFromSceneHandler);
			_camera.localToWorldTransformChanged.remove(localToWorldChangedHandler);
			_camera = null;
		}
		
		private function addedToSceneHandler(camera : AbstractCamera, scene : Scene) : void
		{
			var sceneBindings : DataBindings = scene.bindings;
			
			resetSceneCamera(scene);
            
            if (camera.enabled)
                sceneBindings.addProvider(_data);

			camera.activated.add(cameraActivatedHandler);
			camera.deactivated.add(cameraDeactivatedHandler);
			
			_data.changed.add(cameraPropertyChangedHandler);
			
			sceneBindings.addCallback('viewportWidth', viewportSizeChanged);
			sceneBindings.addCallback('viewportHeight', viewportSizeChanged);
			
			updateProjection();
		}
		
		private function removedFromSceneHandler(camera : AbstractCamera, scene : Scene) : void
		{
			var sceneBindings : DataBindings = scene.bindings;
			
			resetSceneCamera(scene);
			
			if (camera.enabled)
				sceneBindings.removeProvider(_data);
			
			camera.activated.remove(cameraActivatedHandler);
			camera.deactivated.remove(cameraDeactivatedHandler);
			
			_data.changed.remove(cameraPropertyChangedHandler);
			
			sceneBindings.removeCallback('viewportWidth', viewportSizeChanged);
			sceneBindings.removeCallback('viewportHeight', viewportSizeChanged);
		}
		
		private function localToWorldChangedHandler(camera 			: Camera,
													localToWorld 	: Matrix4x4) : void
		{
			var cameraData		: CameraDataProvider	= _data;
			var worldToScreen	: Matrix4x4				= cameraData.worldToScreen;
			
			worldToScreen.lock()
				.copyFrom(localToWorld)
				.invert()
				.append(cameraData.projection)
				.unlock();

			cameraData.frustum.updateFromMatrix(worldToScreen);
		}
		
		private function viewportSizeChanged(bindings 	: DataBindings,
											 key 		: String,
											 oldValue 	: Object,
											 newValue 	: Object) : void
		{
			updateProjection();
		}
		
		private function cameraPropertyChangedHandler(provider : IDataProvider,
                                                      property : String) : void
		{
			if (property == 'zFar' || property == 'zNear' || property == 'fieldOfView'
                || property == 'zoom')
				updateProjection();
		}
		
		private function updateProjection() : void
		{
			var sceneBindings	: DataBindings	= Scene(_camera.root).bindings;
			
			if (sceneBindings.propertyExists('viewportWidth')
				&& sceneBindings.propertyExists('viewportHeight'))
			{
				var cameraData		: CameraDataProvider	= _data;
				var viewportWidth	: Number				= sceneBindings.getProperty('viewportWidth');
				var viewportHeight	: Number				= sceneBindings.getProperty('viewportHeight');
				var ratio			: Number				= viewportWidth / viewportHeight;
				
				var projection		: Matrix4x4				= cameraData.projection;
				var worldToScreen	: Matrix4x4				= cameraData.worldToScreen;
				
				projection.lock();
				worldToScreen.lock();
				
				if (_ortho)
				{
					var cameraZoom : Number = cameraData.zoom;
					projection.ortho(
						viewportWidth / cameraZoom,
						viewportHeight / cameraZoom,
						cameraData.zNear,
						cameraData.zFar
					);
				}
				else
					projection.perspectiveFoV(
						cameraData.fieldOfView,
						ratio,
						cameraData.zNear,
						cameraData.zFar
					);
				
				worldToScreen.copyFrom(_camera.getWorldToLocalTransform()).append(projection);
				
				cameraData.frustum.updateFromMatrix(worldToScreen);
				
				projection.unlock();
				worldToScreen.unlock();
			}
		}
		
		private function cameraActivatedHandler(camera : AbstractCamera) : void
		{
			var scene : Scene = camera.root as Scene;
			
			resetSceneCamera(scene);
			scene.bindings.addProvider(_data);
		}
		
		private function cameraDeactivatedHandler(camera : AbstractCamera) : void
		{
			var scene 	: Scene	= camera.root as Scene;
			
			resetSceneCamera(scene);
			scene.bindings.removeProvider(_data);
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