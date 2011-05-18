package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.ns.minko;

	public class WorldParameter extends AbstractParameter
	{
		use namespace minko;
		
		minko var _class : Class;
		
		public function WorldParameter(size		: uint, 
									   key		: Class,
									   field	: String	= null, 
									   index	: int		= -1)
		{
			super(size, null, field, index);
			_class = key;
		}
		
		override public function clone() : AbstractConstant
		{
			return new WorldParameter(_size, _class, _field, _index);
		}
	}
	
}
