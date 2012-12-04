package aerys.minko.render.shader
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Extract;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;

	/**
	 * SFloat (Shader Float) objects are GPU-side computed value proxies
	 * declared, defined and used in ActionScript shaders.
	 *
	 * <p>
	 * ActionScript shaders define what operations will be
	 * performed on the GPU. Those operations take arguments and return
	 * values that will be computed and accessible on the graphics hardware
	 * only. Those values are represented by SFloat objects.
	 * </p>
	 *
	 * <p>
	 * Because SFloat objects are just hardware memory proxies, it is not
	 * possible (and does not make sense to try) to read their actual value
	 * using CPU-side code. For the very same reason, most of the errors will
	 * be detected at runtime only. The only available property is the size
	 * (number of components) of the corresponding value (ie. 3D vector
	 * operations will return SFloat objects of size 3, dot-product will
	 * return a scalar SFloat object of size 1, ...).
	 * </p>
	 *
	 * <p>
	 * SFloat objects also provide OOP shader programming by encapsulating
	 * common operations (add, multiply, ...). They also allow the use of
	 * dynamic properties in order to read or write sub-components of
	 * non-scalar values. Example:
	 * </p>
	 *
	 * <pre>
	 * public function getOutputColor() : void
	 * {
	 * 	var diffuse : SFloat = sampleTexture(BasicStyle.DIFFUSE_MAP);
	 *
	 * 	// use the RGB components of the diffuse map but use a
	 * 	// fixed alpha = 0.5
	 *  return combine(diffuse.rgb, 0.5);
	 * }
	 * </pre>
	 *
	 * <p>
	 * Each SFloat object wraps a shader graph node that will be evaluated
	 * by the compiler in order to create the corresponding AGAL bytecode.
	 * </p>
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public dynamic final class SFloat extends Proxy
	{
		use namespace minko_shader;
		
		minko_shader var _node	: AbstractNode	= null;
		
		public final function get size() : uint
		{
			return _node.size;
		}
		
		public function SFloat(value : Object)
		{
			_node = getNode(value);
		}

		public final function scaleBy(arg : Object) : SFloat
		{
			_node = new Instruction(Instruction.MUL, _node, getNode(arg));

			return this;
		}

		public final function incrementBy(value : Object) : SFloat
		{
			_node = new Instruction(Instruction.ADD, _node, getNode(value));

			return this;
		}

		public final function decrementBy(value : Object) : SFloat
		{
			_node = new Instruction(Instruction.SUB, _node, getNode(value));

			return this;
		}

		public final function normalize() : SFloat
		{
			_node = new Instruction(Instruction.NRM, _node);

			return this;
		}

		public final function negate() : SFloat
		{
			_node = new Instruction(Instruction.NEG, _node);

			return this;
		}

		override flash_proxy function getProperty(name : *) : *
		{
			return new SFloat(new Extract(_node, Components.stringToComponent(name)));
		}

		override flash_proxy function setProperty(name : *, value : *) : void
		{
			var propertyName		: String		= String(name);
			var propertyComponent	: uint			= getPropertyWriteComponentMask(propertyName);
			var propertyNode		: AbstractNode	= getNode(value);
			var nodeComponent		: uint 			= Components.createContinuous(0, 0, _node.size, _node.size);
			
			propertyNode = new Overwriter(
				new <AbstractNode>[propertyNode],
				new <uint>[Components.createContinuous(0, 0, 4, propertyNode.size)]
			);
			
			_node = new Overwriter(
				new <AbstractNode>[_node, propertyNode],
				new <uint>[nodeComponent, propertyComponent]
			);
		}
		
		public static function getPropertyWriteComponentMask(string : String) : uint
		{
			var result : uint = 0x04040404;
			
			for (var i : uint = 0; i < 4; ++i)
				if (i < string.length)
					switch (string.charAt(i))
					{
						case 'x': case 'X': case 'r': case 'R':
							result = (0xffffff00 & result) | i;
							result |= i << (8 * 0);
							break;
						
						case 'y': case 'Y': case 'g': case 'G':
							result = (0xffff00ff & result) | i << 8;
							break;
						
						case 'z': case 'Z': case 'b': case 'B':
							result = (0xff00ffff & result) | i << 16;
							break;
						
						case 'w': case 'W': case 'a': case 'A':
							result = (0x00ffffff & result) | i << 24;	
							break;
						
						default:
							throw new Error('Invalid string.');
					}
			
			return result;
		}

		private function getNode(value : Object) : AbstractNode
		{
			if (value is AbstractNode)
				return value as AbstractNode;

			if (value is SFloat)
				return (value as SFloat)._node;
			
			if (value is uint || value is int || value is Number)
				return new Constant(new <Number>[Number(value)]);
			
			if (value is Matrix4x4)
				return new Constant(Matrix4x4(value).getRawData(null, 0, false));
			
			throw new Error('This type cannot be casted to a shader value.');
		}
	}
}
