package aerys.minko.scene.data
{
	import aerys.minko.type.IVersionable;
	import aerys.minko.type.math.Matrix4x4;

	public final class LocalData implements IVersionable
	{
		private static const _SCREEN_TO_UV : Matrix4x4 = new Matrix4x4(
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
		
		protected var _world							: Matrix4x4;
		protected var _view								: Matrix4x4;
		protected var _projection						: Matrix4x4;
		
		protected var _viewInverse						: Matrix4x4;
		protected var _viewInverse_viewVersion			: uint;
		
		protected var _worldInverse						: Matrix4x4;
		protected var _worldInverse_worldVersion		: uint;
		
		protected var _localToView						: Matrix4x4;
		protected var _localToView_viewVersion			: uint;
		protected var _localToView_worldVersion			: uint;
		
		protected var _localToScreen					: Matrix4x4;
		protected var _localToScreen_localToViewVersion : uint;
		protected var _localToScreen_projectionVersion	: uint;
		
		protected var _localToUv						: Matrix4x4;
		protected var _localToUv_localToScreenVersion	: uint;
		
		protected var _globalToScreen					: Matrix4x4;
		protected var _globalToScreen_viewVersion		: uint;
		protected var _globalToScreen_projectionVersion	: uint;
		
		public function get version() : uint
		{
			return _world.version + _view.version + _projection.version;
		}
		
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
		
		public function get screentoUv() : Matrix4x4
		{
			return _SCREEN_TO_UV;
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
		
		public function get worldToScreen() : Matrix4x4
		{
			var projectionMatrix	: Matrix4x4 = projection;
			var viewMatrix			: Matrix4x4 = view;
			
			if (_globalToScreen_projectionVersion != projectionMatrix.version ||
				_globalToScreen_viewVersion != viewMatrix.version)
			{
				_globalToScreen = Matrix4x4.multiply(projectionMatrix, viewMatrix, _globalToScreen);
				_globalToScreen_projectionVersion	= projectionMatrix.version;
				_globalToScreen_viewVersion			= viewMatrix.version;
			}
			
			return _globalToScreen;
		}
		
		public function set world(value : Matrix4x4) : void
		{
			Matrix4x4.copy(value, _world);
		}
		
		public function set view(value : Matrix4x4) : void
		{
			Matrix4x4.copy(value, _view);
		}
		
		public function set projection(value : Matrix4x4) : void
		{
			Matrix4x4.copy(value, _projection);
		}
		
		public function LocalData()
		{
			_world		= new Matrix4x4();
			_view		= new Matrix4x4();
			_projection	= new Matrix4x4();
			
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