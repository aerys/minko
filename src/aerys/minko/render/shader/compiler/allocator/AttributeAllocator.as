package aerys.minko.render.shader.compiler.allocator
{
	import aerys.minko.render.shader.node.INode;
	

	public class AttributeAllocator extends Allocator
	{
		public function AttributeAllocator(registerCount	: uint = 8)
		{
			super(registerCount);
		}
		
		public function getAllocations() : Array
		{
			var allocLength	: uint	= _allocations.length;
			var result		: Array	= new Array();
			
			for (var i:int = 0; i < allocLength; ++i)
			{
				var alloc : Allocation	= _allocations[i];
				result.push(alloc.node);
			}
			
			return result;
		}
		
		override public function getWriteMask(op : INode) : uint
		{
			throw new Error('Cannot write over an attribute!');
		}
		
		override public function allocate(op	: INode, 
										  opId	: uint = 0) : void
		{
			// an attribute is permanent, they all allocate at operation 0
			return super.allocate(op, 0);
		}
		
		override public function computeRegisterState(alignEverything : Boolean = false):void
		{
			// always align attributes
			return super.computeRegisterState(true);
		}

	}
}
