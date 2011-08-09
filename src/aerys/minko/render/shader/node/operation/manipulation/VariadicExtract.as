package aerys.minko.render.shader.node.operation.manipulation
{
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.AbstractConstant;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	
	public class VariadicExtract extends AbstractOperation
	{
		private var _cellSize : uint;
		
		override public function get instructionName() : String
		{
			throw new Error('This is a virtual operand');
		}
		
		override public function get opCode() : uint
		{
			throw new Error('This is a virtual operand');
		}
		
		override public function get size() : uint
		{
			return _cellSize;
		}
		
		public function VariadicExtract(index		: INode,
										table		: AbstractConstant,
										cellSize	: uint)
		{
			_cellSize	= cellSize;
			
			var leftOperand : INode = index;
			if (cellSize != 4)
			{
				leftOperand = new Multiply(leftOperand, new Constant(cellSize / 4));
				
			}
			// the following is a patch to adobe's agal validator.
			// we have to fill a complete register for the index to work
			leftOperand = new RootWrapper(new Extract(leftOperand, Components.XXXX));
			
			var rightOperand : INode = table;
			
			super(leftOperand, rightOperand);
			
			if (cellSize % 4 != 0)	throw new Error();
//			if (index.size != 1)	throw new Error();
		}
		
		override public function isSame(node : INode) : Boolean
		{
			var castedNode : VariadicExtract = node as VariadicExtract;
			
			return castedNode != null && 
				castedNode._arg1.isSame(_arg1) && 
				castedNode._arg2.isSame(_arg2) &&
				castedNode._cellSize == _cellSize;
		}
	}
}
