package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.ns.minko;

	public class TransformParameter extends AbstractParameter
	{
		use namespace minko;
		
		public function TransformParameter(size	: uint, 
										   key	: String)
		{
			super(size, key);
		}
		
		override public function clone() : AbstractConstant
		{
			return new TransformParameter(_size, _key);
		}
	}
}
