package aerys.minko.render.shader.compiler.visitor.allocator
{
	import aerys.minko.render.shader.compiler.allocator.Allocator;
	import aerys.minko.render.shader.compiler.register.RegisterLimit;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.AbstractConstant;
	import aerys.minko.render.shader.node.leaf.Sampler;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.render.shader.node.operation.manipulation.VariadicExtract;
	
	/**
	 * Allocates registers for the fragment allocator.
	 *  
	 * @author Romain Gilliotte <romain.gilliotte@aerys.in>
	 */	
	public class FragmentAllocator extends AbstractAllocator
	{
		protected var _vertexShaderOutputs	: Vector.<INode>;
		protected var _samplers				: Vector.<int>;
		
		protected var _stack				: Vector.<INode>;
		protected var _visited				: Vector.<INode>;
		
		protected var _fgSamplerId			: uint;
		protected var _variyngAlloc			: Allocator;
		
		public function get vertexShaderOutputs() : Vector.<INode>
		{
			return _vertexShaderOutputs;
		}
		
		public function get samplers() : Vector.<int>
		{
			return _samplers;
		}
		
		public function FragmentAllocator(tmpAlloc		: Allocator,
										  variyngAlloc	: Allocator,
										  constAlloc	: Allocator)
		{
			_vertexShaderOutputs	= new Vector.<INode>();
			_samplers				= new Vector.<int>();
			
			_operationId	= 0;
			_variyngAlloc	= variyngAlloc;
			_tmpAlloc		= tmpAlloc;
			_constAlloc		= constAlloc;
			
			_stack			= new Vector.<INode>();
			_visited		= new Vector.<INode>();
			
			_fgSamplerId	= 0;
		}
		
		public function processFragmentShader(color : INode) : Vector.<INode> 
		{
			visit(color);
			return _vertexShaderOutputs;
		}
		
		/**
		 * Visite graphe a partir du fragment shader pour trouver les noeuds d'interpolation
		 * et les ajouter dans la liste d'outputs du vertex shader
		 * 
		 * De plus, on en profite pour allouer les registres des leafs et des temporaires
		 * illimites pour les operations
		 */
		override public function visit(shaderNode : INode) : void
		{
			if (_visited.indexOf(shaderNode) !== -1)
				return;
			
			_visited.push(shaderNode);
			_stack.push(shaderNode);
			
			// visit the children only if not and interpolation node.
			if (!(shaderNode is Interpolate))
				shaderNode.accept(this);
			
			if (shaderNode is Interpolate)
			{
				// An interpolate node is a vertex shader output,
				// everything under it must not be processed here.
				_vertexShaderOutputs.push(shaderNode);
				_variyngAlloc.allocate(shaderNode);
			}
			else if (shaderNode is AbstractConstant)
			{
				_constAlloc.allocate(shaderNode);
			}
			else if (shaderNode is Sampler)
			{
				// sampler allocation is done by hand.
				var samplerNode : Sampler = Sampler(shaderNode);
				samplerNode.samplerId = _fgSamplerId++;
				_samplers.push(samplerNode.styleId);
				
				if (_fgSamplerId > RegisterLimit.FG_MAX_SAMPLER)
					throw new Error('Unable to allocate a new sampler registry.');
			}
			else if (shaderNode is Extract)
			{
				// do nothing, we already allocated the arguments for this node
			}
			else if (shaderNode is VariadicExtract)
			{
				// do nothing, we already allocated the arguments for this node
			}
			else if (shaderNode is Combine)
			{
				var combineNode : Combine = shaderNode as Combine;
				
				++_operationId;
				reportArgumentUsage(combineNode.arg1, false);
				
				_tmpAlloc.allocate(shaderNode, _operationId);
				
				++_operationId;
				reportArgumentUsage(combineNode.arg2, false);
			}
			else if (shaderNode is AbstractOperation)
			{
				++_operationId;
				reportOperationArgumentsUsage(shaderNode as AbstractOperation);
				
				if (_stack.length > 1)
					_tmpAlloc.allocate(shaderNode, _operationId);
			}
			
			_stack.pop();
		}
		
		override protected function reportArgumentUsage(arg:INode, aligned : Boolean) : void
		{
			if (arg is Interpolate)
				_variyngAlloc.reportUsage(arg, _operationId, aligned);
			
			else if (arg is VariadicExtract)
			{
				var variadic : VariadicExtract = arg as VariadicExtract;
				_tmpAlloc.reportUsage(variadic.arg1, _operationId, aligned);
				_constAlloc.reportUsage(variadic.arg2, _operationId, aligned);
			}
			
			else if (arg is AbstractConstant)
				_constAlloc.reportUsage(arg, _operationId, aligned);
			
			else if (arg is Extract)
				reportArgumentUsage((arg as Extract).arg1, aligned);
			
			else if (arg is AbstractOperation)
				_tmpAlloc.reportUsage(arg, _operationId, aligned);
		}
	}
}
