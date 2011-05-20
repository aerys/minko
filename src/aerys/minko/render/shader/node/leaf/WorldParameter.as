package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.ns.minko;
	
	import flash.utils.getQualifiedClassName;

	public class WorldParameter extends AbstractParameter
	{
		use namespace minko;
		
		public function get className() : String
		{
			var className : String = getQualifiedClassName(_class);
			return className.substr(className.lastIndexOf(":") + 1);
		}
		
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
