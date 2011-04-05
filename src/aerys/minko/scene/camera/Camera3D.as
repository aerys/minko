package aerys.minko.scene.camera
{
	import aerys.minko.stage.Viewport3D;
	import aerys.minko.query.rendering.RenderingQuery;
	import aerys.minko.scene.AbstractScene3D;
	import aerys.minko.query.rendering.TransformManager;
	import aerys.minko.type.math.ConstVector4;
	import aerys.minko.type.math.Frustum3D;
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
	 * @see ArcBallCamera3D
	 * @see FirstPersonCamera3D
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class Camera3D extends AbstractScene3D implements ICamera3D
	{
		public static const DEFAULT_FOV				: Number			= Math.PI * .25;
		public static const DEFAULT_NEAR_CLIPPING	: Number			= .1;
		public static const DEFAULT_FAR_CLIPPING	: Number			= 1000.;
		
		private static const RAW_DATA				: Vector.<Number>	= new Vector.<Number>();
		
		private static const UPDATE_NONE			: uint				= 0;
		private static const UPDATE_VIEW			: uint				= 1;
		private static const UPDATE_PROJ			: uint				= 2;
		private static const UPDATE_ALL				: uint				= UPDATE_VIEW | UPDATE_PROJ;
		
		private var _version		: uint		= 0;
		
		private var _enabled		: Boolean	= true;
		
		private var _update			: uint		= 0;

		private var _position		: Vector4	= new Vector4();
		private var _lookAt			: Vector4	= Vector4.copy(ConstVector4.Z_AXIS);
		private var _up				: Vector4	= Vector4.copy(ConstVector4.Y_AXIS);
		
		private var _view			: Matrix4x4	= new Matrix4x4();
		private var _projection		: Matrix4x4	= new Matrix4x4();
		
		private var _pv				: uint		= 0;
		private var _lv				: uint		= 0;
		private var _uv				: uint		= 0;
		private var _vv				: uint		= 0;
		
		private var _fov			: Number	= 0.;
		private var _zNear			: Number	= 0.;
		private var _zFar			: Number	= 0.;
		private var _frustum		: Frustum3D	= new Frustum3D();

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
		
		public function get view()			: Matrix4x4	{ return _view; }
		public function get projection()	: Matrix4x4	{ return _projection; }
		public function get frustum()		: Frustum3D	{ return _frustum; }
		
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
			if (value != _fov)
			{
				_fov = value;
				++_version;
				_update |= UPDATE_PROJ;
			}
		}
		
		public function set nearClipping(value : Number) : void
		{
			if (value != _zNear)
			{
				_zNear = value;
				++_version;
				_update |= UPDATE_PROJ;
			}
		}
		
		public function set farClipping(value : Number) : void
		{
			if (value != _zFar)
			{
				_zFar = value;
				++_version;
				_update |= UPDATE_PROJ;
			}
		}
		
		public function Camera3D(fieldOfView 	: Number 	= DEFAULT_FOV,
								 nearClipping	: Number	= DEFAULT_NEAR_CLIPPING,
								 farClipping	: Number	= DEFAULT_FAR_CLIPPING)
		{
			this.fieldOfView = fieldOfView;
			this.nearClipping = nearClipping;
			this.farClipping = farClipping;
		}
		
		protected function invalidateView() : void
		{
			_update |= UPDATE_VIEW;
			++_version;
		}
		
		protected function invalidateProjection() : void
		{
			_update |= UPDATE_PROJ;
			++_version;
		}
		
		protected function updateMatrices(query : RenderingQuery = null) : void
		{
			Matrix4x4.lookAtLH(_position, _lookAt, _up, _view)
					 .multiply(query.transform.world);
			
			var viewport : Viewport3D = query.viewport;
			
			if (viewport.version != _vv || (_update & UPDATE_PROJ))
			{
				var ratio : Number = viewport.width / viewport.height;
				
				_vv = viewport.version;
				_update &= ~UPDATE_PROJ;
				Matrix4x4.perspectiveFoVLH(_fov, ratio, _zNear, _zFar, _projection);
				_frustum.update(_projection);
			}
			
		}
		
		override protected function acceptRenderingQuery(query : RenderingQuery) : void
		{
			if (!_enabled)
				return ;
						
			updateMatrices(query);
			
			var transform : TransformManager = query.transform;
			
			transform.projection = _projection;
			transform.view = _view;
		}
	}
}