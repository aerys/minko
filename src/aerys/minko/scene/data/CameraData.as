package aerys.minko.scene.data
{
	import aerys.minko.type.math.Frustum;
	import aerys.minko.type.math.Matrix3D;
	import aerys.minko.type.math.Vector4;
	
	import flash.utils.Dictionary;
	
	public class CameraData implements IWorldData
	{
		public static const POSITION		: String = 'position';
		public static const LOOK_AT			: String = 'lookAt';
		public static const UP				: String = 'up';

		public static const FOV				: String = 'fov';
		public static const Z_NEAR			: String = 'zNear';
		
		public static const Z_FAR			: String = 'zFar';
		public static const Z_FAR_PARTS		: String = 'zFarParts';
		
		public static const LOCAL_POSITION	: String = 'localPosition';
		public static const LOCAL_LOOK_AT	: String = 'localLookAt';
		
		public static const DIRECTION		: String = 'direction';
		public static const LOCAL_DIRECTION	: String = 'localDirection';
		
		public static const FRUSTUM			: String = 'frustum';
		
		// data provider
		protected var _styleStack	: StyleStack;
		protected var _transformData	: TransformData;
		protected var _worldData	: Dictionary;
		
		// data available on initialisation
		protected var _position	: Vector4;
		protected var _lookAt	: Vector4;
		protected var _up		: Vector4;
		
		protected var _fov		: Number;
		protected var _zNear	: Number;
		protected var _zFar		: Number;
		
		// computed data
		protected var _direction							: Vector4;
		protected var _direction_positionVersion			: uint;
		protected var _direction_lookAtVersion				: uint;
		
		protected var _zFarParts							: Vector4;
		protected var _zFarParts_invalidated				: Boolean;
		
		protected var _view									: Matrix3D;
		protected var _view_positionVersion					: uint;
		protected var _view_lookAtVersion					: uint;
		protected var _view_upVersion						: uint;
		
		protected var _projection							: Matrix3D;
		protected var _projection_invalidated				: Boolean;
		
		protected var _localPosition						: Vector4;
		protected var _localPosition_positionVersion		: uint;
		protected var _localPosition_worldInverseVersion	: uint;
		
		protected var _localLookAt							: Vector4;
		protected var _localLookAt_lookAtVersion			: uint;
		protected var _localLookAt_worldInverseVersion		: uint;
		
		protected var _localDirection						: Vector4;
		protected var _localDirection_positionVersion		: uint;
		protected var _localDirection_lookAtVersion			: uint;
		
		protected var _frustum								: Frustum;
		protected var _frustum_projectionVersion			: uint;
		
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
		
		public function get frustrum() : Frustum
		{
			var projectionMatrix : Matrix3D = _transformData.projection;
			
			if (_frustum_projectionVersion != projectionMatrix.version)
			{
				if (_frustum == null)
					_frustum = new Frustum();
				
				_frustum.updateWithProjectionMatrix(projectionMatrix);
				_frustum_projectionVersion = projectionMatrix.version;
			}
			
			return _frustum;
		}
		
		public function get direction() : Vector4
		{
			var positionVector	: Vector4 = position;
			var lookAtVector	: Vector4 = lookAt;
			
			if (_direction_positionVersion != positionVector.version ||
				_direction_lookAtVersion != lookAtVector.version)
			{
				_direction = Vector4.subtract(lookAtVector, positionVector, _direction);
				_direction.normalize();
				_direction_positionVersion	= positionVector.version;
				_direction_lookAtVersion	= lookAtVector.version;
			}
			
			return _direction;
		}
		
		public function get localPosition() : Vector4
		{
			var worldInverseMatrix : Matrix3D = _transformData.worldInverse;
			
			if (_localPosition_positionVersion != _position.version ||
				_localPosition_worldInverseVersion != worldInverseMatrix.version)
			{
				_localPosition = worldInverseMatrix.transformVector(_position, _localPosition);
				_localPosition_positionVersion		= _position.version;
				_localPosition_worldInverseVersion	= worldInverseMatrix.version;
			}
			
			return _localPosition;
		}
		
		public function get localLookAt() : Vector4
		{
			var worldInverseMatrix : Matrix3D = _transformData.worldInverse;
			
			if (_localLookAt_lookAtVersion != _lookAt.version ||
				_localLookAt_worldInverseVersion != worldInverseMatrix.version)
			{
				_localLookAt = worldInverseMatrix.transformVector(_lookAt, _localLookAt);
				_localLookAt_lookAtVersion 			= _lookAt.version;
				_localLookAt_worldInverseVersion 	= worldInverseMatrix.version;
			}
			
			return _localLookAt;
		}
		
		public function get localDirection() : Vector4
		{
			var localPositionVector	: Vector4 = localPosition;
			var localLookAtVector	: Vector4 = localLookAt;
			
			if (_localDirection_positionVersion != localPositionVector.version ||
				_localDirection_lookAtVersion != localLookAtVector.version)
			{
				_localDirection = Vector4.subtract(localLookAtVector, localPositionVector, _localDirection);
				_localDirection.normalize();
				_localDirection_positionVersion	= localPositionVector.version;
				_localDirection_lookAtVersion	= localLookAtVector.version;
			}
			
			return _localDirection;
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
		
		public function set fieldOfView(v : Number) : void
		{
			_fov = v;
		}
		
		public function set zNear(v : Number) : void
		{
			_zNear = v;
		}
		
		public function set zFar(v : Number) : void
		{
			_zFar = v;
		}
		
		public function CameraData()
		{
			reset()
		}
		
		public function setDataProvider(styleStack	: StyleStack, 
										transformData	: TransformData,
										worldData	: Dictionary) : void
		{
			_styleStack	= styleStack;
			_transformData	= transformData;
			_worldData	= worldData;
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
			
			_fov	= -1;
			_zNear	= -1;
			_zFar	= -1;
			
			_direction_positionVersion			= uint.MAX_VALUE;
			_direction_lookAtVersion			= uint.MAX_VALUE;
			_view_positionVersion				= uint.MAX_VALUE;
			_view_lookAtVersion					= uint.MAX_VALUE;
			_view_upVersion						= uint.MAX_VALUE;
			_localPosition_positionVersion		= uint.MAX_VALUE;
			_localPosition_worldInverseVersion	= uint.MAX_VALUE;
			_localLookAt_lookAtVersion			= uint.MAX_VALUE;
			_localLookAt_worldInverseVersion	= uint.MAX_VALUE;
			_localDirection_positionVersion		= uint.MAX_VALUE;
			_localDirection_lookAtVersion		= uint.MAX_VALUE;
			_frustum_projectionVersion			= uint.MAX_VALUE;
			
			_zFarParts_invalidated	= true;
			_projection_invalidated = true;
		}
	}
}