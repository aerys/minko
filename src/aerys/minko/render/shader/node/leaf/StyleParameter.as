package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.ns.minko;

	public class StyleParameter extends AbstractParameter
	{
		use namespace minko;
		
		public function StyleParameter(size		: uint, 
									   key		: String, 
									   field	: String=null, 
									   index	: int = -1)
		{
			super(size, key, field, index);
		}
		
		override public function clone() : AbstractConstant
		{
			return new StyleParameter(_size, _key, _field, _index);
		}
	}
}