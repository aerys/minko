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
			'position'		: 'cameraPosition',
			'direction'		: 'cameraDirection',
			'fieldOfView'	: 'cameraFov',
			'zNear'			: 'cameraZNear',
			'zFar'			: 'cameraZFar',
			'worldToView'	: 'worldToView',
			'viewToWorld'	: 'viewToWorld',
			'projection'	: 'projection',
			'screenToView'	: 'screenToView',
			'worldToScreen'	: 'worldToScreen',
			'screenToWorld'	: 'screenToWorld',
			'zoom'			: 'zoom'
		};
		
//		private var _changed			: Signal		= new Signal('CameraDataProvider.changed');
//		private var _propertyChanged	: Signal		= new Signal('CameraDataProvider.propertyChanged');
		
		private var _position			: Vector4		= new Vector4();
		private var _direction			: Vector4		= new Vector4();
		private var _fieldOfView		: Number		= 0.0;
		private var _zNear				: Number		= 0.0;
		private var _zFar				: Number		= 0.0;
		private var _zoom				: Number		= 0.0;
		private var _worldToView		: Matrix4x4		= null;
		private var _viewToWorld		: Matrix4x4		= null;
		private var _worldToScreen		: Matrix4x4		= new Matrix4x4();
		private var _screenToWorld		: Matrix4x4		= new Matrix4x4();
		private var _projection			: Matrix4x4		= new Matrix4x4();
		private var _screenToView		: Matrix4x4		= new Matrix4x4();
		private var _frustum			: Frustum		= new Frustum();

		public function set viewToWorld(value:Matrix4x4):void
		{
			_viewToWorld = value;
		}

		public function set worldToView(value:Matrix4x4):void
		{
			_worldToView = value;
		}

		override public function get usage() : uint
		{
			return DataProviderUsage.MANAGED;
		}
		
		override public function get dataDescriptor() : Object
		{
			return DATA_DESCRIPTOR;
		}
		
		public function get position() : Vector4
		{
			return _position;
		}
		
		public function get direction() : Vector4
		{
			return _direction;
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
		
		public function get worldToView() : Matrix4x4
		{
			return _worldToView;
		}
		
		public function get viewToWorld() : Matrix4x4
		{
			return _viewToWorld;
		}
		
		public function get worldToScreen() : Matrix4x4
		{
			return _worldToScreen;
		}
		
		public function get screenToWorld() : Matrix4x4
		{
			return _screenToWorld;
		}
		
		public function get projection() : Matrix4x4
		{
			return _projection;
		}
		
		public function get screenToView() : Matrix4x4
		{
			return _screenToView;
		}
		
		public function get frustum() : Frustum
		{
			return _frustum;
		}
		
		public function CameraDataProvider(worldToView	: Matrix4x4,
										   viewToWorld	: Matrix4x4)
		{
			_worldToView = worldToView;
			_viewToWorld = viewToWorld;
			
			watchProperty('position', _position);
			watchProperty('direction', _direction);
			watchProperty('projection', _projection);
			watchProperty('screenToView', _screenToView);
			watchProperty('screenToWorld', _screenToWorld);
			watchProperty('viewToWorld', _viewToWorld);
			watchProperty('worldToScreen', _worldToScreen);
			watchProperty('worldToView', _worldToView);
		}
		
		override public function clone() : IDataProvider
		{
			throw new Error('This provider is managed, and must not be cloned.');
		}
	}
}