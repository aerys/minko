package aerys.minko.scene.data
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
	import aerys.minko.type.math.Matrix4x4;
	
	public final class TransformDataProvider extends DataProvider
	{
		private static const DATA_DESCRIPTOR	: Object	= {
			'localToWorld'	: 'localToWorld',
			'worldToLocal'	: 'worldToLocal'
		};
		
		private var _changed		: Signal;
		
		private var _localToWorld	: Matrix4x4;
		private var _worldToLocal	: Matrix4x4;
		
		override public function get dataDescriptor() : Object
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
		
		public function TransformDataProvider(localToWorld	: Matrix4x4,
											  worldToLocal	: Matrix4x4)
		{
			super(null, null, DataProviderUsage.MANAGED);
			
			initialize(localToWorld, worldToLocal);
		}
		
		private function initialize(localToWorld	: Matrix4x4,
									worldToLocal	: Matrix4x4) : void
		{
			_changed = new Signal('TransformDataProvider.changed');
			_localToWorld = localToWorld;
			_worldToLocal = worldToLocal;
			
			watchProperty('localToWorld', _localToWorld);
			watchProperty('worldToLocal', _worldToLocal);
		}
	}
}
