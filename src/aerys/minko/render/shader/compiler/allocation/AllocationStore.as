package aerys.minko.render.shader.compiler.allocation
{
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	
	import flash.utils.Dictionary;

	public class AllocationStore
	{
		private var _vsStore	: Dictionary;
		private var _fsStore	: Dictionary;
		
		public function AllocationStore()
		{
			_vsStore = new Dictionary();
			_fsStore = new Dictionary();
		}
		
		public function getStore(isVertexShader : Boolean) : Dictionary
		{
			return isVertexShader ? _vsStore : _fsStore;
		}
		
		public function getAlloc(key			: Object,
								 isVertexShader	: Boolean) : IAllocation
		{
			var allocationTable : Dictionary = isVertexShader ? _vsStore : _fsStore;
			
			if (allocationTable[key] == undefined)
				throw new Error('No such allocation was stored');
			
			return allocationTable[key];
		}
		
		public function getSimpleAlloc(key				: Object,
									   isVertexShader	: Boolean) : SimpleAllocation
		{
			var allocation : IAllocation = getAlloc(key, isVertexShader);
			
			if (!(allocation is SimpleAllocation))
				throw new Error('Allocation is not a SimpleAllocation');
			
			return SimpleAllocation(allocation);
		}
		
		public function storeAlloc(allocation		: IAllocation,
								   key				: Object,
								   isVertexShader	: Boolean) : void
		{
			var allocationTable : Dictionary = isVertexShader ? _vsStore : _fsStore;
			
			if (allocationTable[key] != undefined)
				throw new Error('An allocation is already stored for this node.');
			
			allocationTable[key] = allocation;
		}
	}
}
