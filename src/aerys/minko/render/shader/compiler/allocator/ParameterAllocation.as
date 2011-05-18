package aerys.minko.render.shader.compiler.allocator
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.shader.node.leaf.AbstractParameter;

	public final class ParameterAllocation
	{
		use namespace minko;
		
		minko var _offset		: uint;
		minko var _parameter	: AbstractParameter;
		
//		public final function get offset() : uint
//		{
//			return _offset;
//		}
//
//		public final function get size() : uint
//		{
//			return _parameter._size;
//		}
//		
//		public final function get key() : String
//		{
//			return _parameter._key;
//		}
//		
//		public final function get index() : int
//		{
//			return _parameter._index;
//		}
//		
//		public final function get field() : String
//		{
//			return _parameter._field;
//		}
		
		public function ParameterAllocation(offset		: uint, 
											parameter	: AbstractParameter)
		{
			_parameter			= parameter;
			_offset				= offset;
		}
		
		public final function toString() : String
		{
			return _parameter.toString();
		}
	}
}
