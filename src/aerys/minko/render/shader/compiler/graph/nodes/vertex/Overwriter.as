package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Attribute;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Parameter;
	
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
					xDefined ||= Components.getReadAtIndex(0, component) != Components._;
					yDefined ||= Components.getReadAtIndex(1, component) != Components._;
					zDefined ||= Components.getReadAtIndex(2, component) != Components._;
					wDefined ||= Components.getReadAtIndex(3, component) != Components._;
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
		}
		
		public function invalidateHashAndSize() : void
		{
			_hashIsValid = false;
			_sizeIsValid = false;
		}
		
		public function toString() : String
		{
			return 'Overwriter';
		}
	}
}