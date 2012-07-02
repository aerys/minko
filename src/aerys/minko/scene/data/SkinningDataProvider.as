package aerys.minko.scene.data
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.IDataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
	import aerys.minko.type.math.Matrix4x4;
	
	public final class SkinningDataProvider implements IDataProvider
	{
		private static const DATA_DESCRIPTOR : Object = {
			'method'		: 'skinningMethod',
			'numBones'		: 'skinningNumBones',
			'bindShape'		: 'skinningBindShape',
			'maxInfluences'	: 'skinningMaxInfluences',
			'matrices'		: 'skinningMatrices',
			'dqN'			: 'skinningDQn',
			'dqD'			: 'skinningDQd' 
		};
		
		private var _method				: uint				= 0;
		private var _numBones			: uint				= 0;
		private var _bindShape			: Matrix4x4			= null;
		private var _maxInfluences		: uint				= 0;
		private var _matrices			: Vector.<Number>	= new <Number>[];
		private var _dqN				: Vector.<Number>	= new <Number>[];
		private var _dqD				: Vector.<Number>	= new <Number>[];
		
		private var _changed			: Signal			= new Signal('SkinningDataProvider.changed');
		private var _propertyChanged	: Signal			= new Signal('SkinningDataProvider.propertyChanged');
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
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
			_changed.execute(this, 'method');
		}
		
		public function get numBones() : uint
		{
			return _numBones;
		}
		public function set numBones(value : uint) : void
		{
			_numBones = value;
			_changed.execute(this, 'numBones');
		}
		
		public function get bindShape() : Matrix4x4
		{
			return _bindShape;
		}
		public function set bindShape(value : Matrix4x4) : void
		{
			if (_bindShape)
				_bindShape.changed.remove(bindShapeChangedHandler);
			
			_bindShape = value;
			_bindShape.changed.add(bindShapeChangedHandler);
			
			_changed.execute(this, 'bindShape');
		}
		
		public function get maxInfluences() : uint
		{
			return _maxInfluences;
		}
		public function set maxInfluences(value : uint) : void
		{
			_maxInfluences = value;
			_changed.execute(this, 'maxInfluences');
		}
		
		public function get matrices() : Vector.<Number>
		{
			return _matrices;
		}
		public function set matrices(value : Vector.<Number>) : void
		{
			_matrices = value;
			_changed.execute(this, 'matrices');
		}
		
		public function get dqN() : Vector.<Number>
		{
			return _dqN;
		}
		public function set dqN(value : Vector.<Number>) : void
		{
			_dqN = value;
			_changed.execute(this, 'dqN');
		}
		
		public function get dqD() : Vector.<Number>
		{
			return _dqD;
		}
		public function set dqD(value : Vector.<Number>) : void
		{
			_dqD = value;
			_changed.execute(this, 'dqD');
		}
		
		public function SkinningDataProvider()
		{
			super();
		}
		
		private function bindShapeChangedHandler(source : Matrix4x4, key : String) : void
		{
			_propertyChanged.execute(this, 'bindShape');
		}
		
		public function clone() : IDataProvider
		{
			throw new Error('This provider is managed, and must not be cloned');
		}
	}
}