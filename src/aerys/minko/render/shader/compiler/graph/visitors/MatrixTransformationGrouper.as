package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.binding.EvalExp;
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
	import aerys.minko.type.math.Matrix4x4;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class MatrixTransformationGrouper extends AbstractVisitor
	{
		private var _computableMatrixId : int;
		
		public function MatrixTransformationGrouper()
		{
		}
		
		override protected function start() : void
		{
			super.start();
			
			_computableMatrixId = 0;
		}
		
		override protected function visitTraversable(node:AbstractNode, isVertexShader:Boolean):void
		{
			if (node is Instruction)
				visitInstruction(Instruction(node), isVertexShader);
			else if (node is Interpolate)
				visitArguments(node, true);
			else
				visitArguments(node, isVertexShader);
		}
		
		override protected function visitInstruction(instruction : Instruction, isVertexShader : Boolean) : void
		{
			var currentArg	: AbstractNode	= instruction;
			var matrices	: AbstractNode	= null;
			var numMerged	: uint			= 0;
			
			if (instruction.id == Instruction.M44)
				while (currentArg is Instruction && Instruction(currentArg).id == Instruction.M44)
				{
					var curM44 : Instruction = Instruction(currentArg);
					
					matrices	= matrices == null ? curM44.argument2 : new Instruction(Instruction.MUL_MAT44, matrices, curM44.argument2);
					currentArg	= Instruction(currentArg).argument1;
					++numMerged;
				}
			
			else if (instruction.id == Instruction.M33)
				while (currentArg is Instruction && Instruction(currentArg).id == Instruction.M44)
				{
					var curM33 : Instruction = Instruction(currentArg);
					
					matrices	= matrices == null ? curM33.argument2 : new Instruction(Instruction.MUL_MAT33, matrices, curM33.argument2);
					currentArg	= Instruction(currentArg).argument1;
					++numMerged;
				}
			
			if (numMerged > 1)
			{
				var constantName : String = 'computableMatrix' + (_computableMatrixId++);
				_shaderGraph.computableConstants[constantName] = matrices;
				
				var evalExp			: EvalExp			= new EvalExp(matrices);
				var bindedConstant	: BindableConstant	= new BindableConstant(constantName, 16);
				instruction.argument1 = currentArg;
				instruction.argument2 = bindedConstant;
			}
		}
		
		override protected function visitNonTraversable(node : AbstractNode, isVertexShader : Boolean):void
		{
		}
	}
}