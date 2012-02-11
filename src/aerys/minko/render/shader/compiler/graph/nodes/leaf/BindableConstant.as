package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	
	public class BindableConstant implements INode
	{
		public static const COMPUTABLE_CONSTANT_PREFIX : String = 'computableConstant';
		
		private var _hash			: uint;
		private var _bindingName	: String;
		private var _size			: uint;
		
		public function get hash() : uint
		{
			return _hash;
		}
		
		public function get bindingName() : String
		{
			return _bindingName;
		}
		
		public function get size() : uint
		{
			return _size;
		}
		
		public function BindableConstant(bindingName : String, size : uint)
		{
			if (size == 0)
				throw new Error('Cannot create a zero-sized parameter');
			
			if (size > 4 && size % 4 != 0)
				throw new Error('Size of a parameter must be either less or equal than 4, or a multiple of 4');
			
			_bindingName	= bindingName;
			_size			= size;
			_hash			= CRC32.computeForString('BindableConstant_' + bindingName + '_' + size);
		}
		
		public function toString() : String
		{
			return 'BindableConstant_' + bindingName + '_' + size;
		}
	}
}
