package aerys.minko.render.shader.compiler.graph.nodes
{
	import aerys.minko.render.shader.compiler.register.Components;
	
	import flash.utils.Dictionary;

	public class AbstractNode
	{
		private var _parents		: Vector.<AbstractNode>;
		
		private var _hashIsValid	: Boolean;
		private var _sizeIsValid	: Boolean;
		
		private var _hash			: uint;
		private var _size			: uint;
		
		private var _arguments		: Vector.<AbstractNode>;
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
		
		public function AbstractNode(arguments  : Vector.<AbstractNode>,
							  components : Vector.<uint>)
		{
			_arguments		= arguments;
			_components		= components;
			
			_hashIsValid	= false;
			_sizeIsValid	= false;
			_parents		= new Vector.<AbstractNode>();
			
			if (arguments.length != components.length)
				throw new ArgumentError('Invalid arguments, both vector must have the same size.');
			
			for each (var argument : AbstractNode in arguments)
				argument.addParent(this);
		}
		
		public function addParent(node : AbstractNode) : void
		{
			_parents.push(node);
		}
		
		public function removeParent(node : AbstractNode) : void
		{
			_parents.splice(_parents.indexOf(node), 1);
		}

		public function getParentAt(index : uint) : AbstractNode
		{
			return _parents[index];
		}
		
		public function getArgumentAt(index : uint) : AbstractNode
		{
			return _arguments[index];
		}
		
		public function setArgumentAt(index		: uint,
									  argument	: AbstractNode) : void
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
									  argument	: AbstractNode,
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
		
		public function clone() : AbstractNode
		{
			throw new Error('Must be overriden');
		}
		
		public function deepClone() : AbstractNode
		{
			var nodeToClone : Dictionary	= new Dictionary();
			var clone		: AbstractNode			= this.clone();
			var numArgs		: uint			= this.numArguments;
			var argumentId	: uint; 
			
			for (argumentId = 0; argumentId < numArgs; ++argumentId)
				if (!nodeToClone[clone.getArgumentAt(argumentId)])
					nodeToClone[clone.getArgumentAt(argumentId)] = clone.getArgumentAt(argumentId).deepClone();
			
			for (argumentId = 0; argumentId < numArgs; ++argumentId)
				clone.setArgumentAt(argumentId, nodeToClone[clone.getArgumentAt(argumentId)]);
			
			return clone;
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