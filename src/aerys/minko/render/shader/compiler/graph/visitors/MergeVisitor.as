package aerys.minko.render.shader.compiler.graph.visitors
{
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
	
	import flash.utils.Dictionary;

	public class MergeVisitor extends AbstractVisitor
	{
		private var _hashsToNodes	: Dictionary;
		private var _visitMethods	: Dictionary;
		
		public function MergeVisitor()
		{
			super(false);
		}
		
		override protected function start() : void
		{
			_hashsToNodes = new Dictionary();
		}
		
		override protected function finish() : void
		{
		}
		
		override protected function visitInstruction(instruction	: Instruction,
													 isVertexShader	: Boolean) : void
		{
			var hash			: uint	= instruction.hash;
			var instructionEq	: INode	= _hashsToNodes[hash];
			
			if (instructionEq)
				replaceInParent(instruction, instructionEq);
			else
			{
				_hashsToNodes[hash] = instruction;
				
				visit(instruction.arg1, isVertexShader);
				if (!instruction.isSingle)
					visit(instruction.arg2, isVertexShader);
			}
		}
		
		override protected function visitInterpolate(interpolate	: Interpolate,
													 isVertexShader	: Boolean) : void
		{
			var hash			: uint	= interpolate.hash;
			var interpolateEq	: INode	= _hashsToNodes[hash];
			
			if (interpolateEq)
				replaceInParent(interpolate, interpolateEq);
			else
			{
				_hashsToNodes[hash] = interpolate;
				
				visit(interpolate.arg, true);
			}
		}
		
		override protected function visitOverwriter(overwriter		: Overwriter,
													isVertexShader	: Boolean) : void
		{
			var hash			: uint	= overwriter.hash;
			var overwriterEq	: INode	= _hashsToNodes[hash];
			
			if (overwriterEq)
				replaceInParent(overwriter, overwriterEq);
			else
			{
				_hashsToNodes[hash] = overwriter;
				
				var args	: Vector.<INode> = overwriter.args;
				var numArgs	: uint = args.length;
				
				for (var argId : uint = 0; argId < numArgs; ++argId)
					visit(args[argId], isVertexShader);
			}
		}
		
		override protected function visitVariadicExtract(variadicExtract : VariadicExtract, 
														 isVertexShader	 : Boolean) : void
		{
			if (!isVertexShader)
				throw new Error('VariadicExtract can only be found on vertex shaders.');
			
			var hash				: uint = variadicExtract.hash;
			var variadicExtractEq	: INode = _hashsToNodes[hash];
			
			if (variadicExtractEq)
				replaceInParent(variadicExtract, variadicExtractEq);
			else
			{
				_hashsToNodes[hash] = variadicExtract;
				
				visit(variadicExtract.index, true);
				visit(variadicExtract.constant, true);
			}
		}
		
		override protected function visitExtract(extract		: Extract,
												 isVertexShader	: Boolean) : void
		{
			throw new Error('Found invalid node: ' + extract.toString());
		}
		
		override protected function visitAttribute(attribute		: Attribute,
												   isVertexShader	: Boolean) : void
		{
			visitNonTraversable(attribute);
		}
		
		override protected function visitConstant(constant		 : Constant,
												  isVertexShader : Boolean) : void
		{
			visitNonTraversable(constant);
		}
		
		override protected function visitBindableConstant(parameter		 : BindableConstant,
														  isVertexShader : Boolean) : void
		{
			visitNonTraversable(parameter);
		}
		
		override protected function visitSampler(sampler		: Sampler,
												 isVertexShader	: Boolean) : void
		{
			visitNonTraversable(sampler);
		}
		
		override protected function visitBindableSampler(bindableSampler : BindableSampler,
														 isVertexShader	 : Boolean) : void
		{
			visitNonTraversable(bindableSampler);
		}
		
		private function visitNonTraversable(node : INode) : void
		{
			var hash	: uint	= node.hash;
			var nodeEq	: INode	= _hashsToNodes[hash];
			
			if (nodeEq)
				replaceInParent(node, nodeEq);
			else
				_hashsToNodes[hash] = node;
		}
		
	}
}