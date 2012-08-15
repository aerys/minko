package aerys.minko.scene.data
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.IDataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
	import aerys.minko.type.math.Matrix4x4;
	
	public final class TransformDataProvider implements IDataProvider
	{
		private static const DATA_DESCRIPTOR	: Object	= {
			'localToWorld'	: 'localToWorld',
			'worldToLocal'	: 'worldToLocal'
		};
		
		private var _changed			: Signal	= new Signal('TransformDataProvider.changed');
		private var _propertyChanged	: Signal;
		
		private var _localToWorld		: Matrix4x4	= new Matrix4x4();
		private var _worldToLocal		: Matrix4x4	= new Matrix4x4();
		
		public function get usage()		: uint
		{
			return DataProviderUsage.EXCLUSIVE;
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function get propertyChanged() : Signal
		{
			_propertyChanged ||= new Signal('TransformDataProvider.propertyChanged');
			return _propertyChanged;
		}
		
		public function get dataDescriptor() : Object
		{
			return DATA_DESCRIPTOR;
		}
		
		public function get localToWorld() : Matrix4x4
		{
			return _localToWorld;
		}
		
		public function get worldToLocal() : Matrix4x4
		{
			return _worldToLocal;
		}
		
		public function TransformDataProvider()
		{
			_localToWorld.changed.add(onLocalToWorldChangedHandler);
			_worldToLocal.changed.add(onWorldToLocalChangedHandler);
		}
		
		private function onLocalToWorldChangedHandler(source : Matrix4x4) : void
		{
			if (_propertyChanged)
				_propertyChanged.execute(this, 'localToWorld');
		}
		
		private function onWorldToLocalChangedHandler(source : Matrix4x4) : void
		{
			if (_propertyChanged)
				_propertyChanged.execute(this, 'worldToLocal');
		}
		
		public function clone() : IDataProvider
		{
			throw new Error('TransformDataProviders must not be cloned.');
		}
		
	}
}
