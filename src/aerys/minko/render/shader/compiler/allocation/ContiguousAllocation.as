package aerys.minko.render.shader.compiler.allocation
{
	import aerys.minko.render.shader.compiler.register.Components;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class ContiguousAllocation implements IAllocation
	{
		private var _subAllocations : Vector.<SimpleAllocation>;
		private var _offsets		: Vector.<uint>;
		
		public function get type() : uint
		{
			return _subAllocations[0].type;
		}
		
		public function get registerId() : uint
		{
			return _subAllocations[0].registerId;
		}
		
		public function get registerOffset() : uint
		{
			return offset % 4;
		}
		
//		public function get writeMask() : uint
//		{
//			
//		}
		
		public function get aligned() : Boolean
		{
			return false;
		}
		
		public function get offset() : uint
		{
			return _subAllocations[0].offset - _offsets[0];
		}
		
		public function get maxSize() : uint
		{
			var numArgs : uint = _subAllocations.length;
			var maxSize	: uint = 0;
			
			for (var argId : uint = 0; argId < numArgs; ++argId)
			{
				var currentSize : uint = _offsets[argId] + _subAllocations[argId].maxSize;
				if (currentSize > maxSize)
					maxSize = currentSize;
			}
			
			return maxSize;
		}
		
		public function get subAllocations() : Vector.<SimpleAllocation>
		{
			return _subAllocations;
		}
		
		public function set offset(v : uint) : void
		{
			var numArgs : uint = _subAllocations.length;
			
			for (var argId : uint = 0; argId < numArgs; ++argId)
				_subAllocations[argId].offset = v + _offsets[argId];
		}
		
		public function ContiguousAllocation(subAllocations : Vector.<SimpleAllocation>,
											 offsets		: Vector.<uint>)
		{
			_subAllocations = subAllocations;
			_offsets		= offsets;
		}
		
		public function getReadSwizzle(readingOpWriteOffset	: uint,
									   readingOpComponents	: uint) : uint
		{
			var components : uint;
			
			components = readingOpComponents;
			components = Components.applyWriteOffset(components, readingOpWriteOffset);
			components = Components.applyReadOffset(components, registerOffset);
			
			if (components == Components.stringToComponent('____'))
				throw new Error();
			
			return Components.generateReadSwizzle(components);
		}
		
		public function extendLifeTime(operationId : uint) : void
		{
			for each (var sub : SimpleAllocation in _subAllocations)
				sub.extendLifeTime(operationId);
		}
		
		public function overlapsWith(other : IAllocation, readOnly : Boolean) : Boolean
		{
			if (other is SimpleAllocation)
			{
				return other.overlapsWith(this, readOnly);
			}
			else if (other is ContiguousAllocation)
			{
				var contiOther	: ContiguousAllocation	= ContiguousAllocation(other);
				
				for each (var sub1 : SimpleAllocation in _subAllocations)
					for each (var sub2 : SimpleAllocation in contiOther.subAllocations)
						if (sub1.overlapsWith(sub2, readOnly))
							return true;
				
				return false;
			}
			else
				throw new Error('Unknown allocation type.');
		}
		
		
		public function toString() : String
		{
			var result : String;
			result = 'Contiguous\n';
			for each (var subAlloc : SimpleAllocation in _subAllocations)
				result += "\t" + subAlloc.toString() + "\n";
				
			return result;
		}
	}
}