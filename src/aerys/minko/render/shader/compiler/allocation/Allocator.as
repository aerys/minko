package aerys.minko.render.shader.compiler.allocation
{
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;
	import aerys.minko.render.shader.compiler.register.RegisterType;
	
	import flash.utils.Dictionary;

	/**
	 * This class meant to allocate shader registries in the AllocationVisitor.
	 * 
	 * @private
	 * @author Romain Gilliotte
	 */	
	public class Allocator
	{
		private var _allocations			: Vector.<IAllocation>;
		
		private var _type					: uint;
		private var _offsetLimit			: uint;
		private var _readOnly				: Boolean;
		private var _alignEverything		: Boolean;
		private var _isVertexShader			: Boolean;
		
		/**
		 * List of allocations done with the allocate method.
		 * computeRegisterState must be called first for this to make any sense.
		 */
		public function get allocations() : Vector.<IAllocation>
		{
			return _allocations;
		}
		
		/**
		 * Type of this allocator.
		 * @see RegisterType
		 */
		public function get type() : uint
		{
			return _type;
		}
		
		/**
		 * OperationId of the last data read from this memory segment.
		 */		
		public function get maxOperationId() : uint
		{
			var maxOpId : uint = 0;
			for each (var allocation : SimpleAllocation in _allocations)
				if (allocation.endId > maxOpId)
					maxOpId = allocation.endId;
			return maxOpId;
		}
		
		/**
		 * Current program break
		 */	
		public function get maxOffset() : uint
		{
			var maxOffset : uint = 0;
			for each (var allocation : SimpleAllocation in _allocations)
				if (allocation.offset + allocation.maxSize > maxOffset)
					maxOffset = allocation.offset + allocation.maxSize;
			
			return maxOffset;
		}
		
		/**
		 * Maximum program break of this memory segment.
		 * Raising the break over this limit will raise an error.
		 */		
		public function get offsetLimit() : uint
		{
			return _offsetLimit;
		}
		
		public function Allocator(registerCount		: uint,
								  type				: uint,
								  readOnly			: Boolean,
								  aligned			: Boolean,
								  isVertexShader	: Boolean)
		{
			_allocations		= new Vector.<IAllocation>();
			_type				= type;
			_offsetLimit		= 4 * registerCount;
			_readOnly			= readOnly;
			_alignEverything	= aligned;
			_isVertexShader		= isVertexShader;
		}
		
		public function allocate(size			: uint,
								 componentWise	: Boolean,
								 operationId	: uint = 0) : SimpleAllocation
		{
			if (size == 0 || (size > 4 && size % 4 != 0))
				throw new Error('Invalid allocation size.');
			
			var alloc : SimpleAllocation = new SimpleAllocation(operationId, !componentWise, size, _type);
			_allocations.push(alloc);
			return alloc;
		}
		
		public function combineAllocations(allocations	: Vector.<SimpleAllocation>,
										   offsets		: Vector.<uint>) : ContiguousAllocation
		{
			for each (var allocation : SimpleAllocation in allocations)
				// remove the subAllocation from the allocation list: we don't want to process it later
				// when computing register states.
				_allocations.splice(_allocations.indexOf(allocation), 1);
			
			var contiguousAlloc : ContiguousAllocation = new ContiguousAllocation(allocations, offsets);
			_allocations.push(contiguousAlloc);
			
			return contiguousAlloc;
		}
		
		/**
		 * Set the offset property in all allocations previously made with the allocate method.
		 * The computation will be made so that no collisions occure, neither in space nor in time.
		 */		
		public function computeRegisterState() : void
		{
			sortAllocations();
			greedyColorAllocationGraph();
		}
		
		/**
		 * Sort allocations in reverse size order, and alignement, to save as much space
		 * as possible when using a first fit allocator over the allocation dataset.
		 */		
		private function sortAllocations() : void
		{
			
			var allocationCount : int = _allocations.length - 1;
			
			for (var allocationId : int = 0; allocationId < allocationCount; ++allocationId)
			{
				var allocation		: IAllocation = _allocations[allocationId];
				var nextAllocation	: IAllocation = _allocations[allocationId + 1];
				
				var mustReverse : Boolean =
					(!allocation.aligned && nextAllocation.aligned) ||
					(nextAllocation.maxSize > allocation.maxSize 
						&& !(allocation.aligned && !nextAllocation.aligned));
				
				if (mustReverse)
				{
					_allocations[allocationId] = nextAllocation;
					_allocations[allocationId + 1] = allocation;
					allocationId != 0 && (allocationId -= 2)
				}
			}
		}
		
		/**
		 * Kludge implementation of a first fit allocator.
		 * Should be replaced if too slow for interactive shader generation,
		 * or if we get short on registers and must use a more efficient
		 * algoritm.
		 */
		private function greedyColorAllocationGraph() : void
		{
			var allocLength : uint = _allocations.length;
			
			for (var allocationId : int = 0; allocationId < allocLength; ++allocationId)
			{
				var alloc			: IAllocation	= _allocations[allocationId];
				var aligned			: Boolean		= alloc.maxSize > 4 || alloc.aligned || _alignEverything;
				var maxLocalOffset	: uint			= aligned ? 0 : 4 - alloc.maxSize;
				
				searchEmptySlot:for (var regOffset : int = 0; regOffset < _offsetLimit; regOffset += 4)
					for (var localOffset : int = 0; localOffset <= maxLocalOffset; ++localOffset)
					{
						// we are going to try this offset
						alloc.offset = regOffset + localOffset;
						
						// search for a collision with other allocations
						for (var innerAllocationId : int = 0; innerAllocationId < allocationId; ++innerAllocationId)
							if (alloc.overlapsWith(_allocations[innerAllocationId], _readOnly))
								break
						
						// if no collision occured, we are done for this one.
						if (innerAllocationId == allocationId)
							break searchEmptySlot;
					}
				
				// if the previous loop are not broken, we are out of registers!
				if (regOffset == _offsetLimit && localOffset == maxLocalOffset + 1)
				{
					var registerType	: String = RegisterType.stringifyType(_type, _isVertexShader);
					var numRegisters	: String = (_offsetLimit / 4).toString();
					var allocSize		: String = (alloc.maxSize).toString();
					var alignement		: String = alloc.aligned ? 'aligned' : 'non-aligned';
					
					var errorMessage	: String = 'Unable to allocate: all "' + registerType + '" ' +
						numRegisters + ' registers are full, or too fragmented to allocate ' +
						allocSize + ' contiguous ' + alignement + ' floats';
					
					throw new Error(errorMessage);
				}
			}
		}
		
//		public function writeAllocationSummary(fieldWidth		: Number = 25,
//											   fieldHeight		: Number = 12) : BitmapData
//		{
//			var rectangle		: Rectangle		= new Rectangle();
//			var matrix			: Matrix		= new Matrix();
//			
//			var maxOffset		: uint			= this.maxOffset;
//			var maxOperationId	: uint			= this.maxOperationId;
//			var bitmapData		: BitmapData	= 
//				new BitmapData(fieldWidth * maxOffset, fieldHeight * maxOperationId, false, 0xFFFFFF);
//			
//			var textField		: TextField		= new TextField();
//			var textFormat		: TextFormat	= new TextFormat();
//			
//			for each (var allocation : SimpleAllocation in _allocations)
//			{
//				rectangle.x			= allocation.offset * fieldWidth;
//				rectangle.y			= (allocation.beginId - 1) * fieldHeight;
//				rectangle.width		= allocation.size * fieldWidth;
//				rectangle.height	= (allocation.endId - allocation.beginId) * fieldHeight;
//				matrix.tx			= rectangle.x;
//				matrix.ty			= rectangle.y;
//				
//				textField.text		= Instruction(allocation.node).name;
//				textFormat.bold		= allocation.aligned;
//				textField.setTextFormat(textFormat);
//				
//				bitmapData.fillRect(rectangle, Math.random() * 0x505050 + 0x909090);
//				bitmapData.draw(textField, new Matrix(1, 0, 0, 1, rectangle.x, rectangle.y - 4));
//			}
//			
//			// horizontal lines
//			for (var operationId : uint = 1; operationId < maxOperationId; ++operationId)
//			{
//				rectangle.x			= 0;
//				rectangle.y			= operationId * fieldHeight;
//				rectangle.width		= maxOffset * fieldWidth;
//				rectangle.height	= 1;
//				
//				bitmapData.fillRect(rectangle, 0xBBBBBB);
//			}
//			
//			// vertical lines
//			for (var offsetId : uint = 1; offsetId < maxOffset; offsetId += 1)
//			{
//				rectangle.x			= offsetId * fieldWidth;
//				rectangle.y			= 0;
//				rectangle.width		= 1;
//				rectangle.height	= fieldHeight * maxOperationId;
//				
//				bitmapData.fillRect(rectangle, offsetId % 4 == 0 ? 0 : 0xBBBBBB);
//			}
//			
//			return bitmapData;
//		}
	}
}
