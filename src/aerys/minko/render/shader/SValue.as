package aerys.minko.render.shader
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Extract;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	
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
	 * operations will return SValue objects of size 3, dot-product will
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
	public dynamic final class SValue extends Proxy
	{
		use namespace minko_shader;
		
		minko_shader var _node	: INode	= null;
		
		public function SValue(value : Object)
		{
			_node = getNode(value);
		}

		public final function multiply(arg : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.MUL, _node, getNode(arg)));
		}

		public final function scaleBy(arg : Object) : SValue
		{
			_node = new Instruction(Instruction.MUL, _node, getNode(arg));

			return this;
		}

		public final function divide(arg : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.DIV, _node, getNode(arg)));
		}

		public final function modulo(base : Object) : SValue
		{
			var baseNode : INode = getNode(base);
			
			return new SValue(
				new Instruction(Instruction.MUL, 
					baseNode,
					new Instruction(Instruction.FRC, 
						new Instruction(Instruction.DIV, _node, baseNode)
					)
				)
			);
		}

		public final function pow(arg : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.POW, _node, getNode(arg)));
		}

		public final function add(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.ADD, _node, getNode(value)));
		}

		public final function incrementBy(value : Object) : SValue
		{
			_node = new Instruction(Instruction.ADD, _node, getNode(value));

			return this;
		}

		public final function subtract(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SUB, _node, getNode(value)));
		}

		public final function decrementBy(value : Object) : SValue
		{
			_node = new Instruction(Instruction.SUB, _node, getNode(value));

			return this;
		}

		public final function dotProduct3(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.DP3, _node, getNode(value)));
		}

		public final function dotProduct4(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.DP4, _node, getNode(value)));
		}

		public final function multiply4x4(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.M44, _node, getNode(value)));
		}

		public final function normalize() : SValue
		{
			_node = new Instruction(Instruction.NRM, _node);

			return this;
		}

		public final function negate() : SValue
		{
			_node = new Instruction(Instruction.NEG, _node);

			return this;
		}

		public final function greaterEqual(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SGE, _node, getNode(value)));
		}
		
		public final function lessThan(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SLT, _node, getNode(value)));
		}
		
		public final function equal(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SEQ, _node, getNode(value)));
		}
		
		public final function notEqual(value : Object) : SValue
		{
			return new SValue(new Instruction(Instruction.SNE, _node, getNode(value)));
		}

		override flash_proxy function getProperty(name : *) : *
		{
			return new SValue(new Extract(_node, Components.stringToComponent(name)));
		}

		override flash_proxy function setProperty(name : *, value : *) : void
		{
			throw new Error('implement me, it should be easy with an overwriter');
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
