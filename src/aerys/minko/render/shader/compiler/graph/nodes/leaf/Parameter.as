package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.shader.accessor.IParameterAccessor;
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	
	public class Parameter implements INode
	{
		private var _name		: String;
		private var _accessor	: IParameterAccessor;
		private var _hash		: uint;
		
		private var _size		: uint;
		
		public function get hash() : uint
		{
			return _hash;
		}
		
		public function get name() : String
		{
			return _name;
		}
		
		public function get size() : uint
		{
			return _size;
		}
		
		public function get accessor() : IParameterAccessor
		{
			return _accessor;
		}
		
		public function Parameter(name		: String,
								  size		: uint,
								  accessor	: IParameterAccessor = null)
		{
			_name		= name;
			_size		= size;
			_accessor	= accessor;
			_hash		= CRC32.computeForString('Parameter' + name + size + (accessor != null ? accessor.hash.toString() : ''));
		}
		
		public function toString() : String
		{
			return 'Parameter';
		}
	}
}
