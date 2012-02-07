package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.Serializer;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;

	public class Constant implements INode
	{
		private var _hash	: uint;
		private var _value	: Vector.<Number>
		
		public function get hash() : uint
		{
			return _hash;
		}
		
		public function get size() : uint
		{
			return _value.length;
		}
		
		public function get value() : Vector.<Number>
		{
			return _value;
		}
		
		public function Constant(value : Object, size : uint = 0)
		{
			_value = new Vector.<Number>();
			
			if (size == 0)
				Serializer.serializeUnknownLength(value, _value, 0);
			else
				Serializer.serializeKnownLength(value, _value, 0, size);
			
			_hash = CRC32.computeForNumberVector(_value);
		}
		
		public function toString() : String
		{
			return 'Constant';
		}
	}
}
