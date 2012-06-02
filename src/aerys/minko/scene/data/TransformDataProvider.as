package aerys.minko.scene.data
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.IDataProvider;
	import aerys.minko.type.math.Matrix4x4;
	
	public final class TransformDataProvider implements IDataProvider
	{
		private static const DATA_DESCRIPTOR	: Object	= {
			'localToWorld'	: 'localToWorld',
			'worldToLocal'	: 'worldToLocal'
		};
		
		private var _changed		: Signal	= new Signal('TransformDataProvider.changed');
		
		private var _localToWorld	: Matrix4x4	= new Matrix4x4();
		private var _worldToLocal	: Matrix4x4	= new Matrix4x4();
		
		public function get changed() : Signal
		{
			return _changed;
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
	}
}