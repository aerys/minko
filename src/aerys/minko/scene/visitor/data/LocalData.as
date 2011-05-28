package aerys.minko.scene.visitor.data
{
	import aerys.minko.type.math.Matrix4x4;

	public final class LocalData
	{
		public static const VIEW						: String = 'view';
		public static const WORLD						: String = 'world';
		public static const PROJECTION					: String = 'projection';
		
		public static const WORLD_INVERSE				: String = 'worldInverse';
		public static const VIEW_INVERSE				: String = 'viewInverse';
		public static const PROJECTION_INVERSE			: String = 'projectionInverse';
		
		public static const LOCAL_TO_VIEW				: String = 'localToView';
		public static const LOCAL_TO_SCREEN				: String = 'localToScreen';
		public static const LOCAL_TO_UV					: String = 'localToUv';
		
		private static const UPDATE_NONE				: uint		= 0;
		private static const UPDATE_WORLD_TO_LOCAL		: uint		= 1;
		private static const UPDATE_CAMERA_POSITION		: uint		= 2;
		private static const UPDATE_LOCAL_TO_VIEW		: uint		= 4;
		private static const UPDATE_LOCAL_TO_SCREEN		: uint		= 8;
		private static const UPDATE_VIEW_TO_LOCAL		: uint		= 16;
		private static const UPDATE_ALL					: uint		= UPDATE_WORLD_TO_LOCAL
																	  | UPDATE_CAMERA_POSITION
																	  | UPDATE_LOCAL_TO_VIEW
																	  | UPDATE_LOCAL_TO_SCREEN
																	  | UPDATE_VIEW_TO_LOCAL;
		
		private var _update								: uint			= 0;
		
		private var _world								: Matrix4x4		= new Matrix4x4();
		private var _view								: Matrix4x4		= new Matrix4x4();
		private var _projection							: Matrix4x4		= new Matrix4x4();
		private var _viewInverse						: Matrix4x4		= null;
		private var _viewInverse_viewVersion			: uint			= 0;
		
		private var _worldInverse						: Matrix4x4		= null;
		private var _worldInverse_worldVersion			: uint			= 0;
		
		private var _localToView						: Matrix4x4		= null;
		private var _localToView_viewVersion			: uint			= 0;
		private var _localToView_worldVersion			: uint			= 0;
		
		private var _localToScreen						: Matrix4x4		= null;
		private var _localToScreen_localToViewVersion 	: uint			= 0;
		private var _localToScreen_projectionVersion	: uint			= 0;
		
		private var _localToUv							: Matrix4x4		= null;
		private var _localToUv_localToScreenVersion		: uint			= 0;
		
		public function get view() : Matrix4x4
		{
			return _view;
		}
		
		public function get world() : Matrix4x4
		{
			return _world;
		}
		
		public function get projection() : Matrix4x4
		{
			return _projection;
		}
		
		public function get worldInverse() : Matrix4x4
		{
			var worldMatrix : Matrix4x4 = world;
			
			if (_worldInverse_worldVersion != worldMatrix.version)
			{
				_worldInverse = Matrix4x4.invert(worldMatrix, _worldInverse);
				_worldInverse_worldVersion = worldMatrix.version;
			}
			
			return _worldInverse;
		}
		
		public function get viewInverse() : Matrix4x4
		{
			var viewMatrix : Matrix4x4 = view;
			
			if (_viewInverse_viewVersion != viewMatrix.version)
			{
				_viewInverse = Matrix4x4.invert(viewMatrix, _viewInverse);
				_viewInverse_viewVersion = viewMatrix.version;
			}
			
			return _viewInverse;
		}
		
		public function get localToView() : Matrix4x4
		{
			var worldMatrix	: Matrix4x4 = world;
			var viewMatrix	: Matrix4x4 = view;
			
			if (_localToView_worldVersion != worldMatrix.version ||
				_localToView_viewVersion != viewMatrix.version)
			{
				_localToView = Matrix4x4.multiply(viewMatrix, worldMatrix, _localToView);
				_localToView_worldVersion	= worldMatrix.version;
				_localToView_viewVersion	= viewMatrix.version;
			}
			
			return _localToView;
		}
		
		public function get localToScreen() : Matrix4x4
		{
			var localToViewMatrix	: Matrix4x4 = localToView;
			var projectionMatrix	: Matrix4x4 = projection;
			
			if (_localToScreen_localToViewVersion != localToViewMatrix.version ||
				_localToScreen_projectionVersion != projectionMatrix.version)
			{
				_localToScreen = Matrix4x4.multiply(projectionMatrix, localToViewMatrix, _localToScreen);
				_localToScreen_localToViewVersion	= localToViewMatrix.version;
				_localToScreen_projectionVersion	= projectionMatrix.version;
			}
			
			return _localToScreen;
		}
		
		/**
		 * FIXME
		 */
		public function get screentoUv() : Matrix4x4
		{
			var offset : Number = 0.5 + (0.5 / 2048);
			return new Matrix4x4(
				0.5,		0.0,		0.0,	0.0,
				0.0, 		-0.5,		0.0,	0.0,
				0.0,		0.0,		1.0,	0.0,
				offset, 	offset,		0.0, 	1.0
			);
		}
		
		public function get localToUv() : Matrix4x4
		{
			var localToScreenMatrix : Matrix4x4 = localToScreen;
			var screenToUvMatrix	: Matrix4x4 = screentoUv;
			
			if (_localToUv_localToScreenVersion != localToScreenMatrix.version)
			{
				_localToUv = Matrix4x4.multiply(screenToUvMatrix, localToScreenMatrix);
				_localToUv_localToScreenVersion = localToScreenMatrix.version;
			}
			
			return _localToUv;
		}
		
		public function set world(value : Matrix4x4) : void
		{
			Matrix4x4.copy(value, _world);
			_update |= UPDATE_LOCAL_TO_SCREEN | UPDATE_LOCAL_TO_VIEW | UPDATE_WORLD_TO_LOCAL;
		}
		
		public function set view(value : Matrix4x4) : void
		{
			Matrix4x4.copy(value, _view);
			_update |= UPDATE_LOCAL_TO_SCREEN | UPDATE_LOCAL_TO_VIEW | UPDATE_VIEW_TO_LOCAL;
		}
		
		public function set projection(value : Matrix4x4) : void
		{
			Matrix4x4.copy(value, _projection);
			_update |= UPDATE_LOCAL_TO_SCREEN;
		}
		
		public function LocalData()
		{
			reset();
		}
		
		public function reset() : void
		{
			_viewInverse_viewVersion = uint.MAX_VALUE;
			
			_worldInverse_worldVersion = uint.MAX_VALUE;
			
			_localToView_viewVersion = uint.MAX_VALUE;
			_localToView_worldVersion = uint.MAX_VALUE;
			
			_localToScreen_localToViewVersion = uint.MAX_VALUE;
			_localToScreen_projectionVersion = uint.MAX_VALUE;
		}
	}
}