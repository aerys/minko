package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.shader.node.INode;

	import flash.utils.getQualifiedClassName;

	public class WorldParameter extends AbstractParameter
	{
		use namespace minko;

		minko var _class : Class;

		public function get className() : String
		{
			var className : String = getQualifiedClassName(_class);

			return className.substr(className.lastIndexOf(":") + 1);
		}
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

		override public function isSame(otherNode : INode) : Boolean
		{
			var otherWorldParam : WorldParameter = otherNode as WorldParameter;

			return otherWorldParam != null
				&&_size		== otherWorldParam._size
				&& _name	== otherWorldParam._name
				&& _field	== otherWorldParam._field
				&& _key		== otherWorldParam._key
				&& _index	== otherWorldParam._index
				&& _class	== otherWorldParam._class;
		}
	}
}
