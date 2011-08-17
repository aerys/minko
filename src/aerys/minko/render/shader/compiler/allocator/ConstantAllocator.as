package aerys.minko.render.shader.compiler.allocator
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.AbstractParameter;
	import aerys.minko.render.shader.node.leaf.Constant;

	public class ConstantAllocator extends Allocator
	{
		public function ConstantAllocator(registerCount : uint = 8)
		{
			super(registerCount);
		}
		
		override public function allocate(op			: INode, 
										  operationId	: uint = 0) : void
		{
			super.allocate(op, 0);
		}
		
		public function computeParameterAllocation() : Vector.<ParameterAllocation>
		{
			var result : Vector.<ParameterAllocation> = new Vector.<ParameterAllocation>();
			
			var allocationsLength:int = _allocations.length;
			for (var i : int = 0; i < allocationsLength; ++i)
			{
				var alloc:Allocation = _allocations[i];
				
				if (alloc.node is AbstractParameter)
				{
					var param		: AbstractParameter		= alloc.node as AbstractParameter;
					var paramAlloc	: ParameterAllocation	= new ParameterAllocation(alloc.offset, param);
					
					result.push(paramAlloc);
				}
			}
			
			return result;
		}
		
		public function computeConstantAllocation() : Vector.<Number>
		{
			var result		: Vector.<Number>	= new Vector.<Number>(_maxOffset);
			var allocLength	: int				= _allocations.length;
			var size		: int				= 0;
			
			for (var i : uint = 0; i < allocLength; ++i)
			{
				var alloc : Allocation = _allocations[i];
				
				if (size < alloc.offset + alloc.size)
					size = alloc.offset + alloc.size;
				
				if (alloc.node is Constant)
				{
					for (var j : int = 0; j < alloc.size; ++j)
						result[int(alloc.offset + j)] = Constant(alloc.node).constants[j];
				}
			}
			
			// make sure table.length % 4 == 0
			result.length = Math.ceil(size / 4) * 4;
			
			return result;
		}
		
	}
}