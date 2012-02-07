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
	import aerys.minko.render.shader.compiler.register.Components;
	
	public class ResolveParametrizedComputationVisitor extends AbstractVisitor
	{
		private static const COMPUTABLE_CONSTANT_PREFIX : String = 'computableConstant';
		
		private var _isComputable			: Boolean;
		private var _computableConstantId	: uint;
		
		public function ResolveParametrizedComputationVisitor()
		{
			super(false);
		}
		
		override protected function start() : void
		{
			_computableConstantId = 0;	
		}
		
		override protected function finish() : void
		{
		}
		
		override protected function visitInterpolate(interpolate	: Interpolate, 
													 isVertexShader	: Boolean) : void
		{
			visit(interpolate.arg, true);
		}
		
		override protected function visitVariadicExtract(variadicExtract : VariadicExtract, isVertexShader : Boolean) : void
		{
			if (!isVertexShader)
				throw new Error('VariadicExtract are only available in the vertex shader.');
			
			visit(variadicExtract.index, true);
			
			if (_isComputable && _stack.length < 2)
				replaceInParent(variadicExtract, createComputableConstant(variadicExtract));
		}
		
		override protected function visitInstruction(instruction : Instruction, isVertexShader : Boolean) : void
		{
			var isComputable1 : Boolean;
			var isComputable2 : Boolean;
			
			visit(instruction.arg1, isVertexShader);
			isComputable1 = _isComputable;
			
			if (!instruction.isSingle)
			{
				visit(instruction.arg2, isVertexShader);
				isComputable2 = _isComputable;
			}
			
			if (instruction.isSingle)
			{
				_isComputable = isComputable1;
				
				if (_isComputable)
				{
					// do nothing. this will be wrapped when going up
					// if stack.length is 1, we should replace here
					if (_stack.length < 2)
					{
						replaceInParent(instruction, createComputableConstant(instruction));
					}
				}
				else
				{
					// this is not computable, nothing to do!
				}
			}
			else
			{
				_isComputable = isComputable1 && isComputable2;
				
				if (_isComputable)
				{
					// do nothing, go up, and hope for the best
					// if we cannot go up, replace tree root by an evalexp parameter
					if (_stack.length < 2)
						replaceInParent(instruction, createComputableConstant(instruction));
				}
				else
				{
					// check if either arg1 or 2 is computable, and useful to compute
					if (isComputable1 && !(instruction.arg1 is Constant || instruction.arg1 is BindableConstant))
						instruction.arg1 = createComputableConstant(instruction.arg1);
					
					if (isComputable2 && !(instruction.arg2 is Constant || instruction.arg2 is BindableConstant))
						instruction.arg2 = createComputableConstant(instruction.arg2);
				}
			}
		}
		
		override protected function visitOverwriter(overwriter	   : Overwriter, 
													isVertexShader : Boolean) : void
		{
			var argId			: uint;
			var args			: Vector.<INode>	= overwriter.args;
			var comps			: Vector.<uint>		= overwriter.components;
			var numArgs			: uint				= args.length;
			
			var isComputable	: Vector.<Boolean>	= new Vector.<Boolean>(numArgs, true);
			
			var computableArgs	: Vector.<INode>	= new Vector.<INode>();
			var computableComps	: Vector.<uint>		= new Vector.<uint>();
			
			// which arguments are computable?
			// remove them from the overwriter
			for (argId = 0; argId < numArgs; ++argId)
			{
				visit(args[argId], isVertexShader);
				
				if (_isComputable)
				{
					computableComps.push(comps[argId]);
					computableArgs.push(args[argId]);
					
					args.splice(argId, 1);
					comps.splice(argId, 1);
					
					--argId;
					--numArgs;
				}
			}
			
			var computableArgsCount : uint = computableArgs.length;
			
			// all arguments are computable!
			if (numArgs == 0)
			{
				// put the arguments and components back
				numArgs = computableArgs.length;
				for (argId = 0; argId < numArgs; ++argId)
				{
					overwriter.args.push(computableArgs[argId]);
					overwriter.components.push(computableComps[argId]);
				}
				overwriter.invalidateHashAndSize(); // that's useless, just in case
				
				// if this is the tree root, we replace the overwriter by a parameter:
				// this shader is going to be a simple "mov op, fc0"
				if (_stack.length < 2)
					replaceInParent(overwriter, createComputableConstant(overwriter));
				
				// tell the parent we are computable
				_isComputable = true;
				return;
			}
			// no arguments are computable
			else if (computableArgsCount == 0)
			{
				_isComputable = false;
				return;
			}
			// only one argument is computable. 
			else if (computableArgsCount == 1)
			{
				// there is no need to replace it if it's already a parameter or a contant.
				if (computableArgs[0] is Constant || computableArgs[0] is BindableConstant)
				{
					overwriter.args.unshift(computableArgs[0]);
					overwriter.components.unshift(computableComps[0]);
				}
				// we must replace it otherwise
				else
				{
					overwriter.args.unshift(createComputableConstant(computableArgs[0]));
					overwriter.components.unshift(computableComps[0]);
				}
				
				overwriter.invalidateHashAndSize();
			}
			// more than one argument is computable in CPU, we have to merge them, and shift them back into the overwriter.
			else
			{
				// we are going to decompote this overwriter into 2 overwriters...
				
				// first we find holes
				numArgs = computableArgs.length;
				var currentWrite : uint = 0;
				var resultingComponents : uint = computableComps[0];
				for (argId = 1; argId < numArgs; ++argId) // the order doesn't really matters...
					resultingComponents = Components.applyOverwriting(resultingComponents, computableComps[argId]);
				
				var xIsHole : Boolean = ((resultingComponents >>> (8 * 0)) & 0xff) == 4;
				var yIsHole : Boolean = ((resultingComponents >>> (8 * 1)) & 0xff) == 4;
				var zIsHole : Boolean = ((resultingComponents >>> (8 * 2)) & 0xff) == 4;
				var wIsHole : Boolean = ((resultingComponents >>> (8 * 3)) & 0xff) == 4;
				
				// now we can generate our final resulting component (the one going to the GPU executed overwriter).
				// there are only 16 possible components, if this is not working, precompute all values.
				var finalComponent		: uint = 0;
				var currentReadOffset	: uint = 0;
				finalComponent |= (xIsHole ? 4 : currentReadOffset++) << (8 * 0);
				finalComponent |= (yIsHole ? 4 : currentReadOffset++) << (8 * 1);
				finalComponent |= (zIsHole ? 4 : currentReadOffset++) << (8 * 2);
				finalComponent |= (wIsHole ? 4 : currentReadOffset++) << (8 * 3);
				
				// now we need to partially write offset existing components, because
				// the cpu executed overwriter cannot have holes in the result. If is
				// does the allocator is going to freak out (and will be right to do so:
				// it would make no sense).
				for (argId = 0; argId < numArgs; ++argId)
				{
					if (xIsHole)
						// this one is easy, we just make the first byte fall into the bit bucket
						computableComps[argId] = computableComps[argId] >>> 8 | (4 << (3 * 8));
					
					if (yIsHole)
						// remove bits 8 to 15
						computableComps[argId] = (computableComps[argId] & 0xff) | ((computableComps[argId] >>> 8) & (~0xff)) | (4 << (3 * 8));
					
					if (zIsHole)
						computableComps[argId] = (computableComps[argId] & 0xffff) | ((computableComps[argId] >>> 8) & (~0xffff)) | (4 << (3 * 8));
					
					if (wIsHole)
						// useless
						computableComps[argId] = (computableComps[argId] & 0xffffff) | (4 << (3 * 8));
				}
				
				var cpuOverwriter : Overwriter = new Overwriter(computableArgs, computableComps);
				
				overwriter.args.unshift(createComputableConstant(cpuOverwriter));
				overwriter.components.unshift(finalComponent);
				overwriter.invalidateHashAndSize();
				
				_isComputable = false;
				return;
			}
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
		
		override protected function visitBindableConstant(bindableConstant	: BindableConstant,
														  isVertexShader	: Boolean):void
		{
			_isComputable = true;
		}
		
		override protected function visitSampler(sampler		: Sampler, 
												 isVertexShader	: Boolean) : void
		{
			_isComputable = false;
		}
		
		override protected function visitBindableSampler(bindableSampler	: BindableSampler, 
														 isVertexShader		: Boolean) : void
		{
			_isComputable = false;
		}
		
		override protected function visitExtract(extract		: Extract, 
												 isVertexShader	: Boolean) : void
		{
			throw new Error('Found invalid node: ' + extract.toString());
		}
		
		private function createComputableConstant(computableNode : INode) : BindableConstant
		{
			var constantName : String = COMPUTABLE_CONSTANT_PREFIX + (_computableConstantId++);
			_shaderGraph.computableConstants[constantName] = computableNode;
			
			return new BindableConstant(constantName, computableNode.size);
		}
	}
}