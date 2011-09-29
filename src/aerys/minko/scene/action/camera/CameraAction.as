package aerys.minko.scene.action.camera
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.data.CameraData;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.ViewportData;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.camera.ICamera;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.type.math.Matrix3D;
	import aerys.minko.type.math.Vector4;
	
	public class CameraAction implements IAction
	{
		private var _cameraData	: CameraData	= null;
		
		private var _view			: Matrix3D		= new Matrix3D();
		private var _projection		: Matrix3D		= new Matrix3D();
		
		private var _worldPosition	: Vector4		= new Vector4();
		private var _worldLookAt	: Vector4		= new Vector4();
		private var _worldUp		: Vector4		= new Vector4();
		
		protected function get cameraData() : CameraData	{ return _cameraData; }
		
		public function get type() 			: uint			{ return ActionType.UPDATE_WORLD_DATA
																	 | ActionType.UPDATE_TRANSFORM_DATA; }
		
		public function CameraAction(cameraData : CameraData = null)
		{
			_cameraData = cameraData || new CameraData();
		}
		
		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var camera			: ICamera		= ICamera(scene);
			
			if (!camera.enabled)
				return false;
			
			var transformData	: TransformData	= visitor.transformData;
			var viewportData	: ViewportData	= visitor.worldData[ViewportData]
												  as ViewportData;
			var worldMatrix		: Matrix3D		= transformData.world;
			
			worldMatrix.transformVector(camera.position, _worldPosition);
			worldMatrix.transformVector(camera.lookAt, _worldLookAt);
			worldMatrix.deltaTransformVector(camera.up, _worldUp);
			_worldUp.normalize()
			
			/*var viewDirection 	: Vector4 	= Vector4.subtract(_worldLookAt, _worldPosition).normalize();
			var dotProduct		: Number	= Vector4.dotProduct(viewDirection, _worldUp);
			
			if (dotProduct == 1. || dotProduct == -1.)
				_worldLookAt.x += 1e-100;*/
				
			_cameraData.reset();
			_cameraData.position	= _worldPosition;
			_cameraData.lookAt		= _worldLookAt;
			_cameraData.up			= _worldUp;
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
							 _worldPosition,
							 _worldLookAt,
							 _worldUp);
			
			transformData.projection 	= _projection;
			transformData.view			= _view;
			
			return true;
		}
		
		protected function updateProjectionMatrix(projection		: Matrix3D,
												  camera			: ICamera,
												  viewportWidth		: int,
												  viewportHeight	: int) : void
		{
			Matrix3D.perspectiveFoVLH(camera.fieldOfView,
									  viewportWidth / viewportHeight,
									  camera.nearClipping,
									  camera.farClipping,
									  projection);
		}
		
		protected function updateViewMatrix(view		: Matrix3D,
											camera		: ICamera,
											eyePosition	: Vector4,
											lookAt		: Vector4,
											up			: Vector4) : void
		{
			Matrix3D.lookAtLH(eyePosition, lookAt, up, view);
		}
	}
}