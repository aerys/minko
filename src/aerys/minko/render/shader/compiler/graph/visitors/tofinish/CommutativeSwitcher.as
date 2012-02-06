package aerys.minko.render.shader.compiler.graph.visitors.tofinish
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
	import aerys.minko.render.shader.compiler.graph.visitors.AbstractVisitor;

	public class CommutativeSwitcher extends AbstractVisitor
	{
		private var _isComputable : Boolean;
		
		public function CommutativeSwitcher()
		{
			super(true);
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
			if (instruction.isSingle)
				visit(instruction.arg2, isVertexShader)
			
		}
		
		override protected function visitOverwriter(overwriter	   : Overwriter, 
													isVertexShader : Boolean) : void
		{
			for each (var arg : INode in overwriter.args)
			{
				visit(arg, isVertexShader);
				
				if (!_isComputable)
				{
					_isComputable = false;
					return;
				}
			}
			
			_isComputable = true;
		}
		
		override protected function visitAttribute(attribute	  : Attribute, 
												   isVertexShader : Boolean) : void
		{
			_isComputable = false;
		}
		
		override protected function visitConstant(constant		 : Constant, 
												  isVertexShader : Boolean) : void
		{
			_isComputable = true;
		}
		
		override protected function visitExtract(extract		: Extract, 
												 isVertexShader	: Boolean) : void
		{
			throw new Error('Found invalid node: ' + extract.toString());
		}
		
		override protected function visitParameter(parameter	  : Parameter, 
												   isVertexShader : Boolean) : void
		{
			_isComputable = true;
		}
		
		override protected function visitSampler(sampler		: Sampler, 
												 isVertexShader	: Boolean) : void
		{
			_isComputable = false;
		}
	}
}