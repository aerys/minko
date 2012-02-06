package aerys.minko.type.stream.format
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

		public static const BONE0		: VertexComponent	= create(["boneId0", "boneWeight0"], VertexComponentType.FLOAT_2);
		public static const BONE1		: VertexComponent	= create(["boneId1", "boneWeight1"], VertexComponentType.FLOAT_2);
		public static const BONE2		: VertexComponent	= create(["boneId2", "boneWeight2"], VertexComponentType.FLOAT_2);
		public static const BONE3		: VertexComponent	= create(["boneId3", "boneWeight3"], VertexComponentType.FLOAT_2);
		public static const BONE4		: VertexComponent	= create(["boneId4", "boneWeight4"], VertexComponentType.FLOAT_2);
		public static const BONE5		: VertexComponent	= create(["boneId5", "boneWeight5"], VertexComponentType.FLOAT_2);
		public static const BONE6		: VertexComponent	= create(["boneId6", "boneWeight6"], VertexComponentType.FLOAT_2);
		public static const BONE7		: VertexComponent	= create(["boneId7", "boneWeight7"], VertexComponentType.FLOAT_2);

		public static const BONES		: Vector.<VertexComponent> = Vector.<VertexComponent>([BONE0, BONE1, BONE2, BONE3,
																							   BONE4, BONE5, BONE6, BONE7]);

		private var _nativeFormat		: int;
		private var _fields				: Vector.<String>;
		private var _offsets			: Object;
		private var _vertexComponentId	: String;	// used to avoid array comparison

		public function get dwords()				: int 				{ return VertexComponentType.NB_DWORDS[_nativeFormat]; }
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
