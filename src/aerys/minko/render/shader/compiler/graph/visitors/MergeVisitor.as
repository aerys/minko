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

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class MergeVisitor extends AbstractVisitor
	{
		private var _hashsToNodes	: Dictionary;
		
		public function MergeVisitor()
		{
			super(true);
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
			
			if (_hashsToNodes[instruction.arg1.hash] != undefined)
				instruction.arg1 = _hashsToNodes[instruction.arg1.hash];
			else
			{
				_hashsToNodes[instruction.arg1.hash] = instruction.arg1;
				visit(instruction.arg1, isVertexShader);
			}
			
			if (!instruction.isSingle)
			{
				if (_hashsToNodes[instruction.arg2.hash] != undefined)
					instruction.arg2 = _hashsToNodes[instruction.arg2.hash];
				else
				{
					_hashsToNodes[instruction.arg2.hash] = instruction.arg2;
					visit(instruction.arg2, isVertexShader);
				}
			}
		}
		
		override protected function visitInterpolate(interpolate	: Interpolate,
													 isVertexShader	: Boolean) : void
		{
			if (_hashsToNodes[interpolate.arg.hash] != undefined)
				interpolate.arg = _hashsToNodes[interpolate.arg.hash];
			else
			{
				_hashsToNodes[interpolate.arg.hash] = interpolate.arg;
				visit(interpolate.arg, false);
			}
		}
		
		override protected function visitOverwriter(overwriter		: Overwriter,
													isVertexShader	: Boolean) : void
		{
			var args	: Vector.<INode>	= overwriter.args;
			var numArgs	: uint				= args.length;
			
			for (var argId : uint = 0; argId < numArgs; ++argId)
			{
				if (_hashsToNodes[args[argId].hash] != undefined)
					args[argId] = _hashsToNodes[args[argId].hash];
				else
				{
					_hashsToNodes[args[argId].hash] = args[argId];
					visit(args[argId], isVertexShader);
				}
			}
		}
		
		override protected function visitVariadicExtract(variadicExtract : VariadicExtract, 
														 isVertexShader	 : Boolean) : void
		{
			if (!isVertexShader)
				throw new Error('VariadicExtract can only be found on vertex shaders.');
			
			if (_hashsToNodes[variadicExtract.index.hash] != undefined)
				variadicExtract.index = _hashsToNodes[variadicExtract.index.hash];
			else
			{
				_hashsToNodes[variadicExtract.index.hash] = variadicExtract.index;
				visit(variadicExtract.index, false);
			}
			
			if (_hashsToNodes[variadicExtract.constant.hash] != undefined)
				variadicExtract.constant = _hashsToNodes[variadicExtract.constant.hash];
			else
			{
				_hashsToNodes[variadicExtract.constant.hash] = variadicExtract.constant;
				visit(variadicExtract.constant, false);
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
		}
		
		override protected function visitConstant(constant		 : Constant,
												  isVertexShader : Boolean) : void
		{
		}
		
		override protected function visitBindableConstant(parameter		 : BindableConstant,
														  isVertexShader : Boolean) : void
		{
		}
		
		override protected function visitSampler(sampler		: Sampler,
												 isVertexShader	: Boolean) : void
		{
		}
		
		override protected function visitBindableSampler(bindableSampler : BindableSampler,
														 isVertexShader	 : Boolean) : void
		{
		}
		
	}
}