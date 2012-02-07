package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.register.Components;
	
	public class Interpolate implements INode
	{
		private var _arg			: INode;
		private var _components		: uint;
		
		private var _hash			: uint;
		private var _hashIsValid	: Boolean;
		
		private var _size			: uint;
		private var _sizeIsValid	: Boolean;
		
		public function get arg() : INode
		{
			return _arg;
		}
		
		public function get components() : uint
		{
			return _components;
		}
		
		public function get hash() : uint
		{
			if (!_hashIsValid)
			{
				_hash			= CRC32.computeForString('Interpolate' + _components.toString() + arg.hash.toString());
				_hashIsValid	= true;
			}
			
			return _hash;
		}
		
		public function get size() : uint
		{
			if (!_sizeIsValid)
			{
				_size			= Components.getMaxWriteOffset(_components) + 1;
				_sizeIsValid	= true;
			}
			
			return _size;
		}
		
		public function set arg(v : INode) : void
		{
			_arg			= v;
			_hashIsValid	= false;
		}
		
		public function set components(v : uint) : void
		{
			_components		= v;
			_hashIsValid	= false;
			_sizeIsValid	= false;
		}
		
		public function Interpolate(arg : INode)
		{
			_arg		= arg;
			_components	= Components.createContinuous(0, 0, arg.size, arg.size);
		}
		
		public function toString() : String
		{
			return 'Interpolate';
		}
	}
}