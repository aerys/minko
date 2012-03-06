package aerys.minko.scene.node
{
	import aerys.minko.render.Viewport;
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.IDataProvider;
	import aerys.minko.type.math.Frustum;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * Camera objects describe the position and the look-at point of a 3D camera.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class Camera extends AbstractSceneNode implements IDataProvider
	{
		public static const DEFAULT_FOV			: Number	= Math.PI * .25;
		public static const DEFAULT_ZNEAR		: Number	= .1;
		public static const DEFAULT_ZFAR		: Number	= 1000.;
		
		private static const DATA_DESCRIPTOR	: Object	= {
			"camera position"		: "position",
			"camera look at"		: "lookAt",
			"camera up"				: "up",
			"camera world position"	: "worldPosition",
			"camera world look at"	: "worldLookAt",
			"camera world up"		: "worldUp",
			"world to view"			: "worldToView",
			"projection"			: "projection",
			"world to screen"		: "worldToScreen"
		};
		
		private var _viewport		: Viewport	= null;
		
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
		
		private var _worldToScreen	: Matrix4x4	= new Matrix4x4();
		
		private var _locked			: Boolean	= false;
		private var _changed		: Signal	= new Signal();
		
		/**
		 * The position of the camera in local space. 
		 * @return 
		 * 
		 */
		public function get position() : Vector4
		{
			return _position;
		}
		
		/**
		 * The look-at point of the camera in local space. 
		 * @return 
		 * 
		 */
		public function get lookAt() : Vector4
		{
			return _lookAt;
		}
		
		/**
		 * The up axis of the camera in local space. Default value
		 * is the +Y axis. 
		 * @return 
		 * 
		 */
		public function get up() : Vector4
		{
			return _up;
		}
		
		/**
		 * The position of the camera in world space. This value is
		 * updated everytime:
		 * <ul>
		 * <li>The parent 3D transformation changes.</li>
		 * <li>The "position" property changes.</li>
		 * </ul> 
		 * @return 
		 * 
		 */
		public function get worldPosition() : Vector4
		{
			return _worldPosition;
		}
		
		/**
		 * The look-at point of the camera in world space. This value is
		 * updated everytime:
		 * <ul>
		 * <li>The parent 3D transformation changes.</li>
		 * <li>The "lookAt" property changes.</li>
		 * </ul> 
		 * @return 
		 * 
		 */
		public function get worldLookAt() : Vector4
		{			
			return _worldLookAt;
		}
		
		/**
		 * The up axis of the camera in world space. This value is
		 * updated everytime:
		 * <ul>
		 * <li>The parent 3D transformation changes.</li>
		 * <li>The "up" property changes.</li>
		 * </ul> 
 
		 * @return 
		 * 
		 */
		public function get worldUp() : Vector4
		{
			return _worldUp;
		}
		
		/**
		 * The matrix that transforms world space coordinates
		 * into view (or camera) space coordinates.
		 * @return 
		 * 
		 */
		public function get worldToView() : Matrix4x4
		{
			return _worldToView;
		}
		
		/**
		 * The matrix that transforms view space coordinates
		 * into clip space (or normalized screen space) coordinates.
		 * @return 
		 * 
		 */
		public function get projection() : Matrix4x4
		{
			return _projection;
		}
		
		/**
		 * The matrix that transforms world space coordinates
		 * into clip space (or normalized space) coordinates. 
		 * @return 
		 * 
		 */
		public function get worldToScreen() : Matrix4x4
		{
			return _worldToScreen;
		}
		
		/**
		 * The field of view of the camera. Setting this property
		 * will update the projection matrix and trigger the "changed"
		 * signal.
		 * @return 
		 * 
		 */
		public function get fieldOfView() : Number
		{
			return _fov;
		}
		public function set fieldOfView(value : Number) : void
		{
			_fov = value;
			updateProjection();
		}
		
		/**
		 * The z-near clipping plane of the camera. Setting this
		 * property will update the projection matrix and trigger the "changed"
		 * signal. 
		 * @return 
		 * 
		 */
		public function get zNear() : Number
		{
			return _zNear;
		}
		public function set zNear(value : Number) : void
		{
			_zNear = value;
			updateProjection();
		}
		
		/**
		 * The z-far clipping plane of the camera. Setting this property
		 * will update the projection matrix and trigger the "changed"
		 * signal. 
		 * @return 
		 * 
		 */
		public function get zFar() : Number
		{
			return _zFar;
		}
		public function set zFar(value : Number) : void
		{
			_zFar = value;
			updateProjection();
		}
		
		public function get locked() : Boolean
		{
			return _locked;
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function get dataDescriptor() : Object
		{
			return DATA_DESCRIPTOR;
		}
		
		public function Camera(viewport		: Viewport,
							   fieldOfView	: Number	= DEFAULT_FOV,
							   zNear		: Number	= DEFAULT_ZNEAR,
							   zFar			: Number	= DEFAULT_ZFAR)
		{
			super();
			
			_viewport = viewport;
			_fov = fieldOfView;
			_zNear = zNear;
			_zFar = zFar;
			
			initialize();
		}
		
		private function initialize() : void
		{
			updateProjection();
			
			_viewport.resized.add(viewportResizedHandler);
			
			_position.changed.add(positionChangedHandler);
			_lookAt.changed.add(lookAtChangedHandler);
			_up.changed.add(upChangedHandler);
			
			localToWorld.changed.add(transformChangedHandler);
		}
		
		private function positionChangedHandler(value : Object, property : Object) : void
		{
			if (!_locked)
				_changed.execute(this, "position");
			
			updateWorldToView();
		}
		
		private function lookAtChangedHandler(value : Object, property : Object) : void
		{
			if (!_locked)
				_changed.execute(this, "lookAt");
			
			updateWorldToView();
		}
		
		private function upChangedHandler(value : Object, property : Object) : void
		{
			if (!_locked)
				_changed.execute(this, "up");
			
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
			var aspectRatio : Number = _viewport.width / _viewport.height;
			
			Matrix4x4.perspectiveFoVLH(_fov, aspectRatio, _zNear, _zFar, _projection);
			_frustum.updateFromDescription(_fov, aspectRatio, _zNear, _zFar);
			
			if (!_locked)
				_changed.execute(this, "projection");
			
			updateWorldToScreen();
		}
		
		private function transformChangedHandler(transform 	: Matrix4x4,
												 key		: String) : void
		{
			updateWorldToView();
		}
		
		private function updateWorldToView() : void
		{
			if (!parent)
				return ;
			
			lock();
			
			localToWorld.transformVector(_position, _worldPosition);
			localToWorld.transformVector(_lookAt, _worldLookAt);
			localToWorld.deltaTransformVector(_up, _worldUp);
			_worldUp.normalize();
			
			Matrix4x4.lookAtLH(
				_worldPosition,
				_worldLookAt,
				_worldUp,
				_worldToView
			);
			
			updateWorldToScreen();
			
			unlock();
		}
		
		private function updateWorldToScreen() : void
		{
			Matrix4x4.multiply(_projection, _worldToView, _worldToScreen);
			
			if (!_locked)
				_changed.execute(this, "worldToScreen");
		}
		
		public function lock() : void
		{
			_locked = true;
		}
		
		public function unlock() : void
		{
			var locked : Boolean = _locked;
			
			_locked = false;
			if (locked)
				_changed.execute(this, null);
		}
	}
}