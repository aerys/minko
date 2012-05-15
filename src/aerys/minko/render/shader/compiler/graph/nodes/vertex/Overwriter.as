package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.register.Components;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class Overwriter extends AbstractNode
	{
		public function Overwriter(args			: Vector.<AbstractNode>,
								   components	: Vector.<uint>)
		{
			super(args, components);
		}
		
		override protected function computeHash() : uint
		{
			var numArgs : uint		= this.numArguments;
			var hash	: String	= 'Overwriter';
			
			for (var argId : uint = 0; argId < numArgs; ++argId)
			{
				var argument	: AbstractNode	= getArgumentAt(argId);
				var component	: uint			= getComponentAt(argId);
				
				hash += argument.hash.toString() + component.toString();
			}
			
			return CRC32.computeForString(hash);
		}
		
		override protected function computeSize() : uint
		{
			var numArgs		: uint		= this.numArguments;
			var xDefined	: Boolean	= false;
			var yDefined	: Boolean	= false;
			var zDefined	: Boolean	= false;
			var wDefined	: Boolean	= false;
			
			for (var argId : uint = 0; argId < numArgs; ++argId)
			{
				var component : uint = getComponentAt(argId);
				xDefined ||= Components.getReadAtIndex(0, component) != 4;
				yDefined ||= Components.getReadAtIndex(1, component) != 4;
				zDefined ||= Components.getReadAtIndex(2, component) != 4;
				wDefined ||= Components.getReadAtIndex(3, component) != 4;
			}
			
			if (xDefined && !yDefined && !zDefined && !wDefined)
				return 1;
			else if (xDefined && yDefined && !zDefined && !wDefined)
				return 2;
			else if (xDefined && yDefined && zDefined && !wDefined)
				return 3;
			else if (xDefined && yDefined && zDefined && wDefined)
				return 4;
			else
				throw new Error('Components are invalid. Cannot compute size');
		}
		
		override public function toString() : String
		{
			return 'Overwriter';
		}
		
		override public function clone() : AbstractNode
		{
			var numArgs 	: uint					= this.numArguments;
			var arguments	: Vector.<AbstractNode>	= new Vector.<AbstractNode>(numArgs);
			var components	: Vector.<uint> 		= new Vector.<uint>(numArgs);
			
			
			for (var argId : uint = 0; argId < numArgs; ++argId)
			{
				arguments[argId]	= getArgumentAt(argId);
				components[argId]	= getComponentAt(argId);
			}
			
			return new Overwriter(arguments, components);
		}
	}
}
