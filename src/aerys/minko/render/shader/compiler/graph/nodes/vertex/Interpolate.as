package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.ANode;
	import aerys.minko.render.shader.compiler.register.Components;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 */
	public class Interpolate extends ANode
	{
		public function get argument() : ANode
		{
			return getArgumentAt(0);
		}
		
		public function get component() : uint
		{
			return getComponentAt(0);
		}
		
		public function Interpolate(arg : ANode)
		{
			var arguments	: Vector.<ANode> = new <ANode>[arg];
			var components	: Vector.<uint>  = new <uint>[
				Components.createContinuous(0, 0, arg.size, arg.size)
			];
			
			arguments.fixed = components.fixed = true;
			
			super(arguments, components);
		}
		
		override protected function computeHash() : uint
		{
			var argument	: ANode		= getArgumentAt(0);
			var components	: uint		= getComponentAt(0);
			var hashString	: String	=
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
	}
}
