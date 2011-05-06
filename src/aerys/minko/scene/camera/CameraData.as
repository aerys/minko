package aerys.minko.scene.camera
{
	import aerys.minko.query.renderdata.style.StyleStack3D;
	import aerys.minko.query.renderdata.transform.TransformData;
	import aerys.minko.query.renderdata.world.IWorldData;
	import aerys.minko.type.math.Frustum3D;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Matrix;
	
	public class CameraData implements IWorldData
	{
		public static const POSITION		: String = 'position';
		public static const LOOK_AT			: String = 'lookAt';
		public static const UP				: String = 'up';

		public static const RATIO			: String = 'ratio';
		public static const FOV				: String = 'fov';
		public static const Z_NEAR			: String = 'zNear';
		
		public static const Z_FAR			: String = 'zFar';
		public static const Z_FAR_PARTS		: String = 'zFarParts';
		
		
		public static const VIEW			: String = 'view';
		public static const PROJECTION		: String = 'projection';
		public static const LOCAL_POSITION	: String = 'localPosition';
		public static const LOCAL_LOOK_AT	: String = 'localLookAt';
		
		public static const FRUSTUM			: String = 'frustum';
		
		// local data provider
		protected var _styleStack		: StyleStack3D;
		protected var _transformData	: TransformData;
		
		// data available on initialisation
		protected var _position	: Vector4;
		protected var _lookAt	: Vector4;
		protected var _up		: Vector4;
		
		protected var _ratio	: Number;
		protected var _fov		: Number;
		protected var _zNear	: Number;
		protected var _zFar		: Number;
		
		// computed data
		protected var _zFarParts						: Vector4;
		protected var _zFarParts_invalidated			: Boolean;
		
		protected var _view								: Matrix4x4;
		protected var _view_positionVersion				: uint;
		protected var _view_lookAtVersion				: uint;
		protected var _view_upVersion					: uint;
		
		protected var _projection						: Matrix4x4;
		protected var _projection_invalidated			: Boolean;
		
		protected var _localPosition					: Vector4;
		protected var _localPosition_positionVersion	: uint;
		
		protected var _localLookAt						: Vector4;
		protected var _localLookAt_lookAtVersion		: uint;
		
		protected var _frustum							: Frustum3D;
		protected var _frustum_projectionVersion		: uint;
		
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
		
		public function get fieldOfView() : Number
		{
			return _fov;
		}
		
		public function get zNear() : Number
		{
			return _zNear;
		}
		
		public function get zFar() : Number
		{
			return _zFar;
		}
		
		public function get zFarParts() : Vector4
		{
			if (_zFarParts_invalidated)
			{
				_zFarParts = new Vector4(0, 0.25 * _zFar, 0.5 * _zFar, 0.75 * _zFar);
				_zFarParts_invalidated = false;
			}
			return _zFarParts;
		}
		
		public function get view() : Matrix4x4
		{
			if (_view_positionVersion != _position.version ||
				_view_lookAtVersion != _lookAt.version ||
				_view_upVersion != _up.version)
			{
				_view = Matrix4x4.lookAtLH(_position, _lookAt, _up, _view);
				
				_view_positionVersion	= _position.version;
				_view_lookAtVersion		= _lookAt.version;
				_view_upVersion			= _lookAt.version;
			}
			
			return _view;
		}
		
		public function get projection() : Matrix4x4
		{
			if (_projection_invalidated)
			{
				_projection = Matrix4x4.perspectiveFoVLH(_fov, _ratio, _zNear, _zFar, _projection);
				_projection_invalidated = false;
			}
			
			return _projection;
		}
		
		public function get frustrum() : Frustum3D
		{
			var projectionMatrix : Matrix4x4 = projection;
			
			if (_frustum_projectionVersion != projectionMatrix.version)
			{
				if (_frustum == null)
					_frustum = new Frustum3D();
				_frustum.update(projectionMatrix);
				_frustum_projectionVersion = projectionMatrix.version;
			}
			
			return _frustum;
		}
		
		public function get localPosition() : Vector4
		{
			if (_localPosition_positionVersion != _position.version)
			{
				_localPosition = _transformData.world.multiplyVector(_position, _localPosition);
				_localPosition_positionVersion = _position.version;
			}
			
			return _localPosition;
		}
		
		public function get localLookAt() : Vector4
		{
			if (_localLookAt_lookAtVersion != _lookAt.version)
			{
				_localLookAt = _transformData.world.deltaMultiplyVector(_lookAt, _localLookAt);
				_localLookAt_lookAtVersion = _lookAt.version;
			}
			
			return _localLookAt;
		}
		
		
		
		
		public function set ratio(v : Number) : void
		{
			_ratio = v;
			_projection_invalidated = true;
		}
		
		public function set position(v : Vector4) : void
		{
			_position = v;
		}
		
		public function set lookAt(v : Vector4) : void
		{
			_lookAt = v;
		}
		
		public function set up(v : Vector4) : void
		{
			_up = v;
		}
		
		public function set fov(v : Number) : void
		{
			_fov = v;
			_projection_invalidated = true;
		}
		
		public function set zNear(v : Number) : void
		{
			_zNear = v;
			_projection_invalidated = true;
		}
		
		public function set zFar(v : Number) : void
		{
			_zFar = v;
			_projection_invalidated = true;
		}
		
		public function CameraData()
		{
			reset()
		}
		
		public function setLocalDataProvider(styleStack		: StyleStack3D, 
											 transformData	: TransformData) : void
		{
			_styleStack		= styleStack;
			_transformData	= transformData;
		}
		
		public function invalidate() : void
		{
			// do nothing
		}
		
		public function reset() : void 
		{
			_position	= null;
			_lookAt		= null;
			_up			= null;
			
			_ratio	= -1;
			_fov	= -1;
			_zNear	= -1;
			_zFar	= -1;
			
			_view_positionVersion			= uint.MAX_VALUE;
			_view_lookAtVersion				= uint.MAX_VALUE;
			_view_upVersion					= uint.MAX_VALUE;
			_localPosition_positionVersion	= uint.MAX_VALUE;
			_localLookAt_lookAtVersion		= uint.MAX_VALUE;
			_frustum_projectionVersion		= uint.MAX_VALUE;
			
			_zFarParts_invalidated	= true;
			_projection_invalidated = true;
		}
	}
}