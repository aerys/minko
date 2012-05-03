package aerys.minko.render.shader.compiler.graph.nodes
{
	import aerys.minko.render.shader.compiler.register.Components;

	public class ANode
	{
		private var _parents		: Vector.<ANode>;
		
		private var _hashIsValid	: Boolean;
		private var _sizeIsValid	: Boolean;
		
		private var _hash			: uint;
		private var _size			: uint;
		
		private var _arguments		: Vector.<ANode>;
		private var _components		: Vector.<uint>;
		
		public final function get hash() : uint
		{
			if (!_hashIsValid)
			{
				_hash = computeHash();
				_hashIsValid = true;
			}
			
			return _hash;
		}
		
		public final function get size() : uint
		{
			if (!_sizeIsValid)
			{
				_size = computeSize();
				_sizeIsValid = true;
			}
			
			return _size;
		}
		
		public final function get numParents() : uint
		{
			return _parents.length;
		}
		
		public final function get numArguments() : uint
		{
			return _arguments.length;
		}
		
		public function ANode(arguments  : Vector.<ANode>,
							  components : Vector.<uint>)
		{
			_arguments		= arguments;
			_components		= components;
			
			_hashIsValid	= false;
			_sizeIsValid	= false;
			_parents		= new Vector.<ANode>();
		}
		
		public function addParent(node : ANode) : void
		{
			_parents.push(node);
		}
		
		public function removeParent(node : ANode) : void
		{
			_parents.splice(_parents.indexOf(node), 1);
		}

		public function getParentAt(index : uint) : ANode
		{
			return _parents[index];
		}
		
		public function getArgumentAt(index : uint) : ANode
		{
			return _arguments[index];
		}
		
		public function setArgumentAt(index		: uint,
									  argument	: ANode) : void
		{
			_arguments[index].removeParent(this);
			_arguments[index] = argument;
			_arguments[index].addParent(this);
			
			invalidate();
		}
		
		public function getComponentAt(index : uint) : uint
		{
			return _components[index];
		}
		
		public function setComponentAt(index		: uint, 
									   component	: uint) : void
		{
			_components[index] = component;
			
			invalidate();
		}
		
		public function addArgumentAt(index		: uint, 
									  argument	: ANode,
									  component	: uint) : void
		{
			argument.addParent(this);
			
			_arguments.splice(index, 0, argument);
			_components.splice(index, 0, component);
		}
		
		public function removeArgumentAt(index : uint) : void
		{
			_arguments[index].removeParent(this);
			
			_arguments.splice(index, 1);
			_components.splice(index, 1);
		}
		
		private function invalidate() : void
		{
			_hashIsValid = _sizeIsValid = false;
		}
		
		protected function computeHash() : uint
		{
			throw new Error('Must be overriden');
		}
		
		protected function computeSize() : uint
		{
			throw new Error('Must be overriden');
		}
		
		public function toString() : String
		{
			throw new Error('Must be overriden');
		}
	}
}