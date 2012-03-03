package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.register.Components;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class Overwriter implements INode
	{
		private var _args			: Vector.<INode>;
		private var _components		: Vector.<uint>;
		
		private var _hash			: uint;
		private var _hashIsValid	: Boolean;
		
		private var _size			: uint;
		private var _sizeIsValid	: Boolean;
		
		public function get args() : Vector.<INode>
		{
			return _args;
		}
		
		public function get components() : Vector.<uint>
		{
			return _components;
		}
		
		public function get hash() : uint
		{
			if (!_hashIsValid)
			{
				var numArgs : uint		= _args.length;
				var hash	: String	= 'Overwriter';
				
				for (var argId : uint = 0; argId < numArgs; ++argId)
					hash += _args[argId].hash.toString() + _components[argId].toString();
				
				_hash			= CRC32.computeForString(hash);
				_hashIsValid	= true;
			}
			
			return _hash;
		}
		
		public function get size() : uint
		{
			if (!_sizeIsValid)
			{
				var numArgs		: uint		= _args.length;
				var xDefined	: Boolean	= false;
				var yDefined	: Boolean	= false;
				var zDefined	: Boolean	= false;
				var wDefined	: Boolean	= false;
				
				for (var argId : uint = 0; argId < numArgs; ++argId)
				{
					var component : uint = _components[argId];
					xDefined ||= Components.getReadAtIndex(0, component) != 4;
					yDefined ||= Components.getReadAtIndex(1, component) != 4;
					zDefined ||= Components.getReadAtIndex(2, component) != 4;
					wDefined ||= Components.getReadAtIndex(3, component) != 4;
				}
				
				if (xDefined && !yDefined && !zDefined && !wDefined)
					_size = 1;
				else if (xDefined && yDefined && !zDefined && !wDefined)
					_size = 2;
				else if (xDefined && yDefined && zDefined && !wDefined)
					_size = 3;
				else if (xDefined && yDefined && zDefined && wDefined)
					_size = 4;
				else
					throw new Error('Components are invalid. Cannot compute size');
				
				_sizeIsValid = true;
			}
			
			return _size;
		}
		
		public function Overwriter(args			: Vector.<INode>,
								   components	: Vector.<uint>)
		{
			_args			= args;
			_components		= components;
			
			_sizeIsValid	= false;
			_hashIsValid	= false;
			
			checkComponents();
		}
		
		public function invalidateHashAndSize() : void
		{
			_hashIsValid = false;
			_sizeIsValid = false;
			
			checkComponents();
		}
		
		public function toString() : String
		{
			return 'Overwriter';
		}
		
		private function checkComponents() : void
		{
			var numArgs : uint = _args.length;
			if (numArgs != _components.length)
				throw new Error('An overwriter must have has many arguments than components');
			
			for (var componentId : uint = 0; componentId < numArgs; ++componentId)
				if (_components[componentId] == Components.____)
					throw new Error('Cannot set an empty component.');
		}
	}
}