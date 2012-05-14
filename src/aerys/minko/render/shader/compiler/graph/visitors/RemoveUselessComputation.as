package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Attribute;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableConstant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableSampler;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Sampler;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Extract;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Interpolate;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.VariadicExtract;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class RemoveUselessComputation extends AbstractVisitor
	{
		public function RemoveUselessComputation()
		{
		}
		
		override protected function visitTraversable(node:AbstractNode, isVertexShader:Boolean):void
		{
			visitArguments(node, true);
			
			if (node is Instruction)
				visitInstruction(Instruction(node), true);
		}
		
		override protected function visitNonTraversable(node:AbstractNode, isVertexShader:Boolean):void
		{
		}
		
		override protected function visitInstruction(instruction	: Instruction, 
													 isVertexShader	: Boolean) : void
		{
			var instructionId : uint = instruction.id;
			
			if (instructionId != Instruction.ADD 
				&& instructionId != Instruction.SUB 
				&& instructionId != Instruction.MUL 
				&& instructionId != Instruction.DIV)
				return;
			
			var arg1Value : Number = computeArgumentValue(instruction.argument1);
			var arg2Value : Number = computeArgumentValue(instruction.argument2);
			
			switch (instructionId)
			{
				case Instruction.ADD:
					if (arg1Value == 0)
						replaceInParentsAndSwizzle(instruction, instruction.argument2, instruction.component2);
					else if (arg2Value == 0)
						replaceInParentsAndSwizzle(instruction, instruction.argument1, instruction.component1);
					
					break;
				
				case Instruction.MUL:
					if (arg1Value == 1 || arg2Value == 0)
						replaceInParentsAndSwizzle(instruction, instruction.argument2, instruction.component2);
					else if (arg1Value == 0 || arg2Value == 1)
						replaceInParentsAndSwizzle(instruction, instruction.argument1, instruction.component1);
					
					break;
				
				case Instruction.DIV:
					if (arg2Value == 1)
						replaceInParentsAndSwizzle(instruction, instruction.argument1, instruction.component1);
					
					break;
				
				case Instruction.SUB:
					if (arg1Value == 0)
					{
						var negate : Instruction = new Instruction(Instruction.NEG, instruction.argument1);
						negate.component1 = instruction.component1;
						replaceInParents(instruction, negate);
					}
					else if (arg2Value == 0)
						replaceInParentsAndSwizzle(instruction, instruction.argument1, instruction.component1);
					
					break;
			}
		}
		
		private function computeArgumentValue(node : AbstractNode) : Number
		{
			if (!(node is Constant))
				return NaN;
			
			var vec			: Vector.<Number>	= Constant(node).value;
			var argValue	: Number			= vec[0];
			var length		: uint				= vec.length;
			
			for (var i : uint = 1; i < length; ++i)
				if (vec[i] != argValue)
					return NaN;
			
			return argValue;
		}
		
	}
}