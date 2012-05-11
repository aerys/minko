package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.register.Components;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 */
	public class Interpolate extends AbstractNode
	{
		public function get argument() : AbstractNode
		{
			return getArgumentAt(0);
		}
		
		public function set argument(v : AbstractNode) : void
		{
			setArgumentAt(0, v);
		}
		
		public function get component() : uint
		{
			return getComponentAt(0);
		}
		
		public function set component(v : uint) : void
		{
			setComponentAt(0, v);
		}
		
		public function Interpolate(arg : AbstractNode)
		{
			var arguments	: Vector.<AbstractNode>	= new <AbstractNode>[arg];
			var components	: Vector.<uint>			= new <uint>[
				Components.createContinuous(0, 0, arg.size, arg.size)
			];
			
			arguments.fixed = components.fixed = true;
			
			super(arguments, components);
		}
		
		override protected function computeHash() : uint
		{
			var argument	: AbstractNode	= getArgumentAt(0);
			var components	: uint			= getComponentAt(0);
			var hashString	: String		=
				'Interpolate' + components.toString() + argument.hash.toString();
			
			return CRC32.computeForString(hashString);
		}
		
		override protected function computeSize() : uint
		{
			var components : uint = getComponentAt(0);
			
			return Components.getMaxWriteOffset(components) + 1;
		}
		
		override public function toString() : String
		{
			return 'Interpolate';
		}
		
		override public function clone() : AbstractNode
		{
			var clone : Interpolate = new Interpolate(argument);
			clone.component = component;
			return clone;
		}
	}
}
