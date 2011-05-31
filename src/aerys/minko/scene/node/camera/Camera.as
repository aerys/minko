package aerys.minko.scene.node.camera
{
	import aerys.minko.scene.action.WorldObjectAction;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.scene.visitor.data.CameraData;
	import aerys.minko.scene.visitor.data.IWorldData;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.type.Factory;
	import aerys.minko.type.math.ConstVector4;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * The AbstractCamera3D is the the base camera class. It should
	 * <strong>not</strong> be used directly. One should use the
	 * <code>ArcBallCamera3D</code> or the <code>FirstPersonCamera3D</code>
	 * classes or extend this one.
	 * 
	 * <p>
	 * When extending this class to create a new camera class, one should
	 * override the <code>invalidateTransform</code> method.
	 * </p>
	 * 
	 * @see ArcBallCamera
	 * @see FirstPersonCamera
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class Camera extends AbstractScene implements ICamera
	{
		protected static const CAMERA_DATA : Factory = Factory.getFactory(CameraData);
		
		public static const DEFAULT_FOV				: Number			= Math.PI * .25;
		public static const DEFAULT_NEAR_CLIPPING	: Number			= .1;
		public static const DEFAULT_FAR_CLIPPING	: Number			= 1000.;
		
		private static const RAW_DATA				: Vector.<Number>	= new Vector.<Number>();
		
		protected var _view			: Matrix4x4	= new Matrix4x4();
		protected var _projection	: Matrix4x4	= new Matrix4x4();
		
		private var _version		: uint		= 0;
		
		private var _enabled		: Boolean	= true;
		
		private var _update			: uint		= 0;

		private var _position		: Vector4	= new Vector4();
		private var _lookAt			: Vector4	= Vector4.copy(ConstVector4.Z_AXIS);
		private var _up				: Vector4	= Vector4.copy(ConstVector4.Y_AXIS);
		
		private var _fov			: Number	= 0.;
		private var _zNear			: Number	= 0.;
		private var _zFar			: Number	= 0.;

		public function get version() : uint
		{
			return _version + _position.version + _lookAt.version + _up.version;
		}
		
		public function get position() 		: Vector4	{ return _position; }
		public function get lookAt()		: Vector4	{ return _lookAt; }
		public function get up()			: Vector4	{ return _up; }
		
		public function get fieldOfView()	: Number	{ return _fov; }
		public function get nearClipping()	: Number	{ return _zNear; }
		public function get farClipping()	: Number	{ return _zFar; }
		
		/**
		 * Specify whether the camera is active or not. If the camera is
		 * not active, it will not push its 3D transformation into the
		 * stack and will not affect the rendering.
		 * 
		 * @return <code>true</code> is the camera is enabled,
		 * <code>false</code> otherwise
		 */
		public function get enabled() 	: Boolean 	{ return _enabled; }
		/**
		 * The <code>x</code> component of the look-at vector.
		 */
		
		public function set enabled(value : Boolean) : void
		{
			if (value != _enabled)
			{
				_enabled = value;
				++_version;
			}
		}
		
		public function set fieldOfView(value : Number) : void
		{
			_fov = value;
		}
		
		public function set nearClipping(value : Number) : void
		{
			_zNear = value;
		}
		
		public function set farClipping(value : Number) : void
		{
			_zFar = value;
		}
		
		public function get isSingle() : Boolean
		{
			return true;
		}
		
		public function Camera(fieldOfView 	: Number 	= DEFAULT_FOV,
							   nearClipping	: Number	= DEFAULT_NEAR_CLIPPING,
							   farClipping	: Number	= DEFAULT_FAR_CLIPPING)
		{
			this.fieldOfView = fieldOfView;
			this.nearClipping = nearClipping;
			this.farClipping = farClipping;
			
			actions[0] = WorldObjectAction.worldObjectAction;
		}
		
		public function getData(localData : LocalData) : IWorldData
		{
			if (!_enabled)
				return null;
			
			var cameraData 		: CameraData	= CAMERA_DATA.create(true) as CameraData;
			var worldMatrix		: Matrix4x4		= localData.world;
			var worldPosition	: Vector4		= worldMatrix.multiplyVector(position);
			var worldLookAt		: Vector4		= worldMatrix.multiplyVector(lookAt);
			var worldUp			: Vector4		= worldMatrix.deltaMultiplyVector(up).normalize();
			
			cameraData.reset();
			
			cameraData.position		= worldPosition;
			cameraData.lookAt		= worldLookAt;
			cameraData.up			= worldUp;
			
			cameraData.fov			= fieldOfView;
			cameraData.zNear		= nearClipping;
			cameraData.zFar			= farClipping;
			
			return cameraData;
		}
		
	}
}