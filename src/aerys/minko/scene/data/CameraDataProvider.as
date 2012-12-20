package aerys.minko.scene.data
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.binding.IDataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
	import aerys.minko.type.math.Frustum;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public final class CameraDataProvider extends DataProvider
	{
		private static const DATA_DESCRIPTOR	: Object = {
			'fieldOfView'	: 'cameraFov',
			'zNear'			: 'cameraZNear',
			'zFar'			: 'cameraZFar',
			'projection'	: 'projection',
			'worldToScreen'	: 'worldToScreen',
			'zoom'			: 'zoom'
		};
		
		private var _fieldOfView		: Number;
		private var _zNear				: Number;
		private var _zFar				: Number;
		private var _zoom				: Number;
		private var _worldToScreen		: Matrix4x4;
		private var _screenToWorld		: Matrix4x4;
		private var _projection			: Matrix4x4;
		private var _frustum			: Frustum;

		override public function get usage() : uint
		{
			return DataProviderUsage.MANAGED;
		}
		
		override public function get dataDescriptor() : Object
		{
			return DATA_DESCRIPTOR;
		}
		
		public function get fieldOfView() : Number
		{
			return _fieldOfView;
		}
		public function set fieldOfView(value : Number) : void
		{
			_fieldOfView = value;
			changed.execute(this, 'fieldOfView');
		}
		
		public function get zoom():Number
		{
			return _zoom;
		}
		
		public function set zoom(value:Number):void
		{
			_zoom = value;
			changed.execute(this, 'zoom');
		}
		
		public function get zNear() : Number
		{
			return _zNear;
		}
		public function set zNear(value : Number) : void
		{
			_zNear = value;
			changed.execute(this, 'zNear');
		}
		
		public function get zFar() : Number
		{
			return _zFar;
		}
		public function set zFar(value : Number) : void
		{
			_zFar = value;
			changed.execute(this, 'zFar');
		}
		
		public function get worldToScreen() : Matrix4x4
		{
			return _worldToScreen;
		}
		
		public function get projection() : Matrix4x4
		{
			return _projection;
		}
		
		public function get frustum() : Frustum
		{
			return _frustum;
		}
		
		public function CameraDataProvider()
		{
			super();
			
			initialize();
		}
		
		private function initialize() : void
		{
			_frustum = new Frustum();
			
			watchProperty('projection', _projection = new Matrix4x4());
			watchProperty('worldToScreen', _worldToScreen = new Matrix4x4());
		}
	}
}