package aerys.minko.scene.data
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.IDataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
	import aerys.minko.type.math.Matrix4x4;
	
	public final class SkinningDataProvider implements IDataProvider
	{
		private static const DATA_DESCRIPTOR : Object = {
			'method'		: 'skinningMethod',
			'numBones'		: 'skinningNumBones',
			'maxInfluences'	: 'skinningMaxInfluences',
			'matrices'		: 'skinningMatrices',
			'dqN'			: 'skinningDQn',
			'dqD'			: 'skinningDQd' 
		};
		
		private var _method				: uint;
		private var _numBones			: uint;
		private var _maxInfluences		: uint;
		private var _matrices			: Vector.<Number>;
		private var _dqN				: Vector.<Number>;
		private var _dqD				: Vector.<Number>;
		
		private var _propertyChanged	: Signal;
		
		public function get propertyChanged() : Signal
		{
			return _propertyChanged;
		}
		
		public function get usage() : uint
		{
			return DataProviderUsage.MANAGED;
		}
		
		public function get dataDescriptor():Object
		{
			return DATA_DESCRIPTOR;
		}
		
		public function get method() : uint
		{
			return _method;
		}
		public function set method(value : uint) : void
		{
			_method = value;
			_propertyChanged.execute(this, 'method', DATA_DESCRIPTOR['method'], value);
		}
		
		public function get numBones() : uint
		{
			return _numBones;
		}
		public function set numBones(value : uint) : void
		{
			_numBones = value;
			_propertyChanged.execute(this, 'numBones', DATA_DESCRIPTOR['numBones'], value);
		}
		
		public function get maxInfluences() : uint
		{
			return _maxInfluences;
		}
		public function set maxInfluences(value : uint) : void
		{
			_maxInfluences = value;
			_propertyChanged.execute(this, 'maxInfluences', DATA_DESCRIPTOR['maxInfluences'], value);
		}
		
		public function get matrices() : Vector.<Number>
		{
			return _matrices;
		}
		public function set matrices(value : Vector.<Number>) : void
		{
			_matrices = value;
			_propertyChanged.execute(this, 'matrices', DATA_DESCRIPTOR['matrices'], value);
		}
		
		public function get dqN() : Vector.<Number>
		{
			return _dqN;
		}
		public function set dqN(value : Vector.<Number>) : void
		{
			_dqN = value;
			_propertyChanged.execute(this, 'dqN', DATA_DESCRIPTOR['dqN'], value);
		}
		
		public function get dqD() : Vector.<Number>
		{
			return _dqD;
		}
		public function set dqD(value : Vector.<Number>) : void
		{
			_dqD = value;
			_propertyChanged.execute(this, 'dqD', DATA_DESCRIPTOR['dqD'], value);
		}
		
		public function SkinningDataProvider()
		{
			super();
			
			initialize();
		}
		
		private function initialize() : void
		{
			_propertyChanged = new Signal('SkinningDataProvider.propertyChanged');
		}
		
		public function clone() : IDataProvider
		{
			throw new Error('This provider is managed, and must not be cloned');
		}
	}
}