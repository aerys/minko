package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Attribute;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Parameter;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Sampler;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Extract;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Interpolate;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;

	public class CopyInserterVisitor extends AbstractVisitor
	{
		public function CopyInserterVisitor()
		{
			super(true);
		}
		
		override protected function start() : void
		{
		}
		
		override protected function finish() : void
		{
		}
		
		override protected function visitInstruction(instruction	: Instruction,
													 isVertexShader	: Boolean) : void
		{
			visit(instruction.arg1, isVertexShader);
			
			if (!instruction.isSingle)
			{
				visit(instruction.arg2, isVertexShader);
			}
			
			if ((instruction.arg1 is Constant || instruction.arg1 is Parameter) && 
				(instruction.isSingle || (instruction.arg2 is Constant || instruction.arg2 is Parameter)))
			{
				instruction.arg1 = new Instruction(Instruction.MOV, instruction.arg1);
			}
		}
		
		override protected function visitInterpolate(interpolate	: Interpolate,
													 isVertexShader	: Boolean) : void
		{
			visit(interpolate.arg, true);
			
			if (interpolate.arg is Constant || interpolate.arg is Parameter)
				interpolate.arg = new Instruction(Instruction.MOV, interpolate.arg);
		}
		
		override protected function visitOverwriter(overwriter		: Overwriter,
													isVertexShader	: Boolean) : void
		{
			var args	: Vector.<INode>	= overwriter.args;
			var numArgs : uint				= args.length;
			
			for (var argId : uint = 0; argId < numArgs; ++argId)
				visit(args[argId], isVertexShader);
		}
		
		override protected function visitAttribute(attribute		: Attribute,
												   isVertexShader	: Boolean) : void
		{
		}
		
		override protected function visitConstant(constant			: Constant,
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
		
		override protected function visitExtract(extract		: Extract,
												 isVertexShader	: Boolean) : void
		{
			throw new Error('Extracts cannot be found at this stage of compilation.');
		}
		
	}
}