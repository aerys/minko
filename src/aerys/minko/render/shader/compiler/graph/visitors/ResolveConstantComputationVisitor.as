package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.Evaluator;
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Attribute;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Parameter;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Sampler;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Extract;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Interpolate;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;

	public class ResolveConstantComputationVisitor extends AbstractVisitor
	{
		public function ResolveConstantComputationVisitor()
		{
			super(false);
		}
		
		override protected function start() : void
		{
		}

		override protected function finish() : void
		{
		}
		
		override protected function visitInterpolate(interpolate	: Interpolate,
													 isVertexShader	: Boolean) : void
		{
			visit(interpolate.arg, true);
			
			if (interpolate.arg is Constant)
			{
				replaceInParentAndSwizzle(interpolate, interpolate.arg, interpolate.components);
				forgetVisiting(interpolate, isVertexShader);
			}
		}
		
		override protected function visitInstruction(instruction	: Instruction,
													 isVertexShader	: Boolean) : void
		{
			var isSingle : Boolean = instruction.isSingle;
			
			visit(instruction.arg1, isVertexShader);
			if (!isSingle)
				visit(instruction.arg2, isVertexShader);
			
			var arg1AsConstant		: Constant = instruction.arg1 as Constant;
			var arg2AsConstant		: Constant = instruction.arg2 as Constant;
			
			var result				: Vector.<Number> = null;
			var orderedConstant1	: Vector.<Number>;
			var orderedConstant2	: Vector.<Number>;
			
			if (isSingle && arg1AsConstant != null)
			{
				orderedConstant1	= Evaluator.evaluateComponents(instruction.arg1Components, arg1AsConstant.value);
				result				= Evaluator.EVALUTION_FUNCTIONS[instruction.id](orderedConstant1);
				
				replaceInParent(instruction, new Constant(result, result.length));
//				forgetVisiting(instruction, isVertexShader);
			}
			else if (!isSingle && arg1AsConstant != null && arg2AsConstant != null)
			{
				orderedConstant1	= Evaluator.evaluateComponents(instruction.arg1Components, arg1AsConstant.value);
				orderedConstant2	= Evaluator.evaluateComponents(instruction.arg2Components, arg2AsConstant.value);
				result				= Evaluator.EVALUTION_FUNCTIONS[instruction.id](orderedConstant1, orderedConstant2);
				
				replaceInParent(instruction, new Constant(result, result.length));
//				forgetVisiting(instruction, isVertexShader);
			}
		}
		
		override protected function visitOverwriter(overwriter		: Overwriter, 
													isVertexShader	: Boolean) : void
		{
			
			var args		: Vector.<INode> = overwriter.args;
			var numArgs		: uint			 = args.length;
			var components	: Vector.<uint>	 = overwriter.components;
			var argId		: uint;
			
			// start by visiting sons
			for (argId = 0; argId < numArgs; ++argId)
				visit(args[argId], isVertexShader);
			
			// find how many constants there are at the beginning of this overwriter.
			var argLimit : uint = 0;
			
			while (argLimit < numArgs && args[argLimit] is Constant)
				++argLimit;
			
			// if there are not at least 2 constants, we are done.
			if (argLimit < 2)
				return;
			
			// pack constant arguments into one of size 4
			var packableArgs		: Vector.<INode>	= args.splice(0, argLimit);
			var packableComponents	: Vector.<uint>		= components.splice(0, argLimit);
			numArgs -= argLimit;
			
			var finalConstantData : Vector.<Number> = new Vector.<Number>();
			finalConstantData[0] = NaN;
			finalConstantData[1] = NaN;
			finalConstantData[2] = NaN;
			finalConstantData[3] = NaN;
			
			for (argId = 0; argId < argLimit; ++argId)
			{
				var constant	: Constant	= Constant(packableArgs[argId]);
				var component	: uint		= packableComponents[argId];
				
				for (var writeIndex : uint = 0; writeIndex < 4; ++writeIndex)
				{
					var readIndex : uint = Components.getReadAtIndex(writeIndex, component);
					if (readIndex != Components._)
						finalConstantData[writeIndex] = constant.value[readIndex];
				}
			}
			
			// pack the size 4 constant argument into the smallest possible value
			// (don't manage duplicates: the constant compressor will do it).
			var finalComponents			: uint = 0;
			var currentWriteIndex		: uint = 0;
			var realFinalConstantData	: Vector.<Number> = new Vector.<Number>();
			for (writeIndex = 0; writeIndex < 4; ++writeIndex)
			{
				if (isNaN(finalConstantData[writeIndex]))
				{
					finalComponents |= Components._ << (8 * writeIndex);
				}
				else
				{
					finalComponents |= (currentWriteIndex++) << (8 * writeIndex);
					realFinalConstantData.push(finalConstantData[writeIndex]);
				}
			}
			
			// unshift the constant argument and component in the overwriter
			args.unshift(new Constant(realFinalConstantData, realFinalConstantData.length));
			components.unshift(finalComponents);
			++numArgs;
			
			// remove the whole node if we reduced numArgs to 1
			if (numArgs == 1)
			{
				replaceInParentAndSwizzle(overwriter, args[0], components[0]);
//				forgetVisiting(overwriter, isVertexShader);
			}
			
			// tell the overwriter it has been updated
			overwriter.invalidateHashAndSize();
		}
		
		override protected function visitAttribute(attribute		: Attribute,
												   isVertexShader	: Boolean) : void
		{
		}
		
		override protected function visitConstant(constant			: Constant,
												  isVertexShader	: Boolean) : void
		{
		}
		
		override protected function visitExtract(extract		: Extract,
												 isVertexShader	: Boolean) : void
		{
		}
		
		override protected function visitParameter(parameter		: Parameter,
												   isVertexShader	: Boolean) : void
		{
		}
		
		override protected function visitSampler(sampler		: Sampler,
												 isVertexShader	: Boolean) : void
		{
		}

	}
}