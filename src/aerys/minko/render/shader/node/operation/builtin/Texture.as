package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.IFragmentNode;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Sampler;
	import aerys.minko.render.shader.node.operation.AbstractOperation;

	public class Texture extends AbstractOperation implements IFragmentNode
	{
		override public function get opCode() : uint
		{
			return 0x28;
		}

		override public function get instructionName() : String
		{
			return 'tex';
		}

		override public function get size() : uint
		{
			return 4;
		}

		public function Texture(uv : INode, sampler : INode)
		{
			super(uv, sampler);

			if (!(sampler is Sampler))
				throw new Error('Texture second argument must be a sampler');
		}

		override public function isSame(node : INode) : Boolean
		{
			var castedNode : Texture = node as Texture;

			return castedNode != null && castedNode._arg1.isSame(_arg1) && castedNode._arg2.isSame(_arg2);
		}
	}
}
