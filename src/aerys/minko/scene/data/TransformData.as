package aerys.minko.scene.data
{
	import aerys.minko.type.IVersionable;
	import aerys.minko.type.math.Matrix3D;

	public final class TransformData implements IVersionable
	{
		private static const _SCREEN_TO_UV : Matrix3D = new Matrix3D(
			0.5,		0.0,		0.0,	0.0,
			0.0, 		-0.5,		0.0,	0.0,
			0.0,		0.0,		1.0,	0.0,
			0.5, 		0.5,		0.0, 	1.0
		);
		
		public static const VIEW						: String = 'view';
		public static const WORLD						: String = 'world';
		public static const PROJECTION					: String = 'projection';
		
		public static const WORLD_INVERSE				: String = 'worldInverse';
		public static const VIEW_INVERSE				: String = 'viewInverse';
		
		public static const SCREEN_TO_UV				: String = 'screenToUv';
		
		public static const LOCAL_TO_VIEW				: String = 'localToView';
		public static const LOCAL_TO_SCREEN				: String = 'localToScreen';
		public static const LOCAL_TO_UV					: String = 'localToUv';
		
		public static const WORLD_TO_SCREEN				: String = 'worldToScreen';
		public static const WORLD_TO_UV					: String = 'worldToUv';
		
		protected var _world							: Matrix3D;
		protected var _view								: Matrix3D;
		protected var _projection						: Matrix3D;
		
		protected var _viewInverse						: Matrix3D;
		protected var _viewInverse_viewVersion			: uint;
		
		protected var _worldInverse						: Matrix3D;
		protected var _worldInverse_worldVersion		: uint;
		
		protected var _localToView						: Matrix3D;
		protected var _localToView_viewVersion			: uint;
		protected var _localToView_worldVersion			: uint;
		
		protected var _localToScreen					: Matrix3D;
		protected var _localToScreen_localToViewVersion : uint;
		protected var _localToScreen_projectionVersion	: uint;
		
		protected var _localToUv						: Matrix3D;
		protected var _localToUv_localToScreenVersion	: uint;
		
		protected var _globalToScreen					: Matrix3D;
		protected var _globalToScreen_viewVersion		: uint;
		protected var _globalToScreen_projectionVersion	: uint;
		
		protected var _globalToUv						: Matrix3D;
		protected var _globalToUv_globalToScreenVersion	: uint;
		
		public function get version() : uint
		{
			return _world.version + _view.version + _projection.version;
		}
		
		public function get view() : Matrix3D
		{
			return _view;
		}
		
		public function get world() : Matrix3D
		{
			return _world;
		}
		
		public function get projection() : Matrix3D
		{
			return _projection;
		}
		
		public function get worldInverse() : Matrix3D
		{
			var worldMatrix : Matrix3D = world;
			
			if (_worldInverse_worldVersion != worldMatrix.version)
			{
				_worldInverse = Matrix3D.invert(worldMatrix, _worldInverse);
				_worldInverse_worldVersion = worldMatrix.version;
			}
			
			return _worldInverse;
		}
		
		public function get viewInverse() : Matrix3D
		{
			var viewMatrix : Matrix3D = view;
			
			if (_viewInverse_viewVersion != viewMatrix.version)
			{
				_viewInverse = Matrix3D.invert(viewMatrix, _viewInverse);
				_viewInverse_viewVersion = viewMatrix.version;
			}
			
			return _viewInverse;
		}
		
		public function get localToView() : Matrix3D
		{
			var worldMatrix	: Matrix3D = world;
			var viewMatrix	: Matrix3D = view;
			
			if (_localToView_worldVersion != worldMatrix.version ||
				_localToView_viewVersion != viewMatrix.version)
			{
				_localToView = Matrix3D.multiply(viewMatrix, worldMatrix, _localToView);
				_localToView_worldVersion	= worldMatrix.version;
				_localToView_viewVersion	= viewMatrix.version;
			}
			
			return _localToView;
		}
		
		public function get localToScreen() : Matrix3D
		{
			var localToViewMatrix	: Matrix3D = localToView;
			var projectionMatrix	: Matrix3D = projection;
			
			if (_localToScreen_localToViewVersion != localToViewMatrix.version ||
				_localToScreen_projectionVersion != projectionMatrix.version)
			{
				_localToScreen = Matrix3D.multiply(projectionMatrix, localToViewMatrix, _localToScreen);
				_localToScreen_localToViewVersion	= localToViewMatrix.version;
				_localToScreen_projectionVersion	= projectionMatrix.version;
			}
			
			return _localToScreen;
		}
		
		public function get screentoUv() : Matrix3D
		{
			return _SCREEN_TO_UV;
		}
		
		public function get localToUv() : Matrix3D
		{
			var localToScreenMatrix : Matrix3D = localToScreen;
			var screenToUvMatrix	: Matrix3D = screentoUv;
			
			if (_localToUv_localToScreenVersion != localToScreenMatrix.version)
			{
				_localToUv = Matrix3D.multiply(screenToUvMatrix, localToScreenMatrix, _localToUv);
				_localToUv_localToScreenVersion = localToScreenMatrix.version;
			}
			
			return _localToUv;
		}
		
		public function get worldToScreen() : Matrix3D
		{
			var projectionMatrix	: Matrix3D = projection;
			var viewMatrix			: Matrix3D = view;
			
			if (_globalToScreen_projectionVersion != projectionMatrix.version ||
				_globalToScreen_viewVersion != viewMatrix.version)
			{
				_globalToScreen = Matrix3D.multiply(projectionMatrix, viewMatrix, _globalToScreen);
				_globalToScreen_projectionVersion	= projectionMatrix.version;
				_globalToScreen_viewVersion			= viewMatrix.version;
			}
			
			return _globalToScreen;
		}
		
		public function get worldToUv() : Matrix3D
		{
			var worldToScreenMatrix	: Matrix3D = worldToScreen;
			var screentoUvMatrix	: Matrix3D = screentoUv;
			
			if (_globalToUv_globalToScreenVersion != worldToScreenMatrix.version)
			{
				_globalToUv = Matrix3D.multiply(screentoUvMatrix, worldToScreenMatrix, _globalToUv);
				_globalToUv_globalToScreenVersion = worldToScreenMatrix.version;
			}

			return _globalToUv;
		}
		
		public function set world(value : Matrix3D) : void
		{
			Matrix3D.copy(value, _world);
		}
		
		public function set view(value : Matrix3D) : void
		{
			Matrix3D.copy(value, _view);
		}
		
		public function set projection(value : Matrix3D) : void
		{
			Matrix3D.copy(value, _projection);
		}
		
		public function TransformData()
		{
			_world		= new Matrix3D();
			_view		= new Matrix3D();
			_projection	= new Matrix3D();
			
			reset();
		}
		
		public function reset() : void
		{
			_world.identity();
			_view.identity();
			_projection.identity();
			
			_viewInverse_viewVersion			= uint.MAX_VALUE;
			_worldInverse_worldVersion			= uint.MAX_VALUE;
			_localToView_viewVersion			= uint.MAX_VALUE;
			_localToView_worldVersion			= uint.MAX_VALUE;
			_localToScreen_localToViewVersion	= uint.MAX_VALUE;
			_localToScreen_projectionVersion	= uint.MAX_VALUE;
			_globalToScreen_projectionVersion	= uint.MAX_VALUE;
			_globalToScreen_viewVersion			= uint.MAX_VALUE;
		}
	}
}