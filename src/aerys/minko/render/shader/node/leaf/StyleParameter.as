package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.shader.node.INode;

	public class StyleParameter extends AbstractParameter
	{
		use namespace minko;
		
		private var _defaultValue	: Object	= null;
		
		public function get defaultValue() : Object	{ return _defaultValue; }
		
		public function StyleParameter(size			: uint, 
									   key			: Object,
									   defaultValue	: Object	= null,
									   field		: String	= null, 
									   index		: int 		= -1)
		{
			super(size, key, field, index);
			
			_defaultValue = defaultValue;
		}
		
		override public function clone() : AbstractConstant
		{
			return new StyleParameter(_size, _key, _defaultValue, _field, _index);
		}
		
		override public function isSame(otherNode : INode) : Boolean
		{
			var otherStyleParam : StyleParameter = otherNode as StyleParameter;
			
			return otherStyleParam != null
				&& _size	== otherStyleParam._size 
				&& _name	== otherStyleParam._name
				&& _key		== otherStyleParam._key
				&& _field	== otherStyleParam._field
				&& _index	== otherStyleParam._index;
		}

	}
}