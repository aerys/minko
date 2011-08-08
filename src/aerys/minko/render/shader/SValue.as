package aerys.minko.render.shader
{
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.builtin.Add;
	import aerys.minko.render.shader.node.operation.builtin.Divide;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct3;
	import aerys.minko.render.shader.node.operation.builtin.DotProduct4;
	import aerys.minko.render.shader.node.operation.builtin.Fractional;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.builtin.Multiply4x4;
	import aerys.minko.render.shader.node.operation.builtin.Negate;
	import aerys.minko.render.shader.node.operation.builtin.Normalize;
	import aerys.minko.render.shader.node.operation.builtin.Power;
	import aerys.minko.render.shader.node.operation.builtin.SetIfGreaterEqual;
	import aerys.minko.render.shader.node.operation.builtin.SetIfLessThan;
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

	/**
	 * SValue (Shader Value) objects are GPU-side computed value proxies
	 * declared, defined and used in ActionScript shaders.
	 * 
	 * <p>
	 * ActionScript shaders define what operations will be
	 * performed on the GPU. Those operations take arguments and return
	 * values that will be computed and accessible on the graphics hardware
	 * only. Those values are represented by SValue objects.
	 * </p>
	 * 
	 * <p>
	 * Because SValue objects are just hardware memory proxies, it is not
	 * possible (and does not make sense to try) to read their actual value
	 * using CPU-side code. For the very same reason, most of the errors will
	 * be detected at runtime only. The only available property is the size
	 * (number of components) of the corresponding value (ie. 3D vector
	 * operations will return SValue objects of size 3, cross-product will
	 * return a scalar SValue object of size 1, ...).
	 * </p>
	 * 
	 * <p>
	 * SValue objects also provide OOP shader programming by encapsulating
	 * common operations (add, multiply, ...). They also allow the use of
	 * dynamic properties in order to read or write sub-components of
	 * non-scalar values. Example:
	 * </p>
	 * 
	 * <pre>
	 * public function getOutputColor() : void
	 * {
	 * 	var diffuse : SValue = sampleTexture(BasicStyle.DIFFUSE_MAP);
	 * 	
	 * 	// use the RGB components of the diffuse map but use a
	 * 	// fixed alpha = 0.5
	 *  return combine(diffuse.rgb, 0.5);
	 * }
	 * </pre>
	 * 
	 * <p>
	 * Each SValue object wraps a shader graph node that will be evaluated
	 * by the compiler in order to create the corresponding AGAL bytecode.
	 * </p>
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
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
		
		public final function multiply(arg : Object, ...args) : SValue
		{
			var p 		: Product 	= new Product(_node, getNode(arg));
			var numArgs : int 		= args.length;
			
			for (var i : int = 0; i < numArgs; ++i)
				p.addTerm(args[i]._node);
			
			_node = p;
			
			return this;
		}
		
		public final function scaleBy(arg : Object) : SValue
		{
			_node = new Multiply(_node, getNode(arg));
			
			return this;
		}
		
		public final function divide(arg : Object) : SValue
		{
			return new SValue(new Divide(_node, getNode(arg)));
		}
		
		public final function modulo(base : Object) : SValue
		{
			var baseNode : INode = getNode(base);
			
			return new SValue(new Multiply(
				baseNode,
				new Fractional(new Divide(_node, baseNode)))
			);
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
		
		public final function negate() : SValue
		{
			_node = new Negate(_node);
			
			return this;
		}
		
		public final function setIfGreaterEqual(test : Object, value : Object) : SValue
		{
			_node = new SetIfGreaterEqual(getNode(test), getNode(value));
			
			return this;
		}
		
		public final function setIfLessThan(test : Object, value : Object) : SValue
		{
			_node = new SetIfLessThan(getNode(test), getNode(value));
			
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
				throw new Error("Unable to set component(s) of a value with size == 1.");
			
			for (var i : int = 0; i < size; ++i)
			{
				var char : String = str.charAt(i);
				
				if (char == "x" || char == "r")
					comps |= i + 1;
				else if (char == "y" || char == "g")
					comps |= (i + 1) << 4;
				else if (size >= 3 && (char == "z" || char == "b"))
					comps |= (i + 1) << 8;
				else if (size == 4 && (char == "w" || char == "a"))
					comps |= (i + 1) << 12;
				else if (char != "")
				{
					throw new Error("Unable to set the '"
									+ char
									+ "' component of a value of size "
									+ size);
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
			var components 	: uint 	= 0;
			var strlen		: int	= componentsString.length;
			var size		: uint	= _node.size;
			
			if (strlen > 4)
				throw new Error("'" + componentsString + "' is not a valid component.");
			
			componentsString = componentsString.toLowerCase();
			
			for (var i : int = 0; i < 4; ++i)
			{
				if (i < strlen)
				{
					var c : String = componentsString.charAt(i);
					
					if (c == "x" || c == "r")
						components |= 0 << (i << 2);
					else if (size >= 2 && (c == "y" || c == "g"))
						components |= 1 << (i << 2);
					else if (size >= 3 && (c == "z" || c == "b"))
						components |= 2 << (i << 2);
					else if (size == 4 && (c == "w" || c == "a"))
						components |= 3 << (i << 2);
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
			
			return new Constant(value);
		}
	}
}