package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.binding.ConstantBinder;
	import aerys.minko.render.shader.binding.EvalExp;
	import aerys.minko.render.shader.binding.IBinder;
	import aerys.minko.render.shader.binding.ProxyConstantBinder;
	import aerys.minko.render.shader.binding.TextureBinder;
	import aerys.minko.render.shader.compiler.allocation.Allocator;
	import aerys.minko.render.shader.compiler.allocation.IAllocation;
	import aerys.minko.render.shader.compiler.allocation.SimpleAllocation;
	import aerys.minko.render.shader.compiler.graph.ShaderGraph;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.AbstractSampler;
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
	import aerys.minko.render.shader.compiler.register.RegisterLimit;
	import aerys.minko.render.shader.compiler.register.RegisterType;
	import aerys.minko.render.shader.compiler.sequence.AgalDestination;
	import aerys.minko.render.shader.compiler.sequence.AgalInstruction;
	import aerys.minko.render.shader.compiler.sequence.AgalSourceCommon;
	import aerys.minko.render.shader.compiler.sequence.AgalSourceEmpty;
	import aerys.minko.render.shader.compiler.sequence.AgalSourceSampler;
	import aerys.minko.render.shader.compiler.sequence.IAgalSource;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.utils.Dictionary;

	public class AllocationVisitor extends AbstractVisitor
	{
		private var _allocations			: Dictionary;
		
		// allocators
		private var _opAllocator			: Allocator;
		private var _ocAllocator			: Allocator;
		private var _attributeAllocator		: Allocator;
		private var _vsConstAllocator		: Allocator;
		private var _fsConstAllocator		: Allocator;
		private var _vsTempAllocator		: Allocator;
		private var _fsTempAllocator		: Allocator;
		private var _varyingAllocator		: Allocator;
		
		// first pass data
		private var _vsInstructions			: Vector.<Instruction>;
		private var _fsInstructions			: Vector.<Instruction>;
		
		private var _vsConstants			: Vector.<Constant>;
		private var _fsConstants			: Vector.<Constant>;
		
		private var _vsParams				: Vector.<BindableConstant>;
		private var _fsParams				: Vector.<BindableConstant>;
		
		private var _samplers				: Vector.<BindableSampler>;
		
		// final compiled program
		private var _vsProgram				: Vector.<AgalInstruction>;
		private var _fsProgram				: Vector.<AgalInstruction>;
		
		private var _paramBindings			: Vector.<IBinder>;
		
		private var _vertexComponents		: Vector.<VertexComponent>;
		private var _vertexIndices			: Vector.<uint>;
		
		private var _vertexConstants		: Vector.<Number>;
		private var _fragmentConstants		: Vector.<Number>;
		private var _textures				: Vector.<ITextureResource>;
		
		public function get parameterBindings() : Vector.<IBinder>
		{
			return _paramBindings;
		}
		
		public function get vertexSequence() : Vector.<AgalInstruction>
		{
			return _vsProgram;	
		}
		
		public function get fragmentSequence() : Vector.<AgalInstruction>
		{
			return _fsProgram;
		}
		
		public function get vertexConstants() : Vector.<Number>
		{
			return _vertexConstants;
		}
		
		public function get fragmentConstants() : Vector.<Number>
		{
			return _fragmentConstants;
		}
		
		public function get vertexComponents() : Vector.<VertexComponent>
		{
			return _vertexComponents;
		}
		
		public function get vertexIndices() : Vector.<uint>
		{
			return _vertexIndices;
		}
		
		public function get textures() : Vector.<ITextureResource>
		{
			return _textures;
		}
		
		public function AllocationVisitor()
		{
			super(true);
		}
		
		override protected function start() : void
		{
			super.start();
			
			// allocators
			_opAllocator				= new Allocator(1, RegisterType.OUTPUT, true, true);
			_ocAllocator				= new Allocator(1, RegisterType.OUTPUT, true, true);
			_attributeAllocator			= new Allocator(RegisterLimit.VS_MAX_ATTRIBUTE, RegisterType.ATTRIBUTE, true, true);
			_vsConstAllocator			= new Allocator(RegisterLimit.VS_MAX_CONSTANT, RegisterType.CONSTANT, true, false);
			_fsConstAllocator			= new Allocator(RegisterLimit.FS_MAX_CONSTANT, RegisterType.CONSTANT, true, false);
			_vsTempAllocator			= new Allocator(RegisterLimit.VS_MAX_TEMPORARY, RegisterType.TEMPORARY, false, false);
			_fsTempAllocator			= new Allocator(RegisterLimit.FS_MAX_TEMPORARY, RegisterType.TEMPORARY, false, false);
			_varyingAllocator			= new Allocator(RegisterLimit.MAX_VARYING, RegisterType.VARYING, true, true);
			
			_allocations				= new Dictionary();
			
			// first pass data
			_vsInstructions				= new Vector.<Instruction>();
			_fsInstructions				= new Vector.<Instruction>();
			_vsConstants				= new Vector.<Constant>();
			_fsConstants				= new Vector.<Constant>();
			_vsParams					= new Vector.<BindableConstant>();
			_fsParams					= new Vector.<BindableConstant>();
			_samplers					= new Vector.<BindableSampler>();
			
			// final compiled data
			_paramBindings				= new Vector.<IBinder>();
			_vertexComponents			= new Vector.<VertexComponent>();
			_vertexIndices				= new Vector.<uint>();
			_textures					= new Vector.<ITextureResource>(8, true);
		}
		
		override public function process(shaderGraph : ShaderGraph) : void
		{
			var i				: uint;
			var numInterpolates	: uint = shaderGraph.interpolates.length;
			var numKills		: uint = shaderGraph.kills.length;
			
			start();
			_shaderGraph = shaderGraph;
			
			var opMove : Instruction = new Instruction(Instruction.MOV, _shaderGraph.position);
			var ocMove : Instruction = new Instruction(Instruction.MOV, _shaderGraph.color);
			
			opMove.arg1Components	= _shaderGraph.positionComponents;
			ocMove.arg1Components	= _shaderGraph.colorComponents;
			_shaderGraph.position	= opMove;
			_shaderGraph.color		= ocMove;
			
			visit(_shaderGraph.position, true);
			for (i = 0; i < numInterpolates; ++i)
				visit(_shaderGraph.interpolates[i], true);
			
			visit(_shaderGraph.color, false);
			
			for (i = 0; i < numKills; ++i)
				visit(_shaderGraph.kills[i], false);
			
			finish();
		}
		
		override protected function finish() : void
		{
			for each (var allocator : Allocator in [ _attributeAllocator,
				_vsConstAllocator, _fsConstAllocator, _vsTempAllocator, 
				_fsTempAllocator, _varyingAllocator, _opAllocator, _ocAllocator])
			{
				allocator.computeRegisterState();
			}
			
			_paramBindings			= _paramBindings.concat(createConstantParameterBindings(_vsParams, true));
			_vertexConstants		= createConstantTables(_vsConstants, true);
			_vsProgram				= writeProgram(_vsInstructions, true);
			
			_paramBindings			= _paramBindings.concat(createConstantParameterBindings(_fsParams, false));
			_fragmentConstants		= createConstantTables(_fsConstants, false);
			_fsProgram				= writeProgram(_fsInstructions, false);
			
			processAttributes();
		}
		
		private function processAttributes() : void
		{
			var maxRegisterId : uint = 0;
			
			_vertexComponents	= new Vector.<VertexComponent>(8);
			_vertexIndices		= new Vector.<uint>(8);
			
			for (var node : Object in _allocations)
			{
				if (node is Attribute)
				{
					var registerId	: uint				= SimpleAllocation(_allocations[node]).registerId;
					var component	: VertexComponent	= Attribute(node).component;
					var index		: uint				= Attribute(node).componentId;
					
					_vertexComponents[registerId] = component;
					_vertexIndices[registerId] = index;
					
					if (registerId > maxRegisterId)
						maxRegisterId = registerId;
				}
			}
			
			_vertexComponents.length = _vertexIndices.length = maxRegisterId + 1;
		}
		
		private function createConstantParameterBindings(bindableConstants	: Vector.<BindableConstant>,
														 isVertexShader		: Boolean) : Vector.<IBinder>
		{
			var result : Vector.<IBinder> = new Vector.<IBinder>();
			
			for each (var bindableConstant : BindableConstant in bindableConstants)
			{
				var bindingName : String			= bindableConstant.bindingName;
				var tree		: INode				= _shaderGraph.computableConstants[bindingName];
				var alloc		: SimpleAllocation	= _allocations[bindableConstant];
				var binder		: IBinder			= new ConstantBinder(bindingName, alloc.offset, alloc.maxSize, isVertexShader);
				
				if (!tree)
					result.push(binder);
				else
				{
					var evalExp				: EvalExp					= new EvalExp(tree);
					var inBindableConsts	: Vector.<BindableConstant> = evalExp.bindableConstants;
					var inBinders			: Vector.<IBinder>			= new Vector.<IBinder>();
					
					for each (var inBindableConst : BindableConstant in inBindableConsts)
						result.push(new ProxyConstantBinder(inBindableConst.bindingName, inBindableConst.size, binder, evalExp));
				}
			}
			
			return result;
		}
		
		private function createConstantTables(constants		 : Vector.<Constant>,
											  isVertexShader : Boolean) : Vector.<Number>
		{
			var result		: Vector.<Number> = new Vector.<Number>();
			var alloc		: SimpleAllocation;
			var offsetBegin	: uint;
			var offsetLimit	: uint;
			
			for each (var constant : Constant in constants)
			{
				var data		: Vector.<Number>	= constant.value;
				
				alloc		= _allocations[constant];
				offsetBegin	= alloc.offset;
				offsetLimit	= offsetBegin + alloc.maxSize;
				
				if (result.length < offsetLimit)
					result.length = offsetLimit;
				
				for (var offset : uint = offsetBegin, localOffset : uint = 0;
					offset < offsetLimit;
					++offset, ++localOffset)
					 result[offset] = data[localOffset];
			}
			
			var parameters : Vector.<BindableConstant> = isVertexShader ? _vsParams : _fsParams;
			
			for each (var parameter : BindableConstant in parameters)
			{
				alloc		= _allocations[parameter];
				offsetBegin	= alloc.offset;
				offsetLimit	= offsetBegin + alloc.maxSize;
				
				if (result.length < offsetLimit)
					result.length = offsetLimit;
			}
			
			result.length = 4 * Math.ceil(result.length / 4);
			
			return result;
		}
		
		private function writeProgram(instructions	 : Vector.<Instruction>,
									  isVertexShader : Boolean) : Vector.<AgalInstruction>
		{
			var result		: Vector.<AgalInstruction> = new Vector.<AgalInstruction>();
			
			for each (var instruction : Instruction in instructions)
			{
				var destAlloc	: SimpleAllocation	= _allocations[instruction];
				var destination	: AgalDestination	= new AgalDestination(destAlloc.registerId, destAlloc.writeMask, destAlloc.type);
				
				var source1		: IAgalSource		= getSourceFor(instruction.arg1, instruction.arg1Components, destAlloc);
				var source2		: IAgalSource		= instruction.isSingle ? 
					new AgalSourceEmpty() : 
					getSourceFor(instruction.arg2, instruction.arg2Components, destAlloc);
				
				result.push(new AgalInstruction(instruction.id, destination, source1, source2));
			}
			
			return result;
		}
		
		private function getSourceFor(argument			: INode, 
									  readComponents	: uint, 
									  destAlloc			: SimpleAllocation) : IAgalSource
		{
			var source : IAgalSource;
			
			if (argument is VariadicExtract)
			{
				var variadicExtract : VariadicExtract = VariadicExtract(argument);
				
				var constantAlloc	: IAllocation = _allocations[variadicExtract.constant];
				var indexAlloc		: IAllocation = _allocations[variadicExtract.index];
				
				source = new AgalSourceCommon(
					indexAlloc.registerId,
					constantAlloc.registerId,
					constantAlloc.getReadSwizzle(destAlloc.registerOffset, readComponents),
					constantAlloc.type,
					indexAlloc.type,
					indexAlloc.registerOffset + variadicExtract.indexComponentSelect,
					false
				);
				
			}
			else if (argument is AbstractSampler)
			{
				var sampler : AbstractSampler = AbstractSampler(argument);
				
				source = new AgalSourceSampler(
					_samplers.indexOf(sampler),
					sampler.dimension,
					sampler.wrapping,
					sampler.filter,
					sampler.mipmap
				);
			}
			else
			{
				var sourceAlloc	: IAllocation = _allocations[argument];
				source = new AgalSourceCommon(
					sourceAlloc.registerId, 
					0,
					sourceAlloc.getReadSwizzle(destAlloc.registerOffset, readComponents),
					sourceAlloc.type,
					0,
					0,
					true
				);
			}
			
			return source;
		}
		
		private function extendLifeTime(argument : INode, isVertexShader : Boolean) : void
		{
			var instructionCounter : uint = getInstructionCounter(isVertexShader);
			
			if (argument is Sampler)
			{
				// do nothing
			}
			else if (argument is VariadicExtract)
			{
				var variadicExtract : VariadicExtract = VariadicExtract(argument);
				
				IAllocation(_allocations[variadicExtract.constant]).extendLifeTime(instructionCounter);
				IAllocation(_allocations[variadicExtract.index]).extendLifeTime(instructionCounter);
			}
			else
			{
				IAllocation(_allocations[argument]).extendLifeTime(instructionCounter);
			}
		}
		
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		
		
		
		
		/**
		 * For now we are combining nodes in order.
		 * To minimize program break, could be better to make cases depending on how many
		 * non-component wise node there are in here.
		 * 
		 * Also, we need to process all constants nodes in one call.
		 */
		override protected function visitOverwriter(overwriter		: Overwriter, 
													isVertexShader	: Boolean) : void
		{
			var overwriterAllocator : Allocator = getAllocatorFor(overwriter, isVertexShader);
			
			var args				: Vector.<INode>			= overwriter.args;
			var components			: Vector.<uint>				= overwriter.components;
			var numArgs				: uint						= args.length;
			
			var subAllocs 			: Vector.<SimpleAllocation>	= new Vector.<SimpleAllocation>();
			var subOffsets			: Vector.<uint>				= new Vector.<uint>();
			
			var mov					: Instruction;
			var instructionCounter	: uint;
			
			for (var argId : uint = 0; argId < numArgs; ++argId)
			{
				var arg				: INode	= args[argId];
				var component		: uint	= components[argId];
				var minWriteOffset	: int	= Components.getMinWriteOffset(component);
				
				component = Components.applyWriteOffset(component, -minWriteOffset);
				
				// The overwriter argument is not allocated on temporaries, we have to use a mov instruction no matter what.
				if (arg is Attribute || arg is Constant || arg is BindableConstant || arg is Interpolate)
				{
					
					// visit the constant/attribute/etc to allocate it.
					visit(arg, isVertexShader);
					
					// execute a mov intruction to copy it to temporaries and swizzle it properly.
					mov = new Instruction(Instruction.MOV, arg);
					mov.arg1Components = component;
					pushInstruction(mov, isVertexShader);
					
					// get the current instruction counter, to report allocations.
					instructionCounter = getInstructionCounter(isVertexShader);
					
					// allocate the result of the mov, and report usage of the constant.
					_allocations[mov] = getAllocatorFor(mov, isVertexShader).allocate(mov.size, true, instructionCounter);
					
					subAllocs.push(_allocations[mov]);
					subOffsets.push(minWriteOffset);
					
					extendLifeTime(arg, isVertexShader);
//					IAllocation(_allocations[arg]).extendLifeTime(instructionCounter);
				}
					// The overwriter argument is already allocated on temporaries, let's try to avoid wasting a mov instruction.
				else if (arg is Instruction)
				{
					var instructionArg : Instruction = Instruction(arg);
					
					// nice! we can avoid using a mov instruction by tweaking 
					// the swizzles of the operation.
					if (instructionArg.isComponentWise)
					{
						// visit the arguments
						visit(instructionArg.arg1, isVertexShader);
						if (!instructionArg.isSingle)
							visit(instructionArg.arg2, isVertexShader);
						
						// create a new operation that combines the swizzles of the overwriter and
						// the instruction under it (this works only because it's all component wise).
						var instructionArgReplacement : Instruction = new Instruction(instructionArg.id, instructionArg.arg1, instructionArg.arg2);
						
						instructionArgReplacement.arg1Components = Components.applyCombination(instructionArg.arg1Components, component);
						if (!instructionArgReplacement.isSingle)
							instructionArgReplacement.arg2Components = Components.applyCombination(instructionArg.arg2Components, component);
						
						// push instruction, allocate and report usages.
						pushInstruction(instructionArgReplacement, isVertexShader);
						
						instructionCounter = getInstructionCounter(isVertexShader);
						
						_allocations[instructionArgReplacement] = getAllocatorFor(instructionArgReplacement, isVertexShader).
							allocate(instructionArgReplacement.size, instructionArgReplacement.isComponentWise, instructionCounter);
						
						subAllocs.push(_allocations[instructionArgReplacement]);
						subOffsets.push(minWriteOffset);
						
						extendLifeTime(instructionArgReplacement.arg1, isVertexShader);
//						IAllocation(_allocations[instructionArgReplacement.arg1]).extendLifeTime(instructionCounter);
						
						if (!instructionArgReplacement.isSingle)
							extendLifeTime(instructionArgReplacement.arg2, isVertexShader);
//							IAllocation(_allocations[instructionArgReplacement.arg2]).extendLifeTime(instructionCounter);
					}
					// if this is the first instruction of the overwriter, and the swizzle we need is x[y[z[w]?]?]?, we should
					// ignore the mov instruction.
					else
					{
						// in fact, we don't care: we are injecting mov instruction in the OverwriterCleanerVisitor
						throw new Error('This cannot be happening. Go fix your code.');
					}
				}
				else if (arg is Overwriter || arg is Extract || arg is Sampler || arg is BindableSampler)
					throw new Error('This cannot be happening. Go fix your code.');
			}
			
			_allocations[overwriter] = overwriterAllocator.combineAllocations(subAllocs, subOffsets);
		}
		
		override protected function visitInstruction(instruction	: Instruction,
													 isVertexShader	: Boolean) : void
		{
			visit(instruction.arg1, isVertexShader);
			if (!instruction.isSingle)
				visit(instruction.arg2, isVertexShader);
			
			pushInstruction(instruction, isVertexShader);
			
			var instructionCounter	: uint = getInstructionCounter(isVertexShader);
			
			_allocations[instruction] = getAllocatorFor(instruction, isVertexShader).
				allocate(instruction.size, instruction.isComponentWise, instructionCounter);
			
			extendLifeTime(instruction.arg1, isVertexShader);
			
			if (!instruction.isSingle)
				extendLifeTime(instruction.arg2, isVertexShader);
		}
		
		override protected function visitVariadicExtract(variadicExtract : VariadicExtract, isVertexShader : Boolean) : void
		{
			if (!isVertexShader)
				throw new Error('VariadicExtracts can only be found in the vertex shader.');
			
			visit(variadicExtract.constant, true);
			visit(variadicExtract.index, true);
		}
		
		override protected function visitInterpolate(interpolate : Interpolate, isVertexShader : Boolean) : void
		{
			if (isVertexShader)
			{
				visit(interpolate.arg, true);
				
				var movInstruction : Instruction = new Instruction(Instruction.MOV, interpolate.arg);
				_allocations[movInstruction] = _allocations[interpolate] =
					_varyingAllocator.allocate(movInstruction.size, true, getInstructionCounter(true));
				
				_vsInstructions.push(movInstruction);
			}
		}
		
		override protected function visitAttribute(attribute : Attribute, isVertexShader : Boolean) : void
		{
			_allocations[attribute] = getAllocatorFor(attribute, isVertexShader).allocate(attribute.size, true, 0);
		}
		
		override protected function visitConstant(constant : Constant, isVertexShader : Boolean) : void
		{
			_allocations[constant] = getAllocatorFor(constant, isVertexShader).allocate(constant.size, true, 0);
			
			if (isVertexShader)
				_vsConstants.push(constant);
			else
				_fsConstants.push(constant);
		}
		
		override protected function visitBindableConstant(bindableConstant : BindableConstant, isVertexShader : Boolean) : void
		{
			_allocations[bindableConstant] = getAllocatorFor(bindableConstant, isVertexShader).allocate(bindableConstant.size, true, 0);
			
			if (isVertexShader)
				_vsParams.push(bindableConstant);
			else
				_fsParams.push(bindableConstant);
		}
		
		override protected function visitSampler(sampler : Sampler, isVertexShader : Boolean) : void
		{
			_samplers.push(sampler);
			_textures[_samplers.length - 1] = sampler.textureResource;
		}
		
		override protected function visitBindableSampler(bindableSampler : BindableSampler, isVertexShader : Boolean) : void
		{
			_samplers.push(bindableSampler);
			_paramBindings.push(new TextureBinder(bindableSampler.bindingName, _samplers.length - 1));
		}
		
		
		override protected function visitExtract(extract		: Extract,
												 isVertexShader	: Boolean) : void
		{
			throw new Error('There cannot be any extract left at this point of shader compilation. Go fix your code.');
		}
		
		private function getAllocatorFor(node			: INode, 
										 isVertexShader	: Boolean) : Allocator
		{
			// if this is the root node, it has a different allocator.
			if (node === _shaderGraph.position)
				return _opAllocator;
			
			if (node === _shaderGraph.color)
				return _ocAllocator;
			
			if (_shaderGraph.kills.indexOf(node) !== -1)
				return null;
			
			if (_shaderGraph.interpolates.indexOf(node) !== -1)
				return _varyingAllocator;
			
			if (node is Instruction || node is Overwriter) // yes overwriter's destination is always in temporary space
				return isVertexShader ? _vsTempAllocator : _fsTempAllocator;
			
			if (node is Constant || node is BindableConstant)
				return isVertexShader ? _vsConstAllocator : _fsConstAllocator;
			
			if (node is Attribute)
			{
				if (isVertexShader)
					return _attributeAllocator;
				else
					throw new Error('Reading vertex attributes in a fragment shader makes no sense. ' +
						'Add some interpolates in your shader.');
			}
			
			if (node is Interpolate)
				return _varyingAllocator;
			
			if (node is BindableSampler || node is Sampler)
			{
				if (!isVertexShader)
					return null;
				else
					throw new Error('Sampling textures in the vertex shader is not allowed.');
			}
			
			throw new Error('There is no allocator for a node of this kind. Go fix your code.');
		}
		
		private function getInstructionCounter(isVertexShader : Boolean) : uint
		{
			return isVertexShader ? _vsInstructions.length : _fsInstructions.length;
		}
		
		private function pushInstruction(instruction : Instruction, isVertexShader : Boolean) : void
		{
			if (isVertexShader)
				_vsInstructions.push(instruction);
			else
				_fsInstructions.push(instruction);
		}
	}
}
