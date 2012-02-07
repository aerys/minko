package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.register.Components;
	
	public class VariadicExtract implements INode
	{
		private var _hash					: uint;
		private var _hashIsValid			: Boolean;
		
		private var _index					: INode;
		private var _constant				: INode;
		private var _isMatrix				: Boolean;
		private var _indexComponentSelect	: uint;
		
		public function get indexComponentSelect() : uint
		{
			return _indexComponentSelect;
		}
		
		public function get hash() : uint
		{
			if (!_hashIsValid)
			{
				_hashIsValid = true;
				_hash = CRC32.computeForString(
					'VariadicExtract'
					+ index.hash.toString(16)
					+ constant.hash.toString(16)
					+ _indexComponentSelect.toString()
					+ isMatrix.toString());
			}
			
			return _hash;
		}
		
		public function get size() : uint
		{
			return _isMatrix ? 16 : 4;
		}
		
		public function get index() : INode
		{
			return _index;
		}
		
		public function get constant() : INode
		{
			return _constant;
		}
		
		public function get isMatrix() : Boolean
		{
			return _isMatrix;
		}
		
		public function set indexComponentSelect(v : uint) : void
		{
			_hashIsValid = false;
			_indexComponentSelect = v;
		}
		
		public function set index(v : INode) : void
		{
			_hashIsValid = false;
			_index = v;
		}
		
		public function VariadicExtract(index		: INode, 
										constant	: INode,
										isMatrix	: Boolean)
		{
			_index					= index;
			_constant				= constant;
			_isMatrix				= isMatrix;
			_indexComponentSelect	= 0; // x
			_hashIsValid			= false;
		}
		
		public function toString() : String
		{
			return 'VariadicExtract';
		}
	}
}