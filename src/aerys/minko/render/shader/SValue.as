package aerys.minko.render.shader
{
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.builtin.Add;
	import aerys.minko.render.shader.node.operation.builtin.Divide;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct3;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct4;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Multiply4x4;
	import aerys.minko.render.shader.node.operation.builtin.Normalize;
	import aerys.minko.render.shader.node.operation.builtin.Power;
	import aerys.minko.render.shader.node.operation.builtin.SquareRoot;
	import aerys.minko.render.shader.node.operation.builtin.Substract;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.math.Product;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Point;
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;

	public dynamic class SValue extends Proxy
	{
		internal var _node	: INode	= null;
		
		public final function get length() : SValue
		{
			if (_node.size == 2)
			{
				var x : INode = new Extract(_node, Components.X);
				var y : INode = new Extract(_node, Components.Y);
				
				return new SValue(
					new SquareRoot(
						new Add(
							new Multiply(x, x),
							new Multiply(y, y)
						)
					)
				);
			}
			else if (_node.size == 3)
				return new SValue(new SquareRoot(new DotProduct3(_node, _node)));
			else if (_node.size == 4)
				return new SValue(new SquareRoot(new DotProduct4(_node, _node)));
			
			throw new Error("Unable to get the length of a value with size > 4.");
		}
		
		public function SValue(value : Object)
		{
			_node = getNode(value);
		}
		
		/*public final function combine(value	: Object) : SValue
		{
			return new SValue(new Combine(_node, getNode(value)));
		}*/
		
		public final function multiply(arg : Object, ...args) : SValue
		{
			var p 		: Product 	= new Product(_node, getNode(arg));
			var numArgs : int 		= args.length;
			
			for (var i : int = 0; i < numArgs; ++i)
				p.addTerm(args[i]._node);
			
			_node = p;
			
			return this;
		}
		
		public final function scale(arg : Object) : SValue
		{
			_node = new Multiply(_node, getNode(arg));
			
			return this;
		}
		
		public final function divide(arg : Object) : SValue
		{
			return new SValue(new Divide(_node, getNode(arg)));
		}
		
		public final function pow(exp : Object) : SValue
		{
			_node = new Power(_node, getNode(exp));
			
			return this;
		}
		
		public final function add(value : Object) : SValue
		{
			return new SValue(new Add(_node, getNode(value)));
		}
		
		public final function increment(value : Object) : SValue
		{
			_node = new Add(_node, getNode(value));
			
			return this;
		}
		
		public final function subtract(value : Object) : SValue
		{
			return new SValue(new Substract(_node, getNode(value)));
		}
		
		public final function decrement(value : Object) : SValue
		{
			_node = new Substract(_node, getNode(value));
			
			return this;
		}
		
		public final function dotProduct3(value : Object) : SValue
		{
			return new SValue(new DotProduct3(_node, getNode(value)));
		}
		
		public final function dotProduct4(value : Object) : SValue
		{
			return new SValue(new DotProduct4(_node, getNode(value)));
		}
		
		public final function multiply4x4(value : Object) : SValue
		{
			return new SValue(new Multiply4x4(_node, getNode(value)));
		}
		
		public final function normalize() : SValue
		{
			_node = new Normalize(_node);
			
			return this;
		}
		
		override flash_proxy function getProperty(name : *) : *
		{
			// TODO: handle [] AGAL notation here
			
			return new SValue(new Extract(_node, getComponentsFromString(name)));
		}
		
		override flash_proxy function setProperty(name : *, value : *) : void
		{
			// TODO: handle [] AGAL notation here

			var str 	: String	= String(name).toLowerCase();
			var val 	: INode	 	= getNode(value);
			var size 	: uint 		= _node.size;
			var comps 	: uint 		= 0;
			
			if (size == 1)
				throw new Error("Unable set component(s) of a value with size == 1.");
			
			for (var i : int = 0; i < size; ++i)
			{
				var char : String = str.charAt(i);
				
				if (char == "x" || char == "r")
					comps |= i + 1;
				else if (char == "y" || char == "g")
					comps |= (i + 1) << 4;
				else if (char == "z" || char == "b")
				{
					if (size < 3)
					{
						throw new Error("Unable to set the '"
										+ char
										+ "' component of a value of size "
										+ size);
						
					}
						
					comps |= (i + 1) << 8;
				}
				else if (char == "w" || char == "a")
				{
					if (size < 4)
					{
						throw new Error("Unable to set the '"
										+ char
										+ "' component of a value of size "
										+ size);
						
					}

					comps |= (i + 1) << 12;
				}
			}
			
			var result	: INode = null;
			
			for (i = 0; i < size; ++i)
			{
				var rhc : int 	= ((comps >>> (i << 2)) & 0xf) - 1;
				var ext : INode = null;
				
				if (rhc >= 0)
					ext = new Extract(val, rhc | 0x4440);
				else
					ext = new Extract(_node, i | 0x4440);					
				
				result = i == 0 ? ext : new Combine(result, ext);
			}
			
			_node = result;
		}
		
		private function getComponentsFromString(componentsString : String) : uint
		{
			var components 		: uint 	= 0;
			var strlen			: int	= componentsString.length;
			var size			: uint	= _node.size;
			
			if (strlen > 4)
				throw new Error("'" + componentsString + "' is not a valid component.");
			
			componentsString = componentsString.toLowerCase();
			
			for (var i : int = 0; i < 4; ++i)
			{
				if (i < strlen)
				{
					var c : String = componentsString.charAt(i);
					
					if (c == "x" || c == "r")
					{
						components |= 0 << (i << 2);
					}
					else if (size >= 2 && (c == "y" || c == "g"))
					{
						components |= 1 << (i << 2);
					}
					else if (size >= 3 && (c == "z" || c == "b"))
					{
						components |= 2 << (i << 2);
					}
					else if (size == 4 && (c == "w" || c == "a"))
					{
						components |= 3 << (i << 2);
					}
					else
					{
						throw new Error("Unkown component '"
										+ c + "' on a value with size "
										+ size + ".");
					}
				}
				else
				{
					components |= 4 << (i << 2);
				}
			}
			
			return components;
		}
		
		private function getNode(value : Object) : INode
		{
			if (value is INode)
				return value as INode;
			
			if (value is SValue)
				return (value as SValue)._node;
			
			var c	: Constant	= new Constant();
			
			if (value is int || value is Number)
			{
				c.constants[0] = value as Number;
			}
			else if (value is Point)
			{
				var point	: Point	= value as Point;
				
				c.constants[0] = point.x;
				c.constants[1] = point.y;
			}
			else if (value is Vector4)
			{
				var vector 	: Vector4 	= value as Vector4;
				
				c.constants[0] = vector.x;
				c.constants[1] = vector.y;
				c.constants[2] = vector.z;
				
				if (!isNaN(vector.w))
					c.constants[3] = vector.w;
			}
			else if (value is Matrix4x4)
			{
				(value as Matrix4x4).getRawData(c.constants);
			}
			
			if (!c)
				throw new Error("Constants can only be int, uint, Number, Point, Vector4 or Matrix4x4 values.");
			
			return c;
		}
	}
}