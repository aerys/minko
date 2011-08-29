package aerys.minko.scene.action.camera
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.data.CameraData;
	import aerys.minko.scene.data.LocalData;
	import aerys.minko.scene.data.ViewportData;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.camera.ICamera;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.type.Factory;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public class CameraAction implements IAction
	{
		private var _cameraData	: CameraData	= null;
		
		private var _view		: Matrix4x4		= new Matrix4x4();
		private var _projection	: Matrix4x4		= new Matrix4x4();
		
		protected function get cameraData() : CameraData	{ return _cameraData; }
		
		public function get type() 			: uint			{ return ActionType.UPDATE_WORLD_DATA
																	 | ActionType.UPDATE_LOCAL_DATA; }
		
		public function CameraAction(cameraData : CameraData = null)
		{
			_cameraData = cameraData || new CameraData();
		}
		
		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var camera			: ICamera		= ICamera(scene);
			
			if (!camera.enabled)
				return false;
			
			var localData		: LocalData		= visitor.localData;
			var viewportData	: ViewportData	= visitor.worldData[ViewportData]
												  as ViewportData;
			var worldMatrix		: Matrix4x4		= localData.world;
			var worldPosition	: Vector4		= worldMatrix.multiplyVector(camera.position);
			var worldLookAt		: Vector4		= worldMatrix.multiplyVector(camera.lookAt);
			var worldUp			: Vector4		= worldMatrix.deltaMultiplyVector(camera.up)
															 .normalize();
			
			_cameraData.reset();
			
			_cameraData.position	= worldPosition;
			_cameraData.lookAt		= worldLookAt;
			_cameraData.up			= worldUp;
			
			_cameraData.fieldOfView	= camera.fieldOfView;
			_cameraData.zNear		= camera.nearClipping;
			_cameraData.zFar		= camera.farClipping;
			
			visitor.worldData[CameraData] = _cameraData;
			
			// update local data
			updateProjectionMatrix(_projection,
								   camera,
								   viewportData.width,
								   viewportData.height);
			updateViewMatrix(_view,
							 camera,
							 worldPosition,
							 worldLookAt,
							 worldUp);
			
			localData.projection 	= _projection;
			localData.view			= _view;
			
			return true;
		}
		
		protected function updateProjectionMatrix(projection		: Matrix4x4,
												  camera			: ICamera,
												  viewportWidth		: int,
												  viewportHeight	: int) : void
		{
			Matrix4x4.perspectiveFoVLH(camera.fieldOfView,
									   viewportWidth / viewportHeight,
									   camera.nearClipping,
									   camera.farClipping,
									   projection);
		}
		
		protected function updateViewMatrix(view		: Matrix4x4,
											camera		: ICamera,
											eyePosition	: Vector4,
											lookAt		: Vector4,
											up			: Vector4) : void
		{
			Matrix4x4.lookAtLH(eyePosition, lookAt, up, view);
		}
	}
}