package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.ns.minko;

	public class AbstractParameter extends AbstractConstant
	{
		use namespace minko;
		
		minko var _size		: uint;
		minko var _key		: Object;
		minko var _field	: String;
		minko var _index	: int;
		
		override public function get size() : uint
		{
			return _size;
		}
		
		public function get key() : Object
		{
			return _key;
		}
		
		public function get field() : String
		{
			return _field;
		}
		
		public function get index() : int
		{
			return _index;
		}
		
		public function AbstractParameter(size	: uint,
										  key	: Object,
										  field	: String	= null,
										  index	: int		= -1)
		{
			super('Parameter_' + key + '_' + size);
			
			_size	= size;
			_key	= key;
			_field	= field;
			_index	= index;
		}
		
		override public function toString() : String
		{
			return "Parameter[name=" + name + ", "
				+ "index=" + index  + ", "
				+ "field=" + field + "]";
		}
	}
}
