package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.accessor.EvalExpAccessor;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Attribute;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Parameter;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Sampler;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Extract;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Interpolate;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;
	import aerys.minko.render.shader.compiler.register.Components;
	
	public class RemoveUselessComputation extends AbstractVisitor
	{
		public function RemoveUselessComputation()
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
		}
		
		override protected function visitInstruction(instruction	: Instruction, 
													 isVertexShader	: Boolean) : void
		{
			visit(instruction.arg1, isVertexShader);
			if (!instruction.isSingle)
				visit(instruction.arg2, isVertexShader);
			
			var i : uint;
			var vec : Vector.<Number>;
			
			var arg1Value : Number = NaN;
			var arg2Value : Number = NaN;
			
			if (instruction.arg1 is Constant)
			{
				vec = Constant(instruction.arg1).value;
				arg1Value = vec[0];
				
				 for (i = 1; i < vec.length; ++i)
					 if (vec[i] != arg1Value)
						 break;
				 
				 if (i != vec.length)
					 arg1Value = NaN;
			}
			
			if (instruction.arg2 is Constant)
			{
				vec = Constant(instruction.arg2).value;
				arg2Value = vec[0];
				
				for (i = 1; i < vec.length; ++i)
					if (vec[i] != arg2Value)
						break;
				
				if (i != vec.length)
					arg2Value = NaN;
			}
			
			switch (instruction.id)
			{
				case Instruction.ADD:
					if (arg1Value == 0)
					{
						replaceInParentAndSwizzle(instruction, instruction.arg2, instruction.arg2Components);
						return;
					}
					if (arg2Value == 0)
					{
						replaceInParentAndSwizzle(instruction, instruction.arg1, instruction.arg1Components);
						return;
					}
					break;
				
				case Instruction.MUL:
					if (arg1Value == 1)
					{
						replaceInParentAndSwizzle(instruction, instruction.arg2, instruction.arg2Components);
						return;
					}
					if (arg2Value == 1)
					{
						replaceInParentAndSwizzle(instruction, instruction.arg1, instruction.arg1Components);
						return;
					}
					break;
				
			}
		}
		
		override protected function visitOverwriter(overwriter	   : Overwriter, 
													isVertexShader : Boolean) : void
		{
			trace(overwriter.args);
			for each (var arg : INode in overwriter.args)
				visit(arg, isVertexShader);
		}
		
		override protected function visitAttribute(attribute	  : Attribute, 
												   isVertexShader : Boolean) : void
		{
		}
		
		override protected function visitConstant(constant		 : Constant, 
												  isVertexShader : Boolean) : void
		{
		}
		
		override protected function visitParameter(parameter	  : Parameter, 
												   isVertexShader : Boolean) : void
		{
			if (parameter.accessor is EvalExpAccessor)
				visit(EvalExpAccessor(parameter.accessor).tree, false);
		}
		
		override protected function visitSampler(sampler		: Sampler, 
												 isVertexShader	: Boolean) : void
		{
		}
		
		override protected function visitExtract(extract		: Extract, 
												 isVertexShader	: Boolean) : void
		{
			throw new Error('Found invalid node: ' + extract.toString());
		}
		
	}
}