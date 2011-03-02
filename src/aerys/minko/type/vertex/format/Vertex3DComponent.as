package aerys.minko.type.vertex.format
{
	public class Vertex3DComponent
	{
		public static const XYZ			: Vertex3DComponent = new Vertex3DComponent(['x', 'y', 'z'], NativeFormat.FLOAT_3);
		public static const UV			: Vertex3DComponent = new Vertex3DComponent(['u', 'v'], NativeFormat.FLOAT_2);
		public static const RGB			: Vertex3DComponent = new Vertex3DComponent(['r', 'g', 'b'], NativeFormat.FLOAT_3);
		public static const NORMAL		: Vertex3DComponent = new Vertex3DComponent(['nx', 'ny', 'nz'], NativeFormat.FLOAT_3);
		public static const LIGHTMAP	: Vertex3DComponent = new Vertex3DComponent(['s', 't'], NativeFormat.FLOAT_2);
		
		private var _nativeFormat	: int;
		private var _fields			: Array;
		private var _offsets		: Object;
		private var _implodedFields	: String;	// used to avoid array comparison
		
		public function get dwords()			: int { return NativeFormat.NB_DWORDS[_nativeFormat]; }
		public function get offsets()			: Object { return _offsets; }
		public function get fields()			: Array { return _fields; } 
		public function get nativeFormat()		: int { return _nativeFormat; }
		public function get nativeFormatString(): String { return NativeFormat.STRINGS[_nativeFormat]; }
		public function get implodedFields()	: String { return _implodedFields; }
		
		public static function getVertex3DComponent(semantic:int):Vertex3DComponent 
		{
			throw new Error('implement me, i BEG YOU');
		}
		
		public function Vertex3DComponent(fields:Array, nativeFormat:int) 
		{
			_offsets			= new Object();
			_nativeFormat		= nativeFormat;
			_fields				= fields;
			_implodedFields		= fields.join('|');
			
			for (var offset:int = 0; offset < fields.length; ++offset)
				_offsets[fields[offset]] = offset;
		}
		
		public function isSameAs(otherComponent:Vertex3DComponent) : Boolean
		{
			return _nativeFormat == otherComponent._nativeFormat && _implodedFields == otherComponent._implodedFields;
		}
		
		public function toString():String {
			return _implodedFields;
		}
		
	}
}