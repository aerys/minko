package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.Evaluator;
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
	import aerys.minko.render.shader.compiler.register.Components;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class ResolveConstantComputationVisitor extends AbstractVisitor
	{
		public function ResolveConstantComputationVisitor()
		{
		}
		
		override protected function visitInterpolate(interpolate:Interpolate, isVertexShader:Boolean):void
		{
			visit(interpolate.argument, true);
			
			if (interpolate.argument is Constant)
				replaceInParentsAndSwizzle(interpolate, interpolate.argument, interpolate.component);
		}
		
		override protected function visitVariadicExtract(variadicExtract:VariadicExtract, isVertexShader:Boolean):void
		{
			if (!isVertexShader)
				throw new Error('Indirect addressing can only be done in the vertex shader.');
			
			visitArguments(variadicExtract, true);
			
			if (variadicExtract.index is Constant && variadicExtract.constant is Constant)
			{
				var index : Number = Constant(variadicExtract.index).value[variadicExtract.indexComponentSelect];
				
				var result : Vector.<Number> = Evaluator.evaluateVariadicExtract(
					index,
					Constant(variadicExtract.constant).value,
					variadicExtract.isMatrix
				);
				
				replaceInParents(variadicExtract, new Constant(result));
			}
		}
		
		override protected function visitInstruction(instruction:Instruction, isVertexShader:Boolean):void
		{
			visitArguments(instruction, isVertexShader);
			
			var isSingle			: Boolean	= instruction.isSingle;
			var arg1AsConstant		: Constant	= instruction.argument1 as Constant;
			var arg2AsConstant		: Constant	= !instruction.isSingle ? instruction.argument2 as Constant : null;
			
			var result				: Vector.<Number> = null;
			var orderedConstant1	: Vector.<Number>;
			var orderedConstant2	: Vector.<Number>;
			
			if (isSingle && arg1AsConstant != null)
			{
				orderedConstant1	= Evaluator.evaluateComponents(instruction.component1, arg1AsConstant.value);
				result				= Evaluator.EVALUATION_FUNCTIONS[instruction.id](orderedConstant1);
				
				replaceInParents(instruction, new Constant(result));
			}
			else if (!isSingle && arg1AsConstant != null && arg2AsConstant != null)
			{
				orderedConstant1	= Evaluator.evaluateComponents(instruction.component1, arg1AsConstant.value);
				orderedConstant2	= Evaluator.evaluateComponents(instruction.component2, arg2AsConstant.value);
				result				= Evaluator.EVALUATION_FUNCTIONS[instruction.id](orderedConstant1, orderedConstant2);
				
				replaceInParents(instruction, new Constant(result));
			}
		}
		
		override protected function visitOverwriter(overwriter:Overwriter, isVertexShader:Boolean):void
		{
			
			var numArgs		: uint = overwriter.numArguments;
			var argumentId	: uint;
			
			// start by visiting sons
			visitArguments(overwriter, isVertexShader);
			
			// find how many constants there are at the beginning of this overwriter.
			var argLimit : uint = 0;
			
			while (argLimit < numArgs && overwriter.getArgumentAt(argLimit) is Constant)
				++argLimit;
			
			// if there are not at least 2 constants, we are done.
			if (argLimit < 2)
				return;
			
			// pack constant arguments into one of size 4
			numArgs -= argLimit;
			
			var constantData : Vector.<Number> = new <Number>[NaN, NaN, NaN, NaN];
			
			for (argumentId = 0; argumentId < argLimit; ++argumentId)
			{
				var constValue	: Vector.<Number>	= Constant(overwriter.getArgumentAt(argumentId)).value;
				var component	: uint				= overwriter.getComponentAt(argumentId);
				
				for (var writeIndex : uint = 0; writeIndex < 4; ++writeIndex)
				{
					var readIndex : uint = Components.getReadAtIndex(writeIndex, component);
					if (readIndex != 4)
						constantData[writeIndex] = constValue[readIndex];
				}
			}
			
			// pack it
			var packedComponents : uint = packConstantWithHoles(constantData);
			
			// replace all constants arguments, by the new one we juste created
			for (argumentId = 0; argumentId < argLimit; ++argumentId)
				overwriter.removeArgumentAt(0);
			overwriter.addArgumentAt(0, new Constant(constantData), packedComponents);
			
			++numArgs;
			
			// remove the whole node if we reduced numArgs to 1
			if (numArgs == 1)
				replaceInParentsAndSwizzle(overwriter, overwriter.getArgumentAt(0), overwriter.getComponentAt(0));
		}
		
		/**
		 * Pack a contant with some holes, and generate a valid swizzle to read from it.
		 * The modification is done in place.
		 * 
		 * For example:
		 * 		[0, NaN, 4, 3] => ([0, 4, 3], X_YZ)
		 * 		[0, NaN, 4, NaN] => ([0, 4, 3], X_Y_)
		 * 
		 * @param value The constant to pack
		 * @return 
		 */		
		private function packConstantWithHoles(value : Vector.<Number>) : uint
		{
			var packedComponents	: uint = 0;
			var currentWriteIndex	: uint = 0;
			
			for (var writeIndex : uint = 0; writeIndex < 4; ++writeIndex)
			{
				if (isNaN(value[writeIndex]))
				{
					packedComponents |= 4 << (8 * writeIndex);
				}
				else
				{
					packedComponents |= currentWriteIndex << (8 * writeIndex);
					value[currentWriteIndex] = value[writeIndex];
					currentWriteIndex++
				}
			}
			
			value.length = currentWriteIndex;
			
			return packedComponents;
		}
		
		override protected function visitNonTraversable(node:AbstractNode, isVertexShader:Boolean):void
		{
		}
		
	}
}