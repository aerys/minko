package aerys.minko.scene.node.camera
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.scene.node.Group;
	import aerys.minko.type.math.Frustum;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public class Camera extends AbstractScene
	{
		public static const DEFAULT_FOV		: Number	= Math.PI * .25;
		public static const DEFAULT_ZNEAR	: Number	= .1;
		public static const DEFAULT_ZFAR	: Number	= 1000.;
		
		private var _position		: Vector4	= new Vector4(0, 0, 0);
		private var _lookAt			: Vector4	= new Vector4(0, 0, 1);
		private var _up				: Vector4	= new Vector4(0, 1, 0);
		
		private var _worldToView	: Matrix4x4	= new Matrix4x4();
		private var _worldPosition	: Vector4	= new Vector4();
		private var _worldLookAt	: Vector4	= new Vector4();
		private var _worldUp		: Vector4	= new Vector4();
		
		private var _fov			: Number	= 0;
		private var _zNear			: Number	= 0;
		private var _zFar			: Number	= 0;
		private var _frustum		: Frustum	= new Frustum();
		private var _projection		: Matrix4x4	= new Matrix4x4();
		
		public function get position() : Vector4
		{
			return _position;
		}
		
		public function get lookAt() : Vector4
		{
			return _lookAt;
		}
		
		public function get up() : Vector4
		{
			return _up;
		}
		
		public function get worldPosition() : Vector4
		{
			return _worldPosition;
		}
		
		public function get worldLookAt() : Vector4
		{			
			return _worldLookAt;
		}
		
		public function get worldUp() : Vector4
		{
			return _worldUp;
		}
		
		public function get worldToView() : Matrix4x4
		{
			return _worldToView;
		}
		
		public function get projection() : Matrix4x4
		{
			return _projection;
		}
		
		public function get fieldOfView() : Number
		{
			return _fov;
		}
		public function set fieldOfView(value : Number) : void
		{
			_fov = value;
		}
		
		public function get zNear() : Number
		{
			return _zNear;
		}
		public function set zNear(value : Number) : void
		{
			_zNear = value;
		}
		
		public function get zFar() : Number
		{
			return _zFar;
		}
		public function set zFar(value : Number) : void
		{
			_zFar = value;
		}
		
		public function Camera(viewport		: Viewport,
							   fieldOfView	: Number	= DEFAULT_FOV,
							   zNear		: Number	= DEFAULT_ZNEAR,
							   zFar			: Number	= DEFAULT_ZFAR)
		{
			super();
			
			_fov = fieldOfView;
			_zNear = zNear;
			_zFar = zFar;
			
			initialize(viewport);
		}
		
		private function initialize(viewport : Viewport) : void
		{
			updateProjection(viewport.width / viewport.height);
			
			added.add(addedHandler);
			removed.add(removedHandler);
		}
		
		private function viewportChangedHandler(viewport : Viewport, property : String) : void
		{
			if (property == "width" || property == "height")
				updateProjection(viewport.width / viewport.height);
		}
		
		private function updateProjection(ratio : Number) : void
		{
			Matrix4x4.perspectiveFoVLH(_fov, ratio, _zNear, _zFar, _projection);
			_frustum.updateFromDescription(_fov, ratio, _zNear, _zFar);
		}
		
		private function addedHandler(child : Camera, parent : Group) : void
		{
			parentTransformChangedHandler(parent.localToWorld, null);
			parent.localToWorld.changed.add(parentTransformChangedHandler);
		}
		
		private function removedHandler(child : Camera, parent : Group) : void
		{
			parent.localToWorld.changed.remove(parentTransformChangedHandler);
			Matrix4x4.lookAtLH(
				_position,
				_lookAt,
				_up,
				_worldToView
			);
		}
		
		private function parentTransformChangedHandler(transform : Matrix4x4,
													   key		 : String) : void
		{
			transform.transformVector(_position, _worldPosition);
			transform.transformVector(_lookAt, _worldLookAt);
			transform.deltaTransformVector(_up, _worldUp);
			_worldUp.normalize();
			
			Matrix4x4.lookAtLH(
				_worldPosition,
				_worldLookAt,
				_worldUp,
				_worldToView
			);
		}
	}
}