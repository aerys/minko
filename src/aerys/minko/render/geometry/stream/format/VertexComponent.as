package aerys.minko.render.geometry.stream.format
{
	public final class VertexComponent
	{
		private static const INSTANCIATED_COMPONENTS	: Object = {};

		public static const XYZ			: VertexComponent 	= create(['x', 'y', 'z'], VertexComponentType.FLOAT_3);
		public static const XY			: VertexComponent 	= create(['x', 'y'], VertexComponentType.FLOAT_2);
		public static const UV			: VertexComponent 	= create(['u', 'v'], VertexComponentType.FLOAT_2);
		public static const RGB			: VertexComponent 	= create(['r', 'g', 'b'], VertexComponentType.FLOAT_3);
		public static const RGBA		: VertexComponent 	= create(['r', 'g', 'b', 'a'], VertexComponentType.FLOAT_4);
		public static const NORMAL		: VertexComponent 	= create(['nx', 'ny', 'nz'], VertexComponentType.FLOAT_3);
		public static const ST			: VertexComponent 	= create(['s', 't'], VertexComponentType.FLOAT_2);
		public static const ID			: VertexComponent 	= create(['id'], VertexComponentType.FLOAT_1);

		public static const TANGENT		: VertexComponent	= create(['tx', 'ty', 'tz'], VertexComponentType.FLOAT_3);

		public static const BONE_0_1	: VertexComponent	= create(['boneId0', 'boneWeight0', 'boneId1', 'boneWeight1'], VertexComponentType.FLOAT_4);
		public static const BONE_2_3	: VertexComponent	= create(['boneId2', 'boneWeight2', 'boneId3', 'boneWeight3'], VertexComponentType.FLOAT_4);
		public static const BONE_4_5	: VertexComponent	= create(['boneId4', 'boneWeight4', 'boneId5', 'boneWeight5'], VertexComponentType.FLOAT_4);
		public static const BONE_6_7	: VertexComponent	= create(['boneId6', 'boneWeight6', 'boneId7', 'boneWeight7'], VertexComponentType.FLOAT_4);
		public static const BONE_8_9	: VertexComponent	= create(['boneId8', 'boneWeight8', 'boneId9', 'boneWeight9'], VertexComponentType.FLOAT_4);
		public static const BONE_10_11	: VertexComponent	= create(['boneId10', 'boneWeight10', 'boneId11', 'boneWeight11'], VertexComponentType.FLOAT_4);
		public static const BONE_S		: VertexComponent	= create(['boneIdS', 'boneWeightS'], VertexComponentType.FLOAT_2);
		
		public static const BONES		: Vector.<VertexComponent> = new <VertexComponent>[
			BONE_0_1, BONE_2_3, BONE_4_5, BONE_6_7, BONE_8_9, BONE_10_11
		];
		
		private var _nativeFormat		: int;
		private var _properties			: Vector.<String>;
		private var _offsets			: Object;
		private var _vertexComponentId	: String;	// used to avoid array comparison
		private var _propertiesString	: String;   // Used to avoid creation of strings all the time in toString()

		public function get numBytes() : uint
		{
			return numProperties << 2;
		}
		
		public function get numProperties() : uint
		{
			return VertexComponentType.NB_DWORDS[_nativeFormat];
		}
		
		public function get nativeFormat() : int
		{
			return _nativeFormat;
		}
		
		public function get nativeFormatString() : String
		{
			return VertexComponentType.STRINGS[_nativeFormat];
		}

		public static function create(fields : Array, nativeFormat : uint) : VertexComponent
		{
			var implodedFields : String = nativeFormat.toString() + '_' + fields.join('_');

			if (!INSTANCIATED_COMPONENTS[implodedFields])
				INSTANCIATED_COMPONENTS[implodedFields] = new VertexComponent(fields, nativeFormat);

			return INSTANCIATED_COMPONENTS[implodedFields];
		}
		
		public function VertexComponent(properties : Array, nativeFormat : int)
		{
			var implodedProperties : String = nativeFormat.toString() + '_' + properties.join('_');

			if (INSTANCIATED_COMPONENTS[implodedProperties])
				throw new Error('This component was already defined. Use VertexComponent.create() instead');

			_offsets		= {};
			_nativeFormat	= nativeFormat;
			_properties		= Vector.<String>(properties);

			var numFields : uint = _properties.length;
			for (var offset : int = 0; offset < numFields; ++offset)
				_offsets[properties[offset]] = offset;
		}
		
		public function hasProperty(property : String) : Boolean
		{
			return _offsets[property] !== undefined;
		}
		
		public function getProperty(index : uint) : String
		{
			return _properties[index];
		}

		public function getPropertyBytesOffset(property : String) : uint
		{
			return getPropertyOffset(property) << 2;
		}
		
		public function getPropertyOffset(property : String) : uint
		{
			return _offsets[property];
		}
		
		public function toString() : String
		{
			if (!_propertiesString)
				_propertiesString = _properties.join(', ');
				
			return _propertiesString;
		}
	}
}
