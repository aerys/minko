package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.ANode;
	import aerys.minko.render.shader.compiler.register.Components;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class Extract extends ANode
	{
		public function get argument() : ANode
		{
			return getArgumentAt(0);
		}
		
		public function get component() : uint
		{
			return getComponentAt(0);
		}
		
		public function Extract(child		: ANode,
								component	: uint)
		{
			var arguments	: Vector.<ANode> = new <ANode>[child];
			var components	: Vector.<uint>	 = new <uint>[component];
			
			arguments.fixed = components.fixed = true;
			
			super(arguments, components);
			
			if (Components.hasHoles(component))
				throw new Error(
					'Extract should be used only with continuous components. '
					+ 'Components with holes are reserved to the Overwriter node.'
				);
		}
		
		override public function toString() : String
		{
			return 'Extract';
		}
		
		override protected function computeHash() : uint
		{
			var child		: ANode	= getArgumentAt(0);
			var component	: uint	= getComponentAt(0);
			
			return CRC32.computeForString('Extract' + child.hash.toString(16) + component.toString(16));
		}
		
		override protected function computeSize() : uint
		{
			var component : uint = getComponentAt(0);
			
			return Components.getMaxWriteOffset(component) + 1;
		}
		
		override public function clone() : ANode
		{
			return new Extract(argument, component);
		}

	}
}