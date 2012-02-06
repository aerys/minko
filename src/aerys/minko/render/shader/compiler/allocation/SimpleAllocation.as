package aerys.minko.render.shader.compiler.allocation
{
	import aerys.minko.render.shader.compiler.register.Components;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;

	public class SimpleAllocation implements IAllocation
	{
		private var _type		: uint;
		
		private var _beginId	: uint;
		private var _endId		: uint;
		private var _aligned	: Boolean;
		
		private var _offset		: int;
		private var _size		: uint;
		
		public function get type() : uint
		{
			return _type;
		}
		
		public function get beginId() : uint
		{
			return _beginId;
		}
		
		public function get endId() : uint
		{
			return _endId;
		}
		
		public function get aligned() : Boolean
		{
			return _aligned;
		}
		
		public function get offset() : uint
		{
			if (_offset == -1)
				throw new Error('Offset of this allocation is still unknown');
			
			return _offset;
		}
		
		public function get maxSize() : uint
		{
			return _size;
		}
		
		public function get registerOffset() : uint
		{
			// use offset getter instead of property to raise error if needed
			return offset % 4;
		}
		
		public function get registerId() : uint
		{
			// use offset getter instead of property to raise error if needed
			return offset / 4;
		}
		
		public function get writeMask() : uint
		{
			var localOffset	: uint = _offset % 4;
			var i			: int;
			var mask		: uint;
			
			if (localOffset != 0 && localOffset + _size > 4)
				throw new Error('localOffset + size must be < 4');
			
			mask = 0x1;
			for (i = 0; i < localOffset; i++)
				mask <<= 1;
			for (i = 0; i < _size - 1; i++)
				mask |= mask << 1;
			
			return mask;
		}
		
		public function set offset(v : uint) : void
		{
			_offset = v;
		}
		
		public function SimpleAllocation(operationId	: uint,
										 aligned		: Boolean,
										 size			: uint,
										 type			: uint)
		{
			_aligned	= aligned;
			_beginId	= operationId;
			_endId		= operationId;
			_size		= size;
			_offset		= -1;
			_type		= type;
		}
		
		public function getReadSwizzle(readingOpWriteOffset : uint,
									   readingOpComponents	: uint) : uint
		{
			var size : uint = Math.min(maxSize, 4);
			
			if (size == 0 || size > 4)
				throw new Error();
			
			var components : uint;
//			trace('reading, size=', size, 'registeroff', registerOffset, 'writeoffset', readingOpWriteOffset);
			
			components = readingOpComponents;
//			trace('read', Components.componentToString(components));
			
			components = Components.applyWriteOffset(components, readingOpWriteOffset);
//			trace('writeoffseted', Components.componentToString(components));
				
			components = Components.applyReadOffset(components, registerOffset);
//			trace('readoffseted', Components.componentToString(components));
//			trace('');
//			trace('============================================================');
//			trace('');
			
			if (components == Components.stringToComponent('____'))
				throw new Error();
			
			return Components.generateReadSwizzle(components);
		}
		
		public function extendLifeTime(operationId : uint) : void
		{
			if (_endId < operationId)
				_endId = operationId;
		}
		
		public function overlapsWith(other : IAllocation, readOnly : Boolean) : Boolean
		{
			var result : Boolean;
			
			if (other is SimpleAllocation)
			{
				var simpleOther : SimpleAllocation = SimpleAllocation(other);
				
				result = overlapsInSpaceWith(simpleOther);
				if (!readOnly)
					result &&= overlapsInTimeWith(simpleOther);
				
				return result;
			}
			else if (other is ContiguousAllocation)
			{
				var contiOther : ContiguousAllocation = ContiguousAllocation(other);
				
				for each (var sub : SimpleAllocation in contiOther.subAllocations)
					if (overlapsWith(sub, readOnly))
						return true;
				
				return false;
			}
			else
				throw new Error('Unknown allocation type.');
		}
		
		private function overlapsInSpaceWith(other : SimpleAllocation) : Boolean
		{
			return other.offset < _offset + _size && other.offset + other.maxSize > _offset;
		}
		
		private function overlapsInTimeWith(other : SimpleAllocation) : Boolean
		{
			return other.beginId < _endId && other.endId > _beginId;
		}
		
		public function toString() : String
		{
			return '[operation=' + _beginId + '-' + _endId + '][offset=' + _offset + '-' + _size + '][alignement=' + _aligned + ']'; 
		}
	}
}
