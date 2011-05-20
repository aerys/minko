package aerys.minko.render.shader.compiler.allocator
{
	import aerys.minko.render.shader.node.INode;

	public class Allocation 
	{
		protected var _beginId	: uint;
		protected var _endId	: uint;
		protected var _aligned	: Boolean;
		
		protected var _offset	: uint;
		protected var _node		: INode;
		
		public function get offset():uint
		{
			return _offset;
		}

		public function get node():INode
		{
			return _node;
		}

		public function get aligned() : Boolean
		{
			return _aligned;
		}

		public function get size() : uint
		{
			return _node.size;
		}
		
		public function get beginId() : uint
		{
			return _beginId;
		}

		public function get endId() : uint
		{
			return _endId;
		}
		
		public function set offset(value:uint):void
		{
			_offset = value;
		}

		public function set aligned(v : Boolean):void
		{
			_aligned = v;
		}

		public function set endId(v : uint):void
		{
			_endId = v;
		}
		
		public function Allocation(operationId	: uint, 
								   node			: INode)
		{
			_aligned	= false;
			_beginId	= operationId;
			_endId		= operationId
			_node		= node;
		}
	}
}
