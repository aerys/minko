package aerys.minko.scene.visitor.data
{
	import aerys.minko.type.math.Matrix4x4;

	/**
	 * Wrap the TransformManager and furnish some useful matrix that can directly be used
	 * in passes or called from dynamic shaders
	 * 
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 */
	public class TransformData
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
		
		protected var _tm									: TransformManager;
		
		protected var _viewInverse							: Matrix4x4;
		protected var _viewInverse_viewVersion				: uint;
		
		protected var _worldInverse							: Matrix4x4;
		protected var _worldInverse_worldVersion			: uint;
		
		protected var _localToView							: Matrix4x4;
		protected var _localToView_viewVersion				: uint;
		protected var _localToView_worldVersion				: uint;
		
		protected var _localToScreen						: Matrix4x4;
		protected var _localToScreen_localToViewVersion 	: uint;
		protected var _localToScreen_projectionVersion		: uint;
		
		protected var _localToUv							: Matrix4x4;
		protected var _localToUv_localToScreenVersion		: uint;
		
		public function get view() : Matrix4x4
		{
			return _tm.view;
		}
		
		public function get world() : Matrix4x4
		{
			return _tm.world;
		}
		
		public function get projection() : Matrix4x4
		{
			return _tm.projection;
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
		 * FIXME URGENTLY I'M A KLUDGE OUT OF LAZINESS
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
		
		public function TransformData(transformManager : TransformManager)
		{
			_tm = transformManager;
			reset();
		}
		
		public function reset() : void
		{
//			_viewInverse_viewVersion = uint.MAX_VALUE;
//			
//			_worldInverse_worldVersion = uint.MAX_VALUE;
//			
//			_localToView_viewInverseVersion = uint.MAX_VALUE;
//			_localToView_worldVersion = uint.MAX_VALUE;
//			
//			_localToScreen_localToViewVersion = uint.MAX_VALUE;
//			_localToScreen_projectionVersion = uint.MAX_VALUE;
		}
		
	}
}