package aerys.minko.render.shader.compiler.allocator
{
	import aerys.minko.render.shader.compiler.register.RegisterLimit;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Attribute;
	
	public class AttributeAllocator extends Allocator
	{
		public function AttributeAllocator()
		{
			super(RegisterLimit.VS_MAX_ATTRIBUTE);
		}
		
		public function getAllocations() : Vector.<Attribute>
		{
			var allocLength	: uint					= _allocations.length;
			var attributes	: Vector.<Attribute>	= new Vector.<Attribute>(allocLength, true);
			
			for (var i : int = 0; i < allocLength; ++i)
				attributes[i] = Attribute(_allocations[i].node);
			
			return attributes;
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
