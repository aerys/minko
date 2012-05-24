package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.binding.*;
	import aerys.minko.render.shader.compiler.allocation.*;
	import aerys.minko.render.shader.compiler.graph.ShaderGraph;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.*;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.*;
	import aerys.minko.render.shader.compiler.register.*;
	import aerys.minko.render.shader.compiler.sequence.*;
	import aerys.minko.type.stream.format.VertexComponent;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class AllocationVisitor extends AbstractVisitor
	{
		private var _allocStore			: AllocationStore;
		
		// allocators
		private var _opAllocator		: Allocator;
		private var _ocAllocator		: Allocator;
		private var _attributeAllocator	: Allocator;
		private var _vsConstAllocator	: Allocator;
		private var _fsConstAllocator	: Allocator;
		private var _vsTempAllocator	: Allocator;
		private var _fsTempAllocator	: Allocator;
		private var _varyingAllocator	: Allocator;
		
		// first pass data
		private var _vsInstructions		: Vector.<Instruction>;
		private var _fsInstructions		: Vector.<Instruction>;
		
		private var _vsConstants		: Vector.<Constant>;
		private var _fsConstants		: Vector.<Constant>;
		
		private var _vsParams			: Vector.<BindableConstant>;
		private var _fsParams			: Vector.<BindableConstant>;
		
		private var _samplers			: Vector.<AbstractSampler>;
		
		// final compiled program
		private var _vsProgram			: Vector.<AgalInstruction>;
		private var _fsProgram			: Vector.<AgalInstruction>;
		
		private var _paramBindings		: Object;
		
		private var _vertexComponents	: Vector.<VertexComponent>;
		private var _vertexIndices		: Vector.<uint>;
		
		private var _vertexConstants	: Vector.<Number>;
		private var _fragmentConstants	: Vector.<Number>;
		private var _textures			: Vector.<ITextureResource>;
		
		public function get parameterBindings() : Object
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
		}
		
		override protected function start() : void
		{
			super.start();
			
			// allocators
			_opAllocator				= new Allocator(1, RegisterType.OUTPUT, true, true, true);
			_ocAllocator				= new Allocator(1, RegisterType.OUTPUT, true, true, false);
			_attributeAllocator			= new Allocator(RegisterLimit.VS_MAX_ATTRIBUTE, RegisterType.ATTRIBUTE, true, true, true);
			_vsConstAllocator			= new Allocator(RegisterLimit.VS_MAX_CONSTANT, RegisterType.CONSTANT, true, false, true);
			_fsConstAllocator			= new Allocator(RegisterLimit.FS_MAX_CONSTANT, RegisterType.CONSTANT, true, false, false);
			_vsTempAllocator			= new Allocator(RegisterLimit.VS_MAX_TEMPORARY, RegisterType.TEMPORARY, false, false, true);
			_fsTempAllocator			= new Allocator(RegisterLimit.FS_MAX_TEMPORARY, RegisterType.TEMPORARY, false, false, false);
			_varyingAllocator			= new Allocator(RegisterLimit.MAX_VARYING, RegisterType.VARYING, true, true, true);
			
			_allocStore					= new AllocationStore();
			
			// first pass data
			_vsInstructions				= new Vector.<Instruction>();
			_fsInstructions				= new Vector.<Instruction>();
			_vsConstants				= new Vector.<Constant>();
			_fsConstants				= new Vector.<Constant>();
			_vsParams					= new Vector.<BindableConstant>();
			_fsParams					= new Vector.<BindableConstant>();
			_samplers					= new Vector.<AbstractSampler>();
			
			// final compiled data
			_paramBindings				= new Object();
			_vertexComponents			= new Vector.<VertexComponent>();
			_vertexIndices				= new Vector.<uint>();
			_textures					= new Vector.<ITextureResource>();
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
			
			opMove.component1		= _shaderGraph.positionComponents;
			ocMove.component1		= _shaderGraph.colorComponents;
			_shaderGraph.position	= opMove;
			_shaderGraph.color		= ocMove;
			
			visit(_shaderGraph.position, true);
			for (i = 0; i < numInterpolates; ++i)
				visit(_shaderGraph.interpolates[i], true);
			
			visit(_shaderGraph.color, false);
			
			for (i = 0; i < numKills; ++i)
			{
				var kill : AbstractNode = _shaderGraph.kills[i];
				if (kill is Constant || kill is BindableConstant)
					_shaderGraph.kills[i] = kill = new Instruction(Instruction.MOV, kill);
				
				visit(kill, false);
				pushInstruction(new Instruction(Instruction.KIL, kill), false);
			}
			
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
			
			createConstantParameterBindings(_vsParams, true);
			createConstantParameterBindings(_fsParams, false);
			
			_vertexConstants		= createConstantTables(_vsConstants, true);
			_vsProgram				= writeProgram(_vsInstructions, true);
			
			_fragmentConstants		= createConstantTables(_fsConstants, false);
			_fsProgram				= writeProgram(_fsInstructions, false);
			
			processAttributes();
			
			super.finish();
		}
		
		public function clear() : void
		{
			// allocators
			_opAllocator				= null;
			_ocAllocator				= null;
			_attributeAllocator			= null;
			_vsConstAllocator			= null;
			_fsConstAllocator			= null;
			_vsTempAllocator			= null;
			_fsTempAllocator			= null;
			_varyingAllocator			= null;
			
			_allocStore					= null;
			
			// first pass data
			_vsInstructions				= null;
			_fsInstructions				= null;
			_vsConstants				= null;
			_fsConstants				= null;
			_vsParams					= null;
			_fsParams					= null;
			_samplers					= null;
			
			// final compiled data
			_paramBindings				= null;
			_vertexComponents			= null;
			_vertexIndices				= null;
			_textures					= null;
		}
		
		private function processAttributes() : void
		{
			var maxRegisterId : uint = 0;
			
			_vertexComponents	= new Vector.<VertexComponent>(8);
			_vertexIndices		= new Vector.<uint>(8);
			
			for (var node : Object in _allocStore.getStore(true))
			{
				if (node is Attribute)
				{
					var registerId	: uint				= _allocStore.getSimpleAlloc(node, true).registerId;
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
														 isVertexShader		: Boolean) : void
		{
			for each (var bindableConstant : BindableConstant in bindableConstants)
			{
				var bindingName : String			= bindableConstant.bindingName;
				var tree		: AbstractNode		= _shaderGraph.computableConstants[bindingName];
				var alloc		: SimpleAllocation	= _allocStore.getSimpleAlloc(bindableConstant, isVertexShader);
				var binder		: IBinder			= new ConstantBinder(bindingName, alloc.offset, alloc.maxSize, isVertexShader);
				
				if (!tree)
					insertNewBinder(binder);
				else
				{
					var evalExp				: EvalExp					= new EvalExp(tree);
					var inBindableConsts	: Vector.<BindableConstant> = evalExp.bindableConstants;
					var inBinders			: Vector.<IBinder>			= new Vector.<IBinder>();
					
					for each (var inBindableConst : BindableConstant in inBindableConsts)
						insertNewBinder(new EvalExpConstantBinder(inBindableConst.bindingName, inBindableConst.size, binder, evalExp));
				}
			}
		}
		
		private function insertNewBinder(binder : IBinder) : void
		{
			var bindingName : String	= binder.bindingName;
			var oldBinding	: IBinder	= _paramBindings[bindingName];
			
			if (oldBinding == null)
				_paramBindings[bindingName] = binder;
			else if (oldBinding is ProxyConstantBinder)
				ProxyConstantBinder(oldBinding).addBinder(binder);
			else
			{
				var proxy : ProxyConstantBinder = new ProxyConstantBinder(bindingName);
				proxy.addBinder(oldBinding);
				proxy.addBinder(binder);
				_paramBindings[bindingName] = proxy;
			}
		}
		
		private function createConstantTables(constants			: Vector.<Constant>, 
											  isVertexShader	: Boolean) : Vector.<Number>
		{
			var result		: Vector.<Number> = new Vector.<Number>();
			var alloc		: SimpleAllocation;
			var offsetBegin	: uint;
			var offsetLimit	: uint;
			
			for each (var constant : Constant in constants)
			{
				var data		: Vector.<Number>	= constant.value;
				
				alloc		= _allocStore.getSimpleAlloc(constant, isVertexShader);
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
				alloc		= _allocStore.getSimpleAlloc(parameter, isVertexShader);
				offsetBegin	= alloc.offset;
				offsetLimit	= offsetBegin + alloc.maxSize;
				
				if (result.length < offsetLimit)
					result.length = offsetLimit;
			}
			
			result.length = 4 * Math.ceil(result.length / 4);
			
			return result;
		}
		
		private function writeProgram(instructions : Vector.<Instruction>, isVertexShader : Boolean) : Vector.<AgalInstruction>
		{
			var result : Vector.<AgalInstruction> = new Vector.<AgalInstruction>();
			
			for each (var instruction : Instruction in instructions)
			{
				var destAlloc	: SimpleAllocation; 
				var destination	: AgalDestination;
				var source1		: IAgalToken;
				var source2		: IAgalToken;
				
				if (instruction.id == Instruction.KIL)
				{
					// fake a destination at ft0.x, so that we can use common allocator for reads
					destAlloc = new SimpleAllocation(0, true, 1, RegisterType.TEMPORARY);
					destAlloc.offset = 0;
					
					destination	= new AgalDestination(0, 0, 0); // empty destination
				}
				else
				{
					destAlloc	= _allocStore.getSimpleAlloc(instruction, isVertexShader);
					destination	= new AgalDestination(destAlloc.registerId, destAlloc.writeMask, destAlloc.type);
				}
				
				source1	= getSourceFor(instruction.argument1, instruction.component1, destAlloc, isVertexShader);
				source2	= instruction.isSingle ? new AgalSourceEmpty() : getSourceFor(instruction.argument2, instruction.component2, destAlloc, isVertexShader);
				
				result.push(new AgalInstruction(instruction.id, destination, source1, source2));
			}
			
			return result;
		}
		
		private function getSourceFor(argument			: AbstractNode, 
									  readComponents	: uint, 
									  destAlloc			: SimpleAllocation, 
									  isVertexShader	: Boolean) : IAgalToken
		{
			var source : IAgalToken;
			
			if (argument is VariadicExtract)
			{
				var variadicExtract : VariadicExtract = VariadicExtract(argument);
				
				var constantAlloc	: IAllocation = _allocStore.getAlloc(variadicExtract.constant, isVertexShader);
				var indexAlloc		: IAllocation = _allocStore.getAlloc(variadicExtract.index, isVertexShader);
				
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
				var sourceAlloc	: IAllocation = _allocStore.getAlloc(argument, isVertexShader);
				
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
		
		private function extendLifeTime(argument : AbstractNode, isVertexShader : Boolean) : void
		{
			var instructionCounter : uint = getInstructionCounter(isVertexShader);
			
			if (argument is Sampler || argument is BindableSampler)
			{
				// do nothing
			}
			else if (argument is VariadicExtract)
			{
				var variadicExtract : VariadicExtract = VariadicExtract(argument);
				
				_allocStore.getAlloc(variadicExtract.constant, isVertexShader).extendLifeTime(instructionCounter);
				_allocStore.getAlloc(variadicExtract.index, isVertexShader).extendLifeTime(instructionCounter);
			}
			else
			{
				_allocStore.getAlloc(argument, isVertexShader).extendLifeTime(instructionCounter);
			}
		}
		
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
			var overwriterAllocator : Allocator					= getAllocatorFor(overwriter, isVertexShader);
			var numArgs				: uint						= overwriter.numArguments;
			
			var subAllocs 			: Vector.<SimpleAllocation>	= new Vector.<SimpleAllocation>();
			var subOffsets			: Vector.<uint>				= new Vector.<uint>();
			
			var mov					: Instruction;
			var instructionCounter	: uint;
			
			for (var argId : uint = 0; argId < numArgs; ++argId)
			{
				var arg				: AbstractNode	= overwriter.getArgumentAt(argId);
				var instructionArg	: Instruction	= arg as Instruction;
				var component		: uint			= overwriter.getComponentAt(argId);
				var minWriteOffset	: int			= Components.getMinWriteOffset(component);
				var newAllocation	: SimpleAllocation;
				
				component = Components.applyWriteOffset(component, -minWriteOffset);
				
				if (instructionArg != null)
					visitArguments(arg, isVertexShader);
				
				if (instructionArg != null && instructionArg.isComponentWise)
				{
					// create a new operation that combines the swizzles of the overwriter and
					// the instruction under it (this works only because it's all component wise).
					var instructionArgReplacement : Instruction = instructionArg.isSingle ? 
						new Instruction(instructionArg.id, instructionArg.argument1) : 
						new Instruction(instructionArg.id, instructionArg.argument1, instructionArg.argument2);
					
					instructionArgReplacement.component1 = Components.applyCombination(instructionArg.component1, component);
					if (!instructionArgReplacement.isSingle)
						instructionArgReplacement.component2 = Components.applyCombination(instructionArg.component2, component);
					
					// push instruction, allocate and report usages.
					pushInstruction(instructionArgReplacement, isVertexShader);
					
					instructionCounter	= getInstructionCounter(isVertexShader);
					newAllocation		= getAllocatorFor(instructionArgReplacement, isVertexShader).
						allocate(instructionArgReplacement.size, instructionArgReplacement.isComponentWise, instructionCounter);
					
					_allocStore.storeAlloc(newAllocation, instructionArgReplacement, isVertexShader);
					
					subAllocs.push(newAllocation);
					subOffsets.push(minWriteOffset);
					
					extendLifeTime(instructionArgReplacement.argument1, isVertexShader);
					if (!instructionArgReplacement.isSingle)
						extendLifeTime(instructionArgReplacement.argument2, isVertexShader);
				}
				else if (arg is Attribute || arg is Constant || arg is BindableConstant ||
					     arg is Interpolate || arg is VariadicExtract ||
						 (instructionArg != null && !instructionArg.isComponentWise))
				{
					// visit the constant/attribute/etc to allocate it.
					visit(arg, isVertexShader);
					
					// execute a mov intruction to copy it to temporaries and swizzle it properly.
					mov = new Instruction(Instruction.MOV, arg);
					mov.component1 = component;
					pushInstruction(mov, isVertexShader);
					
					// get the current instruction counter, to report allocations.
					instructionCounter = getInstructionCounter(isVertexShader);
					
					// allocate the result of the mov, and report usage of the constant.
					newAllocation = getAllocatorFor(mov, isVertexShader).allocate(mov.size, true, instructionCounter);
					_allocStore.storeAlloc(newAllocation, mov, isVertexShader);
					
					subAllocs.push(newAllocation);
					subOffsets.push(minWriteOffset);
					
					extendLifeTime(arg, isVertexShader);
				}
				else if (arg is Overwriter || arg is Extract)
					throw new Error('This should NEVER happen.');
				
				else if (arg is Sampler || arg is BindableSampler)
					throw new Error('Samplers cannot be casted to floats. Go fix your shader code.');
				
				else
					throw new Error('Unknown node type. This shoud never happen.');
			}
			
			var overwriterAlloc : IAllocation = overwriterAllocator.combineAllocations(subAllocs, subOffsets);
			_allocStore.storeAlloc(overwriterAlloc, overwriter, isVertexShader);
		}
		
		override protected function visitInstruction(instruction	: Instruction, 
													 isVertexShader	: Boolean) : void
		{
			// visit children
			visitArguments(instruction, isVertexShader);
			
			// push instruction into list
			pushInstruction(instruction, isVertexShader);
			
			// allocate result of instruction
			var instructionCounter	: uint				= getInstructionCounter(isVertexShader);
			var allocator			: Allocator			= getAllocatorFor(instruction, isVertexShader);
			var allocation			: SimpleAllocation	=
				allocator.allocate(instruction.size, instruction.isComponentWise, instructionCounter);
			
			_allocStore.storeAlloc(allocation, instruction, isVertexShader);

			// extend life time of arguments, so that they are not released too soon.
			extendLifeTime(instruction.argument1, isVertexShader);
			if (!instruction.isSingle)
				extendLifeTime(instruction.argument2, isVertexShader);
		}
		
		override protected function visitVariadicExtract(variadicExtract : VariadicExtract, isVertexShader : Boolean) : void
		{
			if (!isVertexShader)
				throw new Error('VariadicExtracts can only be found in the vertex shader.');
			
			visitArguments(variadicExtract, true);
		}
		
		override protected function visitInterpolate(interpolate	: Interpolate, 
													 isVertexShader	: Boolean) : void
		{
			if (isVertexShader)
			{
				visit(interpolate.argument, true);
				
				var components		: uint			= interpolate.component;
				var size			: int			= Components.getMaxWriteOffset(components) + 1;
				var modifier		: uint			= Components.createContinuous(0, 0, 4, size);
				var final			: uint			= Components.applyCombination(components, modifier);
				
				var movInstruction	: Instruction	= new Instruction(Instruction.MOV, interpolate.argument);
				movInstruction.component1		= final;
				
				var allocation : SimpleAllocation	=
					_varyingAllocator.allocate(movInstruction.size, true, getInstructionCounter(true));
				
				_allocStore.storeAlloc(allocation, movInstruction, true);
				_allocStore.storeAlloc(allocation, interpolate, false);
				
				extendLifeTime(interpolate.argument, true);
				
				_vsInstructions.push(movInstruction);
			}
		}
		
		override protected function visitAttribute(attribute		: Attribute, 
												   isVertexShader	: Boolean) : void
		{
			if (!isVertexShader)
				throw new Error('Vertex attributes can only be found in the vertex shader.');
		
			// allocate
			var allocator	: Allocator			= getAllocatorFor(attribute, true);
			var allocation	: SimpleAllocation	= allocator.allocate(attribute.size, true, 0);
			
			_allocStore.storeAlloc(allocation, attribute, true);
		}
		
		override protected function visitConstant(constant			: Constant, 
												  isVertexShader	: Boolean) : void
		{
			// allocate
			var allocator		: Allocator			= getAllocatorFor(constant, isVertexShader);
			var allocation		: SimpleAllocation	= allocator.allocate(constant.size, true, 0);

			_allocStore.storeAlloc(allocation, constant, isVertexShader);
			
			// store into constant table: makes easy to build Program3D later
			var contantTable	: Vector.<Constant> = isVertexShader ? _vsConstants : _fsConstants;
			contantTable.push(constant);
		}
		
		override protected function visitBindableConstant(bindableConstant	: BindableConstant,
														  isVertexShader	: Boolean) : void
		{
			// allocate
			var allocator		: Allocator			= getAllocatorFor(bindableConstant, isVertexShader);
			var allocation		: SimpleAllocation	= allocator.allocate(bindableConstant.size, true, 0);
			
			_allocStore.storeAlloc(allocation, bindableConstant, isVertexShader);
			
			// store into parameter table: makes easy to build Program3D later
			var paramTable	: Vector.<BindableConstant> = isVertexShader ? _vsParams : _fsParams;
			paramTable.push(bindableConstant);
		}
		
		override protected function visitSampler(sampler : Sampler, isVertexShader : Boolean) : void
		{
			_samplers.push(sampler);
			_textures[_samplers.length - 1] = sampler.textureResource;
		}
		
		override protected function visitBindableSampler(bindableSampler : BindableSampler, isVertexShader : Boolean) : void
		{
			_samplers.push(bindableSampler);
			_textures[_samplers.length - 1] = null;
			insertNewBinder(new TextureBinder(bindableSampler.bindingName, _samplers.length - 1));
		}
		
		override protected function visitExtract(extract : Extract, isVertexShader : Boolean) : void
		{
			throw new Error('There cannot be any extract left at this point of shader compilation. Go fix your code.');
		}
		
		private function getAllocatorFor(node : AbstractNode, isVertexShader : Boolean) : Allocator
		{
			// if this is the root node, it has a different allocator.
			if (node === _shaderGraph.position)
				return _opAllocator;
			
			if (node === _shaderGraph.color)
				return _ocAllocator;
			
			if (_shaderGraph.interpolates.indexOf(node) !== -1)
				return _varyingAllocator;
			
			if (_shaderGraph.kills.indexOf(node) !== -1)
			{
				if (isVertexShader)
					throw new Error('kills are not allowed in the vertex shader.');
				else if (node is Instruction || node is Overwriter)
					return _fsTempAllocator;
				else if (node is Constant || node is BindableConstant)
					return _fsConstAllocator;
				else if (node is Interpolate)
					return _varyingAllocator;
				else
					throw new Error('Invalid allocation type.');
			}
			
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
