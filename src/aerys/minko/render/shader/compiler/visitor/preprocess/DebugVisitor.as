package aerys.minko.render.shader.compiler.visitor.preprocess
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;
	
	public class DebugVisitor implements IShaderNodeVisitor
	{
		protected var _errors : String;
		
		public function DebugVisitor()
		{
		}
		
		public function processShader(clipspacePos	: INode, 
									  color			: INode) : String
		{
			_errors = '';
			
			return _errors;
		}
		
		public function visit(node : INode):void
		{
			/*
			from vertex preprocessor
			
			if (shaderNode is Texture)
			{
				throw new Error('A texture node cannot be reachable from ' +
					'the clipspace position nor from an interpolation node.');
			}
			else if (shaderNode is Interpolate)
			{
				var interpolateNode:Interpolate = Interpolate(shaderNode);
				if (interpolateNode.outputId === -1)
					throw new Error('An interpolation node must be reachable' +
						'from the color node.');
			}
			
			
			else if (shaderNode is Sampler)
			{
			throw new Error('A sampler node cannot be reachable from ' +
			'the clipspace position nor from an interpolation node.');
			}
			
			
			if (constantNode.constantId !== -1)
			throw new Error('Found an already allocated constant. Did you execute' +
			'the constant duplication visitor?');
			
			
			
			*/
		}
	}
}