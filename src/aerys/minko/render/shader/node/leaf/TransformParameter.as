package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.shader.node.INode;

	public class TransformParameter extends AbstractParameter
	{
		use namespace minko;

		public function TransformParameter(size	: uint,
										   key	: Object)
		{
			super(size, key);
		}

		override public function clone() : AbstractConstant
		{
			return new TransformParameter(_size, _key);
		}

		override public function isSame(otherNode : INode) : Boolean
		{
			var otherTransformParam : TransformParameter = otherNode as TransformParameter;

			return otherTransformParam != null
				&&_size		== otherTransformParam._size
				&& _name	== otherTransformParam._name
				&& _key		== otherTransformParam._key
				&& _field	== otherTransformParam._field
				&& _index	== otherTransformParam._index;
		}
	}
}
