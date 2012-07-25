package aerys.minko.render.geometry.stream.format
{
	import flash.utils.ByteArray;

	public final class VertexComponent
	{
		private static const INSTANCIATED_COMPONENTS	: Object = new Object();

		public static const XYZ			: VertexComponent 	= create(["x", "y", "z"], VertexComponentType.FLOAT_3);
		public static const XY			: VertexComponent 	= create(["x", "y"], VertexComponentType.FLOAT_2);
		public static const UV			: VertexComponent 	= create(["u", "v"], VertexComponentType.FLOAT_2);
		public static const RGB			: VertexComponent 	= create(["r", "g", "b"], VertexComponentType.FLOAT_3);
		public static const RGBA		: VertexComponent 	= create(["r", "g", "b", "a"], VertexComponentType.FLOAT_4);
		public static const NORMAL		: VertexComponent 	= create(["nx", "ny", "nz"], VertexComponentType.FLOAT_3);
		public static const ST			: VertexComponent 	= create(["s", "t"], VertexComponentType.FLOAT_2);
		public static const ID			: VertexComponent 	= create(["id"], VertexComponentType.FLOAT_1);

		public static const TANGENT		: VertexComponent	= create(["tx", "ty", "tz"], VertexComponentType.FLOAT_3);

		public static const BONE_0_1	: VertexComponent	= create(["boneId0", "boneWeight0", "boneId1", "boneWeight1"], VertexComponentType.FLOAT_4);
		public static const BONE_2_3	: VertexComponent	= create(["boneId2", "boneWeight2", "boneId3", "boneWeight3"], VertexComponentType.FLOAT_4);
		public static const BONE_4_5	: VertexComponent	= create(["boneId4", "boneWeight4", "boneId5", "boneWeight5"], VertexComponentType.FLOAT_4);
		public static const BONE_6_7	: VertexComponent	= create(["boneId6", "boneWeight6", "boneId7", "boneWeight7"], VertexComponentType.FLOAT_4);
		public static const BONE_8_9	: VertexComponent	= create(["boneId8", "boneWeight8", "boneId9", "boneWeight9"], VertexComponentType.FLOAT_4);
		public static const BONE_10_11	: VertexComponent	= create(["boneId10", "boneWeight10", "boneId11", "boneWeight11"], VertexComponentType.FLOAT_4);
		public static const BONE_S		: VertexComponent	= create(["boneIdS", "boneWeightS"], VertexComponentType.FLOAT_2);
		
		public static const BONES		: Vector.<VertexComponent> = new <VertexComponent>[
			BONE_0_1, BONE_2_3, BONE_4_5, BONE_6_7, BONE_8_9, BONE_10_11
		];
		
		private var _nativeFormat		: int;
		private var _fields				: Vector.<String>;
		private var _offsets			: Object;
		private var _vertexComponentId	: String;	// used to avoid array comparison

		public function get size()				: int 				{ return VertexComponentType.NB_DWORDS[_nativeFormat]; }
		public function get offsets()				: Object 			{ return _offsets; }
		public function get fields()				: Vector.<String> 	{ return _fields; }
		public function get nativeFormat()			: int				{ return _nativeFormat; }
		public function get nativeFormatString()	: String 			{ return VertexComponentType.STRINGS[_nativeFormat]; }

		public static function create(fields : Array, nativeFormat : uint) : VertexComponent
		{
			var implodedFields : String = nativeFormat.toString() + '_' + fields.join("_");

			if (!INSTANCIATED_COMPONENTS[implodedFields])
				INSTANCIATED_COMPONENTS[implodedFields] = new VertexComponent(fields, nativeFormat);

			return INSTANCIATED_COMPONENTS[implodedFields];
		}
		
		public function VertexComponent(fields : Array, nativeFormat : int)
		{
			var implodedFields : String = nativeFormat.toString() + '_' + fields.join("_");

			if (INSTANCIATED_COMPONENTS[implodedFields])
				throw new Error('This component was already defined. Use VertexComponent.create() instead');

			_offsets		= new Object();
			_nativeFormat	= nativeFormat;
			_fields			= Vector.<String>(fields);

			var numFields : uint = _fields.length;
			for (var offset : int = 0; offset < numFields; ++offset)
				_offsets[fields[offset]] = offset;
		}
		
		public function hasField(fieldName : String) : Boolean
		{
			return _offsets[fieldName] !== undefined;
		}

		public function toString() : String
		{
			return _fields.join(', ');
		}
	}
}
