package aerys.minko.scene.data
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.IDataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	public final class CameraDataProvider implements IDataProvider
	{
		private static const DATA_DESCRIPTOR	: Object = {
			'position'				: 'cameraPosition',
			'direction'				: 'cameraDirection',
			'fieldOfView'			: 'cameraFov',
			'zNear'					: 'cameraZNear',
			'zFar'					: 'cameraZFar',
			'worldToView'			: 'worldToView',
			'viewToWorld'			: 'viewToWorld',
			'projection'			: 'projection',
			'screenToView'			: 'screenToView',
			'worldToScreen'			: 'worldToScreen',
			'screenToWorld'			: 'screenToWorld'
		};
		
		private var _changed			: Signal		= new Signal('CameraDataProvider.changed');
		private var _propertyChanged	: Signal		= new Signal('CameraDataProvider.propertyChanged');
		
		private var _position			: Vector4		= new Vector4();
		private var _direction			: Vector4		= new Vector4();
		private var _fieldOfView		: Number		= 0.0;
		private var _zNear				: Number		= 0.0;
		private var _zFar				: Number		= 0.0;
		private var _worldToView		: Matrix4x4		= null;
		private var _viewToWorld		: Matrix4x4		= null;
		private var _worldToScreen		: Matrix4x4		= new Matrix4x4();
		private var _screenToWorld		: Matrix4x4		= new Matrix4x4();
		private var _projection			: Matrix4x4		= new Matrix4x4();
		private var _screenToView		: Matrix4x4		= new Matrix4x4();
		
		public function get usage() : uint
		{
			return DataProviderUsage.MANAGED;
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function get propertyChanged() : Signal
		{
			return _propertyChanged;
		}
		
		public function get dataDescriptor() : Object
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
			_changed.execute(this, 'fieldOfView');
		}
		
		public function get zNear() : Number
		{
			return _zNear;
		}
		public function set zNear(value : Number) : void
		{
			_zNear = value;
			_changed.execute(this, 'zNear');
		}
		
		public function get zFar() : Number
		{
			return _zFar;
		}
		public function set zFar(value : Number) : void
		{
			_zFar = value;
			_changed.execute(this, 'zFar');
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
		
		public function CameraDataProvider(worldToView	: Matrix4x4,
										   viewToWorld	: Matrix4x4)
		{
			_worldToView = worldToView;
			_viewToWorld = viewToWorld;
			
			_position.changed.add(positionChangedHandler);
			_direction.changed.add(directionChangedHandler);
			_projection.changed.add(projectionChangedHandler);
			_screenToView.changed.add(screenToViewChangedHandler);
			_screenToWorld.changed.add(screenToWorldChangedHandler);
			_viewToWorld.changed.add(viewToWorldChangedHandler);
			_worldToScreen.changed.add(worldToScreenChangedHandler);
			_worldToView.changed.add(worldToViewChangedHandler);
		}
		
		private function worldToViewChangedHandler(source : Matrix4x4, key : String):void
		{
			_propertyChanged.execute(this, 'worldToView');
		}
		
		private function worldToScreenChangedHandler(source : Matrix4x4, key : String):void
		{
			_propertyChanged.execute(this, 'worldToScreen');
		}
		
		private function screenToWorldChangedHandler(source : Matrix4x4, key : String):void
		{
			_propertyChanged.execute(this, 'screenToWorld');
		}
		
		private function viewToWorldChangedHandler(source : Matrix4x4, key : String):void
		{
			_propertyChanged.execute(this, 'viewToWorld');
		}
		
		private function screenToViewChangedHandler(source : Matrix4x4, key : String):void
		{
			_propertyChanged.execute(this, 'screenToView');
		}
		
		private function projectionChangedHandler(source : Matrix4x4, key : String):void
		{
			_propertyChanged.execute(this, 'projection');
		}
		
		private function directionChangedHandler(source : Vector4, key : String):void
		{
			_propertyChanged.execute(this, 'direction');
		}
		
		private function positionChangedHandler(source : Vector4, key : String):void
		{
			_propertyChanged.execute(this, 'position');
		}
		
		public function clone() : IDataProvider
		{
			throw new Error('This provider is managed, and must not be cloned');
		}
	}
}