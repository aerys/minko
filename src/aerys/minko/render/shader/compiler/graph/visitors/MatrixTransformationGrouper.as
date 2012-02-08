package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.binding.EvalExp;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
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
	import aerys.minko.type.math.Matrix4x4;

	public class MatrixTransformationGrouper extends AbstractVisitor
	{
		private var _computableMatrixId : int;
		
		public function MatrixTransformationGrouper()
		{
			super(true);
		}
		
		override protected function start() : void
		{
			super.start();
			
			_computableMatrixId = 0;
		}
		
		override protected function finish() : void
		{
		}
		
		override protected function visitInstruction(instruction : Instruction, isVertexShader : Boolean) : void
		{
			var currentArg	: INode = instruction;
			var matrices	: INode = null;
			var numMerged	: uint	= 0;
			
			if (instruction.id == Instruction.M44)
				while (currentArg is Instruction && Instruction(currentArg).id == Instruction.M44)
				{
					var curM44 : Instruction = Instruction(currentArg);
					
					matrices	= matrices == null ? curM44.arg2 : new Instruction(Instruction.MUL_MAT44, matrices, curM44.arg2);
					currentArg	= Instruction(currentArg).arg1;
					++numMerged;
				}
			
			else if (instruction.id == Instruction.M33)
				while (currentArg is Instruction && Instruction(currentArg).id == Instruction.M44)
				{
					var curM33 : Instruction = Instruction(currentArg);
					
					matrices	= matrices == null ? curM33.arg2 : new Instruction(Instruction.MUL_MAT33, curM33.arg2);
					currentArg	= Instruction(currentArg).arg1;
					++numMerged;
				}
			
			if (numMerged > 1)
			{
				var constantName : String = 'computableMatrix' + (_computableMatrixId++);
				_shaderGraph.computableConstants[constantName] = matrices;
				
				var evalExp			: EvalExp			= new EvalExp(matrices);
				var bindedConstant	: BindableConstant	= new BindableConstant(constantName, 16);
				instruction.arg1 = currentArg;
				instruction.arg2 = bindedConstant;
			}
		}
		
		override protected function visitOverwriter(overwriter : Overwriter, isVertexShader : Boolean) : void
		{
			for each (var arg : INode in overwriter.args)
				visit(arg, isVertexShader);
		}
		
		override protected function visitInterpolate(interpolate : Interpolate, isVertexShader : Boolean) : void
		{
			visit(interpolate.arg, isVertexShader);
		}
		
		override protected function visitVariadicExtract(variadicExtract : VariadicExtract, isVertexShader : Boolean) : void
		{
			if (!isVertexShader)
				throw new Error('Indirect adressing is only available on the vertex shader.');
			
			visit(variadicExtract.constant, true);
			visit(variadicExtract.index, true);
		}
		
		override protected function visitAttribute(attribute : Attribute, isVertexShader : Boolean) : void
		{
		}
		
		override protected function visitBindableConstant(bindableConstant : BindableConstant, isVertexShader : Boolean) : void
		{
		}
		
		override protected function visitBindableSampler(bindableSampler : BindableSampler, isVertexShader : Boolean) : void
		{
		}
		
		override protected function visitConstant(constant : Constant, isVertexShader : Boolean) : void
		{
		}
		
		override protected function visitExtract(extract : Extract, isVertexShader : Boolean) : void
		{
			throw new Error('No extract nodes should be reachable at this point of compilation');
		}
		
		override protected function visitSampler(sampler : Sampler, isVertexShader : Boolean) : void
		{
		}
	}
}