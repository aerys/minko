package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class Extract implements INode
	{
		private var _child			: INode;
		private var _components		: uint;
		
		private var _size			: uint;
		private var _sizeIsValid	: Boolean;
		
		private var _hash			: uint;
		private var _hashIsValid	: Boolean;
		
		public function get child() : INode
		{
			return _child;
		}
		
		public function get components() : uint
		{
			return _components;
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
		
		public function get hash() : uint
		{
			if (!_hashIsValid)
			{
				_hash = CRC32.computeForString('Extract' + _child.hash.toString(16) + _components.toString(16));
				_hashIsValid = true;
			}
			
			return _hash;
		}
		
		public function set child(v : INode) : void
		{
			_child			= v;
			_sizeIsValid	= false;
			_hashIsValid	= false;
		}
		
		public function set components(v : uint) : void
		{
			_components 	= v;
			_sizeIsValid	= false;
			_hashIsValid	= false;
			
			checkComponents();
		}
		
		public function Extract(child		: INode,
								components	: uint)
		{
			_child			= child;
			_components		= components;
			_sizeIsValid	= false;
			_hashIsValid	= false;
			
			checkComponents();
		}
		
		private function checkComponents() : void
		{
			if (Components.hasHoles(components))
				throw new Error(
					'Extract should be used only with continuous components. ' +
					'Components with holes are reserved to the Overwriter node');
		}
		
		public function toString() : String
		{
			return 'Extract';
		}
	}
}