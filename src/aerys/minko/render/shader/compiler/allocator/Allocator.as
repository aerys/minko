package aerys.minko.render.shader.compiler.allocator
{
	import aerys.minko.render.shader.compiler.register.RegisterSwizzling;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.IAlignedOperation;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	import aerys.minko.render.shader.node.operation.manipulation.Combine;
	import aerys.minko.render.shader.node.operation.manipulation.Extract;
	
	import flash.utils.Dictionary;

	public class Allocator
	{
		protected var _opUsage 		: Dictionary;
		protected var _allocations	: Vector.<Allocation>;
		
		protected var _maxOffset	: uint;
		
		public function get allocations() : Vector.<Allocation>
		{
			return _allocations;
		}
		
		public function Allocator(registerCount:uint = 8)
		{
			_opUsage		= new Dictionary()
			_allocations	= new Vector.<Allocation>();
			_maxOffset		= 4 * registerCount;
		}
		
		public static function getWriteMask(localOffset:uint, size:uint) : uint
		{
			if (localOffset != 0 && localOffset + size > 4)
				throw new Error('localoffset + size must be < 4');
			
			var i : int, mask : uint;
			
			mask = 0x1;
			for (i = 0; i < localOffset; i++)
				mask <<= 1;
			for (i = 0; i < size - 1; i++)
				mask |= mask << 1;
			
			return mask;
		}
		
		public function getWriteMask(op : INode) : uint
		{
			var alloc		: Allocation = _opUsage[op] as Allocation;
			var localOffset	: uint = alloc.offset % 4;
			var size		: uint = alloc.size;
			
			return Allocator.getWriteMask(localOffset, size);
		}
		
		public function getLocalOffset(op : INode) : uint
		{
			var alloc : Allocation = _opUsage[op] as Allocation;
			if (alloc == null)
				throw new Error('Unable to compiler shader: an allocation is missing. Did you forget an interpolation node?');
			
			return alloc.offset % 4;
		}
		
		public function getId(op : INode) : uint
		{
			var alloc : Allocation = _opUsage[op] as Allocation;
			return alloc.offset / 4;
		}
		
		public function allocate(op				: INode, 
								 operationId	: uint = 0) : void
		{
			if (_opUsage[op])
				throw new Error('A result was already allocated for this operation');
			
			var alloc : Allocation = new Allocation(operationId, op);
			if (op is IAlignedOperation)
				alloc.aligned = true;
			
			_opUsage[op] = alloc;
			_allocations.push(alloc);
		}
		
		public function reportUsage(op			: INode, 
									operationId	: uint,
									aligned		: Boolean) : void 
		{
			if (!_opUsage[op])
				throw new Error('There is no allocated result for operation ' + op.toString());
			
			var allocation : Allocation = _opUsage[op] as Allocation;
			allocation.endId	= operationId;
			allocation.aligned	= allocation.aligned || aligned;
		}
		
		/**
		 * Kludge implementation of a first fit allocator.
		 * Should be replaced if too slow for interactive shader generation,
		 * or if we get short on registers and must use a more efficient
		 * algoritm.
		 * 
		 * @see Note p29 
		 */
		public function computeRegisterState(alignEverything:Boolean = false) : void 
		{
			
			var allocLength:uint = _allocations.length;
			
			for (var i:int = 0; i < allocLength; ++i)
			{
				var success			: Boolean		= false;
				var alloc			: Allocation	= _allocations[i];
				
				var maxLocalOffset	: uint = 
					(alloc.size > 4 || alloc.aligned || alignEverything) ? 0 : 4 - alloc.size;
				
				for (var regOffset : int = 0; regOffset < _maxOffset && !success; regOffset += 4)
					for (var localOffset : int = 0; localOffset <= maxLocalOffset && !success; ++localOffset)
					{
						var realOffset : int = regOffset + localOffset;
						if (isFree(alloc.beginId, realOffset, alloc.size, i))
						{
							alloc.offset = realOffset;
							success = true;
						}
					}
				
				if (!success)
					throw new Error('Unable to allocate: all ' + _maxOffset / 4 
						+ ' registers are full, or too fragmented to allocate '
						+ alloc.size + ' contiguous ' + (!alloc.aligned ? 'non-' : '')
						+ 'aligned floats');
			}
		}
		
		protected function isFree(operationId : int, offset : int, size : int, allocLimit:int = -1) : Boolean
		{
			if (allocLimit == 0)
				return true;
			
			if (allocLimit == -1)
				allocLimit = _allocations.length;
			
			var endOffset : int = offset + size;
			
			for (var i : int = 0; i < allocLimit; ++i)
			{
				var alloc			: Allocation	= _allocations[i];
				var allocOffset		: uint			= alloc.offset;
				var allocSize		: uint			= alloc.size;
				var allocEndOffset	: uint			= allocOffset + allocSize;
				
				if (alloc.beginId <= operationId && alloc.endId > operationId && (
						(offset >= allocOffset && offset < allocEndOffset) ||
						(endOffset > allocOffset && endOffset <= allocEndOffset) ||
						(offset <= allocOffset && endOffset >= allocEndOffset)
					))
					return false;
			}
			return true;
		}
		
	}
	
}
